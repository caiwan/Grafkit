#pragma once 

#include "common.h"

#include "render/shader.h"
#include "render/camera.h"
#include "render/model.h"

#include "render/texture.h"
#include "render/light.h"

#include "demo/sceneelem.h"
#include "demo/demoframework.h"

class CubeScene : public GKDemo::SceneElem {

public:
	CubeScene();
	~CubeScene();

	virtual void Shutdown();

protected:
	virtual void OnBeforePreload(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context, Grafkit::IResourceManager * const & resman);
	virtual void OnAfterPreload(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context);

public:
	virtual void OnBeforeRender(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context);
	virtual void OnRender(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context);


private:
	Grafkit::SceneResRef scene;
	
	Grafkit::ShaderResRef vs;
	Grafkit::ShaderResRef fs;

	Grafkit::TextureResRef texture;

	Grafkit::TextureSamplerResRef sampler;

	Grafkit::LightRef light;
	Grafkit::ActorRef lightActor;

	Grafkit::ActorRef cameraActor;
	Grafkit::CameraRef camera;
};