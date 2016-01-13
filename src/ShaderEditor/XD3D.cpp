#include "stdafx.h"
#include "XD3D.h"


CXD3D::CXD3D() : CXDrawingDevice(), FWrender::Renderer()
{
}


CXD3D::~CXD3D()
{
	// nothing to do here
}

void CXD3D::CXInit()
{
	this->Initialize(
		this->m_originalRect.Width(), this->m_originalRect.Height(),
		false, this->m_hWnd, false
	);
}

void CXD3D::CXRender()
{
	if (!this->m_is_inited)
		return;

	this->BeginScene();

	if (this->m_actualScene)
		this->m_actualScene->DrawScene(this);
	else
		DrawEmptyScene();

	this->EndScene();
}

void CXD3D::CXDestroy()
{
	this->Shutdown();
}

void CXD3D::CXResizeRenderSurface(int cx, int cy)
{
	if (this->m_deviceContext) {
		this->SetSurface(cx, cy);
		this->SetViewport(cx, cy);
	}
}


void CXD3D::DrawEmptyScene()
{
	// do nothing
}
