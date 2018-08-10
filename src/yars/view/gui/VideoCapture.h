#ifndef __VIDEO_CAPTURE_H__
#define __VIDEO_CAPTURE_H__

// this file was copied from
// https://stackoverflow.com/questions/46444474/c-ffmpeg-create-mp4-file?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa

#include <string.h>
#include <algorithm>
#include <string>

using namespace std;

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/avfft.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/opt.h>
#include <libavutil/common.h>
#include <libavutil/channel_layout.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libavutil/time.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libavutil/file.h>
#include <libswscale/swscale.h>
}


class VideoCapture {
  public:

    VideoCapture() {
      oformat = NULL;
      ofctx = NULL;
      videoStream = NULL;
      videoFrame = NULL;
      swsCtx = NULL;
      frameCounter = 0;

      // Initialize libavcodec
      av_register_all();
      // av_log_set_callback(avlog_cb);
    }

    ~VideoCapture() {
      __free();
    }

    void init(int width, int height, int fpsrate, int bitrate, string filename);

    void addFrame(uint8_t *data);

    void finish();

  private:
    void __free();
    void __remux();
    void __end(AVFormatContext *ifmt_ctx, AVFormatContext *ofmt_ctx);

    AVOutputFormat  *oformat;
    AVFormatContext *ofctx;
    AVStream        *videoStream;
    AVFrame         *videoFrame;
    AVCodec         *codec;
    AVCodecContext  *cctx;
    SwsContext      *swsCtx;
    int             frameCounter;
    int             fps;
    string          filename;

};

// VideoCapture* Init(int width, int height, int fps, int bitrate) {
// VideoCapture *vc = new VideoCapture();
// vc->init(width, height, fps, bitrate);
// return vc;
// };

// void AddFrame(uint8_t *data, VideoCapture *vc) {
// vc->addFrame(data);
// }

// void Finish(VideoCapture *vc) {
// vc->finish();
// }

#endif // __VIDEO_CAPTURE_H__
