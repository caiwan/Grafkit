#include <stdio.h>

#include "assets.h"

using namespace FWassets;
using namespace FWdebugExceptions;

// implementation of file managers

FWassets::FileResourceManager::FileResourceManager(const char * root) :
	m_root(root)
{
}

FWassets::FileResourceManager::FileResourceManager(std::string root) : 
	m_root(root)
{
}

FWassets::FileResourceManager::~FileResourceManager()
{
}

// ====================================================================================

IResourceRef FWassets::FileResourceManager::GetResourceByName(std::string name)
{
	FILE* fp = nullptr;

	std::string fullname = m_root + name;
	fopen_s(&fp, fullname.c_str(), "rb");

	if (!fp) {
		throw EX_DETAILS(AssetLoadException, fullname.c_str());
	}
	
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	void* data = malloc(size);
	fread(data, 1, size, fp);
	fclose(fp);

	return new FileResource(data, size);
}

IResourceRef FWassets::FileResourceManager::GetResourceByUUID(Guid uuid)
{
	throw EX_DETAILS(NotImplementedMethodException, "Loading file from uuid is not implemented. Not even close.");
	return IResourceRef();
}

// ==================================================================================== 

inline FWassets::FileResourceManager::FileResource::~FileResource()
{
	//if (m_data)
	free(m_data);
}

