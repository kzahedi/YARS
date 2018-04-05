#include "VideoCodecs.h"


#include <iostream>
using namespace std;
#ifdef USE_CAPTURE_VIDEO
// #include <lqt/lqt.h>
void VideoCodecs::printAvailableVideoCodecs()
{
  // lqt_registry_init();
  // lqt_codec_info_t **codecs = lqt_query_registry(0,1,1,0);
  // int i = 0;
  // while(codecs[i] != NULL)
  // {
    // cout << "************************************************************" << endl;
    // lqt_dump_codec_info(codecs[i]);
    // cout << "************************************************************" << endl;
    // i++;
  // }
  // i = 0;
  // while(codecs[i] != NULL)
  // {
    // cout << "Codec identifier " << i << ": " << codecs[i]->name << " [" << codecs[i]->long_name << "]" << endl;
    // for(int j = 0; j < codecs[i]->num_fourccs; j++)
    // {
      // cout << "   Name " << j << ": " << codecs[i]->fourccs[j] << endl;
    // }
    // i++;
  // }
  // lqt_destroy_codec_info(codecs);
  // cout << ">> found " << i << " codecs for video encoding" << endl;
}
#else
void VideoCodecs::printAvailableVideoCodecs()
{
  cout << "YARS was compiled without video capturing capabilities." << endl;
}
#endif
