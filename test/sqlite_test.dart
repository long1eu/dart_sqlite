// File created by
// Lung Razvan <long1eu>
// on 2019-07-14

import 'dart:io';

import 'package:dart_sqlite/dart_sqlite.dart';
import 'package:path/path.dart';
import 'package:test/test.dart';

Future<void> _createBlogTable(Database db) async => db.execute('CREATE TABLE posts(title TEXT, body TEXT);');

typedef _DatabaseTest = Future<void> Function(Database db);

Future<void> _runWithConnectionOnDisk(_DatabaseTest dbTest) async {
  final String fileName = join(Directory.systemTemp.createTempSync('dart-sqlite-test-').path, 'db.sqlite');
  final Database db = Database(fileName);
  return dbTest(db).whenComplete(() => db.close());
}

Future<void> _runWithConnectionInMemory(_DatabaseTest dbTest) async {
  final Database db = new Database.inMemory();
  return dbTest(db).whenComplete(() => db.close());
}

Future<void> Function() _testRunner(_DatabaseTest dbTest) {
  return () {
    return Future.wait([
      _runWithConnectionOnDisk(dbTest),
      _runWithConnectionInMemory(dbTest),
    ]);
  };
}

void main() {
  test('query with bindings', _testRunner((db) async {
    final Row row = await db.query('SELECT ?+2, UPPER(?)', <dynamic>[3, 'hello']).first;
    expect(row[0], 5);
    expect(row[1], 'HELLO');
  }));

  test('query', _testRunner((db) async {
    final Row row = await db.query('SELECT 42 AS foo').first;

    expect(row.index, 0);
    expect(row[0], 42);
    expect(row['foo'], 42);
    expect(row.toList(), const [42]);
    expect(row.toMap(), const {'foo': 42});
  }));

  test('query multiple', _testRunner((db) async {
    await _createBlogTable(db);
    Future<void> insert(List bindings) async {
      final int inserted = await db.execute('INSERT INTO posts (title, body) VALUES (?,?)', bindings);
      expect(inserted, 1);
    }

    await insert(['hi', 'hello world']);
    await insert(['bye', 'goodbye cruel world']);
    final List<Row> rows = await db.query('SELECT * FROM posts').toList();
    expect(rows.length, 2);
    expect(rows[0]['title'], 'hi');
    expect(rows[1]['title'], 'bye');
    expect(rows[0].index, 0);
    expect(rows[1].index, 1);
  }));

  test('transaction success', _testRunner((db) async {
    await _createBlogTable(db);
    await db.transaction(() => db.execute('INSERT INTO posts (title, body) VALUES (?,?)'));
    expect(await db.query('SELECT * FROM posts').length, 1);
  }));

  test('transaction failure', _testRunner((db) async {
    return expectLater(expectAsync0(() => db.transaction(() => throw 'oh noes!')), throwsA(isA<String>()));
  }));

  test('syntax error', _testRunner((db) async {
    return expectLater(() => db.execute('random non sql'), throwsA(isA<SqliteSyntaxError>()));
  }));

  test('column error', _testRunner((db) async {
    final Row row = await db.query('select 2+2').first;
    expect(() => row['qwerty'], throwsA(isA<SqliteError>()));
  }));

  test('dynamic getters', _testRunner((db) async {
    await _createBlogTable(db);
    final int inserted = await db.execute('INSERT INTO posts (title, body) VALUES ("hello", "world")');
    expect(inserted, 1);

    final List<dynamic> rows = await db.query('SELECT * FROM posts').toList();
    expect(rows.length, 1);
    expect(rows[0].title, 'hello');
    expect(rows[0].body, 'world');
  }));
}
