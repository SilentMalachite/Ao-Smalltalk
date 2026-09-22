!Exception subclass: #Error
  instanceVariableNames: ''
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Kernel-Exceptions'!
!Error methodsFor: 'private'!
isResumable
	"Determine whether an exception is resumable."

	^ false!!
!Error methodsFor: 'exceptionDescription'!
defaultAction

	self noHandler!!
