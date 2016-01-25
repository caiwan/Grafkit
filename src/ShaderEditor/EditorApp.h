/**
	@file TextureGenerator.h : main header file for the TextureGenerator application

*/
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "MainFrm.h"

/**
	CEditorApp:
	@See TextureGenerator.cpp for the implementation of this class
*/
class CEditorApp : public CWinAppEx 
{
	public:
		CEditorApp();
		~CEditorApp();

		// --- framework overides

	public:
		CMainFrame* GetMainFrame(){ASSERT(m_pMainWnd); return (CMainFrame*)(GetMainWnd());}

	// Overrides
	public:
		virtual BOOL InitInstance();

	// Implementation
		UINT  m_nAppLook;
		BOOL  m_bHiColorIcons;

		virtual void PreLoadState();
		virtual void LoadCustomState();
		virtual void SaveCustomState();

		afx_msg void OnAppAbout();
		DECLARE_MESSAGE_MAP()
};

extern CEditorApp theApp;
