
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
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// CShaderEditorView construction/destruction

CShaderEditorView::CShaderEditorView() :
	m_xd3d_view()
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

void CShaderEditorView::OnDraw(CDC* pDC)
{
	CShaderEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	this->m_xd3d_view.OnDraw(pDC);

	// TODO: add draw code for native data here
	// ... 
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


void CShaderEditorView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CRect rect; GetClientRect(rect);
	m_xd3d_view.CXCreate(rect, this);

	// should work at this point
	// further init stuff goez here 
}


void CShaderEditorView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	/*
	switch (nType)
	{
		case SIZE_RESTORED:
		{
			if (m_xd3d_view.m_bIsMaximized)
			{
				m_xd3d_view.m_bIsMaximized = false;
			}

			break;
		}

		case SIZE_MAXIMIZED:
		{
			m_xd3d_view.m_bIsMaximized = true;

			break;
		}
	}
	*/

	m_xd3d_view.OnSize(nType, cx, cy);
}


BOOL CShaderEditorView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	//if ( 1 )
	{
		//::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW)); //https://msdn.microsoft.com/en-us/library/0a4f8y8f.aspx
		return TRUE;
	} // ... 

	return CView::OnSetCursor(pWnd, nHitTest, message);

}
