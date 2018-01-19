#ifndef CVIDEOMON_H
#define CVIDEOMON_H
#include <afxwin.h>
#include <afxres.h>

typedef HANDLE (*FunVideoCreate)(CWnd*,CRect,CString); //定义函数指针，以备调用
typedef BOOL (*FunVideoSetMode)(HANDLE,int); //定义函数指针，以备调用
typedef BOOL (*FunVideoSetModeFour)(HANDLE,int,int,int,int); //定义函数指针，以备调用
typedef BOOL (*FunVideoSetSize)(HANDLE,CRect); //定义函数指针，以备调用
typedef BOOL (*FunVideoPlay)(HANDLE ); //定义函数指针，以备调用
typedef BOOL (*FunVideoStop)(HANDLE); //定义函数指针，以备调用
typedef BOOL (*FunVideoDelete)(HANDLE); //定义函数指针，以备调用
typedef char *(*FunGetVersion)(HANDLE); //定义函数指针，以备调用

class CVideoMon
{
public:
    CVideoMon();
    int loadDll();
    FunVideoCreate VideoCreate;
    FunVideoSetMode VideoSetMode;
    FunVideoSetModeFour VideoSetModeFour;
    FunVideoSetSize VideoSetSize;
    FunVideoPlay VideoPlay;
    FunVideoStop VideoStop;
    FunVideoDelete VideoDelete;
    FunGetVersion GetVersion;
};

#endif // CVIDEOMON_H
