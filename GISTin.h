// GISTin.h : main header file for the GISTIN application
//

#if !defined(AFX_GISTIN_H__2D42AE3E_B1E1_46A5_A3DD_0702F921F7A1__INCLUDED_)
#define AFX_GISTIN_H__2D42AE3E_B1E1_46A5_A3DD_0702F921F7A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CGISTinApp:
// See GISTin.cpp for the implementation of this class
//

class CGISTinApp : public CWinApp
{
public:
	CGISTinApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGISTinApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CGISTinApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GISTIN_H__2D42AE3E_B1E1_46A5_A3DD_0702F921F7A1__INCLUDED_)
