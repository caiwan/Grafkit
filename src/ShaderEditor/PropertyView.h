#pragma once

#include "tree.h"

class CPropertyView;

// CPropertyView

class CPropertyView : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CPropertyView)

public:
	class IProperyItemHandler;
	class IPropertyNode;
	typedef Ref<IPropertyNode> IPropertyNodeRef;

public:
	CPropertyView();
	virtual ~CPropertyView();

	void SetHandler(IProperyItemHandler * pHandler) { m_pHandler = pHandler; }
	void AddItem(IPropertyNode* pNode);

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnPropertyChanged(CMFCPropertyGridProperty* pProp) const;

private:
	std::vector<IPropertyNodeRef> m_vNodes;
	IProperyItemHandler* m_pHandler;


	// ================================================================
	// Nested classes
	// ================================================================

public:
	// ================================================================
	class IProperyItemHandler {
		/**Property event handler interface*/
	public:
		virtual void PropertyChangedEvent(NodeIterator* item) = 0;
	};

	// ================================================================ 
	class IPropertyNode : public Referencable{
		friend class CPropertyView;
	public:
		// ... 

	private:
		IPropertyNode *m_pParent;
		NodeIterator* m_pNode;
		CMFCPropertyGridProperty* m_pProperty;
		IProperyItemHandler *m_pHandler;
	};

	class IBuilder {
	public:
		IBuilder() {}
		virtual void operator() (CPropertyView &props) = 0;
	};
};