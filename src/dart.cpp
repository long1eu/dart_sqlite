//
// Created by Razvan Lung on 2019-07-11.
//

#include "dart.h"

DART_EXPORT Dart_Handle sqlite_Init(Dart_Handle parent_library) {
  if (Dart_IsError(parent_library)) {
    return parent_library;
  }

  DartObjectInstantiator instantiator = DartObjectInstantiator();
  sqlite_wrapper = std::make_unique<SqliteWrapper>(&instantiator);

  // Sets the callback used to resolve Dart native functions for the calling Dart library.
  Dart_Handle result_code = Dart_SetNativeResolver(parent_library, ResolveName, nullptr);
  if (Dart_IsError(result_code)) {
    return result_code;
  }

  return Dart_Null();
}

void New(Dart_NativeArguments arguments) { sqlite_wrapper->New(arguments); }
void Close(Dart_NativeArguments arguments) { sqlite_wrapper->Close(arguments); }
void Version(Dart_NativeArguments arguments) { sqlite_wrapper->Version(arguments); }
void PrepareStatement(Dart_NativeArguments arguments) { sqlite_wrapper->PrepareStatement(arguments); }
void CloseStatement(Dart_NativeArguments arguments) { sqlite_wrapper->CloseStatement(arguments); }
void Reset(Dart_NativeArguments arguments) { sqlite_wrapper->Reset(arguments); }
void Bind(Dart_NativeArguments arguments) { sqlite_wrapper->Bind(arguments); }
void Step(Dart_NativeArguments arguments) { sqlite_wrapper->Step(arguments); }
void ColumnInfo(Dart_NativeArguments arguments) { sqlite_wrapper->ColumnInfo(arguments); }

Dart_NativeFunction ResolveName(Dart_Handle name, int num_of_arguments, bool *auto_setup_scope) {
  assert(Dart_IsString(name));
  const char *cname;
  Dart_Handle check_error = Dart_StringToCString(name, &cname);
  if (Dart_IsError(check_error)) Dart_PropagateError(check_error);
  *auto_setup_scope = false;

  if (!strcmp("New", cname) && num_of_arguments == 1) { return New; }
  else if (!strcmp("Close", cname) && num_of_arguments == 1) { return Close; }
  else if (!strcmp("Version", cname) && num_of_arguments == 0) { return Version; }
  else if (!strcmp("PrepareStatement", cname) && num_of_arguments == 2) { return PrepareStatement; }
  else if (!strcmp("CloseStatement", cname) && num_of_arguments == 1) { return CloseStatement; }
  else if (!strcmp("Reset", cname) && num_of_arguments == 1) { return Reset; }
  else if (!strcmp("Bind", cname) && num_of_arguments == 2) { return Bind; }
  else if (!strcmp("Step", cname) && num_of_arguments == 1) { return Step; }
  else if (!strcmp("ColumnInfo", cname) && num_of_arguments == 1) { return ColumnInfo; }
  else { return nullptr; }
}