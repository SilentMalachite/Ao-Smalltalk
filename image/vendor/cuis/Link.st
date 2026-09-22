!Object subclass: #Link
  instanceVariableNames: 'nextLink'
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Kernel-Support'!
!Link methodsFor: 'accessing'!
nextLink
	"Answer the Link to which the receiver points."
	^nextLink!
nextLink: aLink 
	"Store the argument, as the Link to which the receiver refers."
	^nextLink := aLink!
!Link class methodsFor: 'instance creation'!
nextLink: aLink 
	"Answer an instance of me referring to the argument, aLink."

	^self new nextLink: aLink; yourself!
