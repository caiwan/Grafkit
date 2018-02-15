#pragma once 

#include "render/animation.h"

namespace Idogep{

	class EditorDocument {
	public:
		EditorDocument();

		Grafkit::Animation::FloatTrack Animation() {
			return m_testAnimation;
		}

		void SetDirty() { m_isDirty = true; }
		bool IsDirty() { return m_isDirty; }

	private:
		bool m_isDirty;

		Grafkit::Animation::FloatTrack m_testAnimation;
	};

}