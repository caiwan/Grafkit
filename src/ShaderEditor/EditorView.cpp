
// ShaderEditorView.cpp : implementation of the CEditorView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "EditorApp.h"
#endif

#include "EditorDoc.h"
#include "EditorView.h"

#include "render/renderer.h"
#include "render/model.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace {
#define INCLUDE_DEFAULT_VSHADER
#include "defaultShader.cpp.inc"
#undef INCLUDE_DEFAULT_VSHADER
}

#include "builtin_data/cube.h"


// CEditorView

IMPLEMENT_DYNCREATE(CEditorView, CView)

BEGIN_MESSAGE_MAP(CEditorView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_SHADER_COMPILE, &CEditorView::OnShaderCompile)
END_MESSAGE_MAP()

// CEditorView construction/destruction

CEditorView::CEditorView() : CXScene(),
	m_xd3d_view()
{
	m_xd3d_view.SetScene(this);

}

CEditorView::~CEditorView()
{

}


void CEditorView::InitScene(CXDrawingDevice * parent)
{
	if (!dynamic_cast<CXD3D*>(parent))
		return;

	FWrender::Renderer &render = *(dynamic_cast<CXD3D*>(parent));

	this->m_fullscreen_quad = new FWrender::Model();

	this->m_shader_vertex = new FWrender::Shader();
	this->m_shader_vertex->LoadFromMemory(render, "main", defaultVertexShader, 65536, FWrender::ST_Vertex);

	// +++ compile PSHader

	// build model
	FWrender::SimpleMeshGenerator generator(render, m_shader_vertex);
	generator["POSITION"] = FWBuiltInData::quad;
	generator["TEXCOORD"] = FWBuiltInData::quad_texcoord;

	generator(4, 6, FWBuiltInData::quadIndices, this->m_fullscreen_quad);

	this->OnShaderCompile();
}

void CEditorView::DrawScene(CXDrawingDevice * parent)
{
	CEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (!dynamic_cast<CXD3D*>(parent))
		return;

	FWrender::Renderer &render = *(dynamic_cast<CXD3D*>(parent));

	FWrender::ShaderRef pxshader = pDoc->m_shader_src.GetShader();
	if (pxshader.Invalid())
		return;

	this->m_shader_vertex->Render(render);
	pxshader->Render(render);

	this->m_fullscreen_quad->Render(render);

}

BOOL CEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CEditorView drawing

void CEditorView::OnDraw(CDC* pDC)
{
	CEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	this->m_xd3d_view.OnDraw(pDC);

	// TODO: add draw code for native data here
	// ... 
}

void CEditorView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CEditorView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CEditorView diagnostics

#ifdef _DEBUG
void CEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEditorDoc* CEditorView::GetDocument()// non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEditorDoc)));
	return (CEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CEditorView message handlers


void CEditorView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CMainFrame*mainFrm = theApp.GetMainFrame();
	CShaderEditorSingle &shaderEditor = mainFrm->GetShaderEditorWnd();

	CShaderSrcDoc &shaderDoc = this->GetDocument()->m_shader_src;

	shaderEditor.setDocument(&shaderDoc);
	shaderEditor.docToEditor();

	CRect rect; GetClientRect(rect);
	m_xd3d_view.CXCreate(rect, this);
}


void CEditorView::OnSize(UINT nType, int cx, int cy)
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


BOOL CEditorView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	//if ( 1 )
	{
		//::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW)); //https://msdn.microsoft.com/en-us/library/0a4f8y8f.aspx
		return TRUE;
	} // ... 

	return CView::OnSetCursor(pWnd, nHitTest, message);

}


void CEditorView::OnShaderCompile()
{
	CEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	FWrender::Renderer &render = m_xd3d_view;

	theApp.GetMainFrame()->GetShaderEditorWnd().docFromEditor();
	pDoc->m_shader_src.CompileShader(render);
	theApp.GetMainFrame()->GetShaderEditorWnd().SetFocus();

	// fuckup
//	theApp.GetMainFrame()->GetPropWnd().UpdatePropList(&pDoc->m_shader_src);
	pDoc->m_shader_src(theApp.GetMainFrame()->GetPropWnd().m_wndPropList);
}
