/**
	@file TextureGenerator.h : main header file for the TextureGenerator application

*/
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
//#include "framework.h"
//#include "framework_win32.h"
//#include "utilites/io/package.h"

//#include "..\GrafKitEdit\common\application.h"

//#include "Logger.h"

//class CMainFrame;
#include "MainFrm.h"

/**
	CEditorApp:
	@See TextureGenerator.cpp for the implementation of this class
*/
class CEditorApp : public CWinAppEx // public FWcore::Framework, public BaseApplication
{
	public:
		CEditorApp();
		~CEditorApp();

		// --- framework overides

	public:
		//FWutils::PackageFile m_pkg_file;
		//CLogger m_logger;

		CMainFrame* getMainFrame(){ASSERT(m_pMainWnd); return (CMainFrame*)(GetMainWnd());}

	private:
		//virtual int execute();
		//virtual void terminate(int errorcode);
	public:
		//virtual FWcore::Logger& log();
		//virtual FWcore::ASSETManager& ASSETManager();
			
	private:
		//virtual int peekMessage();
		//virtual void swapBuffers();

	// Overrides
	public:
		virtual BOOL InitInstance();

		//#include "..\GrafKitEdit\common\application_hax.h.inc"

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
