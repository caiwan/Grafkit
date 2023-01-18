#pragma once

#include "utils/ViewModule.h"

#include "utils/Command.h"

namespace  Idogep
{
	class CurveEditorView;
	class CurveSceneModule;

	// ========================================================================================================

	class CurveEditorModule : public Module, public EmitsCommandRole
	{
	public:
		explicit CurveEditorModule(Ref<Module> parent);
		~CurveEditorModule() override;

		void Initialize() override;

	private:
		Ref<CurveEditorView> m_myView;
		Ref<CurveSceneModule> m_curveScene;
	};

}
