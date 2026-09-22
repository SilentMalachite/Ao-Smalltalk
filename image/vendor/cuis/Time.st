!Magnitude subclass: #Time
  instanceVariableNames: 'seconds nanos'
  classVariableNames: 'MillisecondClockOffset'
  poolDictionaries: ''
  category: 'Kernel-Chronology'!
!Time methodsFor: 'ansi protocol'!
< aTime

	^seconds < aTime privateSeconds or: [ 
		seconds = aTime privateSeconds and: [ nanos < aTime privateNanos ]]!
= aTime

	"Any object is equal to itself"
	self == aTime ifTrue: [ ^ true ].

	self class == aTime class ifFalse: [ ^false ].
	^ seconds = aTime privateSeconds and: [ nanos = aTime privateNanos ]!
hash

	^ seconds hash bitXor: nanos hash!
hour

	^ self hour24!
hour12
	"Answer an <integer> between 1 and 12, inclusive, representing the hour 
	of the day in the 12-hour clock of the local time of the receiver."
	^ self hour24 - 1 \\ 12 + 1!
hour24


	^ self asDurationSinceMidnight hours!
meridianAbbreviation

	^ self hour < 12 ifTrue: ['AM'] ifFalse: ['PM'].!
minute

	^ self asDurationSinceMidnight minutes!
second


	^ self asDurationSinceMidnight seconds!
!Time methodsFor: 'printing'!
hhmm24
	"Return a string of the form 1123 (for 11:23 am), 2154 (for 9:54 pm), of exactly 4 digits"

	^(String streamContents: 
		[ :aStream | self print24: true showSeconds: false on: aStream ])
			copyWithout: $:!
print24
	"Return as 8-digit string 'hh:mm:ss', with leading zeros if needed"

	^String streamContents: [ :aStream |
		self print24: true showSeconds: true on: aStream ]!
print24: hr24 on: aStream 
	"Format is 'hh:mm:ss' or 'h:mm:ss am' "

	self print24: hr24 showSeconds: true on: aStream!
print24: hr24 showSeconds: showSeconds on: aStream
	"Format is 'hh:mm:ss' or 'h:mm:ss am'  or, if showSeconds is false, 'hh:mm' or 'h:mm am'"

	^self print24: hr24 showSeconds: showSeconds showSecondsFraction: false on: aStream!
print24: hr24 showSeconds: showSeconds showSecondsFraction: showSecondsFraction on: aStream
	"Format is 'hh:mm:ss' or 'h:mm:ss am'  or, if showSeconds is false, 'hh:mm' or 'h:mm am'"

	| h m s |
	h := self hour.
	m := self minute.
	s := self second.
	hr24
		ifTrue: [
			h < 10 ifTrue: [ aStream nextPutAll: '0' ].
			h printOn: aStream ]
		ifFalse: [
			h > 12
				ifTrue: [h - 12 printOn: aStream]
				ifFalse: 
					[h < 1
						ifTrue: [ 12 printOn: aStream ]
						ifFalse: [ h printOn: aStream ]]].

	aStream nextPutAll: (m < 10 ifTrue: [':0'] ifFalse: [':']).
	m printOn: aStream.

	showSeconds ifTrue: [
		aStream nextPutAll: (s < 10 ifTrue: [':0'] ifFalse: [':']).
		(self nanoSecond = 0 or: [ showSecondsFraction not ])
			ifTrue: [s asInteger printOn: aStream]
			ifFalse: [(s + (1.0 * self nanoSecond / Time nanosInSecond)) printOn: aStream]].

	hr24 ifFalse: [
		aStream nextPutAll: (h < 12 ifTrue: [' am'] ifFalse: [' pm']) ]!
print24: hr24 showSecondsFraction: showSecondsFraction on: aStream
	"Format is 'hh:mm:ss.ssssss' or 'h:mm:ss.ssssss am'  or, if showSecondsFraction is false, 'hh:mm:ss' or 'h:mm:ss am'"

	^self print24: hr24 showSeconds: true showSecondsFraction: showSecondsFraction on: aStream!
printMinutes
	"Return as string 'hh:mm pm'  "

	^String streamContents:
		[ :aStream | self print24: false showSeconds: false on: aStream ]!
printOn: aStream

	self print24: false
		showSeconds: (seconds ~= 0
				or: [ nanos ~= 0])
		showSecondsFraction: nanos ~= 0
		on: aStream!
storeOn: aStream

	aStream print: self printString; nextPutAll: ' asTime'!
!Time methodsFor: 'squeak protocol'!
nanoSecond


	^ nanos!
!Time methodsFor: 'private'!
privateNanos
	^nanos!
privateSeconds
	^seconds!
seconds: secondCount
	"Private - only used by Time class."

	seconds := secondCount.
	nanos := 0!
seconds: secondCount nanoSeconds: nanoCount 
	"Private - only used by Time class."

	self assert: nanoCount < Time nanosInSecond.
	seconds := secondCount.
	nanos := nanoCount!
!Time methodsFor: 'public protocol'!
- aTime
	^ self asDurationSinceMidnight - aTime asDurationSinceMidnight!
asDurationSinceMidnight

	"Answer the duration since midnight"

	^ Duration seconds: seconds nanoSeconds: nanos!
!Time class methodsFor: 'benchmarks'!
benchmarkPrimitiveResponseDelay
	"
	Time benchmarkPrimitiveResponseDelay
	"
	"Benchmark the overhead for primitive dispatches with an active Delay.
	On the VM level, this tests the efficiency of ioLowResMSecs."

	"PII/400 Windows98: 0.128 microseconds per prim"

	"ar 9/6/1999: This value is *extremely* important for stuff like sockets etc.
	I had a bad surprise when Michael pointed this particular problem out:
	Using the hardcoded clock() call for ioLowResMSecs on Win32 resulted in an overhead
	of 157.4 microseconds per primitive call - meaning you can't get no more than
	approx. 6000 primitives per second on my 400Mhz PII system with an active delay!!
	BTW, it finally explains why Squeak seemed soooo slow when running PWS or 
	other socket stuff. The new version (not using clock() but some Windows function) 
	looks a lot better (see above; approx. 8,000,000 prims per sec with an active delay)."

	| nLoops bb index baseTime actualTime delayTime |
	delayTime := 5000. "Time to run this test is approx. 3*delayTime"

	Delay anyActive ifTrue:[
		^self notify:'Some delay is currently active.
Running this benchmark will not give any useful result.'].

	bb := Array new: 1. "The object we send the prim message to"

	"Compute the # of loops we'll run in a decent amount of time"
	[(Delay forMilliseconds: delayTime) wait] 
		forkAt: Processor userInterruptPriority.

	nLoops := 0.
	[Delay anyActive] whileTrue:[
		bb basicSize; basicSize; basicSize; basicSize; basicSize; 
			basicSize; basicSize; basicSize; basicSize; basicSize.
		nLoops := nLoops + 1.
	].

	"Flush the cache and make sure #basicSize is in there"
	Object flushCache.
	bb basicSize.

	"Now run the loop without any active delay
	for getting an idea about its actual speed."
	baseTime := self millisecondClockValue.
	index := nLoops.
	[index > 0] whileTrue:[
		bb basicSize; basicSize; basicSize; basicSize; basicSize; 
			basicSize; basicSize; basicSize; basicSize; basicSize.
		index := index - 1.
	].
	baseTime := self millisecondClockValue - baseTime.

	"Setup the active delay but try to never make it active"
	[(Delay forMilliseconds: delayTime + delayTime) wait] 
		forkAt: Processor userInterruptPriority.

	"And run the loop"
	actualTime := self millisecondClockValue.
	index := nLoops.
	[index > 0] whileTrue:[
		bb basicSize; basicSize; basicSize; basicSize; basicSize; 
			basicSize; basicSize; basicSize; basicSize; basicSize.
		index := index - 1.
	].
	actualTime := self millisecondClockValue - actualTime.

	"And get us some result"
	^String streamContents: [ :strm |
		(actualTime - baseTime) * 1000 asFloat / (nLoops * 10) printOn: strm fractionDigits: 3.
		strm nextPutAll: ' microseconds overhead per primitive call' ]!
!Time class methodsFor: 'general inquiries'!
highResTimerTicksPerMillisecond
	"
	Time highResTimerTicksPerMillisecond
	"
	| t0 ticks0 ticks1 ticksPerMSec |
	t0 := Time millisecondClockValue + 2.
	[Time millisecondClockValue >= t0] whileFalse.
	ticks0 := Time primHighResClock.
	[Time millisecondClockValue >= (t0 + 100)] whileFalse.
	ticks1 := Time primHighResClock.
	ticksPerMSec := (ticks1 - ticks0) 
			// (Time millisecondClockValue - t0).
	"Retry if rollover!!"
	^ ticksPerMSec < 0
		ifTrue: [ self highResTimerTicksPerMillisecond ]
		ifFalse: [ ticksPerMSec ]!
humanWordsForSecondsAgo: secs
	| date today |
	"Return natural language for this date and time in the past."

	secs <= 1 ifTrue: [^ 'a second ago'].
	secs < 45 ifTrue: [^ secs printString, ' seconds ago'].
	secs < 90 ifTrue: [^ 'a minute ago'].
	secs < "45*60" 2700 ifTrue: [^ (secs//self secondsInMinute) printString, ' minutes ago'].
	secs < "90*60" 5400 ifTrue: [^ 'an hour ago'].
	secs < "18*60*60" 64800 ifTrue: [^ (secs//self secondsInHour) printString, ' hours ago'].
	"now work with dates"
	today := Date today.
	date := (DateAndTime now - (Duration seconds: secs)) date.
	date = Date yesterday ifTrue: [^ 'yesterday'].
	date > (today - 8 days) ifTrue: [^ 'last ', date dayOfWeekName].
	date > (today - 13 days) ifTrue: [^ 'a week ago'].
	date > (today - 28 days) ifTrue: [
		^ ((today start - date start) days //7) printString, ' weeks ago'].
	date > (today - 45 days) ifTrue: [^ 'a month ago'].
	date > (today - 300 days) ifTrue: [^ 'last ', date monthName].
	^ date monthName, ', ', date yearNumber printString

"Example
#(0.5 30 62 130 4000 10000 60000 90000 345600 864000 1728000 3456000 17280000 34560000 345600000) 
		collect: [:ss | Time humanWordsForSecondsAgo: ss].
"!
localMicrosecondClock
	"Answer the number of microseconds since the Smalltalk epoch, 
	i.e. 00:00 on the morning of January 1, 1901, in the local host time zone.
	In other words, the number of microseconds since it was that time in this time zone.
	Answer might be a LargePositiveInteger (very likely on 32-bit images, very unlikely on 64-bit images)."

	^self primLocalMicrosecondClock!
localMillisecondClock
	"Answer the number of milliseconds since the Smalltalk epoch, 
	i.e. 00:00 on the morning of January 1, 1901, in the local host time zone.
	In other words, the number of microseconds since it was that time in this time zone."

	^self primLocalMicrosecondClock // 1000!
localSecondClock
	"Answer the number of seconds since the Smalltalk epoch, 
	i.e. 00:00 on the morning of January 1, 1901, in the local host time zone.
	In other words, the number of microseconds since it was that time in this time zone."

	^self primLocalMicrosecondClock // 1000000!
microsecondsToRun: timedBlock 
	"Answer the number of microseconds timedBlock takes to return its value."

	| initialMicroseconds |
	initialMicroseconds := self localMicrosecondClock.
	timedBlock value.
	^self localMicrosecondClock - initialMicroseconds!
millisecondClockValue
	"Answer the number of milliseconds since the millisecond clock was last reset or rolled over.
	Answer 0 if the primitive fails.
	
	Note: Prefer #localMillisecondClock, that doesn't rollover."

	^ self primMillisecondClock!
millisecondsToRun: timedBlock 
	"Answer the number of milliseconds timedBlock takes to return its value."

	| initialMilliseconds |
	initialMilliseconds := self localMillisecondClock.
	timedBlock value.
	^self localMillisecondClock - initialMilliseconds!
!Time class methodsFor: 'smalltalk-80'!
dateAndTimeFromSeconds: secondCount

	^ {
		Date fromSeconds: secondCount.
		self fromSeconds: secondCount \\ self secondsInDay}!
dateAndTimeNow
	"Answer a two-element Array of (Date today, Time now)."

	| dateAndTime |
	dateAndTime := DateAndTime now.
	^{ dateAndTime date . dateAndTime time }!
fromSeconds: secondCount 
	"Answer an instance of me that is secondCount number of seconds since midnight."

	| integerSeconds nanos |
	integerSeconds := secondCount truncated.
	integerSeconds = secondCount
		ifTrue: [nanos := 0]
		ifFalse: [nanos := (secondCount - integerSeconds * self nanosInSecond) asInteger].
	^ self seconds: integerSeconds nanoSeconds: nanos!
new
	"Answer a Time representing midnight"

	^ self midnight!
readFrom: aStream
	"Read a Time from the stream in the form:
		<hour>:<minute>:<second> <am/pm>

	<minute>, <second> or <am/pm> may be omitted.  e.g. 1:59:30 pm; 8AM; 15:30"

	| hour minute second nanos ampm |
	hour := Integer readFrom: aStream.
	minute := 0.
	second := 0.
	nanos := 0.
	(aStream peekFor: $:) 
		ifTrue: [
			minute := Integer readFrom: aStream.
			(aStream peekFor: $:) ifTrue: [
				second := Integer readFrom: aStream.
				(aStream peekFor: $.)
					ifTrue: [ | nanosStart scale |
						nanosStart := aStream position.
						nanos := Integer readFrom: aStream.
						"As if there were trailing zeros to fill 9 positions"
						scale := 10 raisedToInteger: nanosStart + 9 - aStream position.
						nanos := nanos * scale ]]].
	aStream skipSeparators.
	(aStream atEnd not and: ['ap' includes: aStream peek]) ifTrue: [
		ampm := aStream next asLowercase.
		(ampm == $p and: [hour < 12]) ifTrue: [hour := hour + 12].
			(ampm == $a and: [hour = 12]) ifTrue: [hour := 0].
		(aStream peekFor: $m) ifFalse: [aStream peekFor: $M ]].
	^ self 
		hour: hour 
		minute: minute 
		second: second 
		nanoSecond: nanos.

	"
	Time readFrom: '2:23:09 pm' readStream
	Time readFrom: '2:23:09.0 pm' readStream
	Time readFrom: '2:23:09.123402 pm' readStream
	Time readFrom: '2:23:09.00123402 pm' readStream
	Time readFrom: '2:23:09.001234020 pm' readStream
	Time readFrom: '2:23:09.000123402 pm' readStream
	"!
!Time class methodsFor: 'squeak protocol'!
afterMidnight: aDuration
	"Answer a Time aDuration after midnight.
	If aDuration is longer than a day, take the day fraction."

	| nanoCount secondCount |
	secondCount := aDuration totalSeconds \\ self secondsInDay.
	nanoCount := aDuration nanoSeconds.
	^ self basicNew seconds: secondCount nanoSeconds: nanoCount!
fromString: aString
	^ self readFrom: aString readStream!
hour: hour minute: minute second: second
	"Answer a Time"

	^ self hour: hour minute: minute second: second nanoSecond: 0!
hour: hour minute: minute second: second nanoSecond: nanoCount
	"Answer a Time"

	^ self 
		seconds: (hour * self secondsInHour) + (minute * self secondsInMinute) + second 
		nanoSeconds: nanoCount!
midnight

	^ self seconds: 0!
noon

	^ self seconds: self secondsInDay / 2!
seconds: seconds
	"Answer a Time from midnight."

	^ self afterMidnight: (Duration seconds: seconds)!
seconds: seconds nanoSeconds: nanoCount
	"Answer a Time from midnight."

	^ self afterMidnight: (Duration seconds: seconds nanoSeconds: nanoCount)!
!Time class methodsFor: 'ansi protocol'!
now
	"Answer a Time representing the time right now - this is a 24 hour clock.
	Precision is microsecond."

	| microseconds utcMicroSsecondsAndOffset |
	utcMicroSsecondsAndOffset := Time primUtcWithOffset: (Array new: 2).
	microseconds := (utcMicroSsecondsAndOffset first + (utcMicroSsecondsAndOffset second * 1000000)).
	^ self seconds:  (microseconds // 1000000) nanoSeconds: (microseconds \\ 1000000) * 1000.!
!Time class methodsFor: 'primitives'!
primHighResClock	
	"Primitive. Answer the value of the high resolution clock if this computer has one.
	Usually, this should be the highest resolution value available, for example on Intel
	it will be the value of the time stamp counter register.
	Answer might be a LargePositiveInteger (very likely on 32-bit images, very unlikely on 64-bit images).
	Implemented on Cog, but not in standard interpreter VMs."
	"
	Time primHighResClock
	On Cog on Linux, OS-X and Windows, this gives sub nano second ticks!!

	Time highResTimerTicksPerMillisecond
	"
	"Not really a clock, but a timer or ticker"

	<primitive: 'primitiveHighResClock'>
	^0!
primLocalMicrosecondClock
	"Answer the number of microseconds since the local time zone Smalltalk epoch, 
	i.e. 00:00 on the morning of January 1, 1901, in local time.
	Answer might be a LargePositiveInteger (very likely on 32-bit images, very unlikely on 64-bit images).
	Cog VMs implement this. Interpreters might not."
	"
	Time primLocalMicrosecondClock
	Time primLocalMicrosecondClock/ 1000 / 1000 / 60 / 60 / 24 / 365.25
	"
	<primitive: 241>
	^nil!
primMillisecondClock
	"Primitive. Answer the number of milliseconds since the millisecond clock
	was last reset or rolled over. No sync to any system clock.
	Implemented by all major platforms.
	Essential. See Object documentation whatIsAPrimitive. 
	
	Time primMillisecondClock
	Time primMillisecondClock / 1000 / 60.0

	Range is  from zero to 16r1FFFFFFF.
	The VM defines MillisecondClockMask as 16r1FFFFFFF

	Overflows usually every six days.
	"
"Not really a clock, but a timer or ticker"

	<primitive: 135>
	self primitiveFailed!
primUtcMicrosecondClock
	"Answer the number of microseconds since the UTC Smalltalk epoch, 
	i.e. 00:00 on the morning of January 1, 1901, the start of the 20th century, in UTC time.
	 The value is derived from the Posix epoch with a constant offset corresponding to elapsed microseconds
	 between the two epochs according to RFC 868.
	Answer might be a LargePositiveInteger (very likely on 32-bit images, very unlikely on 64-bit images).
	Cog VMs implement this. Interpreters might not."
	"
	Time primUtcMicrosecondClock
	Time primUtcMicrosecondClock/ 1000 / 1000 / 60 / 60 / 24 / 365.25

	(Time primUtcMicrosecondClock / 1000 / 1000 + Time primUtcWithOffset second) / 60 / 60 / 24 / 365.25
	(DateAndTime now - (DateAndTime year: 1901 month: 1 day: 1)) totalSeconds / 60 / 60 / 24 / 365.25
	"
	<primitive: 240>
	^nil!
primUtcWithOffset
	"Answer a two element array.
	Prefer #primUtcWithOffset:

	First element is the number of microseconds since the UTC Posix (Unix) epoch, 
	i.e. 00:00 on the morning of January 1, 1970, in UTC time.
	It might be a LargePositiveInteger (very likely on 32-bit images, very unlikely on 64-bit images).
	
	Second element is the current seconds offset from GMT in the local time zone.

	Recent VMs implement this. Older Interpreters might not.
	"
	"
	Time primUtcWithOffset
	Time primUtcWithOffset first / 1000 / 1000 / 60 / 60 / 24 / 365.25
	Time primUtcWithOffset second / 60 / 60.0
		
	(Time primUtcWithOffset first / 1000 / 1000 + Time primUtcWithOffset second) / 60 / 60 / 24 / 365.25
	(DateAndTime now - (DateAndTime year: 1970 month: 1 day: 1)) totalSeconds / 60 / 60 / 24 / 365.25
	"
	<primitive: 'primitiveUtcWithOffset'>
	^nil!
primUtcWithOffset: arrayOrObjectWithTwoSlots
	"The parameter may be a two element array, or an object whose first two instance
	variables are expected to be UTC microseconds and seconds offset from GMT.

	First element is set to the number of microseconds since the UTC Posix (Unix) epoch, 
	i.e. 00:00 on the morning of January 1, 1970, in UTC time.
	It might be a LargePositiveInteger (very likely on 32-bit images, very unlikely on 64-bit images).
	
	Second element is set to current seconds offset from GMT in the local time zone.

	Recent VMs implement this. Older Interpreters might not.
	Time primUtcWithOffset: {0. 0} 
	"

	<primitive: 'primitiveUtcWithOffset'>
	^nil!
!Time class methodsFor: 'private'!
currentUtcOffset
	"
	Time currentUtcOffset
	"
	^ DateAndTime now offset!
!Time class methodsFor: 'constants'!
microsecondsInDay
	^86400000000!
millisecondsInDay
	^86400000!
nanosInSecond
	^ 1000000000!
nanosecondsInDay
	^86400000000000!
secondsInDay
	^86400!
secondsInHour
	^3600!
secondsInMinute
	^60!
squeakEpoch
	^ 2415386. 		"Julian day number of 1 Jan 1901"!
!Time class methodsFor: 'cached state access'!
releaseClassCachedState

	MillisecondClockOffset := nil!
