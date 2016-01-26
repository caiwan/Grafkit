#include "stdafx.h"
#include "TreeView.h"

#include "exceptions.h"
using namespace FWdebugExceptions;

/********************************************************************************/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/********************************************************************************/
// CTreeView

CTreeView::CTreeView() :
	_pCallbackHndlr(NULL)
{
}

CTreeView::CTreeView(ITreeItemHandler* hndlr) :
	_pCallbackHndlr(hndlr)
{
}


CTreeView::~CTreeView()
{
}

BEGIN_MESSAGE_MAP(CTreeView, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CTreeView::OnTvnSelchanged)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CTreeView::OnNmDoubleClick)
	ON_NOTIFY_REFLECT(NM_CLICK, &CTreeView::OnNmClick)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CTreeView::OnNmRightClick)
END_MESSAGE_MAP()

/********************************************************************************/
// CTreeView message handlers

BOOL CTreeView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

NodeIterator* CTreeView::GetSelectedNode()
{
	int indx = (int)GetItemData(GetSelectedItem());
	if (indx)
	{
		TreeItem* item = this->m_Items[indx-1];
		return this->m_Items[indx-1]->m_pNode;
	}
	else
	{
		return NULL;
	}
}

NodeIterator* CTreeView::GetSelectedNode(CPoint &point)
{
	UINT unFlags = 0 ;
	HTREEITEM hItem = HitTest(point, &unFlags) ;
	if((unFlags & TVHT_ONITEMLABEL) && hItem != NULL) {
		int indx = (int)GetItemData(hItem);
		if (indx){
			TreeItem* item = this->m_Items[indx-1];
			return this->m_Items[indx-1]->m_pNode;
		}
	}
	return NULL;
}

///@todo GetKeyState() https://msdn.microsoft.com/en-us/library/windows/desktop/ms646301(v=vs.85).asp

void CTreeView::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	NodeIterator *node = this->GetSelectedNode();
	if(node && _pCallbackHndlr) _pCallbackHndlr->NodeSelectedEvent(node);

	*pResult = 0;
}

void CTreeView::OnNmClick(NMHDR *pNMHDR, LRESULT *pResult){
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	CPoint pt, pt2;
	GetCursorPos(&pt);
	pt2 = pt;
	ScreenToClient(&pt2);

	NodeIterator *node = this->GetSelectedNode(pt2);

	if (_pCallbackHndlr){
		if(node)
			_pCallbackHndlr->NodeClickOnItemEvent(pt, node);
		else
			_pCallbackHndlr->NodeClickEvent(pt);
	}

	*pResult = 0;
}

void CTreeView::OnNmRightClick(NMHDR *pNMHDR, LRESULT *pResult){
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	CPoint pt, pt2;
	GetCursorPos(&pt);
	pt2 = pt;
	ScreenToClient(&pt2);

	NodeIterator *node = this->GetSelectedNode(pt2);

	if (_pCallbackHndlr){
		if(node)
			_pCallbackHndlr->NodeRightClickOnItemEvent(pt, node);
		else
			_pCallbackHndlr->NodeRightClickEvent(pt);
	}

	*pResult = 0;
}

void CTreeView::OnNmDoubleClick(NMHDR *pNMHDR, LRESULT *pResult){
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	CPoint pt, pt2;
	GetCursorPos(&pt);
	pt2 = pt;
	ScreenToClient(&pt2);

	NodeIterator *node = this->GetSelectedNode(pt2);

	if (_pCallbackHndlr){
		if(node)
			_pCallbackHndlr->NodeDoubleClickOnItemEvent(pt, node);
		else
			_pCallbackHndlr->NodeDoubleClickEvent(pt);
	}

	*pResult = 0;
}

/********************************************************************************/
// CTreeView builders

void CTreeView::FillTreeView(TreeBuilder &builder){
	DeleteAllItems();
	builder.parse(this);
}

void CTreeView::InsertItem(TreeItemRef item){
	if (item.Invalid())
		throw EX(NullPointerException);

	if (item->m_parent)
		item->m_hItem = CTreeCtrl::InsertItem(item->m_name, item->m_idIcon, item->m_idIconSelected, item->m_parent->m_hItem);
	else
		item->m_hItem = CTreeCtrl::InsertItem(item->m_name, item->m_idIcon, item->m_idIconSelected, 0);

	// m_Items << item;
	m_Items.push_back(item);

	DWORD_PTR dptr = m_Items.size(); //(DWORD_PTR)item;
	SetItemData(item->m_hItem, dptr);
}


/********************************************************************************/
// TreeItem

TreeItem::TreeItem(NodeIterator* node, TreeItemRef parent) :
	//m_rBuilder(treeBuilder),
	m_pNode(node),
	m_idIcon(0),
	m_idIconSelected(0),

	m_hItem(0),
	m_parent(parent)
{
}

// nothing else to do here

/********************************************************************************/
// TreeBuilder

TreeBuilder::TreeBuilder(TreeItemBuilder &itemBuilder, NodeIterator* root) :
	m_pwndClassView(NULL),
	m_rItemBuilder(itemBuilder),
	m_pRoot(root), m_isSkipRoot(0)
{
}

TreeBuilder::~TreeBuilder(){
}


TreeItemRef TreeBuilder::InsertItem(NodeIterator* node, TreeItemRef &parent){
	if (!this->m_pwndClassView)
		throw EX(NullPointerException);

	TreeItemRef item = m_rItemBuilder.newTreeItem(node, parent);
	this->m_pwndClassView->InsertItem(item);
	return item;
}

void TreeBuilder::parseNode(NodeIterator* node, TreeItemRef parent, int _maxdepth){
//----- <common part>
	if (_maxdepth <= 0) return; // overload
	if (!node) return;	//kaphat nullptr-t

	TreeItemRef treenode;

	if (m_isSkipRoot && node == m_pRoot){
		
		// node == root && root kiertekeles kihagy => ... 

	}else {
		treenode = parent;
		if (node->hasNode()){
			treenode = this->InsertItem(node, parent);
		}

	}
//----- </common part>

	// move along to the children
	this->parseChildren(node, treenode, _maxdepth-1);
}

void TreeBuilder::parse(CTreeView *container)
{
	this->m_pwndClassView = container;
	parseNode(m_pRoot);
}


void TreeBuilder::parse(CTreeView *container, TreeItemRef &parent)
{
	this->m_pwndClassView = container;
	parseNode(m_pRoot, parent);
}


/********************************************************************************/
// TreeBuilderBinary
TreeBuilderBinary::TreeBuilderBinary(TreeItemBuilder &itemBuilder, BinaryTree* root) : 
	TreeBuilder(itemBuilder, root)
{
}

void TreeBuilderBinary::parseChildren(NodeIterator* _node, TreeItemRef &parent, int _maxdepth){
	if (_maxdepth <= 0) return; // overload
	BinaryTree *node = dynamic_cast<BinaryTree*>(_node);

	// olvasas balrol jobbra
	if(node->hasLeft())  this->parseNode(node->getLeftChild(),  parent, _maxdepth-1);
	if(node->hasRight()) this->parseNode(node->getRightChild(), parent, _maxdepth-1);
}

/********************************************************************************/
// TreeBuilderChain

TreeBuilderChain::TreeBuilderChain(TreeItemBuilder &itemBuilder, ChainTree *root)  :
	TreeBuilder(itemBuilder, root)
{
}

void TreeBuilderChain::parseChildren(NodeIterator* _node, TreeItemRef &parent, int _maxdepth){
	if (_maxdepth <= 0) return; // overload
	ChainTree *node = dynamic_cast<ChainTree*>(_node);

	// eloszor a gyereket dolgozzuk fel
	if(node->hasChild())  this->parseNode(node->getChild(),  parent, _maxdepth-1);

	// aztan a tobbi szomszedot
	// feltetelezzuk, hogy balra van igazitva a fa, es balra mar nincs semmi -> jobbra mozgunk folyamatosan
	if(node->hasRight())  this->parseNode(node->getRight(),  parent, _maxdepth-1);
}

/********************************************************************************/
// TreeBuilderList

TreeBuilderList::TreeBuilderList(TreeItemBuilder &itemBuilder, ListTree *root) : 
	TreeBuilder(itemBuilder, root)
{
}

void TreeBuilderList::parseChildren(NodeIterator* _node, TreeItemRef &parent, int _maxdepth){
	ListTree *node = dynamic_cast<ListTree*>(_node);

	if(node->hasChild()){
		for (int i=0; i<node->getChildCount(); i++){
			this->parseNode(node->getChild(i), parent, _maxdepth-1);
		}
	}
}

///@todo directory kivalasztasi lehetoseget is! <- a fa felepitesenel lehesen szurofelteteleket megadni.