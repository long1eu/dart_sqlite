//
// Created by Razvan Lung on 2019-07-11.
//

#pragma once

#include <dart_api.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include "../third_party/sqlite-amalgamation-3290000/sqlite3.h"

using namespace std;

#define EXPORT(func, args) if (!strcmp(#func, cname) && argc == args) { return func; }

DART_EXPORT Dart_Handle sqlite_Init(Dart_Handle parent_library);

Dart_NativeFunction ResolveName(Dart_Handle name, int num_of_arguments, bool *auto_setup_scope);

void New(Dart_NativeArguments arguments);
void Close(Dart_NativeArguments arguments);
void Version(Dart_NativeArguments arguments);
void PrepareStatement(Dart_NativeArguments arguments);
void CloseStatement(Dart_NativeArguments arguments);
void Reset(Dart_NativeArguments arguments);
void Bind(Dart_NativeArguments arguments);
void ColumnInfo(Dart_NativeArguments arguments);
void Step(Dart_NativeArguments arguments);