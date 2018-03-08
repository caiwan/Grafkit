#pragma once 

#include <qobject.h>

#include "render/renderer.h"
#include "render/animation.h"


namespace Grafkit {
	class IResourceManager;
}

namespace Idogep {

	class EditorDocument;
	class CurveDocument;

	class EditorDocument : public QObject
	{
		Q_OBJECT
	public:
		EditorDocument();

		Ref<Grafkit::Animation::FloatTrack> track() { return m_testAnimation; }

		//void beforePreload(Grafkit::IResourceManager* const & resman);
		//void afterPreload(Grafkit::Renderer & renderer);

		void setDirty() { m_isDirty = true; }
		bool dirty() { return m_isDirty; }

	private:

		bool m_isDirty;

		Ref<Grafkit::Animation::FloatTrack> m_testAnimation;

	};

}
