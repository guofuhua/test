// dll_test.h : main header file for the DLL_TEST application
//

#if !defined(AFX_DLL_TEST_H__4C97E348_B5B0_4083_BD3B_F983EEA33D79__INCLUDED_)
#define AFX_DLL_TEST_H__4C97E348_B5B0_4083_BD3B_F983EEA33D79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDll_testApp:
// See dll_test.cpp for the implementation of this class
//

class CDll_testApp : public CWinApp
{
public:
	CDll_testApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDll_testApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDll_testApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLL_TEST_H__4C97E348_B5B0_4083_BD3B_F983EEA33D79__INCLUDED_)
