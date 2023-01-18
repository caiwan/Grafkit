#pragma once

class CXDrawingDevice;

class CXScene {
	friend class CXDrawingDevice;
public:
	CXScene();
	virtual ~CXScene();

	virtual void InitScene(CXDrawingDevice* parent) = 0;
	virtual void DrawScene() = 0;

	virtual void OnResize(int w, int h);

	virtual void OnPanX(float d);
	virtual void OnPanY(float d);

	virtual void OnRotateX(float d);
	virtual void OnRotateY(float d);

	virtual void OnZoom(float d);

protected:
	struct {
		float x, y;
	}  m_fvPos, m_fvRot;

	float	 m_fZoom;
};

class CXDrawingDevice : public CWnd
{
public:
	CXDrawingDevice();
	~CXDrawingDevice();

	// Theese function has to be implemented by the driver
	virtual void CXCreate(CRect rect, CWnd *parent) = 0;		///< called by onPreCtreate window of view
	virtual void CXInit() = 0;									///< called by onCreate of self
	virtual void CXRender() = 0;								///< called by oinTimer oif self
	virtual void CXDestroy() = 0;								///< called by on destroy

	void SetScene(CXScene* scene) { this->m_actualScene = scene;  }
	CXScene* GetScene() { this->m_actualScene;  }

	afx_msg void OnDraw(CDC* pDC);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nHitTest, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

protected:
	// These function has to be implemented by the drivers 

	virtual void CXResizeRenderSurface(int cx, int cy) = 0;
	virtual void CXApplyViewport(int cx, int cy) = 0; 
	
	virtual void BeginScene() = 0;
	virtual void EndScene() = 0;
	virtual void DrawEmptyScene() = 0;
		
protected:
	CWnd  *m_hWnd;
	HDC   m_hdc;
	HGLRC m_hrc;
	int   m_nPixelFormat;

	CXScene* m_actualScene;

	struct {
		float x, y;
	} m_crsrLast;

	// ez nem tudom mire kell, de biztos van valami haszna
	CRect m_rect;
	CRect m_oldWindow;
	CRect m_originalRect;

	int m_bIsMaximized;

	// if self inited
	int m_is_inited;

public:
	/// @todo timert el kell fedni valahogyan
	UINT_PTR m_unpTimer;

};

