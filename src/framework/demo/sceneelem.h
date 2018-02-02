#pragma once

#include "utils/reference.h"
#include "render/renderer.h"
#include "valtracker/valtracker.h"

namespace GKDemo {

	class DemoApplication;

	class SceneElem : public Referencable
	{
		friend class DemoApplication;
	public:

		SceneElem() 
			//: isActive(1) 
		{
		}

		virtual ~SceneElem() {
		}

	public:
		virtual void OnBeforePreload(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context, Grafkit::IResourceManager * const & resman) = 0;
		virtual void OnAfterPreload(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) = 0;
		//virtual void OnDelegateTracks(Ref<Grafkit::ValueTracker> &tracker, GKDemo::DemoApplication *const& context) {}
		//virtual void OnSetupResources(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) {}

		virtual void OnBeforeRender(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) {}
		virtual void OnRender(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) = 0;
		virtual void OnAfterRender(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) {}

		virtual void Shutdown() {}

	//	int IsActive() { return isActive; }
	//	void Enable() { isActive = 1; }
	//	void Disable() { isActive = 0; }

	//protected:
	//	int isActive;
	};

}