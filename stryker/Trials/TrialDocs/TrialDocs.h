
// TrialDocs.h : main header file for the TrialDocs application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CTrialDocsApp:
// See TrialDocs.cpp for the implementation of this class
//

class CTrialDocsApp : public CWinAppEx
{
public:
	CTrialDocsApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CTrialDocsApp theApp;
