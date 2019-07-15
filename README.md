This library provides sqlite bindings for Dart VM.

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