#include "user_database.h"

RolechatDatabase& GetDB() {
  return RolechatDatabase::instance();
}
