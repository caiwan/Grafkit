#include <fstream>
#include <qfile>
#include <qdebug>

#include "core/logger.h"
#include "AssetFactoryProxy.h"


using namespace Grafkit;
using namespace Idogep;
using namespace FWdebugExceptions;

namespace
{
    // --- 
    // Wrapper class for QT Resource 
    class QResourceAsset : public Grafkit::IStream
    {
    public:

        explicit QResourceAsset(const QString& filename)
            : m_isinited(false)
            , m_qFile(filename)
            , m_inStream(m_ifstream) {
        }

        QResourceAsset(const QResourceAsset&) = delete;

        QResourceAsset& operator=(const QResourceAsset) = delete;

        void Read(char* const& buffer, size_t length) override
        {
            Init();
            m_inStream.Read(buffer, length);
        }

        void Write(const char* const buffer, size_t length) override
        {
            Init();
            m_inStream.Write(buffer, length);
        }

        bool IsSuccess() const override { return m_isinited & m_inStream.IsSuccess(); }

        bool ReadAll(size_t& outSize, StreamDataPtr& outBuffer) override
        {
            Init();
            return m_inStream.ReadAll(outSize, outBuffer);
        }

        explicit operator std::basic_istream<char>&() const override
        {
            const_cast<QResourceAsset*const>(this)->Init();
            return static_cast<std::basic_istream<char>&>(m_inStream);
        }

        explicit operator std::basic_ostream<char>&() const override
        {
            const_cast<QResourceAsset*const>(this)->Init();
            return static_cast<std::basic_ostream<char>&>(m_inStream);
        }

    private:

        void Init()
        {
            if (m_qFile.open(QIODevice::ReadOnly) && m_qFile.isReadable()) { m_ifstream = std::ifstream(_fdopen(m_qFile.handle(), "r")); }
            else
            {
                auto msg = std::string("Cannot open file ") + m_qFile.fileName().toStdString();
                LOGGER(Log::Logger().Error(msg.c_str()));
                throw std::runtime_error(msg);
            }
        }

        bool m_isinited;

        QFile m_qFile;
        std::ifstream m_ifstream;
        InputStream<std::ifstream> m_inStream;
    };
}

AssetFactoryProxy::AssetFactoryProxy(const std::shared_ptr<IAssetFactory> & parentFactory) : IAssetFactory()
, m_parentFactory(parentFactory) {
}

AssetFactoryProxy::~AssetFactoryProxy() {
}

StreamRef AssetFactoryProxy::Get(std::string name)
{
    StreamRef asset = nullptr;

    // On first time failure, redirect ourselves to find our stuff in QT's resources
    try { asset = m_parentFactory->Get(name); }
    catch (AssetLoadException& ex)
    {
        QString filename = QString(":/resources/") + QString::fromStdString(name);
        return std::unique_ptr<Grafkit::IStream>(std::make_unique<QResourceAsset>(filename));
    }

}

void AssetFactoryProxy::SetBasePath(const std::string& path) { m_parentFactory->SetBasePath(path); }
