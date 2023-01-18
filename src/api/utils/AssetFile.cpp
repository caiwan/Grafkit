#include "../stdafx.h"

#include "AssetFile.h"

#include "dirent.h"

using namespace Grafkit;
using namespace FWdebugExceptions;
 

IAssetFactory::filelist_t listdir(std::string root, IAssetFactory::filelist_t &dirlist) {
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(root.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			// skip root and parent
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
				continue;

			if (ent->d_type & DT_DIR) {
				listdir(root + ent->d_name + "/", dirlist);
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

FileAssetManager::FileAssetManager(std::string root) :
	m_root(root)
{
	listdir(root, m_dirlist);
}


FileAssetManager::~FileAssetManager()
{
}


IAssetRef FileAssetManager::Get(std::string name)
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

	return IAssetRef(new FileAsset(data, size));
}

/*
IAssetRef FileAssetManager::GetResourceByUUID(Guid uuid)
{
	// throw EX_DETAILS(NotImplementedMethodException, "Loading file from uuid is not implemented. Not even close.");
	return IResourceRef();
}
*/

std::list<std::string> FileAssetManager::GetAssetList()
{
	return this->m_dirlist;
}


std::list<std::string> FileAssetManager::GetAssetList(AssetFileFilter * filter)
{
	filelist_t filelist;
	for (filelist_t::iterator it = m_dirlist.begin(); it != m_dirlist.end(); it++)
	{
		if (filter->isFileInfilter(*it)) filelist.push_back(*it);
	}

	return filelist;
}


// ==================================================================================== 

inline FileAssetManager::FileAsset::~FileAsset()
{
	//if (m_data)
	free(m_data);
}

