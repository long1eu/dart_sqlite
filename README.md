# Dart Sqlite

This library provides sqlite bindings for Dart VM. Dart is a client-optimized **programming language** for fast apps on multiple platforms. It is developed by Google and is used to build mobile, desktop, backend and web applications. Dart is an object-oriented, class defined, garbage-collected language using a C-style syntax that transcompiles optionally into JavaScript. It supports interfaces, mixins, abstract classes, reified generics, static typing, and a sound type system. Since there is no _sqlite_ implementation written in Dart, this library follows the official [guide](https://dart.dev/server/c-interop-native-extensions) to build sqlite support for for Dart. 
The Dart SDK ships with a stand-alone Dart VM, allowing Dart code to run in a command-line interface environment. This project aims to make it possible for the Dart client to use sqlite. 

## Synopsis

* bring sqlite support for Dart programing language
* doing so by:

     a. using C++ code that can communicates with the Dart code
    
     b. manage memory allocation in C++ in tandem with the Dart Garbage Collector 

## Getting Started

These instructions will get you a copy of the project up and running on your local machine. 

**Linux run guide**

```
sudo apt-get update
sudo apt-get install git curl build-essential apt-transport-https
sudo sh -c 'curl https://dl-ssl.google.com/linux/linux_signing_key.pub | apt-key add -'
sudo sh -c 'curl https://storage.googleapis.com/download.dartlang.org/linux/debian/dart_stable.list > /etc/apt/sources.list.d/dart_stable.list'
sudo apt-get update
sudo apt-get install dart
git clone https://github.com/long1eu/dart_sqlite
cd dart_sqlite
mkdir build && cd build
cmake ..
make
cd ../dart
/usr/lib/dart/bin/pub get
/usr/lib/dart/bin/dart example/statements.dart
```

or just run the `run.sh`

### Prerequisites

* cmake (min version 3.5.1)
* C/C++ compilers
* Dart SDK (version 2.4.0)

### Running

To run this library you need to run the Dart code that depends on it. `example/statements.dart` contains an example app that stores and retrieves values from an in memory sqlite database.

From the root dir:

```
dart ./dart/example/statements.dart
```
This should output the following:
```
-- LIGUE 1 2019 --
2 teams competing
4 teams competing now
PSG        78
Monaco     41
Lyon       39
Nice       38
Who's the best now?
```

### Special considerations
* Since the C++ code works closely with the Dart VM, the use of raw pointers was necessary to make sure the memory was not freed while Dart code was still depending on it.

## Fulfilled requirements
1. The project demonstrates an understanding of C++ functions and control structures.
    
    * [./src/dart.cpp:7](https://github.com/long1eu/dart_sqlite/blob/master/src/dart.cpp#L7)
    * [./src/dart.cpp:34](https://github.com/long1eu/dart_sqlite/blob/master/src/dart.cpp#L34)
    * [./src/SqliteWrapper.cpp:105](https://github.com/long1eu/dart_sqlite/blob/master/src/SqliteWrapper.cpp#L105)
    
2. The project reads data from a file and process the data, or the program writes data to a file.
    
    * [./src/SqliteWrapper.cpp:14](https://github.com/long1eu/dart_sqlite/blob/master/src/SqliteWrapper.cpp#L14)
    * [./src/SqliteWrapper.cpp:235](https://github.com/long1eu/dart_sqlite/blob/master/src/SqliteWrapper.cpp#L235)
    
3. The project uses Object Oriented Programming techniques.

    * [./src/SqliteWrapper.h:23](https://github.com/long1eu/dart_sqlite/blob/master/src/SqliteWrapper.h#L23)
    * [./src/SqliteWrapper.h:31](https://github.com/long1eu/dart_sqlite/blob/master/src/SqliteWrapper.h#L31)
    * [./src/SqliteWrapper.h:50](https://github.com/long1eu/dart_sqlite/blob/master/src/SqliteWrapper.h#L50)
    
4. Classes use appropriate access specifiers for class members.
    
    * [./src/DartObjectInstantiator.h:13](https://github.com/long1eu/dart_sqlite/blob/master/src/DartObjectInstantiator.h#L13)
    * [./src/DartObjectInstantiator.h:28](https://github.com/long1eu/dart_sqlite/blob/master/src/DartObjectInstantiator.h#L28)
    
5. Class constructors utilize member initialization lists.
    
    * [./src/StatementPeer.h:12](https://github.com/long1eu/dart_sqlite/blob/master/src/StatementPeer.h#L12)
    * [./src/SqliteWrapper.h:24](https://github.com/long1eu/dart_sqlite/blob/master/src/SqliteWrapper.h#L24)     
         
    
6. Classes abstract implementation details from their interfaces.

    * [./src/dart.h:21](https://github.com/long1eu/dart_sqlite/blob/master/src/dart.h#L21)     
    * [./src/dart.h:27](https://github.com/long1eu/dart_sqlite/blob/master/src/dart.h#L27)     
    
7. Classes encapsulate behavior.

    * [./src/DartObjectInstantiator.h:25](https://github.com/long1eu/dart_sqlite/blob/master/src/DartObjectInstantiator.h#L25)
    * [./src/DartObjectInstantiator.cpp:40](https://github.com/long1eu/dart_sqlite/blob/master/src/DartObjectInstantiator.cpp#L40)
    * [./src/DartObjectInstantiator.h:26](https://github.com/long1eu/dart_sqlite/blob/master/src/DartObjectInstantiator.h#L26)
    * [./src/DartObjectInstantiator.cpp:50](https://github.com/long1eu/dart_sqlite/blob/master/src/DartObjectInstantiator.cpp#L50)
    
8. Overloaded functions allow the same function to operate on different parameters.

    * [./src/DartObjectInstantiator.h:19](https://github.com/long1eu/dart_sqlite/blob/master/src/DartObjectInstantiator.h#L19)
    * [./src/DartObjectInstantiator.h:23](https://github.com/long1eu/dart_sqlite/blob/master/src/DartObjectInstantiator.h#L23)
    
9. The project makes use of references in function declarations.
    
    * [./src/DartObjectInstantiator.h:19](https://github.com/long1eu/dart_sqlite/blob/master/src/DartObjectInstantiator.h#L19)
    * [./src/DartObjectInstantiator.h:23](https://github.com/long1eu/dart_sqlite/blob/master/src/DartObjectInstantiator.h#L23)
    
10. The project uses scope / Resource Acquisition Is Initialization (RAII) where appropriate.

    * [./src/SqliteWrapper.cpp:176](https://github.com/long1eu/dart_sqlite/blob/master/src/SqliteWrapper.cpp#L176)                 
    * [./src/SqliteWrapper.cpp:217](https://github.com/long1eu/dart_sqlite/blob/master/src/SqliteWrapper.cpp#L217)                 