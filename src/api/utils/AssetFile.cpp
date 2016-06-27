#include "../stdafx.h"
#include "AssetFile.h"
#include "ResourceManager.h"
#include "dirent.h"

#ifndef LIVE_RELEASE

#include "../core/thread.h"

#include <windows.h>
#include <Winbase.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#endif /*LIVE_RELEASE*/  

using namespace Grafkit;
using namespace FWdebugExceptions;
 
#define MAX_DIRS 25
#define MAX_FILES 255
#define MAX_BUFFER 4096

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

#ifndef LIVE_RELEASE

namespace LiveReload {

	/*	Innen van:
		https://developersarea.wordpress.com/2014/09/26/win32-file-watcher-api-to-monitor-directory-changes/
	*/

	class WatchDirectory : public IFileEventWatch, public Thread {
	private:
		HANDLE m_hDir;
		CHAR m_lpszDirName[MAX_PATH];
		CHAR m_lpBuffer[MAX_BUFFER];
		DWORD m_dwBufLength;
		OVERLAPPED Overlapped;

	public:

		WatchDirectory(LPCSTR path) : Thread()
		{
			m_hDir = CreateFile(path, GENERIC_READ | FILE_LIST_DIRECTORY,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
				NULL);

			if (m_hDir == INVALID_HANDLE_VALUE)
			{
				return; //cannot open folder
			}
			lstrcpy(m_lpszDirName, path);

			LOGGER(Log::Logger().Info("Creating watcher for directory: %s", path));
		}

		~WatchDirectory() {
			if (m_hDir) 
				CloseHandle(m_hDir);
		}

		int Run() {
			do {
				Poll();
			} while (true);
		}

		void Poll(){
			char buf[4096];
			DWORD nRet;
			BOOL result = TRUE;
			char filename[MAX_PATH];

			OVERLAPPED PollingOverlap;

			FILE_NOTIFY_INFORMATION* pNotify;
			int offset;
			PollingOverlap.OffsetHigh = 0;
			PollingOverlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			while (result)
			{
				result = ReadDirectoryChangesW(
					m_hDir,// handle to the directory to be watched
					&buf,// pointer to the buffer to receive the read results
					sizeof(buf),// length of lpBuffer
					TRUE,// flag for monitoring directory or directory tree
					FILE_NOTIFY_CHANGE_FILE_NAME |
					FILE_NOTIFY_CHANGE_DIR_NAME |
					FILE_NOTIFY_CHANGE_SIZE |
					FILE_NOTIFY_CHANGE_LAST_WRITE |
					//FILE_NOTIFY_CHANGE_LAST_ACCESS |
					FILE_NOTIFY_CHANGE_CREATION | 
					0,
					&nRet,// number of bytes returned
					&PollingOverlap,// pointer to structure needed for overlapped I/O
					NULL);

				WaitForSingleObject(PollingOverlap.hEvent, INFINITE);
				offset = 0;
				int rename = 0;
				char oldName[260];
				char newName[260];
				do
				{
					pNotify = (FILE_NOTIFY_INFORMATION*)((char*)buf + offset);
					strcpy_s(filename, "");
					int filenamelen = WideCharToMultiByte(CP_ACP, 0, pNotify->FileName, pNotify->FileNameLength / 2, filename, sizeof(filename), NULL, NULL);
					filename[pNotify->FileNameLength / 2] = 0;
					switch (pNotify->Action)
					{
					case FILE_ACTION_ADDED:
						LOGGER(Log::Logger().Info("The file is added to the directory: [%s] \n", filename));
						break;
					case FILE_ACTION_REMOVED:
						LOGGER(Log::Logger().Info("The file is removed from the directory: [%s] \n", filename));
						break;
					case FILE_ACTION_MODIFIED:
						LOGGER(Log::Logger().Info("The file is modified. This can be a change in the time stamp or attributes: [%s]\n", filename));
						break;
					case FILE_ACTION_RENAMED_OLD_NAME:
						LOGGER(Log::Logger().Info("The file was renamed and this is the old name: [%s]\n", filename));
						break;
					case FILE_ACTION_RENAMED_NEW_NAME:
						LOGGER(Log::Logger().Info("The file was renamed and this is the new name: [%s]\n", filename));
						break;
					default:
						LOGGER(Log::Logger().Info("\nDefault error.\n"));
						break;
					}

					offset += pNotify->NextEntryOffset;

				} while (pNotify->NextEntryOffset); //(offset != 0);
			}

		}

	};

	
}

#endif /*LIVE_RELEASE*/

FileAssetFactory::FileAssetFactory(std::string root) :
	m_root(root), m_eventWatcher(nullptr)
{
	listdir(root, m_dirlist);

	/* Livereload */
#ifndef LIVE_RELEASE
	m_eventWatcher = new LiveReload::WatchDirectory(root.c_str());
	((LiveReload::WatchDirectory*)m_eventWatcher)->Start();
#endif
}


FileAssetFactory::~FileAssetFactory()
{
#ifndef LIVE_RELEASE
	((LiveReload::WatchDirectory*)m_eventWatcher)->Join();
	if (m_eventWatcher) delete m_eventWatcher;
#endif
}


IAssetRef FileAssetFactory::Get(std::string name)
{
	FILE* fp = nullptr;

	std::string fullname = m_root + name;
	fopen_s(&fp, fullname.c_str(), "rb");

	LOGGER(Log::Logger().Info("Accessin file %s", fullname.c_str()));

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

std::list<std::string> FileAssetFactory::GetAssetList()
{
	return this->m_dirlist;
}


std::list<std::string> FileAssetFactory::GetAssetList(AssetFileFilter * filter)
{
	filelist_t filelist;
	for (filelist_t::iterator it = m_dirlist.begin(); it != m_dirlist.end(); it++)
	{
		if (filter->isFileInfilter(*it)) filelist.push_back(*it);
	}

	return filelist;
}

void Grafkit::FileAssetFactory::PollEvents(IResourceManager *resman)
{
//#ifndef LIVE_RELEASE
//	static unsigned int count;
//	if (count == 0) {
//		m_eventWatcher->Poll(resman);
//		count = 120;
//	}
//	count--;
//#endif
}


// ==================================================================================== 

inline FileAssetFactory::FileAsset::~FileAsset()
{
	//if (m_data)
	free(m_data);
}

