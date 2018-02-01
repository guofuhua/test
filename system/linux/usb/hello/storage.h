#ifndef STORAGE_H
#define STORAGE_H

#define MAX_CHANNEL (32)
#define MAX_STORAGE_DEVICE_NUM  (3)
#define RTSP_PATH_LEN   (128)
#define SAVE_PATH_LEN   (256)
/* 6A board type */
typedef enum
{
    BOARD_NONE  = 0,
    BOARD_AV1   = 1,
    BOARD_AV2   = 2,
    BOARD_AV3   = 3,
    BOARD_AV4   = 4,
    BOARD_EXP   = 5,
    BOARD_BUTT
}BOARD_TYPE_E;

typedef enum
{
    CHANNEL_SAVE_NONE   = 0,
    CHANNEL_SAVE_MAIN   = 1,
    CHANNEL_SAVE_SUB    = 2,
    CHANNEL_SAVE_ALL    = 3,
    CHANNEL_SAVE_BUTT
}SAVE_MODE_E;

typedef enum
{
    NONE   = 0,
    ACTION_ADD   = 1,
    ACTION_REMOVE    = 2,
    SUBSYSTEM_BLOCK    = 3,
    DEVTYPE_DISK,
    DEVTYPE_PARTITION,
    HOT_PLUG_TYPE_BUTT
}EHOT_PLUG_TYPE;

typedef enum
{
    STORAGE_DEVICE_NONE     = 0,
    STORAGE_DEVICE_INSERT   = 1,
    STORAGE_DEVICE_MOUNTED  = 2,
    STORAGE_DEVICE_REMOVED  = 3,
    STORAGE_DEVICE_BUTT
}STORAGE_DEVICE_STATE_E;

typedef enum
{
    UNKNOW_DEVICE   = 0,
    USB_DEVICE      = 1,
    HDD_DEVICE      = 2,
    BUTT_DEVICE
}STORAGE_DEVICE_TYPE_E;

typedef struct {
    int channel_id;
    BOARD_TYPE_E board;
    SAVE_MODE_E save_mode;
    char mainstream[RTSP_PATH_LEN];
    char substream[RTSP_PATH_LEN];
    char save_main_path[SAVE_PATH_LEN];
    char save_sub_path[SAVE_PATH_LEN];
}CHANNEL_INFO;

typedef struct {
    STORAGE_DEVICE_STATE_E state;
    STORAGE_DEVICE_TYPE_E type;
    char name[12];
    char partition_name[32];
    int partition_count;
    int totalspace;
    int partition_size[12]; /*分区大小，单位KB*/
    char path[SAVE_PATH_LEN];
    int freesize;
    int authorize;
}STORAGE_DEVICE_INFO;

typedef struct {
    int count;
    STORAGE_DEVICE_INFO device[MAX_STORAGE_DEVICE_NUM];
}STORAGE_DEVICE_MANAGE;

typedef struct {
    int channel_count;
    CHANNEL_INFO channel[MAX_CHANNEL];
    STORAGE_DEVICE_MANAGE storage_manage;
}MONITOR_RECORD;



typedef struct {
    EHOT_PLUG_TYPE action;
    EHOT_PLUG_TYPE block;
    EHOT_PLUG_TYPE disk;
    STORAGE_DEVICE_TYPE_E type;
    char name[12];
    int npart;
    int partn;
}THOT_PLUG_MSG;

class storage
{
public:
    storage();
    static void init();
    static void destroy();
};

#endif // STORAGE_H
