#ifndef _IN_STREAM_H_
#define _IN_STREAM_H_
#include <boring.h>
NAMESPACE_BEGIN
class CInStream
{
public:
	CInStream();
	CInStream(const char *data, size_t len);
	void SetData(const char *data, size_t len);
	CInStream& operator>>(uint8 &value);
	CInStream& operator>>(uint16 &value);
	CInStream& operator>>(uint32 &value);
	CInStream& operator>>(int8 &value);
	CInStream& operator>>(int16 &value);
	CInStream& operator>>(int32 &value);
	void Reposition(size_t pos);
	void Skip(size_t len);
	void ReadBytes(void *data, size_t len);
private:
	size_t ReadableBytes() const;
	const char *Peek() const;
private:
	std::vector<char> m_buffer;
	size_t m_currIndex;
};
NAMESPACE_END
#endif // _IN_STREAM_H_
