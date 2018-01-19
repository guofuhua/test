/**********************************************
*  *
**********************************************/

#ifndef __VIDEO_MON_H__
#define __VIDEO_MON_H__

//extern __declspec(dllexport) HANDLE VideoCreate(CWnd* parent,CRect rect,CString ip, int port=7000);
extern __declspec(dllexport) HANDLE VideoCreate(CWnd* parent,CRect rect,CString ip);
//extern __declspec(dllexport) HANDLE VideoCreate(CWnd* parent,CRect rect, LPCTSTR ip, int port = 7000);
extern __declspec(dllexport) BOOL VideoDelete(HANDLE handle);
extern __declspec(dllexport) BOOL VideoPlay(HANDLE handle);
extern __declspec(dllexport) BOOL VideoStop(HANDLE handle);
extern __declspec(dllexport) BOOL VideoSetMode(HANDLE handle,int chn1,int chn2,int chn3,int chn4);
extern __declspec(dllexport) BOOL VideoSetMode(HANDLE handle,int chn);
extern __declspec(dllexport) BOOL VideoSetSize(HANDLE handle, CRect rect);
extern __declspec(dllexport) char * GetVersion(HANDLE handle);

#endif