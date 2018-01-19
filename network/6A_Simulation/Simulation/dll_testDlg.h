// dll_testDlg.h : header file
//

#if !defined(AFX_DLL_TESTDLG_H__D925EFA1_5083_47EE_9831_6FC7BE364558__INCLUDED_)
#define AFX_DLL_TESTDLG_H__D925EFA1_5083_47EE_9831_6FC7BE364558__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDll_testDlg dialog

class CDll_testDlg : public CDialog
{
// Construction
private:
	BOOL b_lastState;

public:
	CDll_testDlg(CWnd* pParent = NULL);	// standard constructor
	HANDLE	m_DisplayHandle;
	CRect	ClientRect, Old_Rect;
	BOOL	b_run;
// Dialog Data
	//{{AFX_DATA(CDll_testDlg)
	enum { IDD = IDD_DLL_TEST_DIALOG };
	CButton	m_but_small;
	CButton	m_but_large;
	CButton	m_but_stop;
	CButton	m_but_play;
	CButton	m_but_4_chn;
	CButton	m_but_1_chn;
	CStatic	m_pictureCtrl;
	CString	m_server_ip;
	int		m_server_port;
	int		m_chn_1;
	int		m_chn_2;
	int		m_chn_3;
	int		m_chn_4;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDll_testDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDll_testDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnButSet4();
	afx_msg void OnButSet1();
	afx_msg void OnButStop();
	afx_msg void OnButLarge();
	afx_msg void OnButSmall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLL_TESTDLG_H__D925EFA1_5083_47EE_9831_6FC7BE364558__INCLUDED_)
