#include "rolestring.h"

roleString::roleString(const char *ch, int code)
    : QString(ch)
{
    roleCode = code;
}

int roleString::getRoleCode()
{
    return roleCode;
}
