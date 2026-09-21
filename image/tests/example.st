"example"
self assert: 1 + 2 equals: 3.
self assert: (true ifTrue: [4] ifFalse: [5]) equals: 4.
self assert: (Object new class) equals: Object.
