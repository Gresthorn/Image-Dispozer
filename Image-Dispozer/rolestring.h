#ifndef ROLESTRING_H
#define ROLESTRING_H

#include <QString>

class roleString : public QString
{
private:
    int roleCode;

public:
    roleString(const char *ch, int code);
    int getRoleCode(void);
};

#endif // ROLESTRING_H
