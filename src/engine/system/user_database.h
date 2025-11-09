#ifndef USER_DATABASE_H
#define USER_DATABASE_H

#include <rolechat/userdata/RolechatDatabase.h>

RolechatDatabase* GetDB();
void InitDB();
void ShutdownDB();

#endif // USER_DATABASE_H
