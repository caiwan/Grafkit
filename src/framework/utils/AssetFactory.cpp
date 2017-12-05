#include "stdafx.h"

#include "AssetFactory.h"

using namespace Grafkit;

// ====================================================================================

AssetFileFilter::AssetFileFilter(const char ** const & extensions, size_t count)
{
	for (size_t i = 0; i < count; i++) {
		if (extensions[i] && extensions[i][0])
			this->m_filterExtensions.insert(extensions[i]);
	}
}


int AssetFileFilter::trimpath(std::string fullpath, std::string &path, std::string &filename, std::string & extension)
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
	path.assign(fullpath, 0, n + 1);

	return result;
}

int AssetFileFilter::IsFileInfilter(std::string path)
{
	std::string dummy0, dummy1, ext;
	AssetFileFilter::trimpath(path, dummy0, dummy1, ext);

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
