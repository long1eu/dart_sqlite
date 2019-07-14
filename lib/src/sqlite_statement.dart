// File created by
// Lung Razvan <long1eu>
// on 2019-07-13

part of "sqlite.dart";

class SqliteStatement {
  SqliteStatement._(Database db, this.sql, [List<dynamic> params = const <dynamic>[]])
      : _ptr = _prepareStatement(db._handler, sql),
        _db = db,
        _params = params;

  final _StatementPeer _ptr;
  final Database _db;
  final String sql;
  final List _params;

  bool _closed = false;
  int _affectedRows;

  Stream<Row> query() {
    StreamController<Row> controller;
    RowMetadata rowMetadata;
    int index = 0;
    Timer timer;

    bool sleep() {
      final dynamic rawResult = _evaluateStatement(_ptr);
      if (rawResult is int) {
        _affectedRows = rawResult;
        controller.close();
        return false;
      }

      final List<dynamic> result = rawResult;
      rowMetadata ??= RowMetadata(_columnInfo(_ptr).cast<String>());
      controller.add(Row(index++, rowMetadata, result));
      return true;
    }

    void loop() {
      timer = Timer(Duration.zero, () {
        if (sleep()) {
          loop();
        }
      });
    }

    void start() {
      if (timer == null) {
        loop();
      }
    }

    void stop() {
      timer?.cancel();
      timer = null;
    }

    void finalize() {
      stop();
      _close();
    }

    _checkClosed();
    if (_params.isNotEmpty) {
      _bindValues(_ptr, _params);
    }

    controller = StreamController<Row>(onListen: start, onPause: stop, onResume: start, onCancel: finalize);
    return controller.stream;
  }

  Future<int> execute() => query().length.then((_) => _affectedRows);

  void _close() {
    _db._checkClosed();
    _checkClosed();
    _closed = true;
    _closeStatement(_ptr);
  }

  void _checkClosed() {
    if (_closed) {
      throw SqliteError('Statement $sql closed.');
    }
  }

  @override
  String toString() => 'SqliteStatement{sql: "$sql"}';
}
