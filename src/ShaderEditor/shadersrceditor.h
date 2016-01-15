#pragma once

#include <string>

#include "ScintillaCtrl.h"
#include "ShaderSrcDoc.h"
#include "EditorDoc.h"

// ===================================================================================================================

/** 
	Shader editor toolbar 
*/

class CShaderEditorToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CShaderEditorMenuButton;

/**
	Shader editor stub
*/
class CShaderEditor : public CWnd
{
	DECLARE_DYNAMIC(CShaderEditor)
	public:
		CShaderEditor();
		virtual ~CShaderEditor();

		BOOL RegisterWindowClass();
		BOOL Create(int n, CWnd* pParentWnd, const RECT& rect, UINT nID, DWORD dwStyle = WS_VISIBLE);

		void AdjustLayout();
		void OnChangeVisualStyle();

		inline Scintilla::CScintillaCtrl& GetCtrl(){return this->m_wndTextEditor;}

		void docFromEditor(CString &dst);
		void docToEditor(CString &src);

	protected:
		CWnd *hWnd;
		
		typedef CShaderEditorToolBar	ToolBar_t;
		typedef CShaderEditorMenuButton MenuButton_t;

		Scintilla::CScintillaCtrl m_wndTextEditor;

		ToolBar_t m_wndToolBar;
		CImageList m_ClassViewImages;

		UINT m_nCurrSort;
		
		void SetupStyle();
		void SetAStyle(int style, COLORREF fore, COLORREF back = RGB(0xff, 0xff, 0xff), int size = -1, const char* face = NULL);
		void DefineMarker(int marker, int markerType, COLORREF fore, COLORREF back);

	// Overrides
	public:
		virtual BOOL PreTranslateMessage(MSG* pMsg);

	protected:
		DECLARE_MESSAGE_MAP()

		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
		afx_msg void OnPaint();
		afx_msg void OnSetFocus(CWnd* pOldWnd);
};


/**
	Egyetlen darab shader editort tartalmaz, egy darab dokkolhato lapon
*/
class CShaderEditorSingle : public CDockablePane{
	DECLARE_DYNAMIC(CShaderEditorSingle)
	public:
		CShaderEditorSingle();
		virtual ~CShaderEditorSingle();

		inline void setDocument(CShaderSrcDoc *pdoc) {ASSERT(pdoc); this->m_pDocument = pdoc;}
		inline CShaderSrcDoc *&getDocument() {ASSERT(m_pDocument); return this->m_pDocument;}

		void docFromEditor();
		void docToEditor();	

		inline Scintilla::CScintillaCtrl& GetActiveCtrl(){return this->m_wndEditor.GetCtrl();}

	protected:

		CShaderEditor	m_wndEditor;
		CShaderSrcDoc *m_pDocument;

	private:
		HACCEL m_accelerators;

	public:
		DECLARE_MESSAGE_MAP()
	
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
		afx_msg void OnSetFocus(CWnd* pOldWnd);
		afx_msg void OnSize(UINT nType, int cx, int cy);

		BOOL PreTranslateMessage(MSG* pMsg);

		void OnEditCopy();
		void OnEditPaste();
		void OnEditCut();
		void OnEditUndo();
		void OnEditRedo();
		void OnEditSelectAll();

		void OnUpdateNeedSelect(CCmdUI* pCmdUI);
		void OnUpdateNeedPaste(CCmdUI* pCmdUI);

		void OnUpdateEditUndo(CCmdUI* pCmdUI);
		void OnUpdateEditRedo(CCmdUI* pCmdUI);
};