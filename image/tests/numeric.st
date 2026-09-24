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
