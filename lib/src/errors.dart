// File created by
// Lung Razvan <long1eu>
// on 2019-07-13

class SqliteError extends Error {
  SqliteError(String message) : this._(message);

  SqliteError._(this.message);

  final String message;

  @override
  String toString() => 'SqliteError: $message';
}

class SqliteSyntaxError extends SqliteError {
  SqliteSyntaxError._(String message, this.query) : super._(message);

  final String query;

  @override
  String toString() => 'SqliteSyntaxError: $message for query: [$query]';
}
