//
// Created by Razvan Lung on 2019-07-21.
//

#include "SqliteWrapper.h"

void SqliteWrapper::New(Dart_NativeArguments arguments) {
  Dart_EnterScope();
  Dart_Handle path = CheckDartError(Dart_GetNativeArgument(arguments, 0));

  sqlite3 *db = nullptr;
  const char *c_path = nullptr;
  CheckDartError(Dart_StringToCString(path, &c_path));
  CheckSqlError(db, sqlite3_open(c_path, &db));
  sqlite3_busy_timeout(db, 100);

  Dart_SetIntegerReturnValue(arguments, (intptr_t) db);
  Dart_ExitScope();
}

void SqliteWrapper::Close(Dart_NativeArguments arguments) {
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
    std::cout << "Warning: sqlite.Database.close(): " << count << " statements still open." << std::endl;
  }

  CheckSqlError(db, sqlite3_close_v2(db));
  Dart_SetReturnValue(arguments, Dart_Null());
  Dart_ExitScope();
}

void SqliteWrapper::Version(Dart_NativeArguments arguments) {
  Dart_EnterScope();
  Dart_Handle version = Dart_NewStringFromCString(sqlite3_version);
  Dart_SetReturnValue(arguments, version);
  Dart_ExitScope();
}

void SqliteWrapper::PrepareStatement(Dart_NativeArguments arguments) {
  Dart_EnterScope();
  Dart_Handle db_handle = Dart_GetNativeArgument(arguments, 0);
  Dart_Handle sql_handle = Dart_GetNativeArgument(arguments, 1);

  sqlite3 *db = nullptr;
  Dart_IntegerToUint64(db_handle, (uint64_t *) &db);
  const char *sql = nullptr;
  CheckDartError(Dart_StringToCString(sql_handle, &sql));

  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, nullptr) != SQLITE_OK) {
    Dart_Handle errmsg = Dart_NewStringFromCString(sqlite3_errmsg(db));
    Dart_Handle syntax_error = instantiator_->InstantiateLibraryObject("SqliteSyntaxError", "_", {errmsg, sql_handle});
    Dart_ThrowException(syntax_error);
    Dart_ExitScope();
    return;
  }

  Dart_Handle result = instantiator_->InstantiateLibraryObject("_StatementPeer");
  auto *peer = new StatementPeer(db, stmt);
  Dart_WeakPersistentHandle finalizer = Dart_NewWeakPersistentHandle(result, peer, 0, finalize_statement);

  peer->finalizer(finalizer);
  Dart_SetNativeInstanceField(result, 0, (intptr_t) peer);

  Dart_SetReturnValue(arguments, result);
  Dart_ExitScope();
}

void SqliteWrapper::CloseStatement(Dart_NativeArguments arguments) {
  Dart_EnterScope();

  StatementPeer *peer = getPeer(Dart_GetNativeArgument(arguments, 0));
  CheckSqlError(peer->db(), sqlite3_finalize(peer->stmt()));

  Dart_DeleteWeakPersistentHandle(Dart_CurrentIsolate(), peer->finalizer());
  delete peer;

  Dart_SetReturnValue(arguments, Dart_Null());
  Dart_ExitScope();
}

void SqliteWrapper::Reset(Dart_NativeArguments arguments) {
  Dart_EnterScope();

  StatementPeer *peer = getPeer(Dart_GetNativeArgument(arguments, 0));
  CheckSqlError(peer->db(), sqlite3_clear_bindings(peer->stmt()));
  CheckSqlError(peer->db(), sqlite3_reset(peer->stmt()));

  Dart_SetReturnValue(arguments, Dart_Null());
  Dart_ExitScope();
}

void SqliteWrapper::Bind(Dart_NativeArguments arguments) {
  Dart_EnterScope();

  StatementPeer *peer = getPeer(Dart_GetNativeArgument(arguments, 0));
  Dart_Handle args = Dart_GetNativeArgument(arguments, 1);
  if (!Dart_IsList(args)) {
    Dart_Handle messageHandle = Dart_NewStringFromCString("args must be a List");
    ThrowDartArgumentError(messageHandle);
  }

  intptr_t count;
  Dart_ListLength(args, &count);
  if (sqlite3_bind_parameter_count(peer->stmt()) != count) {
    Dart_Handle messageHandle = Dart_NewStringFromCString("Number of arguments doesn't match");
    ThrowDartArgumentError(messageHandle);
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
        ThrowDartArgumentError(message);
        return;
      }

      if (type != Dart_TypedData_kUint8) {
        CheckDartError(Dart_TypedDataReleaseData(value));
        Dart_Handle message = Dart_NewStringFromCString("Dart buffer was not a UInt8List.");
        ThrowDartArgumentError(message);
      }

      memcpy(result, data, length);
      CheckDartError(Dart_TypedDataReleaseData(value));
      CheckSqlError(peer->db(), sqlite3_bind_blob(peer->stmt(), i + 1, result, length, sqlite3_free));
    } else {
      Dart_Handle message = Dart_NewStringFromCString("Invalid parameter type.");
      ThrowDartArgumentError(message);
    }
  }

  Dart_SetReturnValue(arguments, Dart_Null());
  Dart_ExitScope();
}

void SqliteWrapper::ColumnInfo(Dart_NativeArguments arguments) {
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

void SqliteWrapper::Step(Dart_NativeArguments arguments) {
  Dart_EnterScope();

  StatementPeer *peer = getPeer(Dart_GetNativeArgument(arguments, 0));
  while (true) {
    int status = sqlite3_step(peer->stmt());
    switch (status) {
      case SQLITE_BUSY:std::cout << "Got sqlite_busy." << std::endl;
        continue;
      case SQLITE_LOCKED:std::cout << "Got sqlite_locked." << std::endl;
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
        ThrowDartArgumentError(message);
    }
  }
}

void SqliteWrapper::finalize_statement(__unused void *isolate_callback_data,
                                       __unused Dart_WeakPersistentHandle handle,
                                       void *peer) {
  static bool warned = false;
  auto *statement = (StatementPeer *) peer;
  sqlite3_finalize(statement->stmt());

  if (!warned) {
    fprintf(stderr, "Warning: sqlite.Statement was not closed before garbage collection.\n");
    warned = true;
  }

  sqlite3_free(statement);
}

StatementPeer *SqliteWrapper::getPeer(Dart_Handle stmt_handler) {
  StatementPeer *peer = nullptr;
  Dart_GetNativeInstanceField(stmt_handler, 0, (intptr_t *) &peer);
  return peer;
}

Dart_Handle SqliteWrapper::get_column_value(StatementPeer *peer, int column) {
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
        ThrowDartArgumentError(message);
        return Dart_Null();
      }

      memcpy(data, binary_data, count);
      CheckDartError(Dart_TypedDataReleaseData(result));
      return result;
    case SQLITE_NULL:return Dart_Null();
    default:Dart_Handle message = Dart_NewStringFromCString("Unknown result type.");
      ThrowDartArgumentError(message);
      return Dart_Null();
  }

}

Dart_Handle SqliteWrapper::get_last_row(StatementPeer *peer) {
  int count = sqlite3_column_count(peer->stmt());
  Dart_Handle list = CheckDartError(Dart_NewList(count));
  for (int i = 0; i < count; i++) {
    Dart_ListSetAt(list, i, get_column_value(peer, i));
  }

  return list;
}

void SqliteWrapper::ThrowDartArgumentError(const Dart_Handle &messageHandle) {
  Dart_Handle error = instantiator_->InstantiateCoreLibraryObject("ArgumentError", {messageHandle});
  Dart_ThrowException(error);
}

Dart_Handle SqliteWrapper::CheckDartError(Dart_Handle result) {
  if (Dart_IsError(result)) {
    const char *message = Dart_GetError(result);
    Dart_Handle messageHandle = Dart_NewStringFromCString(message);
    ThrowDartArgumentError(messageHandle);
  }
  return result;
}

void SqliteWrapper::CheckSqlError(sqlite3 *db, int result) {
  if (result != SQLITE_OK) {
    const char *message = sqlite3_errmsg(db);

    Dart_Handle
        error = instantiator_->InstantiateLibraryObject("SqliteError", "_", {Dart_NewStringFromCString(message)});
    Dart_ThrowException(error);
  }
}
