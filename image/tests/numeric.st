"SPEC §3.6 数の演算（B8）: Integer、Fraction、Float の混合。答えは一般性の高いほうの型"
self assert: 1 + 1.5 equals: 2.5.
self assert: 1 - 1.5 equals: -0.5.
self assert: 2 * 1.5 equals: 3.0.
self assert: 1.5 + 1 equals: 2.5.
self assert: (1 bitShift: 70) + 1.0 equals: 1180591620717411303424.0.
self assert: (1/2) + 0.5 equals: 1.0.
self assert: 0.5 + (1/2) equals: 1.0.
self assert: (1 + (1/2)) class equals: Fraction.
self assert: 2 * (1/2) equals: 1.
self assert: 3 / (1/2) equals: 6.
self assert: 1 / 2.0 equals: 0.5.
self assert: ((1 bitShift: 53) + 1) + 0.0 equals: 9007199254740992.0.
self assert: (Rectangle origin: (Point x: 0.5 y: 0) corner: (Point x: 2 y: 2)) width equals: 1.5.
"SPEC §3.6 数の比較: 値を厳密に比べる。NaN との比較は false"
self assert: 1 < 1.5 equals: true.
self assert: 1.5 > 1 equals: true.
self assert: 2 >= 1.5 equals: true.
self assert: 1 <= 1.0 equals: true.
self assert: 1 = 1.0 equals: false.
self assert: 1152921504606846976.0 < 1152921504606846977 equals: true.
self assert: 1.0 < (1 bitShift: 70) equals: true.
self assert: (0.0 / 0.0) >= 1.0 equals: false.
self assert: 1.0 >= (0.0 / 0.0) equals: false.
self assert: (1/2) = (2/4) equals: true.
self assert: (1/2) < (2/3) equals: true.
self assert: (1/3) > 0.3333333333333333 equals: true.
self assert: ((1/2) max: (2/3)) equals: 2/3.
self assert: ((1/2) between: 0 and: 1) equals: true.
self assert: ((Rectangle origin: (Point x: 0 y: 0) corner: (Point x: 10 y: 10)) containsPoint: (Point x: 0.5 y: 0.5)) equals: true.
"SPEC §3.6 bitShift: のシフト量が -2^24 より小さければ 0 か -1"
self assert: (5 bitShift: (-1 bitShift: 63)) equals: 0.
self assert: (-5 bitShift: (-1 bitShift: 63)) equals: -1.
"SPEC §3.6 = と hash: a = b なら a hash = b hash"
self assert: (1 bitShift: 70) hash equals: (1 bitShift: 70) hash.
self assert: 1.5 hash equals: (1.0 + 0.5) hash.
self assert: 0.0 hash equals: (0.0 * -1) hash.
self assert: (1/2) hash equals: (2/4) hash.
self assert: 'abc' hash equals: 'abc' copy hash.
self assert: #abc hash equals: 'abc' hash.
self assert: #(1 'x' #(2.5 $c)) hash equals: #(1 'x' #(2.5 $c)) hash.
self assert: (Point x: 1 y: 'p') hash equals: (Point x: 1 y: 'p' copy) hash.
"SPEC §3.6 Boolean: true & x と false | x は x を答える"
self assert: (true & nil) equals: nil.
self assert: (false | 3) equals: 3.
self assert: (true xor: false) equals: true.
self assert: (false eqv: false) equals: true.
