#pragma once
#include "utils/ViewModule.h"


namespace Idogep
{
	class CurveSceneView;

	class CurveSceneModule : public Module, EmitsCommandRole
	{
	public:
		CurveSceneModule(Ref<Module> parent);

	    ~CurveSceneModule() override;
	    void Initialize() override;

	private:
		Ref<CurveSceneView> m_myView;
	};
}
