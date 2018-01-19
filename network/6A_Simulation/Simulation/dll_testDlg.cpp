// dll_testDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dll_test.h"
#include "dll_testDlg.h"

#include "VideoMon.h"
#include "LibMediaReader.h"

#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#pragma comment(lib,"..//Debug//VideoMon.lib")
#pragma comment(lib,"..//Release//VideoMon.lib") 
#pragma comment(lib,"..//Release//MediaReader.lib") 

/////////////////////////////////////////////////////////////////////////////

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDll_testDlg dialog

CDll_testDlg::CDll_testDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDll_testDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDll_testDlg)
	m_server_ip = _T("192.168.0.100");
	m_server_port = 9996;
	m_chn_1 = 1;
	m_chn_2 = 2;
	m_chn_3 = 3;
	m_chn_4 = 4;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDll_testDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDll_testDlg)
	DDX_Control(pDX, IDC_BUT_SMALL, m_but_small);
	DDX_Control(pDX, IDC_BUT_LARGE, m_but_large);
	DDX_Control(pDX, IDC_BUT_STOP, m_but_stop);
	DDX_Control(pDX, IDC_BUTTON1, m_but_play);
	DDX_Control(pDX, IDC_BUT_SET_4, m_but_4_chn);
	DDX_Control(pDX, IDC_BUT_SET_1, m_but_1_chn);
	DDX_Control(pDX, IDC_PICTURE, m_pictureCtrl);
	DDX_Text(pDX, IDC_EDIT_IP, m_server_ip);
	DDX_Text(pDX, IDC_EDIT_PORT, m_server_port);
	DDX_Text(pDX, IDC_EDIT_CH_1, m_chn_1);
	DDX_Text(pDX, IDC_EDIT_CH_2, m_chn_2);
	DDX_Text(pDX, IDC_EDIT_CH_3, m_chn_3);
	DDX_Text(pDX, IDC_EDIT_CH_4, m_chn_4);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDll_testDlg, CDialog)
	//{{AFX_MSG_MAP(CDll_testDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUT_SET_4, OnButSet4)
	ON_BN_CLICKED(IDC_BUT_SET_1, OnButSet1)
	ON_BN_CLICKED(IDC_BUT_STOP, OnButStop)
	ON_BN_CLICKED(IDC_BUT_LARGE, OnButLarge)
	ON_BN_CLICKED(IDC_BUT_SMALL, OnButSmall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDll_testDlg message handlers

BOOL CDll_testDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	///////////////////////////////////////////////////////////////////////
	CString str_ip;
	str_ip = "192.168.60.25";
	m_server_ip = str_ip;
	m_server_port = 7000;

	m_DisplayHandle = NULL;
	CWnd* p_topWnd = GetDlgItem(IDC_PICTURE/*IDC_ST_TEXT*/);
	CRect rect;
	p_topWnd->GetClientRect(&rect);
	m_DisplayHandle = VideoCreate(p_topWnd, rect, str_ip);   // 更改IP，端口号

	// 默认单通道
	VideoSetMode(m_DisplayHandle, m_chn_1);

	if(!m_DisplayHandle) 
	{
		AfxMessageBox("不能创建对象");
		return FALSE;
	}
	///////////////////////////////////////////////////////////////////////
	m_pictureCtrl.GetClientRect(&ClientRect);
	Old_Rect = ClientRect;
	m_but_stop.EnableWindow(FALSE);
	m_but_play.EnableWindow(TRUE);
	m_but_large.EnableWindow(FALSE);
	m_but_small.EnableWindow(FALSE);
	b_run = FALSE;

	// 显示版本号
	CString strText = "DVR测试程序——动态库版本：";
	strText += GetVersion(m_DisplayHandle);				// 获取动态库的版本号
	SetWindowText(strText);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDll_testDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDll_testDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDll_testDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDll_testDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	BOOL ret = FALSE;
	if(m_DisplayHandle)
	{
		b_run = TRUE;
		ret = VideoPlay(m_DisplayHandle);
	//std::cout << ret << std::endl;
		if(!ret) 
		{
			AfxMessageBox("不能播放");
			return;
		}
		m_but_stop.EnableWindow(TRUE);
		m_but_play.EnableWindow(FALSE);
	}
}

void CDll_testDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	CDialog::OnCancel();
}

void CDll_testDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	BOOL ret = FALSE;

	if(m_DisplayHandle)
	{
		m_but_stop.EnableWindow(FALSE);
		m_but_play.EnableWindow(TRUE);

		ret = VideoStop(m_DisplayHandle);
		if(!ret) 
		{
			AfxMessageBox("不能停止播放");
			return;
		}

		ret = VideoDelete(m_DisplayHandle);
		if(!ret) 
		{
			AfxMessageBox("不能删除对象");
			return;
		}
	}
}

void CDll_testDlg::OnButSet4() 
{
	std::cout << "click OnButSet4!" << std::endl;
	// TODO: Add your control notification handler code here
	BOOL ret = FALSE;

	if(m_DisplayHandle)
	{
		UpdateData(TRUE);

		m_but_large.EnableWindow(TRUE);
		m_but_small.EnableWindow(TRUE);
		m_but_stop.EnableWindow(FALSE);
		m_but_play.EnableWindow(TRUE);

		ret = VideoSetMode(m_DisplayHandle, m_chn_1, m_chn_2, m_chn_3, m_chn_4);
		if(!ret) 
		{
			AfxMessageBox("不能设置4窗口模式");
			return;
		}

		ret = VideoSetSize(m_DisplayHandle, ClientRect);
		if(!ret) 
		{
			AfxMessageBox("不能设置显示窗口大小");
			return;
		}
	}
}

void CDll_testDlg::OnButSet1() 
{
	std::cout << "click OnButSet1!" << std::endl;

	/*HINSTANCE m_hDll = LoadLibrary(_T("MediaReader.dll"));
	if (NULL == m_hDll)
	{
		MessageBox(_T("加载 MediaReader.dll 失败"));
	}
	typedef int (*hpDllFun)();
	hpDllFun pShowDlg = (hpDllFun)GetProcAddress(m_hDll, "mreader_init");
	if (NULL==pShowDlg)
	{
		MessageBox(_T("DLL中函数寻找失败"));
		return;
	}
	mreader_init();*/

	// TODO: Add your control notification handler code here
	BOOL ret = FALSE;

	if(m_DisplayHandle)
	{
		UpdateData(TRUE);
		m_but_large.EnableWindow(TRUE);
		m_but_small.EnableWindow(TRUE);
		m_but_stop.EnableWindow(FALSE);
		m_but_play.EnableWindow(TRUE);

		ret = VideoSetMode(m_DisplayHandle, m_chn_1);
		if(!ret) 
		{
			AfxMessageBox("不能设置1窗口模式");
			return;
		}
		
		ret = VideoSetSize(m_DisplayHandle, ClientRect);
		if(!ret) 
		{
			AfxMessageBox("不能设置视频显示窗口大小");
			return;
		}
	}
}

void CDll_testDlg::OnButStop() 
{
	// TODO: Add your control notification handler code here
	BOOL ret = FALSE;

	if(m_DisplayHandle)
	{
		ret = VideoStop(m_DisplayHandle);
		if(!ret) 
		{
			AfxMessageBox("不能停止播放");
			return;
		}
	}

	b_run = FALSE;
	m_but_large.EnableWindow(FALSE);
	m_but_small.EnableWindow(FALSE);
	m_but_stop.EnableWindow(FALSE);
	m_but_play.EnableWindow(TRUE);
}

void CDll_testDlg::OnButLarge() 
{
	// TODO: Add your control notification handler code here
	BOOL ret = FALSE;

	//if(b_run)
	if(1)
	{
		if(Old_Rect.Height() < ClientRect.Height() && Old_Rect.Width() < ClientRect.Width())
		{
			Old_Rect.bottom += 20;
			Old_Rect.right += 20;
		}
		
		m_pictureCtrl.MoveWindow(ClientRect.left, ClientRect.top, Old_Rect.Width(), Old_Rect.Height());
		
		if(m_DisplayHandle)
		{
			ret = VideoSetSize(m_DisplayHandle, Old_Rect);
			if(!ret) 
			{
				AfxMessageBox("不能设置视频窗口大小");
				return;
			}
		}
	}
}

void CDll_testDlg::OnButSmall() 
{
	// TODO: Add your control notification handler code here
	BOOL ret = FALSE;

	//if(b_run)
	if(1)
	{
		if(Old_Rect.Height() > 120 && Old_Rect.Width() > 60)
		{
			Old_Rect.bottom -= 20;
			Old_Rect.right -= 20;
		}
		
		m_pictureCtrl.MoveWindow(ClientRect.left, ClientRect.top, Old_Rect.Width(), Old_Rect.Height());
		
		if(m_DisplayHandle)
		{
			ret = VideoSetSize(m_DisplayHandle, Old_Rect);
			if(!ret) 
			{
				AfxMessageBox("不能设置播放窗口大小");
				return;
			}
		}
	}	
}
