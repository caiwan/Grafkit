#include "resource.h"

using namespace Grafkit;


Grafkit::IResource::IResource() 
{
#ifndef _NO_UUID_	
	GuidGenerator gen;
	this->m_guid = gen.newGuid();
#endif 
}

Grafkit::IResource::~IResource()
{
	///@todo a destruktora ennek nem fasza, a virtualis fuggvenyeket a leszarmazott deletekor viszi magaval. Valami koser megoldas kellene a torlesre, ami megkeruli ezt 
}

void Grafkit::IResource::SetName(std::string name)
{
	m_name = name;
}

#ifndef _NO_UUID_
Guid FWassets::IRenderAsset::GenerateUUID()
{
	Guid uuid = m_guid;
	
	GuidGenerator gen;
	uuid = gen.newGuid();

	m_guid = uuid;

	return this->m_guid;
}
#endif
