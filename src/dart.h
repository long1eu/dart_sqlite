//
// Created by Razvan Lung on 2019-07-11.
//

#pragma once

#include <string>
#include <string.h>
#include <utility>
#include <memory>
#include <iostream>
#include <stdio.h>
#include <dart_api.h>
#include "../third_party/sqlite-amalgamation-3290000/sqlite3.h"
#include "StatementPeer.h"
#include "SqliteWrapper.h"
#include "DartObjectInstantiator.h"

static std::unique_ptr<SqliteWrapper> sqlite_wrapper;

/**
 * Called when the library is loaded and it initialize Dart methods bindings between Dart
 * native functions and C++ functions.
 */
DART_EXPORT Dart_Handle sqlite_Init(Dart_Handle parent_library);

/**
 * This function is used to map a name/arity to a Dart_NativeFunction. It returns a valid
 * Dart_NativeFunction which resolves to a native dart entry point for the native dart
 * function.
 */
Dart_NativeFunction ResolveName(Dart_Handle name, int num_of_arguments, bool *auto_setup_scope);
