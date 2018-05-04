#ifndef _OUT_STREAM_H_
#define _OUT_STREAM_H_
#include <boring.h>
NAMESPACE_BEGIN
class COutStream
{
public:
	static const size_t sInitalSize;
	COutStream();
	COutStream& operator<<(uint8 value);
	COutStream& operator<<(uint16 value);
	COutStream& operator<<(uint32 value);
	COutStream& operator<<(int8 value);
	COutStream& operator<<(int16 value);
	COutStream& operator<<(int32 value);
	COutStream& operator<<(const std::string& str);
	void WriteBytes(const void *data, size_t len);
	void Reposition(size_t pos);
	void Skip(size_t len);
	void Clear();
	char* Data();
	const char *Data() const;
	size_t Length();
private:
	size_t WriteableBytes() const;
	void EnsureWriteableBytes(size_t len);
	void Append(const char *data, size_t len);
	void Append(const void *data, size_t len);
private:
	std::vector<char> m_buffer;
	size_t m_currIndex;
};
NAMESPACE_END
#endif //_OUT_STREAM_H_
