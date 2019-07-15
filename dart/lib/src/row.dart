// File created by
// Lung Razvan <long1eu>
// on 2019-07-14

part of 'sqlite.dart';

class Row {
  Row(this.index, this._metadata, this._data);

  final int index;
  final RowMetadata _metadata;
  final List<dynamic> _data;

  dynamic operator [](dynamic i) {
    if (i is int) {
      return _data[i];
    } else {
      final int index = _metadata.columnToIndex[i];
      return _valueOrThrow(index, i);
    }
  }

  List<Object> toList() => new List<Object>.from(_data);

  Map<String, Object> toMap() {
    return <String, Object>{
      for (int i = 0; i < _data.length; i++) _metadata.columns[i]: _data[i],
    };
  }

  @override
  String toString() => toMap().toString();

  @override
  dynamic noSuchMethod(Invocation invocation) {
    if (invocation.isGetter) {
      final String rawName = invocation.memberName.toString();
      final String property = RegExp('"(.*)"').firstMatch(rawName).group(1);

      final index = _metadata.columnToIndex[property];
      return _valueOrThrow(index, property);
    }
    return super.noSuchMethod(invocation);
  }

  dynamic _valueOrThrow(int index, String column) {
    if (index != null) {
      return _data[index];
    } else {
      throw SqliteError("No such column $column");
    }
  }
}

class RowMetadata {
  RowMetadata(this.columns) : columnToIndex = <String, int>{for (int i = 0; i < columns.length; i++) columns[i]: i};

  final List<String> columns;
  final Map<String, int> columnToIndex;

  @override
  String toString() => 'RowMetadata{columns: $columns, columnToIndex: $columnToIndex}';
}
