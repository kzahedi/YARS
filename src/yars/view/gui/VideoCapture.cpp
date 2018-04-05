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

  fps      = fpsrate;
  filename = fn;

  int err;

  if (!(oformat = av_guess_format(NULL, VIDEO_TMP_FILE, NULL))) {
    cout << "Failed to define output format" << endl;
    exit(-1);
  }

  if ((err = avformat_alloc_output_context2(&ofctx, oformat, NULL, VIDEO_TMP_FILE) < 0)) {
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

  if ((err = avcodec_open2(cctx, codec, NULL)) < 0) {
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

  if ((err = avformat_write_header(ofctx, NULL)) < 0) {
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
    swsCtx = sws_getContext(cctx->width, cctx->height, AV_PIX_FMT_RGB32, cctx->width, cctx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, 0, 0, 0);
  }

  int inLinesize[1] = { 4 * cctx->width };

  // From RGB to YUV
  sws_scale(swsCtx, (const uint8_t * const *)&data, inLinesize, 0, cctx->height, videoFrame->data, videoFrame->linesize);

  videoFrame->pts = frameCounter++;

  if ((err = avcodec_send_frame(cctx, videoFrame)) < 0) {
    cout << "Failed to send frame" << endl;
    exit(-1);
  }

  AVPacket pkt;
  av_init_packet(&pkt);
  pkt.data = NULL;
  pkt.size = 0;

  if (avcodec_receive_packet(cctx, &pkt) == 0) {
    pkt.flags |= AV_PKT_FLAG_KEY;
    av_interleaved_write_frame(ofctx, &pkt);
    av_packet_unref(&pkt);
  }
}

void VideoCapture::finish() {
  //DELAYED FRAMES
  AVPacket pkt;
  av_init_packet(&pkt);
  pkt.data = NULL;
  pkt.size = 0;

  for (;;) {
    avcodec_send_frame(cctx, NULL);
    if (avcodec_receive_packet(cctx, &pkt) == 0) {
      av_interleaved_write_frame(ofctx, &pkt);
      av_packet_unref(&pkt);
    }
    else {
      break;
    }
  }

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
}

void VideoCapture::__remux() {
  AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
  int err;

  if ((err = avformat_open_input(&ifmt_ctx, VIDEO_TMP_FILE, 0, 0)) < 0) {
    cout << "Failed to open input file for remuxing" << endl;
    __end(ifmt_ctx, ofmt_ctx);
  }
  if ((err = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
    cout << "Failed to retrieve input stream information" << endl;
    __end(ifmt_ctx, ofmt_ctx);
  }
  if ((err = avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, filename.c_str()))) {
    cout << "Failed to allocate output context" << endl;
    __end(ifmt_ctx, ofmt_ctx);
  }

  AVStream *inVideoStream = ifmt_ctx->streams[0];
  AVStream *outVideoStream = avformat_new_stream(ofmt_ctx, NULL);
  if (!outVideoStream) {
    cout << "Failed to allocate output video stream" << endl;
    __end(ifmt_ctx, ofmt_ctx);
  }
  outVideoStream->time_base = (AVRational){ 1, fps };
  avcodec_parameters_copy(outVideoStream->codecpar, inVideoStream->codecpar);
  outVideoStream->codecpar->codec_tag = 0;

  if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
    if ((err = avio_open(&ofmt_ctx->pb, filename.c_str(), AVIO_FLAG_WRITE)) < 0) {
      cout << "Failed to open output file" << endl;
      __end(ifmt_ctx, ofmt_ctx);
    }
  }

  if ((err = avformat_write_header(ofmt_ctx, 0)) < 0) {
    cout << "Failed to write header to output file" << endl;
    __end(ifmt_ctx, ofmt_ctx);
  }

  AVPacket videoPkt;
  int ts = 0;
  while (true) {
    if ((err = av_read_frame(ifmt_ctx, &videoPkt)) < 0) {
      break;
    }
    videoPkt.stream_index = outVideoStream->index;
    videoPkt.pts = ts;
    videoPkt.dts = ts;
    videoPkt.duration = av_rescale_q(videoPkt.duration, inVideoStream->time_base, outVideoStream->time_base);
    ts += videoPkt.duration;
    videoPkt.pos = -1;

    if ((err = av_interleaved_write_frame(ofmt_ctx, &videoPkt)) < 0) {
      cout << "Failed to mux packet" << endl;
      av_packet_unref(&videoPkt);
      break;
    }
    av_packet_unref(&videoPkt);
  }

  av_write_trailer(ofmt_ctx);
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
