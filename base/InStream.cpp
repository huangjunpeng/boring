#include <assert.h>
#include "InStream.h"
#include <util.h>
using namespace boring::base;

CInStream::CInStream() : m_currIndex(0)
{
}

CInStream::CInStream(const char *data, size_t len) : m_currIndex(0)
{
    SetData(data, len);
}

void CInStream::SetData(const char *data, size_t len)
{
    m_currIndex = 0;
    m_buffer.resize(len);
    std::copy(data, data + len, m_buffer.begin());
}

CInStream& CInStream::operator>>(uint8 &value)
{
    assert(ReadableBytes() >= sizeof(uint8));
    value = *Peek();
    m_currIndex += sizeof value;
    return *this;
}

CInStream& CInStream::operator>>(uint16 &value)
{
    assert(ReadableBytes() >= sizeof(uint16));
    uint16 bety16 = 0;
    ::memcpy(&bety16, Peek(), sizeof bety16);
    m_currIndex += sizeof bety16;
	value = CUtil::hton16(bety16);
    return *this;
}

CInStream& CInStream::operator>>(uint32 &value)
{
    assert(ReadableBytes() >= sizeof(uint32));
    uint32 bety32 = 0;
    ::memcpy(&bety32, Peek(), sizeof bety32);
    m_currIndex += sizeof value;
	value = CUtil::hton32(bety32);
    return *this;
}

CInStream& CInStream::operator>>(int8 &value)
{
    assert(ReadableBytes() >= sizeof(int8));
    value = *Peek();
    m_currIndex += sizeof value;
    return *this;
}
CInStream& CInStream::operator>>(int16 &value)
{
    assert(ReadableBytes() >= sizeof(int16));
    int16 bety16 = 0;
    ::memcpy(&bety16, Peek(), sizeof bety16);
    m_currIndex += sizeof bety16;
	value = CUtil::hton16(bety16);
    return *this;
}
CInStream& CInStream::operator>>(int32 &value)
{
    assert(ReadableBytes() >= sizeof(int32));
    int32 bety32 = 0;
    ::memcpy(&bety32, Peek(), sizeof bety32);
    m_currIndex += sizeof value;
	value = CUtil::hton32(bety32);
    return *this;
}

void CInStream::Reposition(size_t pos)
{
    m_currIndex = pos;
}

void CInStream::Skip(size_t len)
{
    assert(ReadableBytes() > len);
    m_currIndex += len;
}

void CInStream::ReadBytes(void *data, size_t len)
{
    assert(ReadableBytes() >= len);
    std::copy(m_buffer.begin() + m_currIndex,
              m_buffer.begin() + m_currIndex + len,
              static_cast<char*>(data));
    m_currIndex += len;
}

size_t CInStream::ReadableBytes() const
{
    return m_buffer.size() - m_currIndex;
}

const char *CInStream::Peek() const
{
    return &*m_buffer.begin() + m_currIndex;
}

