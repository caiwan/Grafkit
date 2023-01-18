#include "Scenegraph.h"

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

void FWrender::Scenegraph::parseNode(TreeNode * node)
{
}

void FWrender::Scenegraph::push()
{
}

void FWrender::Scenegraph::pop()
{
}
