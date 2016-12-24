#include "stdafx.h"

#include "Scene.h"
#include "Actor.h"
#include "model.h"
#include "camera.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

Grafkit::Scene::Scene():
	// Object(),
	m_pScenegraph(nullptr)
{
}


Grafkit::Scene::~Scene()
{
}

void Grafkit::Scene::Initialize(ActorRef root)
{
	m_pScenegraph = root;
	
	/// @Todo itt lehetne cachelni a scenegraphot, jol.

	std::stack<ActorRef> stack;
	stack.push(root);

	while (!stack.empty()) {
		ActorRef node = stack.top(); stack.pop();
		
		// yield node
		m_nodeMap[node->GetName()] = node;
		
		// collect material for models
		for (auto entity = node->GetEntities().begin(); entity != node->GetEntities().end(); entity++) {
			const Model * model = dynamic_cast<Model*>((*entity).Get());
			if (model) {
				MaterialRef material = model->GetMaterial();
				if (material.Valid())
					m_materialMap[material->GetName()] = material;
			}
		}

		// push next
		for(auto it = node->GetChildren().begin(); it != node->GetChildren().end(); it++)
			stack.push(*it);
	}
	
//#ifndef LIVE_RELEASE
//	_m_init++;
//#endif
}

void Grafkit::Scene::Shutdown()
{
	/// TODO: ... 

//#ifndef LIVE_RELEASE
//	_m_init--;
//#endif
}

void Grafkit::Scene::AddCameraNode(ActorRef camera)
{
	m_cameraNodes.push_back(camera);
	m_cameraMap[camera->GetName()] = camera;
	SetActiveCamera();
}

void Grafkit::Scene::SetActiveCamera(std::string name)
{
	auto it = m_cameraMap.find(name);
	if (it != m_cameraMap.end())
		m_activeCamera = it->second;
}

ActorRef Grafkit::Scene::GetCamera(std::string name)
{
	auto it = m_cameraMap.find(name);
	if (it != m_cameraMap.end())
		return it->second;
	return ActorRef();
}

void Grafkit::Scene::AddLightNode(ActorRef light)
{
	m_lightNodes.push_back(light);
	m_lightMap[light->GetName()] = light;
}

ActorRef Grafkit::Scene::GetLight(std::string name)
{
	auto it = m_lightMap.find(name);
	if (it != m_lightMap.end())
		return it->second;
	return ActorRef();
}

MaterialRef Grafkit::Scene::GetMaterial(std::string name)
{
	auto it = m_materialMap.find(name);
	if (it != m_materialMap.end())
		return it->second;
	return MaterialRef();
}

void Grafkit::Scene::AddAnimation(AnimationRef anim)
{
	m_animations.push_back(anim);
}

/******************************************************************************
 * RENDER
 *****************************************************************************/

void Grafkit::Scene::PreRender(Grafkit::Renderer & render)
{
	// --- animation

	for (int i = 0; i < m_animations.size(); i++) {
		if (m_animations[i].Valid())
			m_animations[i]->Update(m_animation_time);
	}

	// --- kamera
	ActorRef &cameraActor = GetActiveCamera();
	if (cameraActor.Valid() && (!cameraActor->GetEntities().empty() && cameraActor->GetEntities()[0].Valid())) {
		// itt csak az elso entitast vesszuk figyelembe
		Camera * camera = dynamic_cast<Camera *>(cameraActor->GetEntities()[0].Get());
		if (camera) {
			camera->Calculate(render);
			m_cameraProjectionMatrix = camera->GetPerspectiveMatrix();

			Matrix mc = camera->GetViewMatrix();
			Matrix ms = CalcNodeTransformTree(cameraActor);

			m_cameraViewMatrix = mc;
			m_cameraViewMatrix.Multiply(ms);
			m_cameraViewMatrix.Invert();
		}
		else {
			throw EX(NullPointerException);
		}
	}
	else {
		throw EX_DETAILS(NullPointerException, "Camera actor nem jo, vagy Nem seteltel be a nodeba kamerat");
	}
	
	// --- fenyek
	// ugyanezt a fenyekre
	for (size_t i = 0; i < m_lightNodes.size(); i++) {
		// ... 
	}

	return;

	// minden nodeot prerendererel, ha kell;
	// ... 
}

void Grafkit::Scene::Render(Grafkit::Renderer & render)
{
	m_currentWorldMatrix.Identity();

	// ezt a semat ki kell baszni innen 
	struct {
		matrix worldMatrix;
		matrix viewMatrix;
		matrix projectionMatrix;
	} viewMatrices;

	// CameraRef &camera = GetActiveCamera();
	// camera->Calculate(render);

	viewMatrices.worldMatrix = XMMatrixTranspose(m_currentWorldMatrix.Get());

	viewMatrices.viewMatrix = XMMatrixTranspose(m_cameraViewMatrix.Get());
	viewMatrices.projectionMatrix = XMMatrixTranspose(m_cameraProjectionMatrix.Get());

	m_vertexShader->GetParam("MatrixBuffer").SetP(&viewMatrices);

	//ez itt elviekben jo kell, hogy legyen
	m_vertexShader->Render(render);
	m_fragmentShader->Render(render);

	// render scenegraph
	RenderNode(render, m_pScenegraph);
}

void Grafkit::Scene::RenderNode(Grafkit::Renderer & render, Actor * actor, int maxdepth)
{
	if (maxdepth < 0) return;
	if (!actor) return;

	Push();

	m_currentWorldMatrix.Multiply(actor->Matrix());
	m_currentWorldMatrix.Multiply(actor->Transform());
	matrix worldMatrix = XMMatrixTranspose(m_currentWorldMatrix.Get());
	m_vertexShader->GetParam("MatrixBuffer").Get("worldMatrix").SetP(&worldMatrix);

	if (!actor->IsHidden())
		actor->Render(render, this);
	
	if (!actor->IsChildrenHidden())
		for (size_t i = 0; i < actor->m_pChildren.size(); i++) {
			RenderNode(render, actor->m_pChildren[i].Get(), maxdepth - 1);
		}
	
	Pop();
}

void Grafkit::Scene::Push()
{
	this->m_worldMatrixStack.push(m_currentWorldMatrix);
}

void Grafkit::Scene::Pop()
{
	m_currentWorldMatrix = this->m_worldMatrixStack.top();
	this->m_worldMatrixStack.pop();
}

Grafkit::Matrix Grafkit::Scene::CalcNodeTransformTree(ActorRef & actor)
{
	ActorRef & node = actor;

	Matrix result;

	do {
		Matrix matTM = actor->Matrix();
		matTM.Multiply(actor->Transform());

		Matrix mat = matTM;
		mat.Multiply(result);

		result = mat;

		node = node->GetParent();
	} while (node.Valid());

	return result;
}
