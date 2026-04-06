# This TODO is a list of operations for assistant to work on 

## Process block rules 

When there is one process block (only one), the rule is the following : we are in a temporal process. The program should not exit (process might be filled with user later, livecoding for example)

A process can have : a name, a duration 

When several processes are in a program, they must have either : 
- A duration : then, processes will play one after another, according to their durations 
- A name : Then process won't play, until they are fired from external command (livecoding or UDP trigger)

## Catch 

Several catches should be implemented. 
Catch system will be used to catch special events (end, trigger, anything)
They will also be able to capture emitted values (from emit statement)

### end 

For temporal processes, there might be an end. 
We need to introduce an end primitive that would be emitted from functions, modules, or builtins. 
For example, Csound module will provide a way to instanciate a voice, and catch its end. 

### error 

Maybe the name is not good. Though, we might want to catch null pointers or things that could cause memory corruption or violation.

### trigger 

Of course, when a trigger is fired, it could be captured (ideal for creating nice control structures)


## Maybe a real integer primitive

For now, int builtin function only casts the value to int, before casting it to double. 
We might want to have a flag, to know when it has to be treated as int (useful for bitwise operations etc)
The rule is simple : every number is always float, except if explicitely written as int with int builtin. 

