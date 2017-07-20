#pragma once

#include <vector>
#include <stack>

#include "../utils/reference.h"
#include "../utils/resource.h"

#include "../math/matrix.h"

#include "renderer.h"
#include "predefs.h"

#include "../utils/persistence/persistence.h"

namespace Grafkit {

	class Scene;

	class Actor;

	class Entity3D : public Grafkit::IResource
	{
	protected:
		void _serialize(Archive& ar);

	friend class Actor;
	public:
		Entity3D();
		virtual ~Entity3D();

		virtual void Render(Grafkit::Renderer& deviceContext, Scene * const & scene) = 0;
		virtual void Build(Grafkit::Renderer& deviceContext, Scene * const & scene) = 0;
		
	};

	class Actor;
	typedef Ref<Actor> ActorRef;

	/**
	An actor node - ez a scenegraph es a nodeja
	*/
	__declspec(align(16)) 
	class Actor : public AlignedNew<Actor>, public Grafkit::IResource, public Persistent
	{
	friend class Scene;
	public:
		Actor();
		Actor(Ref<Entity3D> entity);
		~Actor();

		// kisse kulonos megoldas ... 
		void Matrix(const Grafkit::Matrix &mat) { m_viewMatrix = mat; }
		void Transform(const Grafkit::Matrix &mat) { m_transformMatrix = mat; }

		Grafkit::Matrix& Matrix() { return m_viewMatrix; }
		Grafkit::Matrix& Transform() { return m_transformMatrix; }

		virtual void Render(Grafkit::Renderer &render, Scene* scene);

		/// @todo igazi ListTree-t hasznaljon, ha lehet, es majd mukodik
		void AddChild(Actor* child);
		Ref<Actor> GetParent() { return m_pParent; }

		std::vector<Ref<Actor>>& GetChildren() { return this->m_pChildren; }

		void AddEntity(Ref<Entity3D> entity) { m_pEntities.push_back(entity); }

		std::vector<Ref<Entity3D>>& GetEntities() { return m_pEntities; }

		/** Elrejti az akutalis nodeot, a gyerkeket meghagyja, a rendert atlepi */
		void Hide() { m_is_nodeHidden = 1; }

		/** a renderban latszik az akualis node */
		void Show() { m_is_nodeHidden = 0; }

		/** Elrejti az akutalis node gyerekeit, a render atlepi a fat */
		void HideChildren() { m_is_childrenHidden = 1; }

		/** latszanak a node gyerekei */
		void ShowChildren() { m_is_childrenHidden = 0; }

		int IsHidden() { return m_is_nodeHidden; }
		int IsChildrenHidden() { return m_is_childrenHidden; }

	protected:
		void WorldMatrix(const Grafkit::Matrix &mat) { m_worldMatrix = mat; }
		Grafkit::Matrix & WorldMatrix() { return m_worldMatrix; }

		Grafkit::Matrix m_viewMatrix;			///< Node transyformacioja
		Grafkit::Matrix m_transformMatrix;		///< Kulon transzformacio a node tetejen (hogy ne legyen szukseg az eredeti matrixra)
		Grafkit::Matrix m_worldMatrix;		///< Szarmaztatott matrix

		BYTE m_is_nodeHidden;
		BYTE m_is_childrenHidden;

		Ref<Actor> m_pParent;
		std::vector<Ref<Actor>> m_pChildren;
		std::vector<Ref<Entity3D>> m_pEntities;

	protected:
		virtual void serialize(Archive& ar);
		PERSISTENT_DECL(Grafkit::Actor, 1);
	};

}

