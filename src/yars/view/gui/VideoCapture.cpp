#include "VideoCapture.h"

// this file was copied from
// https://stackoverflow.com/questions/46444474/c-ffmpeg-create-mp4-file?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa

#include <yars/configuration/YarsConfiguration.h>

#define VIDEO_TMP_FILE "/tmp/tmp.h264"

#include <iostream>
#include <string>
#include <stdio.h>

using namespace std;

void VideoCapture::init(int width, int height, int fpsrate, int bitrate, string fn) {

  fps         = fpsrate;
  filename    = fn;
  videoWidth  = width;
  videoHeight = height;

  int err;

  if (!(oformat = av_guess_format(nullptr, VIDEO_TMP_FILE, nullptr))) {
    cout << "Failed to define output format" << endl;
    exit(-1);
  }

  if ((err = avformat_alloc_output_context2(&ofctx, oformat, nullptr, VIDEO_TMP_FILE) < 0)) {
    cout << "Failed to allocate output context" << endl;
    exit(-1);
  }

  if (!(codec = avcodec_find_encoder(oformat->video_codec))) {
    cout << "Failed to find encoder" << endl;
    exit(-1);
  }

  if (!(videoStream = avformat_new_stream(ofctx, codec))) {
    cout << "Failed to create new stream" << endl;
    __free();
    exit(-1);
  }

  if (!(cctx = avcodec_alloc_context3(codec))) {
    cout << "Failed to allocate codec context" << endl;
    __free();
    exit(-1);
  }

  videoStream->codecpar->codec_id = oformat->video_codec;
  videoStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
  videoStream->codecpar->width = width;
  videoStream->codecpar->height = height;
  videoStream->codecpar->format = AV_PIX_FMT_YUV420P;
  videoStream->codecpar->bit_rate = bitrate * 1000;
  videoStream->time_base = (AVRational){ 1, __YARS_GET_CAPTURE_FRAME_RATE };

  avcodec_parameters_to_context(cctx, videoStream->codecpar);
  cctx->time_base = (AVRational){ 1, __YARS_GET_CAPTURE_FRAME_RATE };
  cctx->max_b_frames = 2;
  cctx->gop_size = 12;
  if (videoStream->codecpar->codec_id == AV_CODEC_ID_H264) {
    av_opt_set(cctx, "preset", "ultrafast", 0);
  }
  av_opt_set(cctx->priv_data, "crf", "1", 0);

  if (ofctx->oformat->flags & AVFMT_GLOBALHEADER) {
    cctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  }
  avcodec_parameters_from_context(videoStream->codecpar, cctx);

  if ((err = avcodec_open2(cctx, codec, nullptr)) < 0) {
    cout << "Failed to open codec" << endl;
    __free();
    exit(-1);
  }

  if (!(oformat->flags & AVFMT_NOFILE)) {
    if ((err = avio_open(&ofctx->pb, VIDEO_TMP_FILE, AVIO_FLAG_WRITE)) < 0) {
      cout << "Failed to open file" << endl;
      __free();
      exit(-1);
    }
  }

  if ((err = avformat_write_header(ofctx, nullptr)) < 0) {
    cout << "Failed to write header" << endl;
    __free();
    exit(-1);
  }

  av_dump_format(ofctx, 0, VIDEO_TMP_FILE, 1);
}

void VideoCapture::addFrame(uint8_t *data) {
  int err;
  if (!videoFrame) {

    videoFrame = av_frame_alloc();
    videoFrame->format = AV_PIX_FMT_YUV420P;
    videoFrame->width = cctx->width;
    videoFrame->height = cctx->height;

    if ((err = av_frame_get_buffer(videoFrame, 32)) < 0) {
      cout << "Failed to allocate picture" << endl;
      exit(-1);
    }
  }

  if (!swsCtx) {
    // OGRE renders in PF_B8G8R8A8 (BGRA byte order), so use AV_PIX_FMT_BGRA
    swsCtx = sws_getContext(cctx->width, cctx->height, AV_PIX_FMT_BGRA, cctx->width, cctx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, 0, 0, 0);
  }

  int inLinesize[1] = { 4 * cctx->width };

  // From BGRA to YUV
  sws_scale(swsCtx, (const uint8_t * const *)&data, inLinesize, 0, cctx->height, videoFrame->data, videoFrame->linesize);

  videoFrame->pts = frameCounter++;

  if ((err = avcodec_send_frame(cctx, videoFrame)) < 0) {
    cout << "Failed to send frame: " << err << endl;
    exit(-1);
  }

  // Receive all available encoded packets
  AVPacket *pkt = av_packet_alloc();
  if (!pkt) {
    cout << "Failed to allocate packet" << endl;
    exit(-1);
  }

  while (avcodec_receive_packet(cctx, pkt) == 0) {
    av_interleaved_write_frame(ofctx, pkt);
    av_packet_unref(pkt);
  }
  av_packet_free(&pkt);
}

void VideoCapture::finish() {
  //DELAYED FRAMES
  AVPacket *pkt = av_packet_alloc();
  if (!pkt) {
    cout << "Failed to allocate packet" << endl;
    return;
  }

  for (;;) {
    avcodec_send_frame(cctx, nullptr);
    if (avcodec_receive_packet(cctx, pkt) == 0) {
      av_interleaved_write_frame(ofctx, pkt);
      av_packet_unref(pkt);
    }
    else {
      break;
    }
  }
  av_packet_free(&pkt);

  av_write_trailer(ofctx);
  if (!(oformat->flags & AVFMT_NOFILE)) {
    int err = avio_close(ofctx->pb);
    if (err < 0) {
      cout << "Failed to close file" << endl;
    }
  }

  __free();
  __remux();
  remove(VIDEO_TMP_FILE);

}

void VideoCapture::__free() {
  if (videoFrame) {
    av_frame_free(&videoFrame);
  }
  if (cctx) {
    avcodec_free_context(&cctx);
  }
  if (ofctx) {
    avformat_free_context(ofctx);
  }
  if (swsCtx) {
    sws_freeContext(swsCtx);
  }
  videoFrame = nullptr;
  cctx       = nullptr;
  ofctx      = nullptr;
  swsCtx     = nullptr;

}

void VideoCapture::__remux() {
  AVFormatContext *ifmt_ctx = nullptr, *ofmt_ctx = nullptr;
  int err;

  // Explicitly specify h264 format for raw H.264 input
  const AVInputFormat *ifmt = av_find_input_format("h264");
  if ((err = avformat_open_input(&ifmt_ctx, VIDEO_TMP_FILE, ifmt, 0)) < 0) {
    cout << "Failed to open input file for remuxing" << endl;
    __end(ifmt_ctx, ofmt_ctx);
    return;
  }

  // For raw H.264, we need to set up stream info manually
  // Try to find stream info but don't fail if it can't determine everything
  avformat_find_stream_info(ifmt_ctx, 0);

  if ((err = avformat_alloc_output_context2(&ofmt_ctx, nullptr, nullptr, filename.c_str())) < 0) {
    cout << "Failed to allocate output context" << endl;
    __end(ifmt_ctx, ofmt_ctx);
    return;
  }

  AVStream *outVideoStream = avformat_new_stream(ofmt_ctx, nullptr);
  if (!outVideoStream) {
    cout << "Failed to allocate output video stream" << endl;
    __end(ifmt_ctx, ofmt_ctx);
    return;
  }

  // Set up codec parameters manually since we know the dimensions
  outVideoStream->time_base = (AVRational){ 1, fps };
  outVideoStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
  outVideoStream->codecpar->codec_id = AV_CODEC_ID_H264;
  outVideoStream->codecpar->width = videoWidth;
  outVideoStream->codecpar->height = videoHeight;
  outVideoStream->codecpar->format = AV_PIX_FMT_YUV420P;
  outVideoStream->codecpar->codec_tag = 0;

  // Copy extradata if available from input stream
  if (ifmt_ctx->nb_streams > 0 && ifmt_ctx->streams[0]->codecpar->extradata_size > 0) {
    outVideoStream->codecpar->extradata_size = ifmt_ctx->streams[0]->codecpar->extradata_size;
    outVideoStream->codecpar->extradata = (uint8_t*)av_mallocz(
      outVideoStream->codecpar->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
    memcpy(outVideoStream->codecpar->extradata,
           ifmt_ctx->streams[0]->codecpar->extradata,
           outVideoStream->codecpar->extradata_size);
  }

  if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
    if ((err = avio_open(&ofmt_ctx->pb, filename.c_str(), AVIO_FLAG_WRITE)) < 0) {
      cout << "Failed to open output file" << endl;
      __end(ifmt_ctx, ofmt_ctx);
      return;
    }
  }

  if ((err = avformat_write_header(ofmt_ctx, 0)) < 0) {
    cout << "Failed to write header to output file" << endl;
    __end(ifmt_ctx, ofmt_ctx);
    return;
  }

  AVPacket *videoPkt = av_packet_alloc();
  if (!videoPkt) {
    cout << "Failed to allocate packet for remuxing" << endl;
    __end(ifmt_ctx, ofmt_ctx);
    return;
  }

  // After write_header, time_base may have changed; use the actual muxer time_base
  // Frame duration in the muxer's time_base (1 frame = 1/fps seconds)
  int64_t frameDuration = av_rescale_q(1, (AVRational){1, fps}, outVideoStream->time_base);
  int64_t ts = 0;

  while (av_read_frame(ifmt_ctx, videoPkt) >= 0) {
    videoPkt->stream_index = outVideoStream->index;
    videoPkt->pts = ts;
    videoPkt->dts = ts;
    videoPkt->duration = frameDuration;
    ts += frameDuration;
    videoPkt->pos = -1;

    if ((err = av_interleaved_write_frame(ofmt_ctx, videoPkt)) < 0) {
      cout << "Failed to mux packet" << endl;
      av_packet_unref(videoPkt);
      break;
    }
    av_packet_unref(videoPkt);
  }
  av_packet_free(&videoPkt);

  av_write_trailer(ofmt_ctx);
  __end(ifmt_ctx, ofmt_ctx);
}

void VideoCapture::__end(AVFormatContext *ifmt_ctx, AVFormatContext *ofmt_ctx)
{
  if (ifmt_ctx) {
    avformat_close_input(&ifmt_ctx);
  }
  if (ofmt_ctx && !(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
    avio_closep(&ofmt_ctx->pb);
  }
  if (ofmt_ctx) {
    avformat_free_context(ofmt_ctx);
  }
}
