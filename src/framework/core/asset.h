#pragma once

#include "reference.h"

namespace Grafkit
{
    typedef Ref<IStream> StreamRef;
    typedef IStream IAsset;

    // I'm sorry, I had to do this "clever trick" in order to keep polimorphic stuff working a layer above
    class IStream
    {
    public:
        virtual ~IStream() {
        }

        virtual void Read(char* const & buffer, size_t length) = 0;
        virtual void Write(const char* const buffer, size_t length) = 0;
        virtual bool IsSuccess() const = 0;

        virtual bool ReadAll(size_t& outSize, char*& outBuffer) = 0;
    };

    // ----------

    template <class STREAM_TYPE>
    class Stream : public IStream
    {
    public:
        Stream(STREAM_TYPE& stream) : m_stream(stream) {
        }

        ~Stream() { m_stream.flush(); }
        void Read(char* const & buffer, size_t length) override { m_stream.read(buffer, length); }
        void Write(const char* const buffer, size_t length) override { m_stream.write(buffer, length); }
        bool IsSuccess() const override { return bool(m_stream); };

        bool ReadAll(size_t& outLength, char*& outBuffer) override
        {
            m_stream.seekg(0, m_stream.end);
            outLength = m_stream.tellg();
            m_stream.seekg(0, m_stream.beg);
            outBuffer = new char[outLength];
            m_stream.read(outBuffer, outLength);
            m_stream.seekg(0, m_stream.beg);
            return IsSuccess();
        }

    protected:
        STREAM_TYPE& m_stream;
    };

    template <class STREAM_TYPE>
    class InputStream : public IStream //: public Stream<STREAM_TYPE>
    {
    public:
        explicit InputStream(STREAM_TYPE& stream) : m_stream(stream) {
        }

        ~InputStream() { /*m_stream.flush();*/ }

        void Read(char* const & buffer, size_t length) override { m_stream.read(buffer, length); }

        void Write(const char* const buffer, size_t length) override { throw std::runtime_error("Can't write to an InputStream"); }
        bool IsSuccess() const override { return bool(m_stream); };

        bool ReadAll(size_t& outLength, char*& outBuffer) override
        {
            m_stream.seekg(0, m_stream.end);
            outLength = m_stream.tellg();
            m_stream.seekg(0, m_stream.beg);
            outBuffer = new char[outLength];
            m_stream.read(outBuffer, outLength);
            m_stream.seekg(0, m_stream.beg);
            return IsSuccess();
        }

    private:
        STREAM_TYPE& m_stream;
    };

    template <class STREAM_TYPE>
    class OutputStream : public IStream //: public Stream<STREAM_TYPE>
    {
    public:
        explicit OutputStream(STREAM_TYPE& stream) : m_stream(stream) {
        }

        ~OutputStream() { m_stream.flush(); }
        void Read(char* const & buffer, size_t length) override { throw std::runtime_error("Can't read from an OutputStream"); }
        void Write(const char* const buffer, size_t length) override { m_stream.write(buffer, length); }

        bool IsSuccess() const override { return bool(m_stream); };

        bool ReadAll(size_t& outSize, char*& outBuffer) override { throw std::runtime_error("Can't read from an OutputStream"); }

    protected:
        STREAM_TYPE& m_stream;
    };
}
