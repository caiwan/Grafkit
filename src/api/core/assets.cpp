#include "assets.h"

#include <string.h>
#include <stdio.h>
#include "easyloggingpp.h"


using namespace FWassets;
using namespace FWdebugExceptions;

// ====================================================================================

FWassets::ResourceFilter::ResourceFilter(const char ** const & extensions, size_t count)
{
	for (size_t i = 0; i < count; i++) {
		if(extensions[i] || extensions[i][0]) 
			this->m_filterExtensions.insert(extensions[i]);
	}
}


int FWassets::ResourceFilter::isFileInfilter(std::string path)
{	
	// miezittkerem
	char * in_str = new char[path.length()+1];
	strcpy_s(in_str, path.length(), path.c_str());
	rsize_t strmax = path.length();
	char * pch = nullptr, *pchs[256], *next = nullptr;
	const char *delim = ".";

	size_t count = 0;
	do 
	{
		// ez buzi ez teljesen 
		pch = strtok_s (in_str, delim, &next);
		pchs[count] = pch;
	} while (pch != NULL);

	if (count == 0) 
		return 0;

	char *& fext = pchs[count - 1];

	int found = 0;
	for (extFilter_t::iterator it = m_filterExtensions.begin(); it != m_filterExtensions.end(); it++) {
		if (*it == fext) {
			found = 1;
			break;
		}
	}

	delete in_str;

	return found;
}


// implementation of file managers

#include "dirent.h"

//typedef FWassets::FileResourceManager::filelist_t _filelist_t;

FWassets::filelist_t listdir(std::string root, FWassets::filelist_t &dirlist) {
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(root.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			// skip root and parent
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) 
				continue;

			if (ent->d_type & DT_DIR) {
				listdir(root + ent->d_name +"/", dirlist);
			}
			else {
				dirlist.push_back(root + std::string(ent->d_name));
			}
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		// return EXIT_FAILURE;

		///@todo raise exception
	}

	return dirlist;
}

FWassets::FileResourceManager::FileResourceManager(std::string root) : 
	m_root(root)
{
	listdir(root, m_dirlist);
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

	LOG(TRACE) << "Accessing file " << fullname.c_str();

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
	// throw EX_DETAILS(NotImplementedMethodException, "Loading file from uuid is not implemented. Not even close.");
	return IResourceRef();
}


std::list<std::string> FWassets::FileResourceManager::GetResourceList()
{
	return this->m_dirlist;
}


std::list<std::string> FWassets::FileResourceManager::GetResourceList(ResourceFilter * filter)
{
	filelist_t filelist;
	for (filelist_t::iterator it = m_dirlist.begin(); it != m_dirlist.end(); it++)
	{
		if (filter->isFileInfilter(*it)) filelist.push_back(*it);
	}

	return filelist;
}


// ==================================================================================== 

inline FWassets::FileResourceManager::FileResource::~FileResource()
{
	//if (m_data)
	free(m_data);
}

