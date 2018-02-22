#pragma once 

#include "render/animation.h"

namespace Idogep{

	class EditorDocument;
	class CurveDocument;

	class EditorDocument {
	public:
		EditorDocument();

		Ref<Grafkit::Animation::FloatTrack> track() { return m_testAnimation; }
		Idogep::CurveDocument * curveDocument() { return m_testCurveDocument; }

		void setDirty() { m_isDirty = true; }
		bool dirty() { return m_isDirty; }

	private:
		bool m_isDirty;

		Ref<Grafkit::Animation::FloatTrack> m_testAnimation;
		Idogep::CurveDocument* m_testCurveDocument;
	};

}
