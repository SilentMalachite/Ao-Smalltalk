"ChangeSet — Ao のチェンジセット（SPEC §3.9 Change Sorter、§3.10 チェンジセット、P17）。モデルだけを持ち、窓は AppKit。Kernel のネイティブだけを使う。file-in は式のチャンクを実行しないので、初期化は遅延。"!

Object subclass: #ClassChanges
  instanceVariableNames: 'className definitionKind instanceSide classSide sortedInstance sortedClass'
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
	classSide := Dictionary new!

forgetOrder
	"側が変わったので、並べた Array を捨てる。"
	sortedInstance := nil.
	sortedClass := nil!

sortedSelectorsOf: aDictionary
	"aDictionary のキーを、バイト順に並べた新しい Array。"
	| out n j |
	out := Array new: aDictionary size.
	n := 0.
	aDictionary keysDo: [:sel |
		j := n.
		[j > 0 and: [ClassChanges is: sel before: (out at: j)]] whileTrue: [
			out at: j + 1 put: (out at: j).
			j := j - 1].
		out at: j + 1 put: sel.
		n := n + 1].
	^out! !

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
	(self sideFor: meta) at: aSelector asSymbol put: aKind.
	self forgetOrder!

forget: aSelector meta: meta
	(self sideFor: meta) removeKey: aSelector asSymbol ifAbsent: [nil].
	self forgetOrder!

clearMethods
	instanceSide := Dictionary new.
	classSide := Dictionary new.
	self forgetOrder!

isEmpty
	"定義の種類もメソッドの項目も無い。"
	^definitionKind isNil and: [instanceSide size = 0 and: [classSide size = 0]]!

selectorsMeta: meta
	"その側のセレクタを、バイト順に並べた Array。側が変わるまで同じ Array を答える（Change Sorter は 1 行ごとに読む）。読むだけにして、書き換えない。"
	| sorted |
	sorted := meta ifTrue: [sortedClass] ifFalse: [sortedInstance].
	sorted isNil ifFalse: [^sorted].
	sorted := self sortedSelectorsOf: (self sideFor: meta).
	meta ifTrue: [sortedClass := sorted] ifFalse: [sortedInstance := sorted].
	^sorted!

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

!ChangeSet class methodsFor: 'fileIn/Out'!
classNamed: aSymbol
	"Smalltalk が aSymbol に束縛しているクラス（メタクラスでもほかの値でもない）。無ければ nil。"
	| value |
	value := Smalltalk at: aSymbol asSymbol ifAbsent: [nil].
	value class class == Metaclass ifFalse: [^nil].
	^value!

superclassOrder: classNames
	"名前の並び。それぞれを、並びの中にあるスーパークラスのあとに置く。ほかは元の順。"
	| placed out chain at cls i |
	placed := OrderedCollection new.
	out := OrderedCollection new.
	classNames do: [:each |
		chain := OrderedCollection new.
		at := each.
		[at notNil and: [(classNames includes: at) and: [(placed includes: at) not and: [(chain includes: at) not]]]] whileTrue: [
			chain add: at.
			cls := self classNamed: at.
			at := (cls isNil or: [cls superclass isNil]) ifTrue: [nil] ifFalse: [cls superclass name asSymbol]].
		i := chain size.
		[i > 0] whileTrue: [
			placed add: (chain at: i).
			out add: (chain at: i).
			i := i - 1]].
	^out!

chunk: aString on: aStream
	"SPEC §3.8 チャンク形式: `!!` をすべて重ね、閉じる `!!` を足す。"
	aString do: [:c |
		c = $!! ifTrue: [aStream nextPut: $!!].
		aStream nextPut: c].
	aStream nextPut: $!!!

commentChunk: aString on: aStream
	"aString を二重引用符で囲み（中の `""` は重ねる）、チャンクにする。"
	| body |
	body := WriteStream on: String new.
	body nextPut: $".
	aString do: [:c |
		c = $" ifTrue: [body nextPut: $"].
		body nextPut: c].
	body nextPut: $".
	self chunk: body contents on: aStream! !

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

!ChangeSet methodsFor: 'fileIn/Out'!
fileOutStringDate: aDate version: aVersion
	"SPEC §3.9 File Out の形式。aDate は YYYY-MM-DD、aVersion は Ao の版（C++ が渡す）。"
	| out header lostSources lostDefinitions |
	out := WriteStream on: String new.
	header := WriteStream on: String new.
	header nextPutAll: 'Change Set: '; nextPutAll: name; cr;
		nextPutAll: 'Date: '; nextPutAll: aDate; cr;
		nextPutAll: 'Ao '; nextPutAll: aVersion.
	ChangeSet commentChunk: header contents on: out.
	out cr; cr.
	lostSources := OrderedCollection new.
	lostDefinitions := OrderedCollection new.
	self fileOutDefinitionsOn: out lost: lostDefinitions.
	classChanges do: [:each |
		self fileOut: each meta: false on: out lost: lostSources.
		self fileOut: each meta: true on: out lost: lostSources].
	classChanges do: [:each |
		self fileOutRemovalsOf: each meta: false on: out.
		self fileOutRemovalsOf: each meta: true on: out].
	classChanges do: [:each |
		each definitionKind == #removed ifTrue: [
			ChangeSet commentChunk: (ChangeSet text: 'Removed class: ' with: each className) on: out.
			out cr]].
	lostSources do: [:text | ChangeSet commentChunk: text on: out. out cr].
	lostDefinitions do: [:text | ChangeSet commentChunk: text on: out. out cr].
	^out contents!

fileOutDefinitionsOn: out lost: lost
	| defined cls |
	defined := OrderedCollection new.
	classChanges do: [:each |
		(each definitionKind notNil and: [each definitionKind ~~ #removed])
			ifTrue: [defined add: each className]].
	(ChangeSet superclassOrder: defined) do: [:each |
		cls := ChangeSet classNamed: each.
		cls isNil
			ifTrue: [lost add: (ChangeSet text: each with: ' definition not available')]
			ifFalse: [out nextPutAll: cls definition; nextPutAll: '!!'; cr; cr]]!

fileOut: entry meta: meta on: out lost: lost
	"その側の、削除でないメソッドのソースを 1 つの methodsFor: の節に。ソースの無いものは lost へ。"
	| cls side sources source |
	cls := ChangeSet classNamed: entry className.
	side := cls isNil ifTrue: [nil] ifFalse: [meta ifTrue: [cls class] ifFalse: [cls]].
	sources := OrderedCollection new.
	(entry selectorsMeta: meta) do: [:sel |
		(entry kindOf: sel meta: meta) == #removed ifFalse: [
			source := side isNil ifTrue: [nil] ifFalse: [side sourceCodeAt: sel].
			source isNil
				ifTrue: [lost add: (ChangeSet text: (ChangeSet entryName: entry className selector: sel meta: meta) with: ' source not available')]
				ifFalse: [sources add: source]]].
	sources size = 0 ifTrue: [^self].
	out nextPutAll: '!!'; nextPutAll: entry className.
	meta ifTrue: [out nextPutAll: ' class'].
	out nextPutAll: ' methodsFor: ''user''!!'; cr.
	1 to: sources size do: [:i |
		ChangeSet chunk: (sources at: i) on: out.
		i < sources size
			ifTrue: [out cr; cr]
			ifFalse: [out nextPutAll: ' !!'; cr; cr]]!

fileOutRemovalsOf: entry meta: meta on: out
	(entry selectorsMeta: meta) do: [:sel |
		(entry kindOf: sel meta: meta) == #removed ifTrue: [
			ChangeSet commentChunk: (ChangeSet text: 'Removed: ' with: (ChangeSet entryName: entry className selector: sel meta: meta)) on: out.
			out cr]]! !
