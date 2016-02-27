#include "Scenegraph.h"
#include "Actor.h"

using FWrender::Actor;
using FWrender::Entity3D;

FWrender::Scenegraph::Scenegraph():
	m_pRoot(nullptr)
{
}


FWrender::Scenegraph::~Scenegraph()
{
	delete m_pRoot;
}


void FWrender::Scenegraph::Render(FWrender::Renderer &render)
{
}


void FWrender::Scenegraph::renderNode(FWrender::Renderer & render, Actor *& p_actor)
{
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
