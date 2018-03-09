#pragma once 

#include "common.h"

#include "render/renderer.h"
#include "render/animation.h"

namespace Grafkit {
	class IResourceManager;
}

namespace Idogep {

	class Editor;
	class Document;
	class CurveDocument;

	class Document
	{
		friend class Editor;
	public:
		Document();
		virtual ~Document();

		Ref<Grafkit::Animation::FloatTrack> track() { return m_testAnimation; }

		void Preload(Grafkit::IResourceManager * const & resman);
		void Initialize(Grafkit::Renderer &render);
		void Shutdown();

		void SetDirty() { m_isDirty = true; }
		bool IsDirty() { return m_isDirty; }

	private:

		bool m_isDirty;

		Ref<Grafkit::Animation::FloatTrack> m_testAnimation;

		Grafkit::ShaderResRef m_vs, m_ps;
		Grafkit::SceneResRef m_scenegraph;

		Grafkit::ActorRef m_rootActor;
		Grafkit::ActorRef m_cameraActor;

	};

}
