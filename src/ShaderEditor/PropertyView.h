#pragma once

#include "tree.h"

class CPropertyView;

// CPropertyView

class CPropertyView : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CPropertyView)

public:
	class IProperyItemHandler;
	class PropertyNode;

public:
	CPropertyView();
	virtual ~CPropertyView();

	void SetHandler(IProperyItemHandler * pHandler) { m_pHandler = pHandler; }
	void AddItem(PropertyNode* pNode);

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnPropertyChanged(CMFCPropertyGridProperty* pProp) const;

private:
	std::vector<PropertyNode*> m_vNodes;
	IProperyItemHandler* m_pHandler;


	// ================================================================
	// Nested classes
	// ================================================================

public:
	// ================================================================
	class IProperyItemHandler 
	{
		/**Property event handler interface*/
		public:
			virtual void PropertyChangedEvent(TreeNode* item) = 0;
	};

	// ================================================================ 
	class PropertyNode
	{
		friend class CPropertyView;
		public:
			PropertyNode() {}
			virtual ~PropertyNode() {}

		protected:
			CString m_name;

			PropertyNode *m_pParent;
			CMFCPropertyGridProperty* m_pProperty;
			IProperyItemHandler *m_pHandler;

			TreeNode* m_pNode;
	};

	class IBuilder {
		public:
			IBuilder() {}
			virtual void operator() (CPropertyView &props) = 0;
	};
};