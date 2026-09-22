!SequenceableCollection subclass: #Heap
  instanceVariableNames: 'array tally sortBlock indexUpdateBlock'
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Collections-Sequenceable'!
!Heap methodsFor: 'accessing'!
at: index
	"Return the element at the given position within the receiver"
	(index < 1 or:[index > tally]) ifTrue:[^self errorSubscriptBounds: index].
	^array at: index!
at: index put: newObject
	"Heaps are accessed with #add: not #at:put:"
	^self shouldNotImplement!
first
	"Return the first element in the receiver"
	^array at: 1!
fullySort
	"Fully sort the heap.
	This method preserves the heap invariants and can thus be sent safely"
	self privateReverseSort.
	1 to: tally // 2 do: [:i | array swap: i with: 1 + tally - i]!
indexUpdateBlock: aBlockOrNil

	indexUpdateBlock := aBlockOrNil.!
reSort
	"Resort the entire heap"
	self isEmpty ifTrue:[^self].
	tally // 2 to: 1 by: -1 do:[:i| self downHeap: i].!
size
	"Return the number of elements in the receiver"
	^tally!
sortBlock
	^sortBlock!
sortBlock: aBlock
	sortBlock := aBlock.
	self reSort!
!Heap methodsFor: 'testing'!
isEmpty
	"Answer whether the receiver contains any elements."
	^tally = 0!
sorts: element1 before: element2
	"Return true if element1 should be sorted before element2.
	This method defines the sort order in the receiver"
	^ sortBlock
		ifNil: [element1 <= element2]
		ifNotNil: [sortBlock value: element1 value: element2]!
!Heap methodsFor: 'adding'!
add: anObject
	"Include newObject as one of the receiver's elements. Answer newObject."
	tally = array size ifTrue:[self grow].
	array at: (tally := tally + 1) put: anObject.
	self updateObjectIndex: tally.
	self upHeap: tally.
	^anObject!
!Heap methodsFor: 'removing'!
remove: oldObject ifAbsent: aBlock
	"Remove oldObject as one of the receiver's elements. If several of the 
	elements are equal to oldObject, only one is removed. If no element is 
	equal to oldObject, answer the result of evaluating anExceptionBlock. 
	Otherwise, answer the argument, oldObject."
	1 to: tally do:[:i| 
		(array at: i) = oldObject ifTrue:[^self privateRemoveAt: i]].
	^aBlock value!
removeAll

	array atAllPut: nil.
	tally := 0!
removeAt: index
	"Remove the element at given position"
	(index < 1 or:[index > tally]) ifTrue:[^self errorSubscriptBounds: index].
	^self privateRemoveAt: index!
removeFirst
	"Remove the first element from the receiver"
	^self removeAt: 1!
!Heap methodsFor: 'comparing'!
= otherCollection

	self == otherCollection ifTrue: [^ true].
	self species == otherCollection species ifFalse: [^ false].

	sortBlock = otherCollection sortBlock
		ifFalse: [ ^false ].

	^self hasEqualElements: otherCollection!
!Heap methodsFor: 'enumerating'!
do: aBlock
	"Evaluate aBlock with each of the receiver's elements as the argument."
	1 to: tally do:[:i| aBlock value: (array at: i)]!
select: aBlock 
	"Evaluate aBlock with each of my elements as the argument. Collect into
	a new collection like the receiver, only those elements for which aBlock
	evaluates to true."

	| answer |
	answer := self species sortBlock: sortBlock.
	self do: [ :each |
		(aBlock value: each)
			ifTrue: [ answer add: each ]].
	^ answer!
!Heap methodsFor: 'growing'!
grow
	"Become larger."
	self growTo: self size + self growSize.!
growSize
	"Return the size by which the receiver should grow if there are no empty slots left."
	^array size max: 5!
growTo: newSize
	"Grow to the requested size."
	| newArray |
	newArray := Array new: (newSize max: tally).
	newArray replaceFrom: 1 to: array size with: array startingAt: 1.
	array := newArray!
trim
	"Remove any empty slots in the receiver."
	self growTo: self size.!
!Heap methodsFor: 'private-heap'!
downHeap: anIndex
	"Check the heap downwards for correctness starting at anIndex.
	 Everything above (i.e. left of) anIndex is ok."
	| value k n j |
	anIndex = 0 ifTrue:[^self].
	n := tally bitShift: -1.
	k := anIndex.
	value := array at: anIndex.
	[k <= n] whileTrue:[
		j := k + k.
		"use max(j,j+1)"
		(j < tally and:[self sorts: (array at: j+1) before: (array at: j)])
				ifTrue:[ j := j + 1].
		"check if position k is ok"
		(self sorts: value before: (array at: j)) 
			ifTrue:[	"yes -> break loop"
					n := k - 1]
			ifFalse:[	"no -> make room at j by moving j-th element to k-th position"
					array at: k put: (array at: j).
					self updateObjectIndex: k.
					"and try again with j"
					k := j]].
	array at: k put: value.
	self updateObjectIndex: k.!
downHeapSingle: anIndex
	"This version is optimized for the case when only one element in the receiver can be at a wrong position. It avoids one comparison at each node when travelling down the heap and checks the heap upwards after the element is at a bottom position. Since the probability for being at the bottom of the heap is much larger than for being somewhere in the middle this version should be faster."
	| value k n j |
	anIndex = 0 ifTrue:[^self].
	n := tally bitShift: -1.
	k := anIndex.
	value := array at: anIndex.
	[k <= n] whileTrue:[
		j := k + k.
		"use max(j,j+1)"
		(j < tally and:[self sorts: (array at: j+1) before: (array at: j)])
				ifTrue:[	j := j + 1].
		array at: k put: (array at: j).
		self updateObjectIndex: k.
		"and try again with j"
		k := j].
	array at: k put: value.
	self updateObjectIndex: k.
	self upHeap: k!
upHeap: anIndex
	"Check the heap upwards for correctness starting at anIndex.
	 Everything below anIndex is ok."
	| value k kDiv2 tmp |
	anIndex = 0 ifTrue:[^self].
	k := anIndex.
	value := array at: anIndex.
	[ (k > 1) and:[self sorts: value before: (tmp := array at: (kDiv2 := k bitShift: -1))] ] 
		whileTrue:[
			array at: k put: tmp.
			self updateObjectIndex: k.
			k := kDiv2].
	array at: k put: value.
	self updateObjectIndex: k.!
!Heap methodsFor: 'private'!
array
	^array!
privateRemoveAt: index
	"Remove the element at the given index and make sure the sorting order is okay"
	| removed |
	removed := array at: index.
	array at: index put: (array at: tally).
	array at: tally put: nil.
	tally := tally - 1.
	index > tally ifFalse:[
		"Use #downHeapSingle: since only one element has been removed"
		self downHeapSingle: index].
	^removed!
privateReverseSort
	"Arrange to have the array sorted in reverse order.
	WARNING: this method breaks the heap invariants. It's up to the sender to restore them afterwards."
	| oldTally |
	oldTally := tally.
	[tally > 1] whileTrue:
		[array swap: 1 with: tally.
		tally := tally - 1.
		self downHeapSingle: 1].
	tally := oldTally!
setCollection: aCollection
	array := aCollection.
	tally := 0.!
setCollection: aCollection tally: newTally
	array := aCollection.
	tally := newTally.!
updateObjectIndex: index
	"If indexUpdateBlock is not nil, notify the object at index of its new position in the heap array."
	indexUpdateBlock ifNotNil: [
		indexUpdateBlock value: (array at: index) value: index]!
!Heap methodsFor: 'copying'!
postCopy

	array := array copy.
	tally := tally copy!
!Heap class methodsFor: 'instance creation'!
new
	^self basicNew setCollection: (Array new: 10)!
newFrom: aCollection
	"Create a new heap with all the elements from aCollection"
	^(self basicNew)
		setCollection: aCollection asArray copy tally: aCollection size;
		reSort;
		yourself!
sortBlock: aBlock
	"Create a new heap sorted by the given block"
	^self new sortBlock: aBlock!
withAll: aCollection sortBlock: sortBlock
	"Create a new heap with all the elements from aCollection"
	^(self basicNew)
		setCollection: aCollection asArray copy tally: aCollection size;
		sortBlock: sortBlock;
		yourself!
!Heap class methodsFor: 'examples'!
heapExample	"Heap heapExample"
	"Create a sorted collection of numbers, remove the elements
	sequentially and add new objects randomly.
	Note: This is the kind of benchmark a heap is designed for."
	| n rnd array time |
	n := 5000. "# of elements to sort"
	rnd := Random new.
	array := (1 to: n) collect:[:i| rnd next].
	"First, the heap version"
	time := Time millisecondsToRun:[| sorted |
		sorted := Heap withAll: array.
		1 to: n do: [ :i | 
			sorted removeFirst.
			sorted add: rnd next].
	].
	Transcript newLine; show:'Time for Heap: ', time printString,' msecs'.
	"The quicksort version"
	time := Time millisecondsToRun:[| sorted |
		sorted := SortedCollection withAll: array.
		1 to: n do:[:i| 
			sorted removeFirst.
			sorted add: rnd next].
	].
	Transcript newLine; show:'Time for SortedCollection: ', time printString,' msecs'.!
heapSortExample	"Heap heapSortExample"
	"Sort a random collection of Floats and compare the results with
	SortedCollection (using the quick-sort algorithm) and 
	ArrayedCollection>>mergeSortFrom:to:by: (using the merge-sort algorithm)."
	| n rnd array time |
	n := 10000. "# of elements to sort"
	rnd := Random new.
	array := (1 to: n) collect:[:i| rnd next].
	"First, the heap version"
	time := Time millisecondsToRun: [
		| sorted |
		sorted := Heap withAll: array.
		1 to: n do: [ :i | sorted removeFirst ]].
	Transcript newLine; show:'Time for heap-sort: ', time printString,' msecs'.
	"The quicksort version"
	time := Time millisecondsToRun:[| sorted |
		sorted := SortedCollection withAll: array.
	].
	Transcript newLine; show:'Time for quick-sort: ', time printString,' msecs'.
	"The merge-sort version"
	time := Time millisecondsToRun:[
		array mergeSortFrom: 1 to: array size by: [:v1 :v2| v1 <= v2].
	].
	Transcript newLine; show:'Time for merge-sort: ', time printString,' msecs'.!
