/*
 * LibMediaReader.h
 *
 *  Created on: 2016年8月12日
 *      Author: zhengboyuan
 */

#ifndef LIBMEDIAREADER_H_
#define LIBMEDIAREADER_H_


////////////////////////////////////////////////////////////////////////////

#ifdef WIN32

    #ifndef NOMINMAX
    #define NOMINMAX
    #endif //NOMINMAX

	#include <Windows.h>
#else

#endif //WIN32


////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
    typedef signed char     int8_t;
    typedef unsigned char   uint8_t;
    typedef short           int16_t;
    typedef unsigned short  uint16_t;
    typedef int             int32_t;
    typedef unsigned        uint32_t;
    typedef _int64       int64_t;
    typedef unsigned _int64   uint64_t;
#else
    #include <stdint.h>
    typedef void*   HANDLE;
#endif //_MSC_VER


///////////////////////////////////////////////////////////////////
#ifdef WIN32
    #ifndef DLLEXPORT
    #define DLLEXPORT __declspec(dllexport)
    #endif //DLLEXPORT
#else
    #define DLLEXPORT __attribute__ ((visibility ("default")))
#endif //WIN32

///////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C"
{
#endif

/////////////////////////////////////////////////////////////////////////////
#ifndef MKBETAG
#define MKBETAG(a,b,c,d) ((d) | ((c) << 8) | ((b) << 16) | ((unsigned)(a) << 24))
#endif //MKBETAG


#ifndef CASTER_TYPE
#define	CASTER_TYPE

/// 编码
enum MCodec
{
	MCODEC_NONE = 0,

	MCODEC_H264 = 28,
	MCODEC_HEVC = 174, /// H.265
	MCODEC_H265 = MCODEC_HEVC,

	MCODEC_G711U = 65542,
	MCODEC_G711A,

	MCODEC_MP3 = 0x15001,
	MCODEC_AAC = 0x15002,
	MCODEC_AC3 = 0x15003,
	MCODEC_VORBIS = 0x15005,

	MCODEC_RAW = 0x10101010

};

enum MType
{
	MTYPE_NONE = -1,
	MTYPE_VIDEO = 0,
	MTYPE_AUDIO,
	MTYPE_DATA,
};


/// 媒体格式 
struct MFormat
{
	int codec;		/// 视频编码  @see MCodec
	int width;		/// 视频高 
	int height;		/// 视频宽 
	int framerate;		/// 帧率 
	int profile;
	int clockRate;  /// 时钟频率 

	int audioCodec;	/// 音频编码  @see MCodec
	int channels;	/// 通道数 
	int sampleRate;	/// 采样率 
	int audioProfile;	/// 档次 
	int audioRate;      /// 音频时钟频率 

	int vPropSize;		/// 视频解码参数, 对于H.264是sps+pps, 对于H.265是vps+sps+pps
	unsigned char* vProp;

	int configSize;		/// 音频解码参数, 如果是AAC编码, 则必须设置为AAC的config参数 
	unsigned char* config;

    int bitrate;    ///码率. 
    int audioBitrate;
};


/// 媒体包 
struct MPacket
{
	int type;       ///
	uint8_t* data;	/// 数据指针 
	int size;		/// 数据长度 
	int64_t pts;	/// 时间戳 
	int duration;	/// 时长 
	int flags;		/// 标识 
};

#endif //CASTER_TYPE


enum MReaderConst
{
	MAX_READER = 64
};

typedef int		mreader_t;

typedef void (*MReaderEventCallback)(void* reader, int event, int64_t value, void* context);

/////////////////////////////////////////////////////////////////////////////

/**
 * 初始化
 * @return
 */
DLLEXPORT int mreader_init();

/**
 *
 */
DLLEXPORT void mreader_quit();

/**
 * 打开媒体源
 * @param handle 	返回的句柄
 * @param url		媒体源URL
 * @param params	可选参数
 * @return 0 表示成功, 其他值表示错误码
 */
DLLEXPORT int mreader_open(mreader_t* handle, const char* url, const char* params);

/**
 * 关闭媒体源
 * @param handle
 * @return
 */
DLLEXPORT int mreader_close(mreader_t handle);

/**
 * 媒体源是否打开
 * @param handle
 * @return > 0 表示已经打开
 */
DLLEXPORT int mreader_isOpen(mreader_t handle);

/**
 * 获取媒体格式
 * @param handle
 * @param fmt
 * @return
 */
DLLEXPORT int mreader_getFormat(mreader_t handle, MFormat* fmt);

/**
 * 获取媒体时长
 * @param handle	媒体源句柄
 * @param duration	媒体时长, 单位为毫秒
 * @return
 */
DLLEXPORT int mreader_getDuration(mreader_t handle, int* duration);

/**
 * 是否为实时媒体源
 * @param handle
 * @return > 0 表示为实时媒体源
 */
DLLEXPORT int mreader_isLive(mreader_t handle);

/**
 * 是否可以定位
 * @param handle
 * @return > 0 表示可定位
 */
DLLEXPORT int mreader_seekable(mreader_t handle);

/**
 * 播放媒体源
 * @param handle
 * @return
 */
DLLEXPORT int mreader_play(mreader_t handle);

/**
 * 暂停媒体源
 * @param handle
 * @return
 */
DLLEXPORT int mreader_pause(mreader_t handle);

/**
 * 停止媒体源 
 * @param handle
 * @return
 */
DLLEXPORT int mreader_stop(mreader_t handle);

/**
 * 获取媒体源状态 
 * @param handle
 * @return
 */
DLLEXPORT int mreader_getState(mreader_t handle);

/**
 * 读取媒体包 
 * @param handle
 * @param pkt
 * @return
 */
DLLEXPORT int mreader_read(mreader_t handle, MPacket* pkt);

/**
 * 中断读取操作 
 * @param handle
 * @return
 */
DLLEXPORT int mreader_interrupt(mreader_t handle);


/**
 * 定位到指定偏移, 单位为微秒 
 * @param handle
 * @param offset
 * @return
 */
DLLEXPORT int mreader_seek(mreader_t handle, int64_t offset);

/**
 * 获取当前时间戳, 单位为微秒 
 * @param handle
 * @param offset
 * @return
 */
DLLEXPORT int mreader_getTime(mreader_t handle, int64_t* offset);

/**
 * 开始记录为文件 
 * @param handle
 * @param filename	录像文件 
 * @return
 */
DLLEXPORT int mreader_startRecord(mreader_t handle, const char* filename);

/**
 * 停止记录
 * @param handle
 * @return
 */
DLLEXPORT int mreader_stopRecord(mreader_t handle);

/**
 * 是否正在记录
 * @param handle
 * @return > 0 表示正在记录 
 */
DLLEXPORT int mreader_isRecording(mreader_t handle);


/**
 * 设置事件回调 
 * @param handle
 * @param cb
 * @param context
 * @return
 */
DLLEXPORT int mreader_setEventCallback(mreader_t handle, MReaderEventCallback cb, void* context);

/*
 * 启用/禁止日志 
 * @param enabled >0 表示启用 
*/
DLLEXPORT void mreader_enableLog(int enabled);

/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif


#endif /* LIBMEDIAREADER_H_ */
