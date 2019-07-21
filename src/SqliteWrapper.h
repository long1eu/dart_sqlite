//
// Created by Razvan Lung on 2019-07-21.
//

#pragma once

#include <iostream>
#include <cstring>
#include <dart_api.h>
#include "../third_party/sqlite-amalgamation-3290000/sqlite3.h"
#include "StatementPeer.h"
#include "DartObjectInstantiator.h"

#ifdef _WIN32
#define __unused [[maybe_unused]]
#endif

#ifdef __linux__
#define __unused  __attribute__((unused))
#endif

class SqliteWrapper {

 public:
  explicit SqliteWrapper(DartObjectInstantiator *instantiator) : instantiator_(instantiator) {}

  static void Version(Dart_NativeArguments arguments);
  static void ColumnInfo(Dart_NativeArguments arguments);

  void New(Dart_NativeArguments arguments);
  void Close(Dart_NativeArguments arguments);
  void PrepareStatement(Dart_NativeArguments arguments);
  void CloseStatement(Dart_NativeArguments arguments);
  void Reset(Dart_NativeArguments arguments);
  void Bind(Dart_NativeArguments arguments);
  void Step(Dart_NativeArguments arguments);

 private:
  // sqlite
  static void finalize_statement(void *isolate_callback_data, Dart_WeakPersistentHandle handle, void *peer);
  static StatementPeer *getPeer(Dart_Handle stmt_handler);

  Dart_Handle get_column_value(StatementPeer *peer, int column);
  Dart_Handle get_last_row(StatementPeer *peer);

  // error
  void ThrowDartArgumentError(const Dart_Handle &messageHandle);
  void CheckSqlError(sqlite3 *db, int result);
  Dart_Handle CheckDartError(Dart_Handle result);

  DartObjectInstantiator *instantiator_;
};
