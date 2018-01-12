/*************************

 *bmp.c文件

 *************************/

#include "bmp.h"



/*************************

 *fbp，映射内存起始地址

 *scrinfo，屏幕信息结构体

 *bmpname，.bmp位图文件名

 *************************/
int show_photo(const char *bmpdata, const int bmpsize)
{
    if(NULL == bmpdata || bmpsize < 54)
        return -1;

//    unsigned long tmp = 0;
//    int bits_per_pixel = 32;//屏幕位数
    BitMapFileHeader FileHead;
    BitMapInfoHeader InfoHead;
    const char *pbmpdata = bmpdata;

    //读文件信息
    memcpy(&FileHead, pbmpdata, sizeof(BitMapFileHeader));
    pbmpdata += sizeof(BitMapFileHeader);
    if (memcmp(FileHead.bfType, "BM", 2) != 0)
    {
        printf("it's not a BMP file\n");
        return -1;
    }

    //读位图信息
    memcpy(&InfoHead, pbmpdata, sizeof(BitMapInfoHeader));
    pbmpdata += sizeof(BitMapInfoHeader);

    //跳转至数据区
    pbmpdata = bmpdata + FileHead.bfOffBits;

    // 输出BMP文件的位图文件头的所有信息
    printf("位图文件头主要是对位图文件的一些描述:BMPFileHeader\n\n");
    printf("文件标识符 = %c%c\n", FileHead.bfType[0], FileHead.bfType[1]);
    printf("BMP 文件大小 = %lu 字节\n", FileHead.bfSize);
    printf("保留值1 = %d \n", FileHead.bfReserved1);
    printf("保留值2 = %d \n", FileHead.bfReserved2);
    printf("文件头的最后到图像数据位开始的偏移量 = %lu 字节\n", FileHead.bfOffBits);

    // 输出BMP文件的位图信息头的所有信息
    printf("\n\n位图信息头主要是对位图图像方面信息的描述:BMPInfo\n\n");
    printf("信息头的大小 = %lu 字节\n", InfoHead.biSize);
    printf("位图的高度 = %ld \n", InfoHead.biHeight);
    printf("位图的宽度 = %ld \n", InfoHead.biWidth);
    printf("图像的位面数(位面数是调色板的数量,默认为1个调色板) = %d \n", InfoHead.biPlanes);
    printf("每个像素的位数 = %d 位\n", InfoHead.biBitCount);
    printf("压缩类型 = %lu \n", InfoHead.biCompression);
    printf("图像的大小 = %lu 字节\n", InfoHead.biSizeImage);
    printf("水平分辨率 = %ld \n", InfoHead.biXPelsPerMeter);
    printf("垂直分辨率 = %ld \n", InfoHead.biYPelsPerMeter);
    printf("使用的色彩数 = %lu \n", InfoHead.biClrUsed);
    printf("重要的色彩数 = %lu \n", InfoHead.biClrImportant);

    printf("\n\n\n压缩说明：有0（不压缩），1（RLE 8，8位RLE压缩），2（RLE 4，4位RLE压缩，3（Bitfields，位域存放）");
    fflush(stdout);
    return 0;
}
