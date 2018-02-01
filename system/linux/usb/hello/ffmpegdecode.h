
#ifndef _FFMPEGDECODE_H_
#define _FFMPEGDECODE_H_
extern "C"
{
#ifdef __cplusplus
 #define __STDC_CONSTANT_MACROS
 #ifdef _STDINT_H
  #undef _STDINT_H
 #endif
 # include <stdint.h>
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"

}
typedef void(*streamcallback)(void *param, AVPacket *packet, int flag);

#include "storage.h"
typedef struct {
    int channel_id;
    char rtsp[RTSP_PATH_LEN];
    char save[SAVE_PATH_LEN];
    streamcallback callback;
    struct timespec startTime;
    struct timespec nowTime;
    char localtime[32];
    char save_path[SAVE_PATH_LEN];
    FILE *file;
    int save_count;
    int thread_exit;
    int duration_sec;
}TAVInfo;

extern void StreamProcess(void *param);
void StreamPacketCallback(void *param, AVPacket *packet, int flag);
#endif
