// PropertyView.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyView.h"

#include "exceptions.h"
using namespace FWdebugExceptions;

// CPropertyView

IMPLEMENT_DYNAMIC(CPropertyView, CMFCPropertyGridCtrl)

CPropertyView::CPropertyView()
{

}

CPropertyView::~CPropertyView()
{
	/// @todo kell-e torolni a nodeokat
}

void CPropertyView::AddItem(PropertyNode * pNode)
{
	this->m_vNodes.push_back(pNode);
}

// ---------------


// ---------------

BEGIN_MESSAGE_MAP(CPropertyView, CMFCPropertyGridCtrl)
END_MESSAGE_MAP()

// CPropertyView message handlers

void CPropertyView::OnPropertyChanged(CMFCPropertyGridProperty* pProp) const
{
	// TODO: Add your specialized code here and/or call the base class

	return CMFCPropertyGridCtrl::OnPropertyChanged(pProp);
}


// ---------------