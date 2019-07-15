// File created by
// Lung Razvan <long1eu>
// on 2019-07-14

part of 'sqlite.dart';

int _open(String name) native 'New';

void _close(int db) native 'Close';

String get _version native 'Version';

_StatementPeer _prepareStatement(int db, String sql) native 'PrepareStatement';

void _closeStatement(_StatementPeer ptr) native 'CloseStatement';

void _bindValues(_StatementPeer ptr, List params) native 'Bind';

dynamic _evaluateStatement(_StatementPeer ptr) native 'Step';

List<dynamic> _columnInfo(_StatementPeer ptr) native 'ColumnInfo';

class _StatementPeer extends NativeFieldWrapperClass2 {
  _StatementPeer();
}
