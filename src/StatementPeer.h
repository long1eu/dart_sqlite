//
// Created by Razvan Lung on 2019-07-21.
//

#pragma once

#include <dart_api.h>
#include "../third_party/sqlite-amalgamation-3290000/sqlite3.h"

class StatementPeer {
 public:
  StatementPeer(sqlite3 *db, sqlite3_stmt *stmt) : db_(db), stmt_(stmt) {}

  sqlite3 *db() const { return db_; }
  sqlite3_stmt *stmt() const { return stmt_; }
  Dart_WeakPersistentHandle finalizer() const { return finalizer_; }
  void finalizer(Dart_WeakPersistentHandle finalizer) { finalizer_ = finalizer; }

 private:
  sqlite3 *db_;
  sqlite3_stmt *stmt_;
  Dart_WeakPersistentHandle finalizer_{};
};
