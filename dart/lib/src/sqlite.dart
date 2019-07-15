// ignore_for_file: unused_import

import 'dart:async';
import 'dart:nativewrappers';

import 'package:dart_sqlite/dart_sqlite.dart';

import 'dart-ext:sqlite';

part 'native.dart';
part 'row.dart';
part 'sqlite_statement.dart';

class Database {
  Database(this._path) : _handler = _open(_path);

  Database.inMemory() : this(':memory:');

  final String _path;
  final int _handler;
  bool _closed = false;

  static String __version;

  static String get version => __version ??= _version;

  void close() {
    _checkClosed();
    _closed = true;
    _close(_handler);
  }

  Future<int> transaction(Future<void> Function() operation) async {
    _checkClosed();

    try {
      await execute('BEGIN');
      await operation();
      return execute('COMMIT');
    } catch (e) {
      await execute('ROLLBACK');
      return Future.error(e);
    }
  }

  Future<int> execute(String sql, [List<dynamic> params = const <dynamic>[]]) {
    _checkClosed();

    return SqliteStatement._(this, sql, params).execute();
  }

  Stream<Row> query(String sql, [List<dynamic> params = const <dynamic>[]]) {
    _checkClosed();
    return SqliteStatement._(this, sql, params).query();
  }

  void _checkClosed() {
    if (_closed) {
      throw SqliteError('Database $_path closed.');
    }
  }

  SqliteStatement prepareStatement(String sql) => SqliteStatement._(this, sql);

  @override
  String toString() =>
      'Sqlite{path: $_path, handler: ${_closed ? '_closed_' : '0x${_handler.toRadixString(16)}'}, version: $version}';
}
