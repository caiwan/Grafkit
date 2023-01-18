
// ShaderEditorView.h : interface of the CShaderEditorView class
//

#pragma once


class CShaderEditorView : public CView
{
protected: // create from serialization only
	CShaderEditorView();
	DECLARE_DYNCREATE(CShaderEditorView)

// Attributes
public:
	CShaderEditorDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CShaderEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ShaderEditorView.cpp
inline CShaderEditorDoc* CShaderEditorView::GetDocument() const
   { return reinterpret_cast<CShaderEditorDoc*>(m_pDocument); }
#endif

