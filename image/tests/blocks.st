"blocks: Blue Book のブロックと、コンパイラが展開する送信（SPEC §3.4, §3.5）"
| y i s sum n counter log r |
y := 0.
3 > 1 ifTrue: [y := 1].
self assert: y equals: 1.
self assert: (3 < 1 ifTrue: [#yes] ifFalse: [#no]) equals: #no.
self assert: (false ifTrue: [1]) equals: nil.
self assert: (false and: [1 / 0]) equals: false.
self assert: (true or: [nil foo]) equals: true.
self assert: (true and: [7]) equals: 7.
i := 0.
[i < 5] whileTrue: [i := i + 1].
self assert: i equals: 5.
sum := 0.
self assert: (1 to: 4 do: [:k | sum := sum + k]) equals: 1.
self assert: sum equals: 10.
s := 0.
10 to: 1 by: -3 do: [:k | s := s * 10 + k].
self assert: s equals: 10741.
sum := 0.
#(1 2 3) do: [:e | sum := sum + e].
self assert: sum equals: 6.
n := 0.
counter := [n := n + 1].
counter value.
counter value.
self assert: counter value equals: 3.
self assert: n equals: 3.
log := OrderedCollection new.
r := [log add: #body. 5] ensure: [log add: #cleanup].
self assert: r equals: 5.
self assert: log size equals: 2.
self assert: (log at: 2) equals: #cleanup.
n := 0.
3 timesRepeat: [n := n + 1].
self assert: n equals: 3.
"最後の文: 展開したループの中の ^ は doIt から返る。返らなければ次の assert が落ちる。"
[:limit | 1 to: 10 do: [:k | k = limit ifTrue: [^self assert: k equals: 4]]] value: 4.
self assert: #fellThrough equals: #returned.
