#ifndef EXECPTION_H
#define EXECPTION_H
#include <boring.h>
using namespace std;
class CException : public exception
{
public:
    CException(const char *errmsg) : errmsg(nullptr)
    {
        this->errmsg = _strdup(errmsg);
    }
    virtual ~CException()
    {
        if (errmsg != nullptr)
        {
            free(errmsg);
            errmsg = nullptr;
        }
        cout << "virtual ~CException().." << endl;
    }
    virtual const char *what() const noexcept
    {
        return errmsg;
    }
private:
    char *errmsg;
};

#endif // EXECPTION_H
