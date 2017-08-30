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

Grafkit::Scene::Scene() :
	m_root(nullptr),
	m_tStart(0.0f),
	m_tEnd(10.0f),
	m_materialCurrentLayer(0)
{
}


Grafkit::Scene::~Scene()
{
}

void Grafkit::Scene::Initialize(ActorRef root)
{
	m_root = root;
	
	std::stack<int> parentIdStack;
	parentIdStack.push(-1);

	std::stack<ActorRef> stack;
	stack.push(root);

	while (!stack.empty()) {
		ActorRef node = stack.top(); stack.pop();
		int id = (int)m_nodes.size();
		int parentId = parentIdStack.top(); parentIdStack.pop();

		// <yield>
		m_nodes.push_back(node);

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

			m_entities.insert(*entity);
		}
		// </yield>

		for (auto it = node->GetChildren().begin(); it != node->GetChildren().end(); it++) {
			parentIdStack.push(id);
			stack.push(*it);
		}
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
	m_entities.clear();
	
	m_root = nullptr;

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
		m_root->AddChild(camera);
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

void Grafkit::Scene::GetAnimations(ActorRef & actor, std::vector<AnimationRef> animations)
{
	animations.clear();
	for (auto it = m_animations.begin(); it != m_animations.end(); it++) {
		ActorAnimation *actorAnim = dynamic_cast<ActorAnimation*>(it->Get());
		if (actorAnim && actorAnim->GetActor().Get() == actor.Get()) {
			animations.push_back(actorAnim);
		}
	}
}

void Grafkit::Scene::BuildScene(Grafkit::Renderer & deviceContext, ShaderResRef vs, ShaderResRef ps)
{
	if(vs.Valid()) 
		m_vertexShader = vs;

	if (ps.Valid()) 
		m_pixelShader = ps;

	//AddMaterialLayer(0, ps);

	for (auto it = m_entities.begin(); it != m_entities.end(); ++it) {
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
			m_animations[i]->Update(m_tAnim);
	}

	// update matrices here 

	PrerenderNode(render, m_root);

	// --- lights
	for (auto it = m_lightNodes.begin(); it != m_lightNodes.end(); it++) {
		if (it->Valid() && !it->Get()->GetEntities().empty() && it->Get()->GetEntities()[0].Valid()) {
			Light* light = dynamic_cast<Light *>(it->Get()->GetEntities()[0].Get());
			if (light) {
				Matrix wm = it->Get()->WorldMatrix();
				light->Calculate(render, this, wm);
			}
		}
	}

	// --- kamera
	ActorRef &cameraActor = GetActiveCamera();
	if (cameraActor.Valid() && (!cameraActor->GetEntities().empty() && cameraActor->GetEntities()[0].Valid())) {
		// itt csak az elso entitast vesszuk figyelembe
		Camera * camera = dynamic_cast<Camera *>(cameraActor->GetEntities()[0].Get());
		if (camera) {
			camera->Calculate(render);

			m_cameraProjectionMatrix = camera->ProjectionMatrix();

			Matrix matv = camera->GetViewMatrix();
			Matrix matw = cameraActor->WorldMatrix();
			
			m_cameraViewMatrix = matv;
			m_cameraViewMatrix.Multiply(matw);
			m_cameraViewMatrix.Invert();
		}
		else {
			throw new EX(NullPointerException);
		}
	}
	else {
		throw new EX_DETAILS(NullPointerException, "Camera actor nem jo, vagy Nem seteltel be a nodeba kamerat");
	}
	
	// to get the matrices before the scenegraph renders
	m_worldMatrices.worldMatrix = XMMatrixTranspose(m_currentWorldMatrix.Get());

	m_worldMatrices.viewMatrix = XMMatrixTranspose(m_cameraViewMatrix.Get());
	m_worldMatrices.projectionMatrix = XMMatrixTranspose(m_cameraProjectionMatrix.Get());
}

void Grafkit::Scene::Render(Grafkit::Renderer & render)
{
	//for (auto it = m_materialShaderMap.begin(); it != m_materialShaderMap.end(); it++) {
		//m_pixelShader = it->second;
		//RenderLayer(render, it->first);
	//}
	RenderLayer(render, 0);
}

void Grafkit::Scene::RenderLayer(Grafkit::Renderer & render, UINT layer)
{
	m_materialCurrentLayer = layer;
	m_currentWorldMatrix.Identity();

	//ez itt elviekben jo kell, hogy legyen
	(*m_vertexShader)->Bind(render);
	(*m_pixelShader)->Bind(render);

	// add lights
	for (auto it = m_lightNodes.begin(); it != m_lightNodes.end(); it++) {
		(*it)->Render(render, this);
	}

	// render scenegraph
	for (auto node = m_nodes.begin(); node != m_nodes.end(); node++) {
		if (node->Valid()) {
			if (!(*node)->IsHidden()) {
				m_worldMatrices.worldMatrix = XMMatrixTranspose((*node)->WorldMatrix().Get());
				(*m_vertexShader)->SetParam(render, "MatrixBuffer", &m_worldMatrices);
				(*m_pixelShader)->SetParam(render, "MatrixBuffer", &m_worldMatrices);
				(*node)->Render(render, this);
			}
		}
	}

	(*m_vertexShader)->Unbind(render);
	(*m_pixelShader)->Unbind(render);
}

void Grafkit::Scene::PrerenderNode(Grafkit::Renderer & render, Actor * actor, int maxdepth)
{
	if (maxdepth < 0) return;
	if (!actor) return;

	Push();

	m_currentWorldMatrix.Multiply(actor->Matrix());
	m_currentWorldMatrix.Multiply(actor->Transform());
	actor->WorldMatrix(m_currentWorldMatrix);
	
	for (size_t i = 0; i < actor->m_pChildren.size(); i++) {
		PrerenderNode(render, actor->m_pChildren[i].Get(), maxdepth - 1);
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

void Grafkit::Scene::serialize(Archive & ar)
{
	// a tobbit a loader vegzi majd 
	PERSIST_FIELD(ar, m_tStart);
	PERSIST_FIELD(ar, m_tEnd);
}
