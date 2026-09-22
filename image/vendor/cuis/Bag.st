!Collection subclass: #Bag
  instanceVariableNames: 'contents'
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Collections-Unordered'!
!Bag methodsFor: 'accessing'!
at: index 
	self errorNotKeyed!
at: index put: anObject
	self errorNotKeyed!
atRandom: aGenerator 
	"Answer a random element of the receiver.  Uses aGenerator which 
	should be kept by the user in a variable and used every time. Use 
	this instead of #atRandom for better uniformity of random numbers  
	because only you use the generator.  Causes an error if self has no  
	elements.
	WARNING: This implementation cannot guarantee reproducibility.
	Calling twice with generators setup with the same seed can produce
	different results (for example if the collection is rehashed between calls).
	If you need reproducibility, you should convert the collection to a sequenceable
	collection first."
	| n |
	self emptyCheck.
	n := self size atRandom: aGenerator.
	self withOccurrencesDo: [:each :times| (n := n - times) <= 0 ifTrue: [^ each]].
	self error: 'collection invariants broken'!
cumulativeCounts
	"Answer with a collection of cumulative percents covered by elements so far."
	| s n |
	s := self size / 100.0.
	n := 0.
	^ self sortedCounts asArray collect: [ :a |
		n := n + a key.
		(n / s roundTo: 0.1) -> a value]!
size

	^contents inject: 0 into: [ :prevValue :each | prevValue + each ]!
sortedCounts
	"Answer with a collection of counts with elements, sorted by decreasing count."
	| counts |
	counts := SortedCollection sortBlock: [:x :y | x >= y].
	contents associationsDo:
		[:assn |
		counts add: (Association key: assn value value: assn key)].
	^ counts!
sortedElements
	"Answer with a collection of elements with counts, sorted by element."
	| elements |
	elements := SortedCollection new.
	contents associationsDo: [:assn | elements add: assn].
	^ elements!
!Bag methodsFor: 'adding'!
add: newObject 
	^self add: newObject withOccurrences: 1!
add: newObject withOccurrences: anInteger 
	"Add the element newObject to the receiver. Do so as though the element  
	were added anInteger number of times. Answer newObject."
	contents at: newObject put: (contents at: newObject ifAbsent: [0])
			+ anInteger.
	^ newObject!
addAll: aCollection
	"Refer to the comment in Collection|addAll:."

	(aCollection is: #Bag) ifFalse: [^ super addAll: aCollection].
	aCollection contents associationsDo: [ :each |
		self add: each key withOccurrences: each value].
	^ aCollection!
!Bag methodsFor: 'copying'!
postCopy

	contents := contents copy!
!Bag methodsFor: 'enumerating'!
allSatisfy: aBlock
	"Evaluate aBlock with the elements of the receiver.
	If aBlock returns false for any element return false.
	Otherwise return true."

	contents keysDo: [:each | (aBlock value: each) ifFalse: [^ false]].
	^ true!
anySatisfy: aBlock
	"Evaluate aBlock with the elements of the receiver.
	If aBlock returns true for any element return true.
	Otherwise return false"

	contents keysDo: [:each| (aBlock value: each) ifTrue: [^ true]].
	^ false!
asSet
	"Answer a set with the elements of the receiver"

	 ^contents keys asSet!
collect: aBlock 
	"Evaluate aBlock with each of the receiver's elements as the argument. Collect the resulting values into a collection that is like the receiver and nswer the new collection."
	| answer |
	answer := self species new.
	self withOccurrencesDo: [:each :times| answer add: (aBlock value: each) withOccurrences: times].
	^ answer!
count: aBlock
	"Evaluate aBlock with each of the receiver's elements as the argument.  Return the number that answered true."
	| count |
	count := 0.
	self withOccurrencesDo: [:each :times| (aBlock value: each) ifTrue: [count := count + times]].
	^ count!
detect: aBlock ifNone: exceptionBlock 
	"Refer to the comment in Collection|detect:ifNone:."

	contents keysDo: [:each | (aBlock value: each) ifTrue: [^ each]].
	^ exceptionBlock value!
do: aBlock 
	contents associationsDo: [:assoc | assoc value timesRepeat: [aBlock value: assoc key]]!
select: aBlock 
	"Refer to the comment in Collection|select:."

	| newCollection |
	newCollection := self species new.
	contents associationsDo: [:each |
		(aBlock value: each key)
			ifTrue: [newCollection add: each key withOccurrences: each value]].
	^ newCollection!
withOccurrencesDo: aBlock
	contents associationsDo: [:each| aBlock value: each key value: each value]!
!Bag methodsFor: 'private'!
contents
	^contents!
setContents: aDictionary
	contents := aDictionary!
!Bag methodsFor: 'removing'!
remove: oldObject ifAbsent: exceptionBlock 
	"Refer to the comment in Collection|remove:ifAbsent:."

	| count |
	count := contents at: oldObject ifAbsent: [^ exceptionBlock value].
	count = 1
		ifTrue: [contents removeKey: oldObject]
		ifFalse: [contents at: oldObject put: count - 1].
	^ oldObject!
!Bag methodsFor: 'testing'!
includes: anObject 
	^contents includesKey: anObject!
is: aSymbol
	^ aSymbol == #Bag or: [ super is: aSymbol ]!
occurrencesOf: anObject 
	(self includes: anObject)
		ifTrue: [^contents at: anObject]
		ifFalse: [^0]!
!Bag methodsFor: 'comparing'!
= aBag
	"Two bags are equal if
	 (a) they are the same 'kind' of thing.
	 (b) they have the same size.
	 (c) each element occurs the same number of times in both of them"

	self == aBag ifTrue: [ ^ true ].		"Any object is equal to itself"
	(aBag is: #Bag) ifFalse: [ ^ false ].
	self size = aBag size ifFalse: [ ^ false ].
	contents associationsDo: [ :assoc |
		(aBag occurrencesOf: assoc key) = assoc value
			ifFalse: [ ^ false ]].
	^true!
!Bag methodsFor: 'statistics'!
max: aBlock
	"Answer the maximum of evaluating aBlock on all the elements of the receiver."
	| answer |
	self emptyCheck.
	contents keysDo: [:each| answer := answer isNil ifTrue: [aBlock value: each] ifFalse: [answer max: (aBlock value: each)]].
	^ answer!
min: aBlock
	"Answer the minimum of evaluating aBlock on all the elements of the receiver."
	| answer |
	self emptyCheck.
	contents keysDo: [:each| answer := answer isNil ifTrue: [aBlock value: each] ifFalse: [answer min: (aBlock value: each)]].
	^ answer!
product: aBlock
	| answer |
	self emptyCheck.
	self withOccurrencesDo: [ :each :times | | v |
		v := aBlock value: each.
		answer := answer isNil
			ifTrue: [v raisedTo: times]
			ifFalse: [answer * (v raisedTo: times)]].
	^ answer!
sum: aBlock ifEmpty: emptyBlockOrValue
	| answer |
	self isEmpty ifTrue: [^ emptyBlockOrValue value].
	self withOccurrencesDo: [ :each :times | | v |
		v := aBlock value: each.
		answer := answer isNil
			ifTrue: [v * times]
			ifFalse: [v * times + answer]].
	^ answer!
!Bag class methodsFor: 'instance creation'!
contentsClass
	^Dictionary!
new
	^ self basicNew setContents: self contentsClass new!
newFrom: aCollection
	"Create a new collection containing all the elements from aCollection"

	^ self new
		addAll: aCollection;
		yourself!
