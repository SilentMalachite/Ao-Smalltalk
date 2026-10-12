"changeset — SPEC §3.9 種類の規則、§3.10 チェンジセット（P17）。ChangeSet を参照する唯一のテストファイルなので、始まりは遅延の初期化の直後。"
| first work other entry sels out fo expect base sub cached target |
"遅延の初期化: 最初の参照で空の Unnamed1 が current になる"
first := ChangeSet current.
self assert: first name equals: 'Unnamed1'.
self assert: ChangeSet allChangeSets size equals: 1.
self assert: first classChanges size equals: 0.
self assert: (ChangeSet withId: first id) == first equals: true.
self assert: (ChangeSet named: 'Unnamed1') == first equals: true.
self assert: (ChangeSet named: 'Nope') equals: nil.
self assert: (ChangeSet withId: 0) equals: nil.

"作成の理由と newNamed:"
self assert: (ChangeSet reasonToCreate: '') equals: 'change set name empty'.
self assert: (ChangeSet reasonToCreate: 'Unnamed1') equals: 'change set name taken: Unnamed1'.
self assert: (ChangeSet reasonToCreate: 'Work') equals: nil.
work := ChangeSet newNamed: 'Work'.
self assert: ChangeSet current == work equals: true.
self assert: work id > first id equals: true.
self assert: ChangeSet allChangeSets size equals: 2.
self assert: (ChangeSet allChangeSets at: 2) == work equals: true.

"メソッドの種類: 無し→追加/変更、追加→追加、変更→変更、削除→変更、追加の削除で項目ごと消える"
ChangeSet noteMethodChange: #Foo selector: #bar meta: false existed: false.
entry := work changesFor: #Foo.
self assert: entry className equals: #Foo.
self assert: entry definitionKind equals: nil.
self assert: (entry kindOf: #bar meta: false) equals: #added.
ChangeSet noteMethodChange: #Foo selector: #bar meta: false existed: true.
self assert: (entry kindOf: #bar meta: false) equals: #added.
ChangeSet noteMethodChange: #Foo selector: #baz meta: false existed: true.
self assert: (entry kindOf: #baz meta: false) equals: #changed.
ChangeSet noteMethodRemoval: #Foo selector: #baz meta: false.
self assert: (entry kindOf: #baz meta: false) equals: #removed.
ChangeSet noteMethodChange: #Foo selector: #baz meta: false existed: false.
self assert: (entry kindOf: #baz meta: false) equals: #changed.
ChangeSet noteMethodRemoval: #Foo selector: #bar meta: false.
self assert: (entry kindOf: #bar meta: false) equals: nil.
ChangeSet noteMethodRemoval: #Foo selector: #baz meta: false.
self assert: (entry kindOf: #baz meta: false) equals: #removed.
"クラス側は別の辞書"
ChangeSet noteMethodChange: #Foo selector: #bar meta: true existed: false.
self assert: (entry kindOf: #bar meta: true) equals: #added.
self assert: (entry kindOf: #bar meta: false) equals: nil.
"追加したメソッドだけの項目は、その削除で項目ごと消える"
ChangeSet noteMethodChange: #Solo selector: #only meta: false existed: false.
ChangeSet noteMethodRemoval: #Solo selector: #only meta: false.
self assert: (work changesFor: #Solo) equals: nil.
"記録は current だけに入る"
self assert: (first changesFor: #Foo) equals: nil.

"クラスの種類: 追加したクラスの削除は項目ごと、既存クラスの削除はメソッドの項目を消して削除"
ChangeSet noteClassChange: #Made existed: false.
self assert: (work changesFor: #Made) definitionKind equals: #added.
ChangeSet noteClassChange: #Made existed: true.
self assert: (work changesFor: #Made) definitionKind equals: #added.
ChangeSet noteMethodChange: #Made selector: #m meta: false existed: false.
ChangeSet noteClassRemoval: #Made.
self assert: (work changesFor: #Made) equals: nil.
ChangeSet noteClassChange: #Old existed: true.
ChangeSet noteMethodChange: #Old selector: #m meta: false existed: true.
ChangeSet noteClassRemoval: #Old.
self assert: (work changesFor: #Old) definitionKind equals: #removed.
self assert: ((work changesFor: #Old) selectorsMeta: false) size equals: 0.
ChangeSet noteClassRemoval: #Gone.
self assert: (work changesFor: #Gone) definitionKind equals: #removed.
"記録の順"
self assert: (work classChanges at: 1) className equals: #Foo.
self assert: (work classChanges at: 2) className equals: #Old.
self assert: (work classChanges at: 3) className equals: #Gone.

"セレクタのバイト順（大文字が先、長さは後）"
ChangeSet noteMethodChange: #Order selector: #zeta meta: false existed: false.
ChangeSet noteMethodChange: #Order selector: #alpha meta: false existed: false.
ChangeSet noteMethodChange: #Order selector: #Alpha meta: false existed: false.
ChangeSet noteMethodChange: #Order selector: #al meta: false existed: false.
ChangeSet noteMethodChange: #Order selector: #at:put: meta: false existed: false.
sels := (work changesFor: #Order) selectorsMeta: false.
self assert: sels size equals: 5.
self assert: (sels at: 1) equals: #Alpha.
self assert: (sels at: 2) equals: #al.
self assert: (sels at: 3) equals: #alpha.
self assert: (sels at: 4) equals: #at:put:.
self assert: (sels at: 5) equals: #zeta.

"Remove from Change Set の理由と操作"
self assert: (work reasonToRemoveClass: #Nope) equals: 'not in change set: Nope'.
self assert: (work reasonToRemoveSelector: #nope class: #Foo meta: false) equals: 'not in change set: Foo>>nope'.
self assert: (work reasonToRemoveSelector: #nope class: #Foo meta: true) equals: 'not in change set: Foo class>>nope'.
self assert: (work reasonToRemoveSelector: #x class: #Nope meta: false) equals: 'not in change set: Nope>>x'.
self assert: (work reasonToRemoveSelector: #baz class: #Foo meta: false) equals: nil.
work removeSelector: #baz class: #Foo meta: false.
self assert: ((work changesFor: #Foo) kindOf: #baz meta: false) equals: nil.
work removeSelector: #bar class: 'Foo' meta: true.
self assert: (work changesFor: #Foo) equals: nil.
self assert: (work reasonToRemoveClass: #Gone) equals: nil.
work removeClass: #Gone.
self assert: (work changesFor: #Gone) equals: nil.

"Move to の理由と操作"
other := ChangeSet newNamed: 'Other'.
self assert: (work reasonToMoveClass: #Order to: work) equals: 'same change set'.
self assert: (work reasonToMoveSelector: #zeta class: #Order meta: false to: work) equals: 'same change set'.
self assert: (work reasonToMoveClass: #Nope to: other) equals: 'not in change set: Nope'.
self assert: (work reasonToMoveSelector: #nope class: #Order meta: false to: other) equals: 'not in change set: Order>>nope'.
work moveSelector: #zeta class: #Order meta: false to: other.
self assert: ((other changesFor: #Order) kindOf: #zeta meta: false) equals: #added.
self assert: ((work changesFor: #Order) kindOf: #zeta meta: false) equals: nil.
"移したクラスの種類が移し先を上書きし、移す側に無い定義の種類は移し先のまま"
ChangeSet noteClassChange: #Order existed: false.
self assert: ChangeSet current == other equals: true.
work moveClass: #Order to: other.
self assert: (work changesFor: #Order) equals: nil.
self assert: (other changesFor: #Order) definitionKind equals: #added.
self assert: ((other changesFor: #Order) kindOf: #alpha meta: false) equals: #added.
self assert: ((other changesFor: #Order) kindOf: #zeta meta: false) equals: #added.

"Make Current"
work makeCurrent.
self assert: ChangeSet current == work equals: true.
self assert: ChangeSet allChangeSets size equals: 3.

"並べた Array は側が変わるまで同じで、追加・削除・消去・Move to のあとで正しく取り直す"
entry := ChangeSet current entryFor: #CacheT.
entry at: #m meta: false put: #added.
entry at: #a meta: false put: #added.
cached := entry selectorsMeta: false.
self assert: cached size equals: 2.
self assert: (cached at: 1) equals: #a.
self assert: (entry selectorsMeta: false) == cached equals: true.
self assert: (entry selectorsMeta: true) size equals: 0.
entry at: #c meta: false put: #changed.
sels := entry selectorsMeta: false.
self assert: sels size equals: 3.
self assert: (sels at: 2) equals: #c.
self assert: (sels at: 3) equals: #m.
entry at: #a meta: false put: #changed.
self assert: (entry selectorsMeta: false) size equals: 3.
entry forget: #c meta: false.
sels := entry selectorsMeta: false.
self assert: sels size equals: 2.
self assert: (sels at: 1) equals: #a.
self assert: (sels at: 2) equals: #m.
entry at: #z meta: true put: #added.
entry at: #y meta: true put: #added.
self assert: ((entry selectorsMeta: true) at: 1) equals: #y.
self assert: (entry selectorsMeta: false) size equals: 2.
entry at: #b meta: true put: #added.
self assert: ((entry selectorsMeta: true) at: 1) equals: #b.
self assert: (entry selectorsMeta: false) size equals: 2.
target := ChangeSet current entryFor: #CacheTarget.
target at: #q meta: false put: #added.
self assert: (target selectorsMeta: false) size equals: 1.
entry mergeInto: target.
sels := target selectorsMeta: false.
self assert: sels size equals: 3.
self assert: (sels at: 1) equals: #a.
self assert: (sels at: 2) equals: #m.
self assert: (sels at: 3) equals: #q.
self assert: (target selectorsMeta: true) size equals: 3.
entry clearMethods.
self assert: (entry selectorsMeta: false) size equals: 0.
self assert: (entry selectorsMeta: true) size equals: 0.
entry at: #only meta: false put: #added.
self assert: ((entry selectorsMeta: false) at: 1) equals: #only.
ChangeSet current forgetEntry: #CacheT.
ChangeSet current forgetEntry: #CacheTarget.

"File Out（SPEC §3.9 File Out の形式）。ao --test はセッションの外なのでソースは無く、メソッドは source not available の行になる"
base := Object subclass: #P17Base instanceVariableNames: 'a' classVariableNames: '' poolDictionaries: '' category: 'P17-Test'.
sub := base subclass: #P17Sub instanceVariableNames: '' classVariableNames: '' poolDictionaries: '' category: 'P17-Test'.
Smalltalk at: #P17NotAClass put: 3.
Smalltalk at: #P17Meta put: base class.
out := ChangeSet newNamed: 'Out "q" !'.
ChangeSet noteClassChange: #P17Sub existed: false.
ChangeSet noteClassChange: #P17Base existed: false.
ChangeSet noteClassChange: #Missing existed: true.
ChangeSet noteMethodChange: #P17Base selector: #kept meta: false existed: false.
ChangeSet noteMethodChange: #P17Base selector: #made meta: true existed: false.
ChangeSet noteMethodRemoval: #P17Base selector: #gone meta: false.
ChangeSet noteClassRemoval: #Dropped.
fo := out fileOutStringDate: '2026-10-12' version: '9.9.9'.
expect := WriteStream on: String new.
expect nextPutAll: '"Change Set: Out ""q"" !!'; cr;
	nextPutAll: 'Date: 2026-10-12'; cr;
	nextPutAll: 'Ao 9.9.9"!'; cr; cr;
	nextPutAll: base definition; nextPutAll: '!'; cr; cr;
	nextPutAll: sub definition; nextPutAll: '!'; cr; cr;
	nextPutAll: '"Removed: P17Base>>gone"!'; cr;
	nextPutAll: '"Removed class: Dropped"!'; cr;
	nextPutAll: '"P17Base>>kept source not available"!'; cr;
	nextPutAll: '"P17Base class>>made source not available"!'; cr;
	nextPutAll: '"Missing definition not available"!'; cr.
self assert: fo equals: expect contents.
self assert: (ChangeSet classNamed: #P17Base) == base equals: true.
self assert: (ChangeSet classNamed: 'P17Sub') == sub equals: true.
self assert: (ChangeSet classNamed: #P17NotAClass) equals: nil.
self assert: (ChangeSet classNamed: #P17Meta) equals: nil.
self assert: (ChangeSet classNamed: #NoSuchGlobal) equals: nil.
"スーパークラスは記録の順に関わらず先"
self assert: (ChangeSet superclassOrder: (OrderedCollection new add: #P17Sub; add: #Missing; add: #P17Base; yourself)) size equals: 3.
self assert: ((ChangeSet superclassOrder: (OrderedCollection new add: #P17Sub; add: #Missing; add: #P17Base; yourself)) at: 1) equals: #P17Base.
