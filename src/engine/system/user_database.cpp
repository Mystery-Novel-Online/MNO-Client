#include "user_database.h"

static RolechatDatabase* g_DB = nullptr;

void InitDB() {
  g_DB = new RolechatDatabase();
}

void ShutdownDB() {
  delete g_DB;
  g_DB = nullptr;
}

RolechatDatabase* GetDB() {
  return g_DB;
}
