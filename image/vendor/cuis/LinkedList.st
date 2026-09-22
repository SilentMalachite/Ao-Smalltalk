!SequenceableCollection subclass: #LinkedList
  instanceVariableNames: 'firstLink lastLink'
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Kernel-Collections'!
!LinkedList methodsFor: 'accessing'!
at: index

	^self at: index ifAbsent: [ self errorSubscriptBounds: index ]!!
at: index ifAbsent: exceptionBlock

	| i |
	index < 1 ifTrue: [ ^exceptionBlock value ].
	i := 0.
	self do: [ :link |
		(i := i + 1) = index ifTrue: [ ^ link ]].
	^ exceptionBlock value!!
first
	"Answer the first link;  create an error if the receiver is empty."

	self emptyCheck.
	^firstLink!!
last
	"Answer the last link;  create an error if the receiver is empty."

	self emptyCheck.
	^lastLink!!
!LinkedList methodsFor: 'testing'!
isEmpty
	^firstLink == nil!!
!LinkedList methodsFor: 'adding'!
add: aLink 
	"Add aLink to the end of the receiver's list."

	^self addLast: aLink!!
add: link before: otherLink

	| aLink |
	firstLink == otherLink ifTrue: [^ self addFirst: link].
	aLink := firstLink.
	[aLink == nil] whileFalse: [
		aLink nextLink == otherLink ifTrue: [
			link nextLink: aLink nextLink.
			aLink nextLink: link.
			^ link
		].
		 aLink := aLink nextLink.
	].
	^ self errorNotFound: otherLink!!
addFirst: aLink 
	"Add aLink to the beginning of the receiver's list."

	self isEmpty ifTrue: [lastLink := aLink].
	aLink nextLink: firstLink.
	firstLink := aLink.
	^aLink!!
addLast: aLink 
	"Add aLink to the end of the receiver's list."

	self isEmpty
		ifTrue: [firstLink := aLink]
		ifFalse: [lastLink nextLink: aLink].
	lastLink := aLink.
	^aLink!!
!LinkedList methodsFor: 'removing'!
remove: aLink ifAbsent: aBlock  
	"Remove aLink from the receiver. If it is not there, answer the result of
	evaluating aBlock."

	| tempLink |
	aLink == firstLink
		ifTrue: [firstLink := aLink nextLink.
				aLink == lastLink
					ifTrue: [lastLink := nil]]
		ifFalse: [tempLink := firstLink.
				[tempLink ifNil: [^aBlock value].
				 tempLink nextLink == aLink]
					whileFalse: [tempLink := tempLink nextLink].
				tempLink nextLink: aLink nextLink.
				aLink == lastLink
					ifTrue: [lastLink := tempLink]].
	aLink nextLink: nil.
	^aLink!!
removeFirst
	"Remove the first element.  If the receiver is empty, cause an error;
	otherwise answer the removed element.  Using the sequence addFirst:/removeFirst
	causes the receiver to behave as a stack; using addLast:/removeFirst causes the
	receiver to behave as a queue."

	| oldLink |
	self emptyCheck.
	oldLink := firstLink.
	firstLink == lastLink
		ifTrue: [firstLink := nil. lastLink := nil]
		ifFalse: [firstLink := oldLink nextLink].
	oldLink nextLink: nil.
	^oldLink!!
removeLast
	"Remove the receiver's last element.  If the receiver is empty, cause an error;
	otherwise answer the removed element.  Using addLast:/removeLast causes the
	receiver to behave as a stack; using addFirst:/removeLast causes the receiver to
	behave as a queue."

	| oldLink aLink |
	self emptyCheck.
	oldLink := lastLink.
	firstLink == lastLink
		ifTrue: [firstLink := nil. lastLink := nil]
		ifFalse: [aLink := firstLink.
				[aLink nextLink == oldLink] whileFalse:
					[aLink := aLink nextLink].
				 aLink nextLink: nil.
				 lastLink := aLink].
	oldLink nextLink: nil.
	^oldLink!!
!LinkedList methodsFor: 'enumerating'!
do: aBlock
	| aLink |
	aLink := firstLink.
	[aLink == nil] whileFalse:
		[aBlock value: aLink.
		 aLink := aLink nextLink]!!
species

	^ Array!!
