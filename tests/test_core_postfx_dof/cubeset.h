#pragma once 

#include "render/shader.h"
#include "render/camera.h"
#include "render/model.h"

#include "demo/sceneelem.h"
#include "demo/demoframework.h"

class CubeScene : public GKDemo::SceneElem {

public:
	CubeScene();
	~CubeScene();

public:
	virtual void OnBeforePreload(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context, Grafkit::IResourceManager * const & resman);
	virtual void OnAfterPreload(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context);
	virtual void OnRender(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context);

private:
	Grafkit::SceneResRef scene;
	
	Grafkit::ShaderResRef vs;
	Grafkit::ShaderResRef fs;

	TextureResRef texture;

	TextureSamplerRef sampler;

	LightRef light;
	ActorRef lightActor;

	ActorRef cameraActor;
	CameraRef camera;
};