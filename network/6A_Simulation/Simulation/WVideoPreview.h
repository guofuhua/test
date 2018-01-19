#if !defined(WVIDEOPREVIEW_H)
#define WVIDEOPREVIEW_H

#pragma once
#include<string>

using namespace std;
HANDLE WVideoCreate(HWND parent,RECT rect,LPCTSTR ip, int len);
BOOL WVideoDelete(HANDLE handle);
BOOL WVideoPlay(HANDLE handle);
BOOL WVideoStop(HANDLE handle);
BOOL WVideoSetModeFour(HANDLE handle,int chn1,int chn2,int chn3,int chn4);
BOOL WVideoSetMode(HANDLE handle,int chn);
BOOL WVideoSetSize(HANDLE handle, RECT rect);
char * WGetVersion(HANDLE handle);


#endif // !defined(WVIDEOPREVIEW_H)