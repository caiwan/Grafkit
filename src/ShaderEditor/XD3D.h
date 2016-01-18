#pragma once

#include "XDraw.h"
#include "render/renderer.h"

class CXD3D :
	public CXDrawingDevice,
	public FWrender::Renderer
{
public:
	CXD3D();
	~CXD3D();

	virtual void CXInit();
	virtual void CXRender();
	virtual void CXDestroy();

protected:
	

private:
	virtual void CXResizeRenderSurface(int cx, int cy);
	//virtual void CXApplyViewport(int cx, int cy);

	virtual void DrawEmptyScene();
};

