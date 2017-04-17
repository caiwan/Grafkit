#include "stdafx.h"

#include "Scene.h"
#include "Actor.h"
#include "model.h"
#include "camera.h"
#include "Light.h"
#include "Material.h"
#include "animation.h"

using namespace Grafkit;
using namespace FWdebugExceptions;


PERSISTENT_IMPL(Grafkit::Scene);

Grafkit::Scene::Scene():
	m_pScenegraph(nullptr)
{
}


Grafkit::Scene::~Scene()
{
}

void Grafkit::Scene::Initialize(ActorRef root)
{
	m_pScenegraph = root;

	std::stack<ActorRef> stack;
	stack.push(root);

	while (!stack.empty()) {
		ActorRef node = stack.top(); stack.pop();
		
		// <yield>
		m_nodeMap[node->GetName()] = node;
		
		// collect material for models
		for (auto entity = node->GetEntities().begin(); entity != node->GetEntities().end(); entity++) {
			
			const Model * model = dynamic_cast<Model*>((*entity).Get());
			if (model) {
				MaterialRef material = model->GetMaterial();
				if (material.Valid())
					m_materialMap[material->GetName()] = material;
			}

			const Light * light = dynamic_cast<Light*>((*entity).Get());
			if (light){
				m_lightNodes.push_back(node);
				m_lightMap[node->GetName()] = node;
				break; // assume if we have only one light uder a node
			}

			const Camera * camera = dynamic_cast<Camera*>((*entity).Get());
			if (camera) {
				//m_cameraNodes.push_back(node);
				//m_cameraMap[node->GetName()] = node;
				AddCamera(node);
				break; // assume if we have only one camera uder a node
			}

			m_Entities.insert(*entity);
		}
		// </yield>

		for(auto it = node->GetChildren().begin(); it != node->GetChildren().end(); it++)
			stack.push(*it);
	}

}

void Grafkit::Scene::Shutdown()
{
	m_cameraMap.clear();
	m_lightMap.clear();
	m_nodeMap.clear();

	m_animations.clear();
	
	m_activeCamera = nullptr;
	m_cameraNodes.clear();
	m_Entities.clear();
	
	m_pScenegraph = nullptr;

	m_pixelShader = nullptr;
	m_vertexShader = nullptr;

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

void Grafkit::Scene::AddCamera(ActorRef camera)
{
	m_cameraNodes.push_back(camera);
	m_cameraMap[camera->GetName()] = camera;
	if (camera->GetParent().Invalid())
		m_pScenegraph->AddChild(camera);
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

void Grafkit::Scene::BuildScene(Grafkit::Renderer & deviceContext, ShaderRef vs, ShaderRef ps)
{
	if(vs.Valid()) 
		m_vertexShader = vs;

	if (ps.Valid()) 
		m_pixelShader = ps;

	for (auto it = m_Entities.begin(); it != m_Entities.end(); ++it) {
		(*it)->Build(deviceContext, this);

	}
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

	m_vertexShader->SetParam(render, "MatrixBuffer", &viewMatrices);

	//ez itt elviekben jo kell, hogy legyen
	m_vertexShader->Bind(render);
	m_pixelShader->Bind(render);

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
	m_vertexShader->SetParamValue(render, "MatrixBuffer", "worldMatrix", &worldMatrix);

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

void Grafkit::Scene::serialize(Archive & ar)
{
	// a tobbit a loder vegzi majd 
}
