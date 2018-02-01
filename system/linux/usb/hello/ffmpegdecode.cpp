
#include "ffmpegdecode.h"


/************************************************************************
**函数：open_rtsp
**功能：从RTSP获取音视频流数据
**参数：
[in]  rtsp - RTSP地址
**返回：无
************************************************************************/
void StreamProcess(void *param)
{
    if (NULL == param) {
        printf("invalid param!\n");
        return;
    }
    TAVInfo *arg = (TAVInfo *)param;
	AVFormatContext	*pFormatCtx;
    unsigned int i;
    int videoindex, audioindex;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	AVFrame	*pFrame;
    AVPacket *packet;
	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();

    if (avformat_open_input(&pFormatCtx, arg->rtsp, NULL, NULL) != 0){
		printf("Couldn't open input stream.\n");
		return ;
	}
    printf(" open input stream:%s.\n", arg->rtsp);
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0){
		printf("Couldn't find stream information.\n");
		return ;
	}
	videoindex = -1;
    audioindex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            videoindex = i;
            if (-1 != audioindex) {
                break;
            }
        } else if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioindex = i;
            if (-1 != videoindex) {
                break;
            }
        }
    }
	if (videoindex == -1){
		printf("Didn't find a video stream.\n");
		return ;
	}
    if (audioindex == -1){
        printf("Didn't find a audio stream.\n");
//        return ;
    }

	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL){
		printf("Codec not found.\n");
		return ;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0){
		printf("Could not open codec.\n");
		return ;
	}
	pFrame = av_frame_alloc();
	packet = (AVPacket *)av_malloc(sizeof(AVPacket));
    while (av_read_frame(pFormatCtx, packet) >= 0 && !arg->thread_exit)
	{
        if ((packet->stream_index == videoindex) || (packet->stream_index == audioindex))
        {
//            framedeal(pobj, packet);
            arg->callback(param, packet, 0);
		}	
//        printf("got_picture:%d, stream index:%d, video index:%d, audio index:%d\n", got_picture, packet->stream_index, videoindex, audioindex);
		av_free_packet(packet);
        av_frame_unref(pFrame);
    }
    arg->callback(param, packet, 1);

	av_frame_unref(pFrame);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);
	return;
}



void StreamPacketCallback(void *param, AVPacket *packet, int flag)
{
    if (NULL == param) {
        printf("invalid param!\n");
        return;
    }
    TAVInfo *arg = (TAVInfo *)param;

    if (flag) {
        if (arg->file){
            fflush(arg->file);
            if (ferror(arg->file)){
                printf("saveStreamFile[%s]: Err: Unknown!***\n", arg->save_path);
            }
            fclose(arg->file);
            arg->file = NULL;
            arg->save_count++;
//            arg->thread_exit = 1;
        }
        return;
    }

    if (NULL == arg->file) {
        struct tm *t;
        time_t tt;
        memset(arg->localtime, 0, 32);
        time(&tt);
        t=localtime(&tt);
        sprintf(arg->localtime, "%d-%d-%d-%d-%d-%d.h264", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
        memset(arg->save_path, 0, SAVE_PATH_LEN);
        strcpy(arg->save_path, arg->save);
        strcat(arg->save_path, arg->localtime);

        arg->file = fopen(arg->save_path, "wb");
        if (arg->file) {
            clock_gettime(CLOCK_MONOTONIC, &arg->startTime);
            printf("saveStreamFile  Open! %s\n", arg->save_path);
        } else {
            printf("saveStreamFile: Err: Open! %s\n", arg->save_path);
        }
    }

    if (arg->file){
        fwrite(packet->data, 1, packet->size, arg->file);

        clock_gettime(CLOCK_MONOTONIC, &arg->nowTime);
        if (arg->nowTime.tv_sec > arg->startTime.tv_sec + arg->duration_sec) {
            fflush(arg->file);
            if (ferror(arg->file)){
                printf("saveStreamFile[%s]: Err: Unknown!***\n", arg->save_path);
            }
            fclose(arg->file);
            arg->file = NULL;
            arg->save_count++;

            arg->thread_exit = 1;
        }
    }
//    printf("stream index:%d\n", packet->stream_index);
    return ;
}
