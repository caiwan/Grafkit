#pragma once

#include "core/Asset.h"
#include "core/thread.h"
#include "AssetFactory.h"

namespace Grafkit {

    /**
        File event watcher interface a live reloadingohz
    */
    class IFileEventWatch {
    public:
        IFileEventWatch() : m_isTerminate(false) {
        }

        virtual ~IFileEventWatch() {}

        std::string PopFile() {
            MutexLocker lock(&m_queueMutex);
            std::string fn;
            if (!m_fileReloadList.empty()) {
                fn = m_fileReloadList.front();
                m_fileReloadList.pop_front();
            }
            return fn;
        }

        bool HasItems() const {
            return !m_fileReloadList.empty();
        }

        void Terminate() { m_isTerminate = true; }

        virtual void Poll() = 0;

    protected:
        std::list<std::string> m_fileReloadList;
        Mutex m_queueMutex;

        bool m_isTerminate;
    };

    /**
        Makes asset directly from files
    */
    class FileAssetFactory : public IAssetFactory
    {
    public:
        class FileAsset;

        explicit FileAssetFactory(std::string root);
        ~FileAssetFactory();

        StreamRef Get(std::string name) override;

        filelist_t GetAssetList() override;
        filelist_t GetAssetList(AssetFileFilter * filter) override;

        // TODO: -> Editor
        bool PollEvents(IResourceManager *resman) override;

        void SetBasePath(const std::string& path) override;

    private:
        // sorry.
        template <class STREAM>
        class InStreamWrapper : IStream
        {
            friend class FileAssetFactory;
        public:
            InStreamWrapper() {}
            ~InStreamWrapper() override {}
            void Read(char* const& buffer, size_t length) override { m_stream.Read(buffer, length); }
            void Write(const char* const buffer, size_t length) override { m_stream.Write(buffer, length); }
            bool IsSuccess() const override { return m_stream.IsSuccess(); }
            bool ReadAll(size_t& outSize, StreamDataPtr& outBuffer) override { return m_stream.ReadAll(outSize, outBuffer); }
        private:
            STREAM m_dataStream;
            InputStream<STREAM> m_stream;
        };

        // -------- 
        std::string m_root;
        filelist_t m_dirlist;

        // TODO: -> Editor
        IFileEventWatch *m_eventWatcher;
    };
}
