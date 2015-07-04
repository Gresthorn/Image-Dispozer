#include "rolestring.h"

roleString::roleString(const char *ch, int code, image_groups group)
    : QString(ch)
{
    roleCode = code;
    group_assignment = group;
}

int roleString::getRoleCode()
{
    return roleCode;
}
