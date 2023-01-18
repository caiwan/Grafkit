
// ShaderEditorView.h : interface of the CEditorView class
//

#pragma once

//#include "CEditorDoc.h"
#include "XD3D.h"

class CEditorDoc;

class CEditorView : public CView, public CXScene
{
protected: // create from serialization only
	CEditorView();
	DECLARE_DYNCREATE(CEditorView)

// Attributes
public:
	CEditorDoc* GetDocument();

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CEditorView();

	virtual void InitScene(CXDrawingDevice* parent);
	virtual void DrawScene(CXDrawingDevice* parent);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// fields
	FWrender::ShaderRef m_shader_vertex;
	FWrender::ModelRef m_fullscreen_quad;

	CXD3D m_xd3d_view;

// Generated message map functions
protected:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnShaderCompile();
};

#ifndef _DEBUG  // debug version in ShaderEditorView.cpp
inline CShaderEditorDoc* CEditorView::GetDocument() const
   { return reinterpret_cast<CShaderEditorDoc*>(m_pDocument); }
#endif

