// File created by
// Lung Razvan <long1eu>
// on 2019-07-14

import 'package:dart_sqlite/dart_sqlite.dart';

Future<void> _runSimpleQuery(Database db) async {
  Row row = await db.query('SELECT ?+19, UPPER(?)', [2000, 'Ligue 1']).first;
  final int year = row[0];
  final String league = row[1];
  print('-- $league $year --');
}

Future<void> _createAndInsertEntries(Database db) async {
  await db.execute('CREATE TABLE rankings (team text, points int)');
  await db.execute('INSERT INTO rankings VALUES ("Nice", 38)');
  await db.execute('INSERT INTO rankings VALUES ("Monaco", 41)');
  final int count = ((await db.query("SELECT COUNT(*) AS count FROM rankings").first) as dynamic).count;
  print('$count teams competing');
}

Future<void> _useStatements(Database db) async {
  await db.execute('INSERT INTO rankings VALUES ("PSG", 78)');
  await db.execute('INSERT INTO rankings VALUES ("Lyon", 39)');
  final teams = db.query("SELECT * FROM rankings");
  final count = await teams.length;
  print('$count teams competing now');
}

Future<void> _inspectResults(Database db) async {
  final subscription = db
      .query('SELECT * FROM rankings ORDER BY points DESC')
      .listen((dynamic row) => print('${row.team.padRight(10)} ${row.points}'));
  await subscription.asFuture();
  print('Who\'s the best now?');
}

Future main(List<String> args) async {
  final db = new Database.inMemory();
  await _runSimpleQuery(db);
  await _createAndInsertEntries(db);
  await _useStatements(db);
  await _inspectResults(db);
}
