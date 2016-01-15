/** 
	@file TextureGenerator.cpp : Defines the class behaviors for the application.
*/

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "shadereditor.h"
#include "MainFrm.h"

//#include "Logger.h"

#include "ShaderEditorDoc.h"
#include "ShaderEditorView.h"

#include <Scintilla.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//using namespace FWcore;
//using FWutils::PackageFile;

// CTextureGeneratorApp

BEGIN_MESSAGE_MAP(CTextureGeneratorApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CTextureGeneratorApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CTextureGeneratorApp construction

CTextureGeneratorApp::CTextureGeneratorApp() 
	//Framework(),
	//BaseApplication(),
	//
	//m_logger(),
	//m_pkg_file("./")
{
	m_bHiColorIcons = TRUE;

	SetAppID(_T("GrafKit.Idogep.TextureGenerator.AppID.NoVersion01")); //@todo nevet es verziot generalni
}

CTextureGeneratorApp::~CTextureGeneratorApp()
{
}

// The one and only CTextureGeneratorApp object
CTextureGeneratorApp theApp;

#define SCINTILLA_STATIC_LINK

#ifndef SCINTILLA_STATIC_LINK
HMODULE CScintillaDemoApp::LoadLibraryFromApplicationDirectory(LPCTSTR lpFileName)
{
  //Get the Application diretory
  TCHAR szFullPath[_MAX_PATH];
  szFullPath[0] = _T('\0');
  if (GetModuleFileName(NULL, szFullPath, _countof(szFullPath)) == 0)
	return NULL;

  //Form the new path
  TCHAR szDrive[_MAX_DRIVE];
  szDrive[0] = _T('\0');
  TCHAR szDir[_MAX_DIR];
  szDir[0] = _T('\0');
  _tsplitpath_s(szFullPath, szDrive, sizeof(szDrive)/sizeof(TCHAR), szDir, sizeof(szDir)/sizeof(TCHAR), NULL, 0, NULL, 0);
   TCHAR szFname[_MAX_FNAME];
   szFname[0] = _T('\0');
   TCHAR szExt[_MAX_EXT];
   szExt[0] = _T('\0');
  _tsplitpath_s(lpFileName, NULL, 0, NULL, 0, szFname, sizeof(szFname)/sizeof(TCHAR), szExt, sizeof(szExt)/sizeof(TCHAR));
  _tmakepath_s(szFullPath, sizeof(szFullPath)/sizeof(TCHAR), szDrive, szDir, szFname, szExt);

  //Delegate to LoadLibrary    
  return LoadLibrary(szFullPath);
}
#else
#endif


// CTextureGeneratorApp initialization

BOOL CTextureGeneratorApp::InitInstance()
{

#ifndef SCINTILLA_STATIC_LINK
  m_hSciDLL = LoadLibraryFromApplicationDirectory(_T("SciLexer.dll"));
  if (m_hSciDLL == NULL)
  { 
	AfxMessageBox(_T("Scintilla DLL is not installed, Please download the SciTE editor and copy the SciLexer.dll into this application's directory"));
	return FALSE;
  }
#else
#ifdef SCI_LEXER
	// Scintilla_LinkLexers();
#endif
	Scintilla_RegisterClasses(this->m_hInstance);
#endif


	CWinAppEx::InitInstance();
	
	EnableTaskbarInteraction(FALSE);

	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CShaderEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CShaderEditorView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}

// CTextureGeneratorApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CTextureGeneratorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CTextureGeneratorApp customization load/save methods

void CTextureGeneratorApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CTextureGeneratorApp::LoadCustomState()
{
}

void CTextureGeneratorApp::SaveCustomState()
{
}

// CTextureGeneratorApp message handlers


//// ---------------------------------------------------
//// frmework + logger + assetman' overrides
//// ---------------------------------------------------
//int CTextureGeneratorApp::execute(){
//	return -1;
//}
//
//void CTextureGeneratorApp::terminate(int errorcode){
//	///@todo megbaszni fasszal szarazon
//}
//
//Logger& CTextureGeneratorApp::log(){
//	return this->m_logger;
//}
//
//ASSETManager& CTextureGeneratorApp::ASSETManager(){
//	return this->m_pkg_file;
//}
//
//int CTextureGeneratorApp::peekMessage(){
//	return -1;
//}
//
//void CTextureGeneratorApp::swapBuffers(){
//}
//
//// ---------------------------------------------------
//// Base app overrides
//// ---------------------------------------------------
//#define APPHAXX_BASE_CLASS CTextureGeneratorApp
//
//#include "..\GrafKitEdit\common\application_hax.cpp.inc"


