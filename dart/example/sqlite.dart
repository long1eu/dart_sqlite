import 'dart:convert';
import 'dart:io';
import 'package:rxdart/rxdart.dart';
import 'package:args/args.dart';
import 'package:dart_sqlite/dart_sqlite.dart';

void main(List<String> args) {
  final ArgParser parser = ArgParser()
    ..addOption(
      'file',
      abbr: 'f',
      help: 'Provide the path forthe database.',
      defaultsTo: 'test.db',
    );

  final File file = File(parser.parse(args)['file']);
  final Database database = Database(file.absolute.path);

  Observable(stdin)
      .transform(utf8.decoder)
      .transform(const LineSplitter())
      .map(database.prepareStatement)
      .asyncMap((row) => row.execute())
      .map((it) => utf8.encode('$it\n'))
      .listen(stdout.add);
}
