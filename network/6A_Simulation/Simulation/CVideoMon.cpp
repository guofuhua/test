#include "CVideoMon.h"
#include <QLibrary>
#include <QMessageBox>
#include <QDebug>

CVideoMon::CVideoMon()
{
    VideoCreate = NULL;
    VideoSetMode = NULL;
    VideoSetModeFour = NULL;
    VideoSetSize = NULL;
    VideoPlay = NULL;
    VideoStop = NULL;
    VideoDelete = NULL;
    GetVersion = NULL;
    qDebug() << loadDll();
}

int CVideoMon::loadDll()
{
    QLibrary mylib("VideoMon.dll");   //声明所用到的dll文件
    int result = 0;
    if (mylib.load())              //判断是否正确加载
    {
        QMessageBox::information(NULL,"OK","DLL load is OK!");
        VideoCreate = (FunVideoCreate)mylib.resolve("VideoCreate");    //援引 add() 函数
        if (VideoCreate)                  //是否成功连接上 add() 函数
        {
            QMessageBox::information(NULL,"OK","Link to Function VideoCreate is OK!");
        } else {
            QMessageBox::information(NULL,"NO","Linke to Function VideoCreate is not OK!!!!");
            result |= 0x1;
        }
        VideoSetMode = (FunVideoSetMode)mylib.resolve("VideoSetMode");    //援引 add() 函数
        if (VideoSetMode)                  //是否成功连接上 add() 函数
        {
            QMessageBox::information(NULL,"OK","Link to Function VideoSetMode is OK!");
        } else {
            QMessageBox::information(NULL,"NO","Linke to Function VideoSetMode is not OK!!!!");
            result |= (0x1 << 1);
        }
        VideoSetModeFour = (FunVideoSetModeFour)mylib.resolve("VideoSetModeFour");    //援引 add() 函数
        if (VideoSetModeFour)                  //是否成功连接上 add() 函数
        {
            QMessageBox::information(NULL,"OK","Link to Function VideoSetModeFour is OK!");
        } else {
            QMessageBox::information(NULL,"NO","Linke to Function VideoSetModeFour is not OK!!!!");
            result |= (0x1 << 2);
        }
        VideoSetSize = (FunVideoSetSize)mylib.resolve("VideoSetSize");    //援引 add() 函数
        if (VideoSetSize)                  //是否成功连接上 add() 函数
        {
            QMessageBox::information(NULL,"OK","Link to Function VideoSetSize is OK!");
        } else {
            QMessageBox::information(NULL,"NO","Linke to Function VideoSetSize is not OK!!!!");
            result |= (0x1 << 3);
        }
        VideoPlay = (FunVideoPlay)mylib.resolve("VideoPlay");    //援引 add() 函数
        if (VideoPlay)                  //是否成功连接上 add() 函数
        {
            QMessageBox::information(NULL,"OK","Link to Function VideoPlay is OK!");
        } else {
            QMessageBox::information(NULL,"NO","Linke to Function VideoPlay is not OK!!!!");
            result |= (0x1 << 4);
        }
        VideoStop = (FunVideoStop)mylib.resolve("VideoStop");    //援引 add() 函数
        if (VideoStop)                  //是否成功连接上 add() 函数
        {
            QMessageBox::information(NULL,"OK","Link to Function VideoStop is OK!");
        } else {
            QMessageBox::information(NULL,"NO","Linke to Function VideoStop is not OK!!!!");
            result |= (0x1 << 5);
        }
        VideoDelete = (FunVideoDelete)mylib.resolve("VideoDelete");    //援引 add() 函数
        if (VideoDelete)                  //是否成功连接上 add() 函数
        {
            QMessageBox::information(NULL,"OK","Link to Function VideoDelete is OK!");
        } else {
            QMessageBox::information(NULL,"NO","Linke to Function VideoDelete is not OK!!!!");
            result |= (0x1 << 6);
        }
        GetVersion = (FunGetVersion)mylib.resolve("GetVersion");    //援引 add() 函数
        if (GetVersion)                  //是否成功连接上 add() 函数
        {
            QMessageBox::information(NULL,"OK","Link to Function GetVersion is OK!");
        } else {
            QMessageBox::information(NULL,"NO","Linke to Function GetVersion is not OK!!!!");
            result |= (0x1 << 7);
        }
    }
    else {
        QMessageBox::information(NULL,"NO","DLL is not loaded!");
        result |= (0x1 << 8);
    }
    return result;  //加载失败则退出28
}

//#include "dll.h"             //引入头文件
//typedef int (*Fun)(int,int); //定义函数指针，以备调用
//int loadLib(int argc,char **argv)
//{
//    QLibrary mylib("VideoMon.dll");   //声明所用到的dll文件
//    int result;
//    if (mylib.load())              //判断是否正确加载
//    {
//        QMessageBox::information(NULL,"OK","DLL load is OK!");
//        Fun open=(Fun)mylib.resolve("add");    //援引 add() 函数
//        if (open)                  //是否成功连接上 add() 函数
//        {
//            QMessageBox::information(NULL,"OK","Link to Function is OK!");
//            result=open(5,6);      //这里函数指针调用dll中的 add() 函数
//            qDebug()<<result;
//        }
//        else
//            QMessageBox::information(NULL,"NO","Linke to Function is not OK!!!!");
//    }
//    else
//        QMessageBox::information(NULL,"NO","DLL is not loaded!");
//        return 0;  //加载失败则退出28
//}
