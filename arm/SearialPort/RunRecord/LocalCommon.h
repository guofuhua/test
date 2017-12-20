#ifndef LOCALCOMMON_H
#define LOCALCOMMON_H

#define RECEIVE_BUFFER_LENGTH   (2048)
#define SEND_BUFFER_LENGTH      (650)
#define READ_DIR_FLAG   (0xFFF0)
#define READ_FILE_FLAG  (0xFFF1)
#define READ_ERR_FLAG   (0xFF55)
#define READ_INTERRUPT_FLAG (0xFF56)
#define SERIAL_FLAG_LEN (2)
#define READ_FILE_HEAD_LEN  (15)

#define READ_DIR_STATE   (1)
#define SIWEI_DIR_INFO_LEN  (16)

#define SERIAL_RW_GPIO  (164)
#define SERIAL_READ_ENABLE  (1)
#define SERIAL_WRITE_ENABLE (0)
#define REFRESH_DIR_INTERVAL    (60000)

/*串口当前通信状态*/
enum EM_SerialStatus
{
    SERIAL_IDLE/*空闲*/,
    SERIAL_READ_DIR/*读取目录中*/,
    SERIAL_READ_FILE/*读取文件中*/,
    SERIAL_READ_SYNC_FILE/*读取新增文件内容中*/
};

enum EM_SerialReadStatus
{
    IDLE/*空闲*/,
    READ_INTERRUPT,
    WAIT_FOR_READ,
    SERIAL_READING
};

#endif // LOCALCOMMON_H
