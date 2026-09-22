!Exception subclass: #Notification
  instanceVariableNames: ''
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Exceptions-Kernel'!
!Notification methodsFor: 'exceptionDescription'!
defaultAction
	"No action is taken. The value nil is returned as the value of the message that signaled the exception."


	^nil!!
isResumable
	"Answer true. Notification exceptions by default are specified to be resumable."

	^true!!
!Notification methodsFor: 'debugger updating'!
isToUpdateDebuggerOn: aContext

	^tag isArray
		and: [tag size = 2
		and: [(tag first == aContext or: [tag first hasSender: aContext])]]!!
withNewDebuggerLabelOn: aContext do: labelBlock ifNone: noneBlok

	^(self isToUpdateDebuggerOn: aContext)
		ifTrue: [ labelBlock value: tag second description ]
		ifFalse: noneBlok !!
!Notification class methodsFor: 'debugger updating - signaling'!
signalToUpdateDebuggerOn: unhandledErrorSignalerContext dueTo: anError

	self new 
		tag: {unhandledErrorSignalerContext. anError};
		signal.!!
