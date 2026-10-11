"ChangeSet — Ao のチェンジセット（SPEC §3.9 Change Sorter、§3.10 チェンジセット、P17）。モデルだけを持ち、窓は AppKit。Kernel のネイティブだけを使う。file-in は式のチャンクを実行しないので、初期化は遅延。"!

Object subclass: #ClassChanges
  instanceVariableNames: 'className definitionKind instanceSide classSide'
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Tools-Changes'!

Object subclass: #ChangeSet
  instanceVariableNames: 'name id classChanges'
  classVariableNames: 'AllChangeSets Current NextId'
  poolDictionaries: ''
  category: 'Tools-Changes'!

!ClassChanges class methodsFor: 'instance creation'!
named: aSymbol
	"aSymbol のクラスの、空の項目。"
	^self new setClassName: aSymbol! !

!ClassChanges class methodsFor: 'ordering'!
is: aString before: anotherString
	"バイト順で aString が先なら true（SPEC §3.9 Change Sorter の並び）。"
	| n |
	n := aString size min: anotherString size.
	1 to: n do: [:i |
		(aString at: i) = (anotherString at: i)
			ifFalse: [^(aString at: i) < (anotherString at: i)]].
	^aString size < anotherString size! !

!ClassChanges methodsFor: 'private'!
setClassName: aSymbol
	className := aSymbol.
	instanceSide := Dictionary new.
	classSide := Dictionary new! !

!ClassChanges methodsFor: 'accessing'!
className
	^className!

definitionKind
	"nil、#added、#changed、#removed。"
	^definitionKind!

definitionKind: aSymbolOrNil
	definitionKind := aSymbolOrNil!

sideFor: meta
	^meta ifTrue: [classSide] ifFalse: [instanceSide]!

kindOf: aSelector meta: meta
	^(self sideFor: meta) at: aSelector asSymbol ifAbsent: [nil]!

at: aSelector meta: meta put: aKind
	(self sideFor: meta) at: aSelector asSymbol put: aKind!

forget: aSelector meta: meta
	(self sideFor: meta) removeKey: aSelector asSymbol ifAbsent: [nil]!

clearMethods
	instanceSide := Dictionary new.
	classSide := Dictionary new!

isEmpty
	"定義の種類もメソッドの項目も無い。"
	^definitionKind isNil and: [instanceSide size = 0 and: [classSide size = 0]]!

selectorsMeta: meta
	"その側のセレクタを、バイト順に並べた Array。"
	| side out n j |
	side := self sideFor: meta.
	out := Array new: side size.
	n := 0.
	side keysDo: [:sel |
		j := n.
		[j > 0 and: [ClassChanges is: sel before: (out at: j)]] whileTrue: [
			out at: j + 1 put: (out at: j).
			j := j - 1].
		out at: j + 1 put: sel.
		n := n + 1].
	^out!

mergeInto: aClassChanges
	"Move to（SPEC §3.10）: 移す種類が移し先を上書きし、移す側に無い定義の種類は移し先のまま。"
	definitionKind isNil ifFalse: [aClassChanges definitionKind: definitionKind].
	instanceSide keysAndValuesDo: [:sel :kind | aClassChanges at: sel meta: false put: kind].
	classSide keysAndValuesDo: [:sel :kind | aClassChanges at: sel meta: true put: kind]! !

!ChangeSet class methodsFor: 'accessing'!
allChangeSets
	"作った順。最初の参照で空の Unnamed1 を作り、current にする。"
	AllChangeSets isNil ifTrue: [
		AllChangeSets := OrderedCollection new.
		NextId isNil ifTrue: [NextId := 1].
		Current := self addNamed: 'Unnamed1'].
	^AllChangeSets!

current
	self allChangeSets.
	^Current!

named: aString
	^self allChangeSets detect: [:each | each name = aString] ifNone: [nil]!

withId: anInteger
	^self allChangeSets detect: [:each | each id = anInteger] ifNone: [nil]! !

!ChangeSet class methodsFor: 'instance creation'!
reasonToCreate: aString
	"作れないなら理由の String、作れるなら nil。"
	aString size = 0 ifTrue: [^'change set name empty'].
	(self named: aString) isNil ifFalse: [^self text: 'change set name taken: ' with: aString].
	^nil!

newNamed: aString
	"新しいセットを current にして答える。"
	| reason set |
	reason := self reasonToCreate: aString.
	reason isNil ifFalse: [^self error: reason].
	set := self addNamed: aString.
	set makeCurrent.
	^set! !

!ChangeSet class methodsFor: 'recording'!
noteMethodChange: className selector: aSymbol meta: meta existed: existed
	"Accept のあと（C++ から）。existed: そのセレクタのメソッドを置き換えた。"
	self current recordMethodChange: className asSymbol selector: aSymbol asSymbol meta: meta existed: existed!

noteClassChange: className existed: existed
	"クラス定義の Accept のあと。existed: その名前は前からクラスだった。"
	self current recordClassChange: className asSymbol existed: existed!

noteMethodRemoval: className selector: aSymbol meta: meta
	self current recordMethodRemoval: className asSymbol selector: aSymbol asSymbol meta: meta!

noteClassRemoval: className
	self current recordClassRemoval: className asSymbol! !

!ChangeSet class methodsFor: 'kinds'!
kindAfterAccept: was existed: existed
	"SPEC §3.9 種類の規則。無し→追加（置き換えたなら変更）、追加→追加、ほか→変更。"
	was isNil ifTrue: [^existed ifTrue: [#changed] ifFalse: [#added]].
	^was == #added ifTrue: [#added] ifFalse: [#changed]!

kindAfterRemoval: was
	"追加→nil（項目が消える）、ほか→削除。"
	^was == #added ifTrue: [nil] ifFalse: [#removed]! !

!ChangeSet class methodsFor: 'private'!
addNamed: aString
	| set |
	set := self new setName: aString id: NextId.
	NextId := NextId + 1.
	AllChangeSets add: set.
	^set!

text: aString with: anotherString
	| s |
	s := WriteStream on: String new.
	s nextPutAll: aString; nextPutAll: anotherString.
	^s contents!

entryName: className selector: aSelector meta: meta
	"<Class>>><selector>、クラス側は <Class> class>><selector>（SPEC §3.9 削除）。"
	| s |
	s := WriteStream on: String new.
	s nextPutAll: className.
	meta ifTrue: [s nextPutAll: ' class'].
	s nextPutAll: '>>'; nextPutAll: aSelector.
	^s contents! !

!ChangeSet methodsFor: 'private'!
setName: aString id: anInteger
	name := aString.
	id := anInteger.
	classChanges := OrderedCollection new!

entryFor: className
	"className の項目。無ければ作って最後に足す。"
	| entry |
	entry := self changesFor: className.
	entry isNil ifFalse: [^entry].
	entry := ClassChanges named: className asSymbol.
	classChanges add: entry.
	^entry!

forgetEntry: className
	| key kept |
	key := className asSymbol.
	kept := OrderedCollection new.
	classChanges do: [:each | each className == key ifFalse: [kept add: each]].
	classChanges := kept!

dropIfEmpty: className
	| entry |
	entry := self changesFor: className.
	(entry notNil and: [entry isEmpty]) ifTrue: [self forgetEntry: className]! !

!ChangeSet methodsFor: 'accessing'!
name
	^name!

id
	^id!

classChanges
	"ClassChanges の OrderedCollection。最初に記録した順。"
	^classChanges!

changesFor: className
	| key |
	key := className asSymbol.
	^classChanges detect: [:each | each className == key] ifNone: [nil]!

makeCurrent
	Current := self! !

!ChangeSet methodsFor: 'recording'!
recordMethodChange: className selector: aSymbol meta: meta existed: existed
	| entry |
	entry := self entryFor: className.
	entry at: aSymbol meta: meta put: (ChangeSet kindAfterAccept: (entry kindOf: aSymbol meta: meta) existed: existed)!

recordMethodRemoval: className selector: aSymbol meta: meta
	| entry kind |
	entry := self entryFor: className.
	kind := ChangeSet kindAfterRemoval: (entry kindOf: aSymbol meta: meta).
	kind isNil
		ifTrue: [
			entry forget: aSymbol meta: meta.
			self dropIfEmpty: className]
		ifFalse: [entry at: aSymbol meta: meta put: kind]!

recordClassChange: className existed: existed
	| entry |
	entry := self entryFor: className.
	entry definitionKind: (ChangeSet kindAfterAccept: entry definitionKind existed: existed)!

recordClassRemoval: className
	"追加したクラスはメソッドごと消え、ほかは削除になってメソッドの項目が消える。"
	| entry |
	entry := self entryFor: className.
	entry definitionKind == #added ifTrue: [^self forgetEntry: className].
	entry definitionKind: #removed.
	entry clearMethods! !

!ChangeSet methodsFor: 'removing'!
reasonToRemoveClass: className
	(self changesFor: className) isNil
		ifTrue: [^ChangeSet text: 'not in change set: ' with: className].
	^nil!

reasonToRemoveSelector: aSelector class: className meta: meta
	| entry |
	entry := self changesFor: className.
	(entry isNil or: [(entry kindOf: aSelector meta: meta) isNil])
		ifTrue: [^ChangeSet text: 'not in change set: ' with: (ChangeSet entryName: className selector: aSelector meta: meta)].
	^nil!

removeClass: className
	"Remove from Change Set: 項目を忘れる。メソッドとクラスはそのまま。"
	| reason |
	reason := self reasonToRemoveClass: className.
	reason isNil ifFalse: [^self error: reason].
	self forgetEntry: className!

removeSelector: aSelector class: className meta: meta
	| reason |
	reason := self reasonToRemoveSelector: aSelector class: className meta: meta.
	reason isNil ifFalse: [^self error: reason].
	(self changesFor: className) forget: aSelector meta: meta.
	self dropIfEmpty: className! !

!ChangeSet methodsFor: 'moving'!
reasonToMoveClass: className to: aChangeSet
	aChangeSet == self ifTrue: [^'same change set'].
	^self reasonToRemoveClass: className!

reasonToMoveSelector: aSelector class: className meta: meta to: aChangeSet
	aChangeSet == self ifTrue: [^'same change set'].
	^self reasonToRemoveSelector: aSelector class: className meta: meta!

moveClass: className to: aChangeSet
	| reason |
	reason := self reasonToMoveClass: className to: aChangeSet.
	reason isNil ifFalse: [^self error: reason].
	(self changesFor: className) mergeInto: (aChangeSet entryFor: className).
	self forgetEntry: className!

moveSelector: aSelector class: className meta: meta to: aChangeSet
	| reason entry |
	reason := self reasonToMoveSelector: aSelector class: className meta: meta to: aChangeSet.
	reason isNil ifFalse: [^self error: reason].
	entry := self changesFor: className.
	(aChangeSet entryFor: className) at: aSelector meta: meta put: (entry kindOf: aSelector meta: meta).
	entry forget: aSelector meta: meta.
	self dropIfEmpty: className! !
