!Object subclass: #Timespan
  instanceVariableNames: 'start duration'
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Kernel-Chronology'!
!Timespan methodsFor: 'ansi protocol'!
+ aDuration

	^self class classDefinesDuration
		ifTrue: [ self class including: start + aDuration ]
		ifFalse: [ self class starting: start + aDuration duration: duration ]!!
- aDurationOrTimespan

	^ aDurationOrTimespan subtractFrom: self!!
= comparand
 	self == comparand ifTrue: [ ^ true ].
	^ self class == comparand class 
		and: [ self start = comparand start ]
		and: [ self duration = comparand duration ]
.!!
hash

	^ start hash bitXor: duration hash
!!
!Timespan methodsFor: 'squeak protocol'!
duration
	"Answer the Duration of this timespan"

	^ duration
!!
end

	^ self duration totalNanoSeconds = 0
		ifTrue: [ self start ]
		ifFalse: [ self next start - DateAndTime clockPrecision ]!!
includes: operand
	"Operand might be a Timespan or a DateAndtime"

	^ (operand is: #Timespan)
			ifTrue: [ (self includes: operand start)
						and: [ self includes: operand end ] ]
			ifFalse: [ operand between: start and: self end ]!!
includesAllOf: aCollection 
	"Answer whether all the elements of aCollection are in the receiver."

	aCollection do: [:elem | (self includes: elem) ifFalse: [^ false]].
	^ true
!!
includesAnyOf: aCollection 
	"Answer whether any element of aCollection is included in the receiver"

	aCollection do: [ :elem | (self includes: elem) ifTrue: [^ true]].
	^false
!!
intersection: aTimespan
	 "Return the Timespan both have in common, or nil"
	"Warning: There's something possibly wrong here. See 
	http://lists.gforge.inria.fr/pipermail/pharo-project/2011-May/048253.html
	
	It seems reasonable to expect this to answer true. However it answers false:
	|start end span|
	start :=DateAndTime now.
	end := start + 1 hour.
	span := Timespan starting: start ending: end.
	(span intersection: span) = span
	"

	 | aBegin anEnd |
	 aBegin := self start max: aTimespan start.
	 anEnd := self end min: aTimespan end.
	 anEnd < aBegin ifTrue: [^nil].

	 ^ Timespan starting: aBegin ending: anEnd!!
printOn: aStream


	super printOn: aStream.
	aStream 
		nextPut: $(;
		print: start;
		nextPut: $D;
		print: duration;
		nextPut: $).
!!
start
	"Answer the start DateAndTime of this timespan"

	^ start
!!
start: aDateAndTime
	"Store the start DateAndTime of this timespan"

	self assert: (aDateAndTime is: #dateAndTime).
	start := aDateAndTime!!
to: anEnd
	"Answer an Timespan. anEnd must be aDateAndTime"

	self assert: (anEnd is: #dateAndTime).
	^ Timespan starting: self start ending: anEnd!!
union: aTimespan
	 "Return the Timespan spanned by both"
	"
	| union timespan |
	timespan := Timespan starting:
					(DateAndTime year: 2003 month: 03 day: 22 hour: 12 minute: 0 second: 0)
						duration: (Duration hours: 100).
	union := timespan union: timespan.
	
	self 
		assert: (union start = timespan start);
		assert: (union duration = timespan duration)
	"

	| aBegin anEnd |

	aBegin := self start min: aTimespan start.
	anEnd := self end max: aTimespan end.
	^ Timespan starting: aBegin ending: anEnd + DateAndTime clockPrecision!!
!Timespan methodsFor: 'enumerating'!
every: aDuration do: aBlock

	| element end |
	element := self start.
	end := self end.
	[ element <= end ] whileTrue:
	
	[ aBlock value: element.
		element := element + aDuration. ]
!!
workDatesDo: aBlock
	"Exclude Saturday and Sunday"

	self do: aBlock with: start date when: [ :d | d dayOfWeek < 6 ].
!!
!Timespan methodsFor: 'private'!
do: aBlock with: aFirstElement

	self do: aBlock with: aFirstElement when: [ :t | true ].
!!
do: aBlock with: aFirstElement when: aConditionBlock

	| element end |
	element := aFirstElement.
	end := self end.
	[ element start <= end ] whileTrue:
	
	[(aConditionBlock value: element)
			ifTrue: [ aBlock value: element ].
		element := element next. ]!!
duration: aDuration
	"Set the Duration of this timespan"

	duration := aDuration
!!
subtractDuration: aDuration

	^self class classDefinesDuration
		ifTrue: [ self class including: start - aDuration ]
		ifFalse: [ self class starting: start - aDuration duration: duration ]!!
subtractMonth: aYear

	^ self subtractTimespan: aYear!!
subtractTimespan: aTimespan
	"
	(Month month: 'March' year: 2017) - (Month month: 'January' year: 2017)
	(Month month: 'February' year: 2017) - (Month month: 'January' year: 2017)

	(Year yearNumber: 2016) - (Year yearNumber: 2015).
	(Year yearNumber: 2017) - (Year yearNumber: 2016).
	(Year yearNumber: 2017) - (Year yearNumber: 2015).

	(Year yearNumber: 2018) - (Date today). 					'Error'.
	"
	aTimespan duration = self duration ifFalse: [
		self error: 'Can not subtract Timespans of different duration' ].
	
	^self start subtractDateAndtime: aTimespan start!!
subtractYear: aYear

	^ self subtractTimespan: aYear!!
!Timespan methodsFor: 'smalltalk-80'!
next

	^self class classDefinesDuration
		ifTrue: [ self class including: start + duration ]
		ifFalse: [ self class starting: start + duration duration: duration ]!!
previous
	"
	(Month month: 10 year: 2017) previous
	(Year yearNumber: 2016) previous
	"
	^self class classDefinesDuration
		ifTrue: [ self class including: self end - duration ]
		ifFalse: [ self class starting: start - duration duration: duration ]!!
!Timespan methodsFor: 'testing'!
is: aSymbol
	^aSymbol == #Timespan or: [ super is: aSymbol ]!!
!Timespan methodsFor: 'double dispatching'!
includingTimespanOf: aTimespanClass

	^ aTimespanClass includingTimespan: self!!
subtractFrom: aTimespan

	^ aTimespan subtractTimespan: self!!
!Timespan class methodsFor: 'squeak protocol'!
current
	"
	Date current
	Week current
	Month current
	"

	^ self including: DateAndTime now!!
fromString: aString
	"Please call with specific subclass."

	^ self readFrom: aString readStream!!
new
	"Answer a Timespan starting on the Squeak epoch: 1 January 1901"
	"Perhaps you need
		Timespan current
	"
	^ self shouldNotImplement!!
starting: aDateAndTime duration: aDuration
	self classDefinesDuration ifTrue: [
		self shouldNotImplement ].
	self assert: (aDateAndTime is: #dateAndTime).
	^ self basicNew
 		start: aDateAndTime;
		duration: aDuration;
		yourself!!
!Timespan class methodsFor: 'inquiries'!
classDefinesDuration
	"Answer yes if we must have a fixed duration.
	Examples of this are Date, Month, Week, Year"
	^false!!
mustEncompassWholeDates
	"Answer yes if we must start and end at midnight.
	Examples of this are Date, Month, Week, Year"
	^false!!
!Timespan class methodsFor: 'instance creation'!
including: aDateAndTime

	^ aDateAndTime includingTimespanOf: self!!
includingDateAndTime: aDateAndTime

	^ self starting: aDateAndTime duration: `Duration zero`.!!
includingTimespan: aTimespan

	| ending starting |
	starting := self includingDateAndTime: aTimespan start.
	ending := self includingDateAndTime: aTimespan end.
	starting = ending ifTrue: [ ^ starting ].
	self error: aTimespan printString, ' can not be included in a ', self name!!
starting: startDateAndTime ending: endDateAndTime
	self classDefinesDuration ifTrue: [
		self shouldNotImplement ].
	self assert: (startDateAndTime is: #dateAndTime).
	self assert: (endDateAndTime is: #dateAndTime).
	^ self 
		starting: startDateAndTime 
		duration: endDateAndTime - startDateAndTime!!
