
#pragma once

#ifndef Reminder

// Statements like:
//		#pragma message(Reminder "Fix this problem!")
// Which will cause messages like:
//		C:\Source\Project\main.cpp(47): Reminder: Fix this problem!
// to show up during compiles.  Note that you can NOT use the
// words "error" or "warning" in your reminders, since it will
// make the IDE think it should abort execution.  You can double
// click on these messages and jump to the line in question.
#define Stringize( L )			#L
#define MakeString( M, L )		M(L)
#define Line____\
	MakeString( Stringize, __LINE__ )
#define Reminder				\
	__FILE__ "(" Line____ ") : Reminder: "

#endif //Reminder
