#pragma once

//#include "reference.h"

namespace Grafkit
{
    class IStream;
    typedef std::unique_ptr<IStream> StreamRef;
    typedef std::unique_ptr<uint8_t> StreamDataPtr;

    // I'm sorry, I had to do this "clever trick" in order to keep polimorphic stuff working a layer above
    class IStream
    {
    public:
        virtual ~IStream() {
        }

        virtual void Read(char* const & buffer, size_t length) = 0;
        virtual void Write(const char* const buffer, size_t length) = 0;
        virtual bool IsSuccess() const = 0;

        virtual bool ReadAll(size_t& outSize, StreamDataPtr& outBuffer) = 0;

        explicit virtual operator std::istream&() const = 0;
        explicit virtual operator std::ostream&() const = 0;
    };

    // ----------

    template <class STREAM_TYPE>
    class Stream : public IStream
    {
    public:
        explicit Stream(STREAM_TYPE& stream) : m_stream(stream) {
        }

        ~Stream() { m_stream.flush(); }
        void Read(char* const & buffer, size_t length) override { m_stream.read(buffer, length); }
        void Write(const char* const buffer, size_t length) override { m_stream.write(buffer, length); }
        bool IsSuccess() const override { return bool(m_stream); };

        bool ReadAll(size_t& outLength, StreamDataPtr& outBuffer) override
        {
            assert(m_stream.good());
            m_stream.seekg(0, m_stream.end);
            outLength = m_stream.tellg();
            m_stream.seekg(0, m_stream.beg);
            outBuffer = StreamDataPtr(new uint8_t[outLength]);
            m_stream.read((char*)(outBuffer.get()), outLength);
            m_stream.seekg(0, m_stream.beg);
            return IsSuccess();
        }

        explicit operator std::istream&() const override { return static_cast<std::istream&>(m_stream); }
        explicit operator std::ostream&() const override { return static_cast<std::ostream&>(m_stream); }

    protected:
        STREAM_TYPE& m_stream;
    };

    template <class STREAM_TYPE>
    class InputStream : public IStream
    {
    public:
        explicit InputStream(STREAM_TYPE& stream) : m_stream(stream) {
        }

        ~InputStream() {
        }

        void Read(char* const & buffer, size_t length) override { m_stream.read(buffer, length); }

        void Write(const char* const buffer, size_t length) override { throw std::runtime_error("Can't write to an InputStream"); }
        bool IsSuccess() const override { return bool(m_stream); };

        bool ReadAll(size_t& outLength, StreamDataPtr& outBuffer) override
        {
            assert(m_stream.good());
            m_stream.seekg(0, m_stream.end);
            outLength = m_stream.tellg();
            m_stream.seekg(0, m_stream.beg);
            outBuffer = StreamDataPtr(new uint8_t[outLength]);
            m_stream.read((char*)(outBuffer.get()), outLength);
            m_stream.seekg(0, m_stream.beg);
            return IsSuccess();
        }
        
        explicit operator std::istream&() const override { return static_cast<std::istream&>(m_stream); }
        explicit operator std::ostream&() const override { throw std::runtime_error("Can't write to an InputStream"); }

    protected:
        STREAM_TYPE& m_stream;
    };

    template <class STREAM_TYPE>
    class OutputStream : public IStream
    {
    public:
        explicit OutputStream(STREAM_TYPE& stream) : m_stream(stream) {
        }

        ~OutputStream() { m_stream.flush(); }
        void Read(char* const & buffer, size_t length) override { throw std::runtime_error("Can't read from an OutputStream"); }
        void Write(const char* const buffer, size_t length) override { m_stream.write(buffer, length); }

        bool IsSuccess() const override { return bool(m_stream); };

        bool ReadAll(size_t& outSize, StreamDataPtr& outBuffer) override { throw std::runtime_error("Can't read from an OutputStream"); }

        explicit operator std::istream&() const override { throw std::runtime_error("Can't read from an OutputStream"); }
        explicit operator std::ostream&() const override { return static_cast<std::ostream&>(m_stream); }

    protected:
        STREAM_TYPE& m_stream;
    };
}
