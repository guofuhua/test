/*************************

*main.c文件

*************************/

#include "bmp.h"

int main()
{
    int devfb, filefb;
    struct fb_var_screeninfo scrinfo;
    unsigned long screensize;
    char *fbp ;
    char bmpname[20] = {0};

    //打开设备文件
    devfb = open("/dev/fb0", O_RDWR);
    if(!devfb)
    {
        printf("devfb open error!\r\n");
        return -1;
    }
    //printf("devfb open OK! %d\r\n", devfb);



    //获取屏幕信息

    //若屏幕显示区域大小不合适，可用ioctl(devfb, FBIOPUT_VSCREENINFO, &scrinfo)设置
    if(ioctl(devfb, FBIOGET_VSCREENINFO, &scrinfo))
    {
        printf("get screen infomation error!\r\n");
        return -1;
    }

    //printf(".xres=%d, .yres=%d, .bit=%d\r\n",scrinfo.xres, scrinfo.yres, scrinfo.bits_per_pixel);

    //printf(".xres_virtual=%d, .yres_virtual=%d\r\n",scrinfo.xres_virtual, scrinfo.yres_virtual);

    if(32 != scrinfo.bits_per_pixel)
    {
        printf("screen infomation.bits error!\r\n");
        return -1;
    }



    //计算需要的映射内存大小
    screensize = scrinfo.xres_virtual * scrinfo.yres_virtual * scrinfo.bits_per_pixel / 8;
    //printf("screensize=%lu!\r\n", screensize);

    //内存映射
    fbp = (char *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, devfb, 0);
    if(-1 == (int)fbp)
    {
        printf("mmap error!\r\n");
        return -1;
    }

    scanf("%s", bmpname);

    //显示图片
    show_photo(fbp, &scrinfo, bmpname);



    //取消映射，关闭文件
    munmap(fbp, screensize);
    close(devfb);

    return 0;
}
