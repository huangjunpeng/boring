#include "OutStream.h"
#include <util.h>
using namespace boring::base;
const size_t COutStream::sInitalSize = 1024;

COutStream::COutStream() : m_buffer(sInitalSize),m_currIndex(0)
{
}

COutStream& COutStream::operator<<(uint8 value)
{
    Append(&value, sizeof(value));
    return(*this);
}

COutStream& COutStream::operator<<(uint16 value)
{
    uint16 byte16 = CUtil::hton16(value);
    Append(&byte16, sizeof(byte16));
    return (*this);
}

COutStream& COutStream::operator<<(uint32 value)
{
	uint32 byte32 = CUtil::hton32(value);
    Append(&byte32, sizeof(byte32));
    return (*this);
}

COutStream& COutStream::operator<<(int8 value)
{
    Append(&value, sizeof(value));
    return(*this);
}

COutStream& COutStream::operator<<(int16 value)
{
	int16 byte16 = CUtil::hton16(value);
    Append(&byte16, sizeof(byte16));
    return (*this);
}

COutStream& COutStream::operator<<(int32 value)
{
	int32 byte32 = CUtil::hton32(value);
    Append(&byte32, sizeof(byte32));
    return (*this);
}

COutStream& COutStream::operator<<(const std::string& str)
{
    uint16 len = static_cast<uint16>(str.length());
    (*this)<<len;
    Append(str.c_str(), len);
    return (*this);
}

void COutStream::WriteBytes(const void *data, size_t len)
{
    Append(data, len);
}

void COutStream::Reposition(size_t pos)
{
    m_currIndex = pos;
}

void COutStream::Skip(size_t len)
{
    EnsureWriteableBytes(len);
    m_currIndex += len;
}

void COutStream::Clear()
{
    m_buffer.clear();
    m_currIndex = 0;
}

char *COutStream::Data()
{
    return &*m_buffer.begin();
}

const char *COutStream::Data() const
{
    return &*m_buffer.begin();
}

size_t COutStream::Length()
{
    return m_currIndex;
}

size_t COutStream::WriteableBytes() const
{
    return m_buffer.size() - m_currIndex;
}

void COutStream::EnsureWriteableBytes(size_t len)
{
    if (WriteableBytes() < len){
        m_buffer.resize(m_currIndex + len);
    }
}

void COutStream::Append(const char *data, size_t len)
{
    EnsureWriteableBytes(len);
    std::copy(data, data + len, m_buffer.begin() + m_currIndex);
    m_currIndex += len;
}

void COutStream::Append(const void *data, size_t len)
{
    Append(static_cast<const char*>(data), len);
}
