"process: 協調スケジューラ（SPEC §3.4 プロセスと協調スケジューラ）。fork の順序、セマフォのピンポン、SharedQueue、activeProcess の同一性"
| digits log base inner p s1 s2 q done |
digits := [:coll | coll inject: 0 into: [:acc :e | acc * 10 + e]].
"fork は切り替えるまで走らない。実行可能キューは 1 本の FIFO で、fork の中の fork は後ろに並ぶ。"
log := OrderedCollection new.
[log add: 1] fork.
[log add: 2. [log add: 4] fork] fork.
[log add: 3] fork.
self assert: log size equals: 0.
Processor yield.
self assert: (digits value: log) equals: 123.
Processor yield.
self assert: (digits value: log) equals: 1234.
"セマフォのピンポン: signal は切り替えず、wait で相手に切り替わる。1 が ping、2 が pong。"
log := OrderedCollection new.
s1 := Semaphore new.
s2 := Semaphore new.
[3 timesRepeat: [s1 wait. log add: 2. s2 signal]] fork.
3 timesRepeat: [log add: 1. s1 signal. s2 wait].
self assert: (digits value: log) equals: 121212.
"SharedQueue: 空の next は nextPut: まで待つ。"
q := SharedQueue new.
log := OrderedCollection new.
done := Semaphore new.
[3 timesRepeat: [log add: q next]. done signal] fork.
[1 to: 3 do: [:i | q nextPut: i. Processor yield]] fork.
done wait.
self assert: (digits value: log) equals: 123.
"activeProcess: fork の中はそのプロセス。外はベースのままで、切り替えをまたいでも同じ。"
base := Processor activeProcess.
p := [inner := Processor activeProcess] fork.
self assert: inner equals: nil.
Processor yield.
self assert: inner == p equals: true.
self assert: inner == base equals: false.
self assert: Processor activeProcess == base equals: true.
"terminate: 待っているプロセスを巻き戻し、ensure: の後始末をそのプロセスの上で走らせる。"
log := OrderedCollection new.
p := [[s1 wait] ensure: [log add: 9]] fork.
Processor yield.
p terminate.
self assert: (digits value: log) equals: 9.
"signal を受けてまだ wait から戻っていないプロセスを terminate すると、その signal はセマフォに返る。"
q := SharedQueue new.
p := [q next] fork.
Processor yield.
q nextPut: 7.
p terminate.
self assert: q next equals: 7.
self assert: Processor activeProcess == base equals: true.
"ファイルの終わりに待ったまま残ったプロセスは、ao --test が terminate で終わらせる（SPEC §4.4）。"
[s1 wait] fork.
Processor yield
