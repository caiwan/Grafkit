#include "assets.h"

#include <string.h>
#include <stdio.h>
#include "easyloggingpp.h"


using namespace Grafkit;
using namespace FWdebugExceptions;

// ====================================================================================

Grafkit::ResourceFilter::ResourceFilter(const char ** const & extensions, size_t count)
{
	for (size_t i = 0; i < count; i++) {
		if(extensions[i] && extensions[i][0]) 
			this->m_filterExtensions.insert(extensions[i]);
	}
}


int Grafkit::ResourceFilter::trimpath(std::string fullpath, std::string &path, std::string &filename, std::string & extension)
{
	int result = 0;

	size_t in_len = fullpath.length(), n = 0;
	const char * in_str = fullpath.c_str();

	// finding extension
	n = in_len + 1;
	while (n--) if (in_str[n] == '.') break;

	const char * fext = nullptr;
	if (!n)  result |= 1 << 0; else  fext = &in_str[n + 1];
	if (fext) extension = fext; else result |= 1 << 1;

	// finding filename
	// reset or continue 
	if (!n) n = in_len + 1; else n++;
	while (n--) if (in_str[n] == '/') break;

	// setup name
	const char * fname = nullptr;
	if (!n)  result |= 1 << 2; else  fname = &in_str[n + 1];
	if (fext) filename = fname; else result |= 1 << 3;

	// trim out path
	path.assign(fullpath, 0, n+1);

	return result;
}

int Grafkit::ResourceFilter::isFileInfilter(std::string path)
{	
	std::string dummy0, dummy1, ext;
	Grafkit::ResourceFilter::trimpath(path, dummy0, dummy1, ext);

	if (ext.empty()) 
		return 0;

	int found = 0;
	for (extFilter_t::iterator it = m_filterExtensions.begin(); it != m_filterExtensions.end(); it++) {
		if (*it == ext) {
			found = 1;
			break;
		}
	}

	return found;
}


// implementation of file managers

#include "dirent.h"

//typedef Grafkit::FileResourceManager::filelist_t _filelist_t;

Grafkit::filelist_t listdir(std::string root, Grafkit::filelist_t &dirlist) {
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

Grafkit::FileResourceManager::FileResourceManager(std::string root) : 
	m_root(root)
{
	listdir(root, m_dirlist);
}


Grafkit::FileResourceManager::~FileResourceManager()
{
}


// ====================================================================================

IResourceRef Grafkit::FileResourceManager::GetResourceByName(std::string name)
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


IResourceRef Grafkit::FileResourceManager::GetResourceByUUID(Guid uuid)
{
	// throw EX_DETAILS(NotImplementedMethodException, "Loading file from uuid is not implemented. Not even close.");
	return IResourceRef();
}


std::list<std::string> Grafkit::FileResourceManager::GetResourceList()
{
	return this->m_dirlist;
}


std::list<std::string> Grafkit::FileResourceManager::GetResourceList(ResourceFilter * filter)
{
	filelist_t filelist;
	for (filelist_t::iterator it = m_dirlist.begin(); it != m_dirlist.end(); it++)
	{
		if (filter->isFileInfilter(*it)) filelist.push_back(*it);
	}

	return filelist;
}


// ==================================================================================== 

inline Grafkit::FileResourceManager::FileResource::~FileResource()
{
	//if (m_data)
	free(m_data);
}

