#include "stdafx.h"
#include "XDraw.h"

#include "exceptions.h"

using namespace FWdebugExceptions;

/* ================================================================================================================ */
/** CXDrawingDevice */
/* ================================================================================================================ */

CXScene::CXScene()
{
	throw EX(NotImplementedMethodException);
}

CXScene::~CXScene()
{
	throw EX(NotImplementedMethodException);
}

void CXScene::OnResize(int w, int h)
{
	throw EX(NotImplementedMethodException);
}

void CXScene::OnPanY(float d)
{
	throw EX(NotImplementedMethodException);
}

void CXScene::OnRotateX(float d)
{
	throw EX(NotImplementedMethodException);
}

void CXScene::OnRotateY(float d)
{
	throw EX(NotImplementedMethodException);
}

void CXScene::OnZoom(float d)
{
	throw EX(NotImplementedMethodException);
}

void CXScene::OnPanX(float d)
{
	throw EX(NotImplementedMethodException);
}


/* ================================================================================================================ */
/** CXDrawingDevice */
/* ================================================================================================================ */

CXDrawingDevice::CXDrawingDevice() : CWnd(),
	m_actualScene(nullptr),
	m_unpTimer(0),
	m_is_inited(0),
	m_nPixelFormat(0),
	m_hWnd(nullptr),
	m_hdc(nullptr),
	m_hrc(nullptr)
{
	// ... nothing exactly to do here
}

CXDrawingDevice::~CXDrawingDevice()
{
}


BEGIN_MESSAGE_MAP(CXDrawingDevice, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


void CXDrawingDevice::OnDraw(CDC * pDC)
{

	this->BeginScene();
	if (this->m_actualScene)
	{
		this->m_actualScene->DrawScene();
	}
	else
	{
		this->DrawEmptyScene();
	}

	this->EndScene();
}

void CXDrawingDevice::OnPaint()
{
	ValidateRect(NULL);
}


void CXDrawingDevice::OnSize(UINT nType, int cx, int cy)
{
	if (!m_hWnd) return; // Q&D hotfix
	CWnd::OnSize(nType, cx, cy);

	if (0 >= cx || 0 >= cy || nType == SIZE_MINIMIZED) return;

	//FWrender::applyView(cx, cy);
	this->CXResizeRenderSurface(cx, cy);

	// -- ez minek kellett itt anno vajon?
	switch (nType)
	{
		// If window resize token is "maximize"
	case SIZE_MAXIMIZED:
	{
		GetWindowRect(m_rect);
		m_oldWindow = m_rect;

		break;
	}

	// If window resize token is "restore"
	case SIZE_RESTORED:
	{
		// If the window is currently maximized
		if (m_bIsMaximized)
		{
			GetWindowRect(m_rect);
			m_oldWindow = m_rect;
		}

		break;
	}
	}

	this->CXApplyViewport(cx, cy);

	if (this->m_actualScene)
		this->m_actualScene->OnResize(cx, cy);

	MoveWindow(0, 0, cx, cy);
}

void CXDrawingDevice::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == this->m_unpTimer) {
		this->OnDraw(NULL);
	}

	CWnd::OnTimer(nIDEvent);
}


void CXDrawingDevice::OnMouseMove(UINT nHitTest, CPoint point)
{
	struct {
		float x, y;
	} diff;

	diff.x = ((int)(point.x - m_crsrLast.x), diff.y =(int)(point.y - m_crsrLast.y));
	m_crsrLast.x = (float)point.x, m_crsrLast.y = (float)point.y;

	if (this->m_actualScene) {
		// Left mouse button
		if (nHitTest & MK_LBUTTON)
		{
			this->m_actualScene->OnRotateX((float)0.5f * diff.y);
			this->m_actualScene->OnRotateY((float)0.5f * diff.x);
		}

		// Right mouse button
		else if (nHitTest & MK_RBUTTON)
		{
			this->m_actualScene->OnZoom(-(float)0.1f * diff.y);
		}

		// Middle mouse button
		else if (nHitTest & MK_MBUTTON)
		{
			this->m_actualScene->OnPanX(+(float)0.05f * diff.x);
			this->m_actualScene->OnPanY(-(float)0.05f * diff.y);
		}

	}
	OnDraw(NULL);

	CWnd::OnMouseMove(nHitTest, point);
}


int CXDrawingDevice::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1) return -1;
	CXInit();
	this->m_unpTimer = this->SetTimer(1, 1, 0);
	return 0;
}


void CXDrawingDevice::OnDestroy()
{
	CWnd::OnDestroy();
	CXDestroy();
}

