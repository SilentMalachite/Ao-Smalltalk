!Timespan subclass: #Date
  instanceVariableNames: ''
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Kernel-Chronology'!
!Date methodsFor: 'printing'!
mmddyyyy
	"Answer the receiver rendered in standard fmt mm/dd/yyyy.  Good for avoiding year 2000 bugs.  Note that the name here is slightly misleading -- the month and day numbers don't show leading zeros, so that for example feb 1 1996 is 2/1/96"

	"Date today mmddyyyy"

	^ self printFormat: #(2 1 3 $/ 1 1)!
printFormat: formatArray 
	"Answer a String describing the receiver using the format denoted by the 
	argument, formatArray."

	^ String streamContents: [ :strm |
		self printOn: strm format: formatArray].!
printOn: aStream

	self printOn: aStream format: #(1 2 3 $  3 1 )!
printOn: aStream format: formatArray 
	"Print a description of the receiver on aStream using the format 
	denoted the argument, formatArray: 
	
		#(item item item sep monthfmt yearfmt twoDigits) 
	
		items: 1=day 2=month 3=year will appear in the order given, 
	
		separated by sep which is eaither an ascii code or character. 
	
		monthFmt: 1=09 2=Sep 3=September 
	
		yearFmt: 1=1996 2=96 
	
		digits: (missing or)1=9 2=09. 
	
	See the examples in #printOn: and #mmddyyyy"
	| gregorian twoDigits element monthFormat |
	gregorian := self dayMonthYearDo: [ :d :m :y | {d. m. y} ].
	twoDigits := formatArray size > 6 and: [(formatArray at: 7) > 1].
	1
		to: 3
		do: [ :i |
			element := formatArray at: i.
			"Day"
			element = 1 ifTrue: [
				twoDigits
					ifTrue: [ aStream nextPutAll: (gregorian first printString padded: #left to: 2 with: $0) ]
					ifFalse: [ gregorian first printOn: aStream ]].
			"Month"
			element = 2 ifTrue: [
				monthFormat := formatArray at: 5.
				monthFormat = 1 ifTrue: [
					twoDigits
						ifTrue: [ aStream nextPutAll: (gregorian second printString padded: #left to: 2 with: $0) ]
						ifFalse: [ gregorian second printOn: aStream ]].
				monthFormat = 2 ifTrue: [
					aStream nextPutAll: ((Month nameOfMonth: gregorian second) copyFrom: 1 to: 3) ].
				monthFormat = 3 ifTrue: [ aStream nextPutAll: (Month nameOfMonth: gregorian second) ]].
			"Year"
			element = 3 ifTrue: [
				(formatArray at: 6) = 1
					ifTrue: [
						gregorian third >= 0 "Print all positive years with at minimum four difits"
							ifTrue: [ aStream nextPutAll: (gregorian third printString padded: #left to: 4 with: $0) ]
							ifFalse: [ gregorian third printOn: aStream ]]
					ifFalse: [
						aStream nextPutAll: ((gregorian third \\ 100) printString padded: #left to: 2 with: $0) ]].
			"Separator"
			i < 3 ifTrue: [
				(formatArray at: 4) ~= 0 ifTrue: [
					aStream nextPut:
						((formatArray at: 4) isNumber ifTrue: [ Character codePoint: (formatArray at: 4) ] ifFalse: [ formatArray at: 4 ]) ]]].!
storeOn: aStream

	aStream print: self printString; nextPutAll: ' asDate'!
yyyymmdd
	"Format the date in ISO 8601 standard like '2002-10-22'."

	^ self printFormat: #(3 2 1 $- 1 1 2)!
!Date methodsFor: 'smalltalk-80'!
previous: dayName 
	"Answer the previous date whose weekday name is dayName."

	| n |
	n := 7 + self weekdayIndex - (self class dayOfWeek: dayName) \\ 7.
	n = 0 ifTrue: [ n := 7 ].
	^ self - n days!
weekday
	"Answer the name of the day of the week on which the receiver falls."

	^ self dayOfWeekName!
weekdayIndex
	"Sunday=1, ... , Saturday=7
	Monday=1, ... , Sunday=7
	'12 May 2017 ' asDate weekdayIndex = 5
	"

	^ self dayOfWeek!
!Date methodsFor: 'squeak protocol'!
< aDate

	self assert: aDate class == Date.
	^ self start < aDate start!
> aDate

	self assert: aDate class == Date.
	^ self start > aDate start!
dayMonthYearDo: aBlock 
	"Supply integers for day, month and year to aBlock and return the result"

	^ start dayMonthYearDo: aBlock!
!Date methodsFor: 'public protocol'!
dayOfMonth
	"Answer the day of the month represented by the receiver."

	^ start dayOfMonth!
dayOfWeek
	"Answer the day of the week represented by the receiver."

	^ start dayOfWeek!
dayOfWeekName
	"Answer the day of the week represented by the receiver."

	^ start dayOfWeekName!
dayOfYear
	"Answer the day of the year represented by the receiver."

	^ start dayOfYear!
daysInMonth


	^ start daysInMonth!
daysInYear
	"Answer the number of days in the month represented by the receiver."

	^ start daysInYear!
daysLeftInYear
	^ start daysLeftInYear!
firstDayOfMonth

	^ start firstDayOfMonth!
isLeapYear

	^ start isLeapYear!
julianDayNumber

	^ start julianDayNumber!
month

	^start month!
monthAbbreviation

	^ start monthAbbreviation!
monthIndex

	^ start monthIndex!
monthName

	^ start monthName!
secondsSinceSqueakEpoch
	"Answer the seconds since the Squeak epoch: 1 January 1901"

	^ start secondsSinceSqueakEpoch!
week

	^start week!
year

	^start year!
yearNumber

	^ start yearNumber!
!Date class methodsFor: 'smalltalk-80'!
dayOfWeek: dayName 

	^ Week indexOfDay: dayName!
daysInMonth: monthName forYear: yearInteger 

	^ Month daysInMonth: monthName forYear: yearInteger.!
daysInYear: yearInteger 

	^ Year daysInYear: yearInteger.!
firstWeekdayOfMonth: month year: year
	"Answer the weekday index of the first day in <month> in the <year>."

	^ (self newDay: 1 month: month year: year) weekdayIndex!
fromDays: dayCount 
	"Days since 1 January 1901"

	^ self julianDayNumber: dayCount + Time squeakEpoch!
fromSeconds: seconds
	"Answer an instance of me which is 'seconds' seconds after January 1, 1901."

	^ self fromDays: ((Duration seconds: seconds) days)!
indexOfMonth: aMonthName 

	^ Month indexOfMonth: aMonthName.!
leapYear: yearInteger 

	^ Year leapYear: yearInteger!
nameOfDay: dayIndex 

	^ Week nameOfDay: dayIndex!
nameOfMonth: anIndex 

	^ Month nameOfMonth: anIndex.!
newDay: day month: month year: year 

	^ self year: year month: month day: day!
newDay: dayCount year: yearInteger

	^ self year: yearInteger day: dayCount!
today
	"In local time, the date in the current time zone.
	This is, the date 'here'."

	^ DateAndTime now date!
!Date class methodsFor: 'squeak protocol'!
julianDayNumber: aJulianDayNumber

	^ (DateAndTime julianDayNumber: aJulianDayNumber) date!
tomorrow

	^ self today next!
year: year day: dayOfYear

	^ (DateAndTime year: year day: dayOfYear) date!
year: year month: month day: day

	^ (DateAndTime year: year month: month day: day) date!
yesterday

	^ self today previous!
!Date class methodsFor: 'general inquiries'!
dateAndTimeNow
	"Answer an array with first element Date today and second element Time now."

	^Time dateAndTimeNow!
easterDateFor: year

 "  compute the easter date.
    source: Physikalisch-Technische Bundesanstalt Braunschweig.
    Lichtenberg, H.: Zur Interpretation der Gaussschen Osterformel
                     und ihrer Ausnahmeregeln,
                     Historia Mathematica 24 (1997), pp. 441-444
  
    http://www.ptb.de/de/org/4/44/441/oste.htm
  "

  | k m s a d r og sz oe day |

  k := year // 100.
  m := 15 + (3*k + 3//4) - (8*k + 13//25).
   s := 2 - (3*k + 3// 4).
  a := year \\ 19.
  d := 19*a + m \\ 30.
  r := d//29 + ((d//28) - (d//29)* (a// 11)).

  og := 21 + d - r.
  sz := 7 - (year//4 + year + s\\7).
  oe := 7 - (og - sz\\7).
  day := og + oe.
  ^day <= 31
    ifTrue: [Date newDay: day month: 3 year: year ]
    ifFalse: [Date newDay: day - 31 month: 4 year: year].!
orthodoxEasterDateFor: year

 "  compute the easter date according to the rules of the orthodox calendar.
    source: 
    http://www.smart.net/~mmontes/ortheast.html 
  "
     | r1 r2 r3 r4 ra rb r5 rc date |

    r1 := year \\ 19.
    r2 := year \\ 4.
    r3 := year \\ 7.
    ra := 19*r1 + 16.
    r4 := ra \\ 30.
    rb := r2 + r2 + (4*r3) + (6*r4).
    r5 := rb \\ 7.
    rc := r4 + r5.
    date := Date newDay: 3 month: 4 year: year.
    ^date + rc days!
!Date class methodsFor: 'inquiries'!
classDefinesDuration

	^true!
mustEncompassWholeDates

	^true!
!Date class methodsFor: 'instance creation'!
includingDateAndTime: aDateAndTime

	^self basicNew
 		start: aDateAndTime midnight;
		duration: (Duration days: 1);
		yourself!
readFrom: aStream
	"Read a Date from the stream in any of the forms:
		<year>-<month>-<day>			(1982-04-15) (ISO8601)
		<month>-<day>-<year>			(04-15-1982) (Only if third argument has 3 or more characters)
		<monthName> <day> <year>		(April 15, 1982)
		<year> <monthName> <day>		(1982 April 15)
		<day> <monthName> <year>		(15 April 1982) (Only if third argument has 3 or more characters)"

	^self readFrom: aStream allow90sStyleStamps: false!
readWithNamedMonthFrom: aStream firstNumber: firstAsNumberOrNil
	"See sender. Handle these forms:
		<monthName> <day> <year>		(April 15, 1982)
		<year> <monthName> <day>		(1982 April 15)
		<day> <monthName> <year>		(15 April 1982) (Only if third argument has 3 or more characters)
		<day> <monthName> <year>		(15 April 82) (Only if third argument > 31 => can't be a day number)
												Note: This format is only to support 20th century legacy stuff!!
												Don't use this format for dates in the 21st or 1st centuries."
	| day month year |

	"MonthName-DD-YY or DD-MonthName-YY or YY-MonthName-DD"
	month := String streamContents: [ :strm |
		[ aStream peek isLetter ] whileTrue: [ strm nextPut: aStream next ]].
	firstAsNumberOrNil
		ifNil: [
			"MonthName DD YY"
			[aStream peek isAlphaNumeric] whileFalse: [aStream skip].
			day := Integer readFrom: aStream.
			[aStream peek isAlphaNumeric] whileFalse: [aStream skip].
			year := Integer readFrom: aStream ]
		ifNotNil: [ | third thirdLength thirdStart |
			"DD-MonthName-YYYY, DD-MonthName-YY or YY-MonthName-DD"
			[aStream peek isAlphaNumeric] whileFalse: [aStream skip].
			thirdStart := aStream position.
			third := Integer readFrom: aStream.
			thirdLength := aStream position - thirdStart.
			thirdLength > 2
				ifTrue: [
					"DD MonthName YYYY"
					day := firstAsNumberOrNil.
					year := third ]
				ifFalse: [
					third > 31
						ifTrue: [
							"DD MonthName YY.  - Only included to support the latter years of the 20th century."
							day := firstAsNumberOrNil.
							year := 1900 + third ]
						ifFalse: [
							"YY-MonthName-DD"
							year := firstAsNumberOrNil.
							day := third ]]].

	^ self year: year month: month day: day!
readFrom: aStream allow90sStyleStamps: aBoolean
	"Read a Date from the stream in any of the forms:
		<year>-<month>-<day>			(1982-04-15) (ISO8601)
		<month>-<day>-<year>			(04-15-1982) (Only if third argument has 3 or more characters)
		<monthName> <day> <year>		(April 15, 1982)
		<year> <monthName> <day>		(1982 April 15)
		<day> <monthName> <year>		(15 April 1982) (Only if third argument has 3 or more characters)
		
		<month>-<day>-<year>			(04-15-82) (Only if third argument > 31 => can't be a day number)
												Only allowed if aBoolean is True.
												Note: This format is only to support 20th century legacy stuff!!
												Don't use this format for dates in the 21st or 1st centuries.
		<day> <monthName> <year>		(15 April 82) (Only if third argument > 31 => can't be a day number)
												Only allowed if aBoolean is True.
												Note: This format is only to support 20th century legacy stuff!!
												Don't use this format for dates in the 21st or 1st centuries."

	| firstAsNumber firstAsNumberSign second third thirdLength thirdStart |
	firstAsNumberSign := aStream peek == $- ifTrue: [-1] ifFalse: [1].

	[aStream peek isAlphaNumeric] whileFalse: [aStream skip].
	aStream peek isDigit ifTrue: [
		firstAsNumber := (Integer readFrom: aStream) * firstAsNumberSign ].

	[aStream peek isAlphaNumeric] whileFalse: [aStream skip].
	aStream peek isLetter
		ifTrue: [
			"MonthName-DD-YY or DD-MonthName-YY or YY-MonthName-DD"
			^self readWithNamedMonthFrom: aStream
				firstNumber: firstAsNumber allow90sStyleStamps: aBoolean ].
	
	"YY-MM-DD, YYYY-MM-DD, etc.  (ISO8601), MM-DD-YYYY"
	second := Integer readFrom: aStream.
	[aStream peek isAlphaNumeric] whileFalse: [aStream skip].
	thirdStart := aStream position.
	third := Integer readFrom: aStream.
	thirdLength := aStream position - thirdStart.

	thirdLength > 2 ifTrue: [
		"MM-DD-YYYY"
		^self year: third month: (Month nameOfMonth: firstAsNumber) day: second ].

	(aBoolean and: [third > 31]) ifTrue: [
		"MM-DD-YY - Only included to support the latter years of the 20th century."
		^self year: 1900 + third month: (Month nameOfMonth: firstAsNumber) day: second ].

	"YY-MM-DD, YYYY-MM-DD, etc.  (ISO8601)"
	^self year: firstAsNumber month: (Month nameOfMonth: second) day: third!
readWithNamedMonthFrom: aStream firstNumber: firstAsNumberOrNil allow90sStyleStamps: aBoolean
	"See sender. Handle these forms:
		<monthName> <day> <year>		(April 15, 1982)
		<year> <monthName> <day>		(1982 April 15)
		<day> <monthName> <year>		(15 April 1982) (Only if third argument has 3 or more characters)
		
		<day> <monthName> <year>		(15 April 82) (Only if third argument > 31 => can't be a day number)
												Only allowed if aBoolean is True.
												Note: This format is only to support 20th century legacy stuff!!
												Don't use this format for dates in the 21st or 1st centuries."
	| day month year |

	"MonthName-DD-YY or DD-MonthName-YY or YY-MonthName-DD"
	month := String streamContents: [ :strm |
		[ aStream peek isLetter ] whileTrue: [ strm nextPut: aStream next ]].
	firstAsNumberOrNil
		ifNil: [
			"MonthName DD YY"
			[aStream peek isAlphaNumeric] whileFalse: [aStream skip].
			day := Integer readFrom: aStream.
			[aStream peek isAlphaNumeric] whileFalse: [aStream skip].
			year := Integer readFrom: aStream ]
		ifNotNil: [ | third thirdLength thirdStart |
			"DD-MonthName-YYYY, DD-MonthName-YY or YY-MonthName-DD"
			[aStream peek isAlphaNumeric] whileFalse: [aStream skip].
			thirdStart := aStream position.
			third := Integer readFrom: aStream.
			thirdLength := aStream position - thirdStart.
			thirdLength > 2
				ifTrue: [
					"DD MonthName YYYY"
					day := firstAsNumberOrNil.
					year := third ]
				ifFalse: [
					(aBoolean and: [third > 31])
						ifTrue: [
							"DD MonthName YY.  - Only included to support the latter years of the 20th century."
							day := firstAsNumberOrNil.
							year := 1900 + third ]
						ifFalse: [
							"YY-MonthName-DD"
							year := firstAsNumberOrNil.
							day := third ]]].

	^ self year: year month: month day: day!
