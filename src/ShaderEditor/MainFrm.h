
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "ClassView.h"
//#include "Logger.h"

#include "shadersrceditor.h"

#include "PropertiesWnd.h"

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:
	CShaderEditorSingle &getEditor(){return this->m_shaderEditor;}

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

	

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;

	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	
	CClassView        m_wndClassView;
	//CLoggerView       m_wndLogger;
	CPropertiesWnd    m_wndProperties;

	CShaderEditorSingle m_shaderEditor;

	private:
		CList<CBasePane*> m_regCmdMsg;

		void RegCmdMsg(CBasePane* pane);
		void UnregCmdMsg(CBasePane* pane);

	private:
		BOOL CreateDockingWindows();
		void SetDockingWindowIcons(BOOL bHiColorIcons);

		// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
	
public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};


