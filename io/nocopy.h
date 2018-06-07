#ifndef _NOCOPY_H_
#define _NOCOPY_H_

class nocopy
{
public:
	nocopy(){}
	virtual ~nocopy(){}
private:
	nocopy(const nocopy& cm);
	nocopy& operator=(const nocopy& cm);
};

#endif //_NOCOPY_H_