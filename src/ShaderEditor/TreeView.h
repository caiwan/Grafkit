#pragma once

// #include <list>
#include <vector>

#include "stdafx.h"
#include "tree.h"

#include "reference.h"

class TreeBuilder;
class TreeItemBuilder;

class TreeItem;

typedef Ref<TreeItem> TreeItemRef;

/********************************************************************************/
// CTreeView window

class ITreeItemHandler{
public:
	virtual void NodeSelectedEvent(NodeIterator* node) = 0;	///< akkor hivodik meg, ha a treeben megvaltozott a kijelolt elem
	
	virtual void NodeClickEvent(CPoint &point){}
	virtual void NodeClickOnItemEvent(CPoint &point, NodeIterator* node){}
		
	virtual void NodeDoubleClickEvent(CPoint &point){}		///< akkor hivodik meg, amikor duplakattintas tortent, de a kattintas helye nem egy elemre esik
	virtual void NodeDoubleClickOnItemEvent(CPoint &point, NodeIterator* node){}	///< akkor hivodik meg, amikor egy duplakattintas egy elemre esett

	virtual void NodeRightClickEvent(CPoint &point){}
	virtual void NodeRightClickOnItemEvent(CPoint &point, NodeIterator* node){}

	///@todo ide jonnek a tovabbi Evt. handler fuggvenyek
};

class CTreeView : public CTreeCtrl
{
	// Construction
	public:
		CTreeView();
		CTreeView(ITreeItemHandler* hndlr);
		virtual ~CTreeView();

		// Implementation
		inline void SetEventHandler(ITreeItemHandler* hndlr){this->_pCallbackHndlr = hndlr;}

		void FillTreeView(TreeBuilder &builder);
		void InsertItem(TreeItemRef m_parent);

		NodeIterator* GetSelectedNode();
		NodeIterator* GetSelectedNode(CPoint &point);

	// Overrides
	protected:
		virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

		afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnNmClick(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnNmRightClick(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnNmDoubleClick(NMHDR *pNMHDR, LRESULT *pResult);

	protected:
		DECLARE_MESSAGE_MAP()

	private:
		std::vector<TreeItemRef> m_Items;
		ITreeItemHandler *_pCallbackHndlr;

};


/********************************************************************************/
// TreeItem object

//class TreeItem;

class TreeItem : virtual public Referencable{
	friend class TreeBuilder;
	friend class TreeItemBuilder;
	friend class CTreeView;

	public:
		TreeItem(NodeIterator* node, TreeItemRef parent);
		virtual ~TreeItem(){}

		inline void setNode(NodeIterator* node){m_pNode = node;}
		inline NodeIterator* getNode(void){return m_pNode;}

	public:
		int m_idIcon;
		int m_idIconSelected;
		CString m_name;

	protected:
		NodeIterator* m_pNode;

		TreeItemRef m_parent;
		HTREEITEM m_hItem;

};

/********************************************************************************/
// TreeItemBuilder
// TreeBuilder 

///@todo ezt az interfacet refractorozni kell Builder-bol Factory-re
class TreeItemBuilder{
	friend class TreeBuilder;
	protected:
		virtual TreeItemRef newTreeItem(NodeIterator* node, TreeItemRef &parent) = 0;
};

class TreeBuilder{
	public:
		TreeBuilder(TreeItemBuilder &itemBuilder, NodeIterator* root);
		virtual ~TreeBuilder();

		void parse(CTreeView *container);
		void parse(CTreeView *container ,TreeItemRef &parent);

		inline void switchForceSkipRoot(int isForce=1){m_isSkipRoot = isForce;}
		// +++ images 
		// +++ things 

	protected:
		virtual void parseNode(NodeIterator* node, TreeItemRef parent = TreeItemRef(), int _maxdepth = TREE_MAXDEPTH);
		virtual void parseChildren(NodeIterator* node, TreeItemRef &parent, int _maxdepth = TREE_MAXDEPTH) = 0;

		TreeItemRef InsertItem(NodeIterator* node, TreeItemRef &m_parent);

		std::vector<TreeItemRef> m_Items;

		// --- 
		TreeItemBuilder &m_rItemBuilder;
		NodeIterator *m_pRoot;
	
		CTreeView *m_pwndClassView;
		//CImageList m_ClassViewImages;
		int m_isSkipRoot;


	private:
		// prevent copy constructor and assign
		TreeBuilder (const TreeBuilder&) = delete;
		TreeBuilder& operator= (TreeBuilder&) = delete;


};

/********************************************************************************/
// TreeBuilderBinary

class TreeBuilderBinary : public TreeBuilder{
	public:
		TreeBuilderBinary(TreeItemBuilder &itemBuilder, BinaryTree* root);

	protected:
		virtual void parseChildren(NodeIterator* node, TreeItemRef &parent, int _maxdepth = TREE_MAXDEPTH);
};

// TreeBuilderChain
class TreeBuilderChain : public TreeBuilder{
	public:
		TreeBuilderChain(TreeItemBuilder &itemBuilder, ChainTree *root);

	protected:
		virtual void parseChildren(NodeIterator* node, TreeItemRef &parent, int _maxdepth = TREE_MAXDEPTH);
};

// TreeBuilderList
class TreeBuilderList : public TreeBuilder{
	public:
		TreeBuilderList(TreeItemBuilder &itemBuilder, ListTree *root);

	protected:
		virtual void parseChildren(NodeIterator* node, TreeItemRef &parent, int _maxdepth = TREE_MAXDEPTH);
};



