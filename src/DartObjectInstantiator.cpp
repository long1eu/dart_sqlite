//
// Created by Razvan Lung on 2019-07-21.
//

#include "DartObjectInstantiator.h"

Dart_Handle DartObjectInstantiator::InstantiateCoreLibraryObject(const std::string &className,
                                                                 const std::vector<Dart_Handle> &arguments) {
  Dart_Handle _class = Dart_GetClass(core(), Dart_NewStringFromCString(className.c_str()));

  int number_of_arguments = arguments.size();
  Dart_Handle _args[number_of_arguments];
  for (int i = 0; i < number_of_arguments; ++i) {
    _args[i] = arguments[i];
  }

  return Dart_New(_class, Dart_Null(), number_of_arguments, _args);
}

Dart_Handle DartObjectInstantiator::InstantiateLibraryObject(const std::string &className) {
  Dart_Handle _class = Dart_GetClass(library(), Dart_NewStringFromCString(className.c_str()));
  return Dart_New(_class, Dart_Null(), 0, nullptr);
}

Dart_Handle DartObjectInstantiator::InstantiateLibraryObject(const std::string &className,
                                                             const std::string &constructorName,
                                                             const std::vector<Dart_Handle> &arguments) {
  Dart_Handle _class = Dart_GetClass(library(), Dart_NewStringFromCString(className.c_str()));
  Dart_Handle _constructor = Dart_NewStringFromCString(constructorName.c_str());

  int number_of_arguments = arguments.size();
  Dart_Handle _args[number_of_arguments];
  for (int i = 0; i < number_of_arguments; ++i) {
    _args[i] = arguments[i];
  }

  return Dart_New(_class, _constructor, number_of_arguments, _args);
}

const Dart_Handle DartObjectInstantiator::library() {
  library_ =
      Dart_NewPersistentHandle(Dart_LookupLibrary(Dart_NewStringFromCString("package:dart_sqlite/src/sqlite.dart")));
  return Dart_HandleFromPersistent(library_);
}

const Dart_Handle DartObjectInstantiator::core() {
  // We lazy initialize this so we don't delay the startup
  if (core_ == nullptr || !Dart_IsLibrary(Dart_HandleFromPersistent(core_))) {
    core_ =
        Dart_NewPersistentHandle(Dart_LookupLibrary(Dart_NewStringFromCString("dart:core")));
  }

  return Dart_HandleFromPersistent(core_);
}
