#include "stdafx.h"

#include <algorithm>
#include <fstream>
#include <filesystem>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <Winbase.h>

#include "core/thread.h"

#include "AssetFile.h"
#include "resource/ResourceManager.h"
#include "utils/dirent.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

#define MAX_DIRS 25
#define MAX_FILES 255
#define MAX_BUFFER (4*4096)

namespace
{
    // sorry.
    template <class STREAM>
    class InStreamWrapper : Grafkit::IStream
    {
        friend class FileAssetFactory;
    public:
        explicit InStreamWrapper(std::string filename) : m_isInited(false)
            , m_filename(filename)
            , m_stream(m_dataStream) {
        }

        InStreamWrapper(const InStreamWrapper& other) :
            m_isInited(false)
            , m_filename(other.m_filename)
            , m_stream(m_dataStream) {
        }

        InStreamWrapper& operator=(const InStreamWrapper& other)
        {
            if (this == &other)
                return *this;
            m_filename = other.m_filename;
            return *this;
        }

        ~InStreamWrapper() override {
        }

        void Read(char* const& buffer, size_t length) override
        {
            Init();
            m_stream.Read(buffer, length);
        }

        void Write(const char* const buffer, size_t length) override
        {
            Init();
            m_stream.Write(buffer, length);
        }

        bool IsSuccess() const override { return m_isInited && m_stream.IsSuccess(); }

        bool ReadAll(size_t& outSize, StreamDataPtr& outBuffer) override
        {
            Init();
            return m_stream.ReadAll(outSize, outBuffer);
        }

    private:
        void Init()
        {
            if (m_isInited)
                return;

            assert(!m_dataStream.is_open());
            m_dataStream.open(m_filename, std::ios::binary);
            LOGGER(Log::Logger().Info("Accessing file: %s", m_filename.c_str()));

            if (m_dataStream.fail())
            {
                LOGGER(Log::Logger().Error("Could not access file: %s errno: %s", m_filename.c_str(), std::strerror(errno)));
                THROW_EX_DETAILS(FWdebugExceptions::AssetLoadException, m_filename.c_str());
            }
            m_isInited = true;
        }

        bool m_isInited;
        std::string m_filename;
        STREAM m_dataStream;
        InputStream<STREAM> m_stream;
    };

    std::string TrimSlashes(std::string in)
    {
        std::string out = "";
        out.reserve(in.length());
        size_t left = 0;
        do
        {
            std::string::size_type pos = in.find("//", left);
            if (pos != std::string::npos)
            {
                out.append(in.substr(left, pos - left));
                left += pos + 1;
            }
            else
            {
                out.append(in.substr(left, in.length() - left));
                break;
            }
        } while (left != in.length());

        return out;
    }

    IAssetFactory::filelist_t ListDir(std::string root, IAssetFactory::filelist_t& dirlist, std::string droot = "./")
    {
        DIR* dir;
        struct dirent* ent;
        std::string path = root + droot;
        if ((dir = opendir(path.c_str())) != nullptr)
        {
            while ((ent = readdir(dir)) != nullptr)
            {
                // skip root and parent
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                    continue;

                if (ent->d_type & DT_DIR) { ListDir(root, dirlist, ent->d_name); }
                else
                {
                    // rm unnesesarry double slashes 
                    std::string in = TrimSlashes(droot + "/" + std::string(ent->d_name));
                    dirlist.push_back(in);
                }
            }
            closedir(dir);
        }

        return dirlist;
    }
}

// TODO: -> Editor
#ifndef LIVE_RELEASE

namespace LiveReload
{
    /*	Innen van:
        https://developersarea.wordpress.com/2014/09/26/win32-file-watcher-api-to-monitor-directory-changes/
    */

    class WatchDirectory : public IFileEventWatch, public Thread
    {
    public:

        explicit WatchDirectory(LPCSTR path) : IFileEventWatch()
            , Thread()
            , m_dwBufLength(0)
        {
            m_hDir = CreateFile(path, GENERIC_READ | FILE_LIST_DIRECTORY,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                nullptr);

            if (m_hDir == INVALID_HANDLE_VALUE)
            {
                return; //cannot open folder
            }
            lstrcpy(m_lpszDirName, path);

            LOGGER(Log::Logger().Info("Creating watcher for directory: %s", path));
        }

        ~WatchDirectory()
        {
            Terminate();
            //Join();
            Stop();

            if (m_hDir)
                CloseHandle(m_hDir);
        }

        void PushFile(std::string infn)
        {
            MutexLocker locker(&m_queueMutex);
            replace(infn.begin(), infn.end(), '\\', '/');
            m_fileReloadList.push_back(infn);
        }

        int Run() override
        {
            LOGGER(Log::Logger().Trace("FS Watcher Thread started"));
            do { Poll(); } while (!m_isTerminate);
            LOGGER(Log::Logger().Trace("FS Watcher Thread terminated"));
            return true;
        }

        void Poll() override
        {
            char buf[8192];
            DWORD nRet;
            BOOL result = TRUE;
            char filename[MAX_PATH];

            OVERLAPPED PollingOverlap;

            FILE_NOTIFY_INFORMATION* pNotify = nullptr;
            PollingOverlap.OffsetHigh = 0;
            PollingOverlap.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
            //while (result)
            //{
            ZeroMemory(buf, 8192);
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
                nullptr);

            if (!result)
                return;

            DWORD pollingResult = WaitForSingleObject(PollingOverlap.hEvent, INFINITE); // instead of INFINITE, prevent stuck forever
            if (pollingResult == WAIT_TIMEOUT) { return; }

            int offset = 0;
            int rename = 0;
            char oldName[260];
            char newName[260];

            do
            {
                pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(static_cast<char*>(buf) + offset);

                filename[0] = 0;
                int filenamelen = WideCharToMultiByte(CP_ACP, 0, pNotify->FileName, pNotify->FileNameLength / 2, filename, sizeof(filename), nullptr, nullptr);
                filename[pNotify->FileNameLength / 2] = 0;
                switch (pNotify->Action)
                {
                case FILE_ACTION_ADDED:
                    LOGGER(Log::Logger().Info("The file is added to the directory: [%s] ", filename));
                    break;

                case FILE_ACTION_REMOVED:
                    LOGGER(Log::Logger().Info("The file is removed from the directory: [%s] ", filename));
                    break;

                case FILE_ACTION_MODIFIED:
                    LOGGER(Log::Logger().Info("The file is modified. This can be a change in the time stamp or attributes: [%s]", filename));
                    PushFile(filename);
                    break;

                case FILE_ACTION_RENAMED_OLD_NAME:
                    LOGGER(Log::Logger().Info("The file was renamed and this is the old name: [%s]", filename));
                    break;

                case FILE_ACTION_RENAMED_NEW_NAME:
                    LOGGER(Log::Logger().Info("The file was renamed and this is the new name: [%s]", filename));
                    break;

                default:
                    LOGGER(Log::Logger().Info("Default error."));
                    break;
                }

                offset += pNotify->NextEntryOffset;
            } while (pNotify->NextEntryOffset); //(offset != 0);
            //}
        }

    private:
        HANDLE m_hDir;
        CHAR m_lpszDirName[MAX_PATH];
        CHAR m_lpBuffer[MAX_BUFFER];
        DWORD m_dwBufLength;
        OVERLAPPED Overlapped;
    };
}

#endif /*LIVE_RELEASE*/

FileAssetFactory::FileAssetFactory(std::string root) : m_root()
, m_eventWatcher(nullptr) {
    FileAssetFactory::SetBasePath(root);
}


FileAssetFactory::~FileAssetFactory()
{
#ifndef LIVE_RELEASE
    if (m_eventWatcher) { delete m_eventWatcher; }
#endif
}


StreamRef FileAssetFactory::Get(std::string name)
{
    std::string fullname = TrimSlashes(m_root + name);
    LOGGER(Log::Logger().Info("Creating wrapper for file: %s", fullname.c_str()));
    return StreamRef(new InStreamWrapper<std::ifstream>(fullname));
}

IAssetFactory::filelist_t FileAssetFactory::GetAssetList() { return this->m_dirlist; }

IAssetFactory::filelist_t FileAssetFactory::GetAssetList(AssetFileFilter* filter)
{
    filelist_t filelist;
    for (filelist_t::iterator it = m_dirlist.begin(); it != m_dirlist.end(); ++it)
    {
        if (filter->IsFileInfilter(*it))
            filelist.push_back(*it);
    }

    return filelist;
}

bool FileAssetFactory::PollEvents(IResourceManager* resman)
{
#ifndef LIVE_RELEASE
    static unsigned char count;
    if (count == 0)
    {
        LiveReload::WatchDirectory* w = static_cast<LiveReload::WatchDirectory*>(m_eventWatcher);
        bool triggered = false;
        if (w && w->HasItems())
        {
            do
            {
                triggered = true;
                if (resman)
                    resman->TriggerReload(w->PopFile());
                else
                    w->PopFile(); // Todo: find a better way to purge stuff 
            } while (w->HasItems());
        }
        count = 30;
        return triggered;
    }
    count--;
    return false;
#else
    return false;
#endif
}

void FileAssetFactory::SetBasePath(const std::string& root)
{
    assert(!root.empty());

#ifndef LIVE_RELEASE
    if (m_eventWatcher) { delete m_eventWatcher; }
#endif

    m_root = root + "/";

    // TODO: sanitize double slash?

    m_dirlist.clear();
    ListDir(root, m_dirlist);

    /* Livereload */
#ifndef LIVE_RELEASE
    m_eventWatcher = new LiveReload::WatchDirectory(root.c_str());
    static_cast<LiveReload::WatchDirectory*>(m_eventWatcher)->Start();
#endif
}
