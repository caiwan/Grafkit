#include "Scenegraph.h"
#include "Actor.h"

using FWrender::Actor;
using FWrender::Entity3D;

FWrender::Scenegraph::Scenegraph():
	m_pRoot(nullptr)
{
	m_pRoot = new ListTree();
	m_pRoot->setObject(this);
}

FWrender::Scenegraph::~Scenegraph()
{
	delete m_pRoot;
}

void FWrender::Scenegraph::Render(FWrender::Renderer &render)
{
	m_render = &render;
	m_pRoot->parse(this);
}

void FWrender::Scenegraph::parseNode(TreeNode * node)
{
	if (!(node && node->getObject()))
		return;
	
	///@todo ezzel a szarral kell kezdeni meg valamit
	//ActorRef actor = (Actor*)node->getObject();
	Actor* actor = reinterpret_cast<Actor*>(node->getObject());
	actor->Render(m_render);

	actor->Render(*(m_render));
}

void FWrender::Scenegraph::push()
{
	m_matrix_stack.push(m_matrix);
}

void FWrender::Scenegraph::pop()
{
	m_matrix = m_matrix_stack.top();
	m_matrix_stack.pop();
}
