//
// Created by Razvan Lung on 2019-07-11.
//

#include "dart.h"

static Dart_PersistentHandle library;

DART_EXPORT Dart_Handle sqlite_Init(Dart_Handle parent_library) {
  if (Dart_IsError(parent_library)) {
    return parent_library;
  }

  library = Dart_NewPersistentHandle(parent_library);

  Dart_Handle result_code = Dart_SetNativeResolver(parent_library, ResolveName, nullptr);
  if (Dart_IsError(result_code)) {
    return result_code;
  }

  return Dart_Null();
}

Dart_NativeFunction ResolveName(Dart_Handle name, int argc, bool *auto_setup_scope) {
  assert(Dart_IsString(name));
  const char *cname;
  Dart_Handle check_error = Dart_StringToCString(name, &cname);
  if (Dart_IsError(check_error)) Dart_PropagateError(check_error);
  *auto_setup_scope = false;

  EXPORT(New, 1)
  EXPORT(Close, 1)
  EXPORT(Version, 0)
  EXPORT(PrepareStatement, 2)
  EXPORT(CloseStatement, 1)
  EXPORT(Reset, 1)
  EXPORT(Bind, 2)
  EXPORT(Step, 1)
  EXPORT(ColumnInfo, 1)
  return nullptr;
}

class StatementPeer {
 public:
  StatementPeer(sqlite3 *db, sqlite3_stmt *stmt)
      : db_(db), stmt_(stmt) {}

  sqlite3 *db() const { return db_; }
  sqlite3_stmt *stmt() const { return stmt_; }
  Dart_WeakPersistentHandle finalizer() const { return finalizer_; }
  void finalizer(Dart_WeakPersistentHandle finalizer) { finalizer_ = finalizer; }

 private:
  sqlite3 *db_;
  sqlite3_stmt *stmt_;
  Dart_WeakPersistentHandle finalizer_{};
};

void throwArgumentError(Dart_Handle &messageHandle) {
  Dart_Handle exceptionClass =
      Dart_GetClass(Dart_LookupLibrary(Dart_NewStringFromCString("dart:core")),
                    Dart_NewStringFromCString("ArgumentError"));

  Dart_ThrowException(Dart_New(exceptionClass, Dart_Null(), 1, &messageHandle));
}

Dart_Handle CheckDartError(Dart_Handle result) {
  if (Dart_IsError(result)) {
    const char *message = Dart_GetError(result);
    Dart_Handle messageHandle = Dart_NewStringFromCString(message);
    throwArgumentError(messageHandle);
  }
  return result;
}

void CheckSqlError(sqlite3 *db, int result) {
  if (result != SQLITE_OK) {
    const char *message = sqlite3_errmsg(db);
    cout << message << endl;
    Dart_Handle messageHandle = Dart_NewStringFromCString(message);
    Dart_Handle exceptionClass = Dart_GetClass(library, Dart_NewStringFromCString("SqliteError"));

    Dart_ThrowException(Dart_New(exceptionClass, Dart_NewStringFromCString("_"), 1, &messageHandle));
  }
}

void New(Dart_NativeArguments arguments) {
  Dart_EnterScope();
  Dart_Handle path = CheckDartError(Dart_GetNativeArgument(arguments, 0));

  sqlite3 *db = nullptr;
  const char *c_path;
  CheckDartError(Dart_StringToCString(path, &c_path));
  CheckSqlError(db, sqlite3_open(c_path, &db));
  sqlite3_busy_timeout(db, 100);

  Dart_SetIntegerReturnValue(arguments, (uint64_t) db);
  Dart_ExitScope();
}

void Close(Dart_NativeArguments arguments) {
  Dart_EnterScope();

  Dart_Handle db_handle = Dart_GetNativeArgument(arguments, 0);
  sqlite3 *db = nullptr;
  Dart_IntegerToUint64(db_handle, (uint64_t *) &db);

  sqlite3_stmt *statement = nullptr;
  int count = 0;
  while ((statement = sqlite3_next_stmt(db, statement))) {
    sqlite3_finalize(statement);
    count++;
  }

  if (count) {
    cout << "Warning: sqlite.Database.close(): " << count << " statements still open." << endl;
  }

  CheckSqlError(db, sqlite3_close_v2(db));
  Dart_SetReturnValue(arguments, Dart_Null());
  Dart_ExitScope();
}

void Version(Dart_NativeArguments arguments) {
  Dart_EnterScope();
  Dart_Handle version = Dart_NewStringFromCString(sqlite3_version);
  Dart_SetReturnValue(arguments, version);
  Dart_ExitScope();
}

void finalize_statement(void *isolate_callback_data, Dart_WeakPersistentHandle handle, void *peer) {
  static bool warned = false;
  auto *statement = (StatementPeer *) peer;
  sqlite3_finalize(statement->stmt());

  if (!warned) {
    fprintf(stderr, "Warning: sqlite.Statement was not closed before garbage collection.\n");
    warned = true;
  }

  sqlite3_free(statement);
}

void PrepareStatement(Dart_NativeArguments arguments) {
  Dart_EnterScope();
  Dart_Handle db_handle = Dart_GetNativeArgument(arguments, 0);
  Dart_Handle sql_handle = Dart_GetNativeArgument(arguments, 1);

  sqlite3 *db = nullptr;
  Dart_IntegerToUint64(db_handle, (uint64_t *) &db);
  const char *sql;
  CheckDartError(Dart_StringToCString(sql_handle, &sql));

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, nullptr) != SQLITE_OK) {
    Dart_Handle params[2];
    params[0] = Dart_NewStringFromCString(sqlite3_errmsg(db));
    params[1] = sql_handle;

    Dart_Handle syntaxErrorClass =
        CheckDartError(Dart_GetClass(library, Dart_NewStringFromCString("SqliteSyntaxError")));
    Dart_ThrowException(Dart_New(syntaxErrorClass, Dart_NewStringFromCString("_"), 2, params));
  }

  Dart_Handle cls = Dart_GetClass(library, Dart_NewStringFromCString("_StatementPeer"));
  Dart_Handle result = Dart_New(cls, Dart_Null(), 0, nullptr);

  auto *peer = new StatementPeer(db, stmt);
  Dart_WeakPersistentHandle finalizer = Dart_NewWeakPersistentHandle(result, peer, 0, finalize_statement);
  peer->finalizer(finalizer);
  Dart_SetNativeInstanceField(result, 0, (intptr_t) peer);

  Dart_SetReturnValue(arguments, result);
  Dart_ExitScope();
}

StatementPeer *getPeer(Dart_Handle stmt_handler) {
  StatementPeer *peer = nullptr;
  Dart_GetNativeInstanceField(stmt_handler, 0, (intptr_t *) &peer);
  return peer;
}

void CloseStatement(Dart_NativeArguments arguments) {
  Dart_EnterScope();

  StatementPeer *peer = getPeer(Dart_GetNativeArgument(arguments, 0));
  CheckSqlError(peer->db(), sqlite3_finalize(peer->stmt()));

  Dart_DeleteWeakPersistentHandle(Dart_CurrentIsolate(), peer->finalizer());
  delete peer;

  Dart_SetReturnValue(arguments, Dart_Null());
  Dart_ExitScope();
}

void Reset(Dart_NativeArguments arguments) {
  Dart_EnterScope();

  StatementPeer *peer = getPeer(Dart_GetNativeArgument(arguments, 0));
  CheckSqlError(peer->db(), sqlite3_clear_bindings(peer->stmt()));
  CheckSqlError(peer->db(), sqlite3_reset(peer->stmt()));

  Dart_SetReturnValue(arguments, Dart_Null());
  Dart_ExitScope();
}

void Bind(Dart_NativeArguments arguments) {
  Dart_EnterScope();

  StatementPeer *peer = getPeer(Dart_GetNativeArgument(arguments, 0));
  Dart_Handle args = Dart_GetNativeArgument(arguments, 1);
  if (!Dart_IsList(args)) {
    Dart_Handle messageHandle = Dart_NewStringFromCString("args must be a List");
    throwArgumentError(messageHandle);
  }

  intptr_t count;
  Dart_ListLength(args, &count);
  if (sqlite3_bind_parameter_count(peer->stmt()) != count) {
    Dart_Handle messageHandle = Dart_NewStringFromCString("Number of arguments doesn't match");
    throwArgumentError(messageHandle);
  }

  for (int i = 0; i < count; i++) {
    Dart_Handle value = Dart_ListGetAt(args, i);

    if (Dart_IsInteger(value)) {
      uint64_t result;
      Dart_IntegerToUint64(value, &result);
      CheckSqlError(peer->db(), sqlite3_bind_int64(peer->stmt(), i + 1, result));
    } else if (Dart_IsDouble(value)) {
      double result;
      Dart_DoubleValue(value, &result);
      CheckSqlError(peer->db(), sqlite3_bind_double(peer->stmt(), i + 1, result));
    } else if (Dart_IsNull(value)) {
      CheckSqlError(peer->db(), sqlite3_bind_null(peer->stmt(), i + 1));
    } else if (Dart_IsString(value)) {
      const char *result;
      Dart_StringToCString(value, &result);
      CheckSqlError(peer->db(), sqlite3_bind_text(peer->stmt(), i + 1, result, strlen(result), SQLITE_TRANSIENT));
    } else if (Dart_IsTypedData(value)) {
      Dart_TypedData_Type type;
      unsigned char *data;
      intptr_t length;
      CheckDartError(Dart_TypedDataAcquireData(value, &type, (void **) &data, &length));
      auto *result = (unsigned char *) sqlite3_malloc(length);

      if (length < 0) {
        CheckDartError(Dart_TypedDataReleaseData(value));
        Dart_Handle message = Dart_NewStringFromCString("Dart buffer was too small");
        throwArgumentError(message);
        return;
      }

      if (type != Dart_TypedData_kUint8) {
        CheckDartError(Dart_TypedDataReleaseData(value));
        Dart_Handle message = Dart_NewStringFromCString("Dart buffer was not a UInt8List.");
        throwArgumentError(message);
      }

      memcpy(result, data, length);
      CheckDartError(Dart_TypedDataReleaseData(value));
      CheckSqlError(peer->db(), sqlite3_bind_blob(peer->stmt(), i + 1, result, length, sqlite3_free));
    } else {
      Dart_Handle message = Dart_NewStringFromCString("Invalid parameter type.");
      throwArgumentError(message);
    }
  }

  Dart_SetReturnValue(arguments, Dart_Null());
  Dart_ExitScope();
}

void ColumnInfo(Dart_NativeArguments arguments) {
  Dart_EnterScope();

  StatementPeer *peer = getPeer(Dart_GetNativeArgument(arguments, 0));
  int count = sqlite3_column_count(peer->stmt());
  Dart_Handle result = Dart_NewList(count);
  for (int i = 0; i < count; i++) {
    Dart_ListSetAt(result, i, Dart_NewStringFromCString(sqlite3_column_name(peer->stmt(), i)));
  }

  Dart_SetReturnValue(arguments, result);
  Dart_ExitScope();
}

Dart_Handle get_column_value(StatementPeer *peer, int column) {
  int count;
  const unsigned char *binary_data = nullptr;
  Dart_Handle result = nullptr;
  switch (sqlite3_column_type(peer->stmt(), column)) {
    case SQLITE_INTEGER:return Dart_NewInteger(sqlite3_column_int64(peer->stmt(), column));
    case SQLITE_FLOAT:return Dart_NewDouble(sqlite3_column_double(peer->stmt(), column));
    case SQLITE_TEXT:return Dart_NewStringFromCString((const char *) sqlite3_column_text(peer->stmt(), column));
    case SQLITE_BLOB:count = sqlite3_column_bytes(peer->stmt(), column);
      result = CheckDartError(Dart_NewTypedData(Dart_TypedData_kUint8, count));
      binary_data = (const unsigned char *) sqlite3_column_blob(peer->stmt(), column);
      Dart_TypedData_Type type;
      unsigned char *data;
      intptr_t length;
      CheckDartError(Dart_TypedDataAcquireData(result, &type, (void **) &data, &length));

      if (length < count) {
        CheckDartError(Dart_TypedDataReleaseData(result));
        Dart_Handle message = Dart_NewStringFromCString("Dart buffer was too small");
        throwArgumentError(message);
        return Dart_Null();
      }

      memcpy(data, binary_data, count);
      CheckDartError(Dart_TypedDataReleaseData(result));
      return result;
    case SQLITE_NULL:return Dart_Null();
    default:Dart_Handle message = Dart_NewStringFromCString("Unknown result type.");
      throwArgumentError(message);
      return Dart_Null();
  }

}

Dart_Handle get_last_row(StatementPeer *peer) {
  int count = sqlite3_column_count(peer->stmt());
  Dart_Handle list = CheckDartError(Dart_NewList(count));
  for (int i = 0; i < count; i++) {
    Dart_ListSetAt(list, i, get_column_value(peer, i));
  }

  return list;
}

void Step(Dart_NativeArguments arguments) {
  Dart_EnterScope();

  StatementPeer *peer = getPeer(Dart_GetNativeArgument(arguments, 0));
  while (true) {
    int status = sqlite3_step(peer->stmt());
    switch (status) {
      case SQLITE_BUSY:cout << "Got sqlite_busy." << endl;
        continue;
      case SQLITE_LOCKED:cout << "Got sqlite_locked." << endl;
        continue;
      case SQLITE_DONE:
        // Note: sqlite3_changes will stil return a non-0 value for statements
        // which don't affect rows (e.g. SELECT). It simply returns the number
        // of changes by the last row-altering statement.
        Dart_SetIntegerReturnValue(arguments, sqlite3_changes(peer->db()));
        Dart_ExitScope();
        return;
      case SQLITE_ROW:Dart_SetReturnValue(arguments, get_last_row(peer));
        Dart_ExitScope();
        return;
      default:CheckSqlError(peer->db(), status);
        Dart_Handle message = Dart_NewStringFromCString("Unreachable");
        throwArgumentError(message);
    }
  }
}
