// File created by
// Lung Razvan <long1eu>
// on 2019-07-13

export 'src/errors.dart';
export 'src/sqlite.dart';

import 'src/sqlite.dart';

void main() {
  print(RowMetadata(['a', 'b']).columnToIndex);
  print((Row(0, RowMetadata(['a', 'b']), [1, 2]) as dynamic).sasas);
}
