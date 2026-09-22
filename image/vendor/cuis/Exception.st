!Object subclass: #Exception
  instanceVariableNames: 'messageText tag signalContext handlerContexts outerContext handlerBlockNotCurtailed'
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Kernel-Exceptions'!
!Exception methodsFor: 'Camp Smalltalk'!
sunitExitWith: aValue

        self return: aValue!!
!Exception methodsFor: 'defaults'!
defaultResumeValue
	"Answer the value that by default should be returned if the exception is resumed"
	^nil!!
!Exception methodsFor: 'exceptionBuilder'!
messageText: signalerText
	
"Set an exception's message text."

	messageText := signalerText!!
tag: t
	"This message is not specified in the ANSI protocol, but that looks like an oversight because #tag is specified, and the spec states that the signaler may store the tag value."

	tag := t!!
!Exception methodsFor: 'exceptionDescription'!
defaultAction
	"The default action taken if the exception is signaled."

	self subclassResponsibility!!
description
	
"Return a textual description of the exception."

	| desc |
	desc := self class name asPlainString.
	^ self messageText
		ifNil: [ desc]
		ifNotNil: [ :mt | desc , ': ' , mt ]!!
messageText
	
"Return an exception's message text."

	^messageText!!
noHandler
	"No one has handled this error, but now give them a chance to decide how to debug it.  If none handle this either then open debugger (see UnhandedError-defaultAction)"

	UnhandledError signalForException: self!!
tag
       "Return an exception's tag value."

	^tag ifNil: [ self messageText ]!!
!Exception methodsFor: 'handling'!
isNested
	"Determine whether the current exception handler is within the scope of another handler for the same exception."

	^self topHandlerContext nextHandlerContext canHandleSignal: self!!
outer
	"Evaluate the enclosing exception action and return to here instead of signal if it resumes (see #resumeUnchecked:)."

	| prevOuterContext |
	handlerBlockNotCurtailed := true.
	self isResumable ifTrue: [
		prevOuterContext := outerContext.
		outerContext := thisContext contextTag.
	].
	self topHandlerContext nextHandlerContext handleSignal: self!!
pass
	"Yield control to the enclosing exception action for the receiver."

	| nextHandler |
	handlerBlockNotCurtailed := true.
	nextHandler := self topHandlerContext nextHandlerContext.
	self popHandlerContext.
	nextHandler handleSignal: self!!
resignalAs: replacementException
	"Signal an alternative exception in place of the receiver."

	handlerBlockNotCurtailed := true.
	signalContext resumeEvaluating: [replacementException signal]!!
resume
	"Return from the message that signaled the receiver."

	self resume: self defaultResumeValue!!
resume: resumptionValue
	"Return resumptionValue as the value of the signal message."

	handlerBlockNotCurtailed := true.
	self isResumable ifFalse: [IllegalResumeAttempt signal].
	self resumeUnchecked: resumptionValue!!
resumeUnchecked: resumptionValue
	"Return resumptionValue as the value of #signal, unless this was called after an #outer message, then return resumptionValue as the value of #outer."

	| ctxt |
	handlerBlockNotCurtailed := true.
	outerContext ifNil: [
		signalContext return: resumptionValue
	] ifNotNil: [
		ctxt := outerContext.
		outerContext := ctxt tempAt: 1. "prevOuterContext in #outer"
		ctxt return: resumptionValue
	].
!!
retry
	"Abort an exception handler and re-evaluate its protected block."

	handlerBlockNotCurtailed := true.
	self topHandlerContext restart!!
retryUsing: alternativeBlock
	"Abort an exception handler and evaluate a new block in place of the handler's protected block."

	handlerBlockNotCurtailed := true.
	self topHandlerContext restartWithNewReceiver: alternativeBlock
!!
return
	"Return nil as the value of the block protected by the active exception handler."

	self return: nil!!
return: returnValue
	"Return the argument as the value of the block protected by the active exception handler."

	handlerBlockNotCurtailed := true.
	self topHandlerContext return: returnValue!!
!Exception methodsFor: 'debug support'!
canSearchForSignalerContext
	"This method is /only/ to support the debugger's catching of exceptions in stepIntoBlock."
	^signalContext isContext!!
!Exception methodsFor: 'printing'!
printOn: stream

	stream nextPutAll: self description!!
receiver

	^ self signalerContext receiver!!
signalerContext
	"Find the first sender of signal(:)"

	^ signalContext findContextSuchThat: [:ctxt |
		(ctxt receiver == self or: [ctxt receiver == self class]) not]!!
!Exception methodsFor: 'priv handling'!
evaluateHandlerBlock: aBlock

	| handlerEx |
	handlerBlockNotCurtailed := false.
	^[
			| answer |
			answer := [aBlock valueWithPossibleArgument: self] on: Exception do: [:ex | handlerEx := ex.  ex pass].
			signalContext := nil.	"To enable recycling of exceptions, but only after handler block has finished execution."
			handlerBlockNotCurtailed := true.
			answer
	] ifCurtailed:
		[
			signalContext := nil.	"To enable recycling of exceptions, but only after handler block has finished execution."
			(handlerBlockNotCurtailed not and: [handlerEx isNil or: [handlerEx handlerBlockNotCurtailed not]])
				ifTrue: [
					"Please see
					https://lists.cuis.st/mailman/archives/cuis-dev/2019-October/000800.html
					https://lists.cuis.st/mailman/archives/cuis-dev/2019-October/000809.html
					Also see the rest of the tread in detail.
					This is work in progress."
					(Preferences at: #allowNonLocalReturnsInExceptionHandlers)
						ifFalse: [ self error: 'Exception handler blocks must not do non local returns' ]
						ifTrue: [
							(Preferences at: #warnAboutNonLocalReturnsInExceptionHandlers)
								ifTrue: [ 'It is advisable to avoid method returns (non local returns) in exception handler blocks' print ].
							handlerBlockNotCurtailed := true ].
					]
				ifFalse: [handlerBlockNotCurtailed := true]
		]!!
handlerBlockNotCurtailed

	^handlerBlockNotCurtailed!!
handlerContexts

	^handlerContexts!!
handlerContexts: aCollection

	handlerContexts := aCollection!!
initialize

	self handlerContexts: OrderedCollection new!!
isResumable
	"Determine whether an exception is resumable."

	^ true!!
popHandlerContext

	self handlerContexts removeLast!!
pushHandlerContext: aContextTag

	self handlerContexts add: aContextTag!!
topHandlerContext

	^self handlerContexts isEmpty ifTrue: [nil] ifFalse: [self handlerContexts last]!!
!Exception methodsFor: 'signaling'!
signal

	^self signalIn: thisContext!!
signal: signalerText
	"Signal the occurrence of an exceptional condition with a specified textual description."

	self messageText: signalerText.
	^ self signal!!
signalIn: aContext
	"Ask ContextHandlers in the sender chain to handle this signal.  The default is to execute and return my defaultAction."

	signalContext ifNotNil: [self error: 'This exception has already been signaled and its handler block is being executed.'].
	signalContext := aContext contextTag.
	^ aContext nextHandlerContext handleSignal: self!!
!Exception class methodsFor: 'exceptionInstantiator'!
signal
	"Signal the occurrence of an exceptional condition."

	^ self new signal!!
signal: signalerText
	"Signal the occurrence of an exceptional condition with a specified textual description."

	^ self new signal: signalerText!!
!Exception class methodsFor: 'error handling'!
handles: exception
	"Determine whether an exception handler will accept a signaled exception."

	^exception isKindOf: self!!
!Exception class methodsFor: 'Camp Smalltalk'!
sunitSignalWith: aString

        ^self signal: aString!!
!Exception class methodsFor: 'handling condition creation'!
- anExceptionHandlingCondition

	"See ExceptionHandlingCondition>>-"

	^anExceptionHandlingCondition createFilterConditionWithExceptionType: self!!
!Exception class methodsFor: 'handling condition creation - private'!
createFilterConditionWithExceptionType: anExceptionType

	^FilterExceptionHandlingCondition handling: anExceptionType filtering: self!!
createFilterConditionWithFilterCondition: aFilterExceptionHandlingCondition 
	
	^FilterExceptionHandlingCondition handling: aFilterExceptionHandlingCondition filtering: self!!
createFilterConditionWithOrCondition: anOrExceptionHandlingCondition 
	
	^FilterExceptionHandlingCondition handling: anOrExceptionHandlingCondition filtering: self!!
createOrConditionWithExceptionType: anExceptionType

	^OrExceptionHandlingCondition handling: anExceptionType or: self
!!
createOrConditionWithFilterCondition: aFilterExceptionHandlingCondition 
	
	^aFilterExceptionHandlingCondition handleCondition, self - aFilterExceptionHandlingCondition filterCondition 
	
	!!
createOrConditionWithOrCondition: anOrExceptionHandlingCondition 
	
	^OrExceptionHandlingCondition handling: anOrExceptionHandlingCondition or: self!!
