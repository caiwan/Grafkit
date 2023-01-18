#pragma once

#include "utils/ViewModule.h"

#include "utils/Command.h"

namespace  Idogep
{
	class AnimationEditorView;
	class CurveEditor;

	class TreeItem;

	// ========================================================================================================


	class AnimationEditorModule : public Controller, public EmitsCommandRole
	{
	public:
		explicit AnimationEditorModule(const Ref<Controller>& parent);
		~AnimationEditorModule() override;

		void Initialize() override;

		Event<float*&, size_t&, size_t&, size_t&> onRequestWaveform; // ptr, samplecount, channelcount, samplePerSec

		void AnimationSelectedEvent(TreeItem * const item);

	private:
		Ref<AnimationEditorView> m_myView;
		Ref<CurveEditor> m_curveScene;
        
	};

}
