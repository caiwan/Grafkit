
// ShaderEditorView.cpp : implementation of the CShaderEditorView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "ShaderEditor.h"
#endif

#include "ShaderEditorDoc.h"
#include "ShaderEditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CShaderEditorView

IMPLEMENT_DYNCREATE(CShaderEditorView, CView)

BEGIN_MESSAGE_MAP(CShaderEditorView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CShaderEditorView construction/destruction

CShaderEditorView::CShaderEditorView()
{
	// TODO: add construction code here

}

CShaderEditorView::~CShaderEditorView()
{
}

BOOL CShaderEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CShaderEditorView drawing

void CShaderEditorView::OnDraw(CDC* /*pDC*/)
{
	CShaderEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

void CShaderEditorView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CShaderEditorView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CShaderEditorView diagnostics

#ifdef _DEBUG
void CShaderEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CShaderEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CShaderEditorDoc* CShaderEditorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CShaderEditorDoc)));
	return (CShaderEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CShaderEditorView message handlers
