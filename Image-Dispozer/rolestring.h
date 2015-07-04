#ifndef ROLESTRING_H
#define ROLESTRING_H

#include <QString>

#include "image_handler.h" // because of image_groups enumeration type

class roleString : public QString
{
private:
    int roleCode;
    image_groups group_assignment;

public:
    roleString(const char *ch, int code, image_groups group = NONE);
    int getRoleCode(void);
    image_groups getRoleGroup(void) { return group_assignment; }
};

#endif // ROLESTRING_H
