//
// Created by Razvan Lung on 2019-07-21.
//

#pragma once

#include <dart_api.h>
#include <string>
#include <iostream>
#include <vector>

class DartObjectInstantiator {
 public:
  DartObjectInstantiator() = default;

  Dart_Handle InstantiateCoreLibraryObject(const std::string &className,
                                           const std::vector<Dart_Handle> &arguments);

  Dart_Handle InstantiateLibraryObject(const std::string &className,
                                       const std::string &constructorName,
                                       const std::vector<Dart_Handle> &arguments);

  Dart_Handle InstantiateLibraryObject(const std::string &className);

  const Dart_Handle library();
  const Dart_Handle core();

 private:
  Dart_PersistentHandle library_ = nullptr;
  Dart_PersistentHandle core_ = nullptr;
};


