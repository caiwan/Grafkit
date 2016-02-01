#pragma once

// #include <list>
#include <vector>

#include "stdafx.h"
#include "tree.h"

#include "reference.h"

class TreeBuilder;
class TreeItemBuilder;

class TreeItem;


/********************************************************************************/
// CTreeView window

class ITreeItemHandler{
public:
	virtual void NodeSelectedEvent(TreeNode* node) = 0;	///< akkor hivodik meg, ha a treeben megvaltozott a kijelolt elem
	
	virtual void NodeClickEvent(CPoint &point){}
	virtual void NodeClickOnItemEvent(CPoint &point, TreeNode* node){}
		
	virtual void NodeDoubleClickEvent(CPoint &point){}		///< akkor hivodik meg, amikor duplakattintas tortent, de a kattintas helye nem egy elemre esik
	virtual void NodeDoubleClickOnItemEvent(CPoint &point, TreeNode* node){}	///< akkor hivodik meg, amikor egy duplakattintas egy elemre esett

	virtual void NodeRightClickEvent(CPoint &point){}
	virtual void NodeRightClickOnItemEvent(CPoint &point, TreeNode* node){}

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
		void InsertItem(TreeItem* m_parent);

		TreeNode* GetSelectedNode();
		TreeNode* GetSelectedNode(CPoint &point);

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
		std::vector<TreeItem*> m_Items;
		ITreeItemHandler *_pCallbackHndlr;

};


///@todo inteherit these classes 

/********************************************************************************/
// TreeItem object

//class TreeItem;

class TreeItem : virtual public Referencable{
	friend class TreeBuilder;
	friend class TreeItemBuilder;
	friend class CTreeView;

	public:
		TreeItem(TreeNode* node, TreeItem* parent);
		virtual ~TreeItem(){}

		inline void setNode(TreeNode* node){m_pNode = node;}
		inline TreeNode* getNode(void){return m_pNode;}

	public:
		int m_idIcon;
		int m_idIconSelected;
		CString m_name;

	protected:
		TreeNode* m_pNode;

		TreeItem* m_parent;
		HTREEITEM m_hItem;

};

/********************************************************************************/
// TreeItemBuilder
// TreeBuilder 

///@todo ezt az interfacet refractorozni kell Builder-bol Factory-re
class TreeItemBuilder{
	friend class TreeBuilder;
	protected:
		virtual TreeItem* newTreeItem(TreeNode* node, TreeItem* &parent) = 0;
};

class TreeBuilder{
	public:
		TreeBuilder(TreeItemBuilder &itemBuilder, TreeNode* root);
		virtual ~TreeBuilder();

		void parse(CTreeView *container);
		void parse(CTreeView *container ,TreeItem* &parent);

		inline void switchForceSkipRoot(int isForce=1){m_isSkipRoot = isForce;}
		// +++ images 
		// +++ things 

	protected:
		virtual void parseNode(TreeNode* node, TreeItem* parent = nullptr, int _maxdepth = TREE_MAXDEPTH);
		virtual void parseChildren(TreeNode* node, TreeItem* &parent, int _maxdepth = TREE_MAXDEPTH) = 0;

		TreeItem* InsertItem(TreeNode* node, TreeItem* &m_parent);

		std::vector<TreeItem*> m_Items;

		// --- 
		TreeItemBuilder &m_rItemBuilder;
		TreeNode *m_pRoot;
	
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
		virtual void parseChildren(TreeNode* node, TreeItem* &parent, int _maxdepth = TREE_MAXDEPTH);
};

// TreeBuilderChain
class TreeBuilderChain : public TreeBuilder{
	public:
		TreeBuilderChain(TreeItemBuilder &itemBuilder, ChainTree *root);

	protected:
		virtual void parseChildren(TreeNode* node, TreeItem* &parent, int _maxdepth = TREE_MAXDEPTH);
};

// TreeBuilderList
class TreeBuilderList : public TreeBuilder{
	public:
		TreeBuilderList(TreeItemBuilder &itemBuilder, ListTree *root);

	protected:
		virtual void parseChildren(TreeNode* node, TreeItem* &parent, int _maxdepth = TREE_MAXDEPTH);
};



