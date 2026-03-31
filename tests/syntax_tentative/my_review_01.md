# 1.2 

Something wrong : you say functions defined in libraries : it can be module or libraries. And this is optional (can be loaded to namespace as well as in global space). 

# 3.1

You introduce a new syntax for lambdas, with the operator `~~`. Switch back to `|>` or `~>`. 

Then, in examples, you write : "With emitted states" and "Counter with multiple emissions", though the corresponding functions do not emit anything. Emit is described later, let's remove it from here. 


# 3.2 and 3.3 

Inside the lambdas, you introduce the concept of init bloc, and update bloc. 
I think an option init bloc is very useful, so great. 
Though, do we really need an update bloc ? The lambda is already an update bloc itself, so everything outside the init bloc is actually update. 
So for now we remove the update bloc.

# 3.4 

We should find a way to avoid repeating the `emit` both at init & update sections. 

# 3.5 

There is no 3.5 in your document. Though, I think 3.5 should be used to describe the memory operators we take from Faust : `@` and `'`. 
Could be written as `@(some_function)` to delay one sample. 
And `@(some_function, 8)` for 8 sample delay. 

# 4 

Change the flow definition to make it clear it is a type of function as well. 

We should also add an integer representation (with the `int` primitive function) that would convert internally a number to 64bit integer (or 32) to unlock bitwise operations.  

# 5.3 

We need to define this more accurately. The automatic inheritance of `dt` from parent is not clear yet : who is parent ? Callee or caller ? Make it explicit. 

# 6.3 

Your `selector` proposition is useless, since it does exactly the same as my proposition for ternary operator. We need a clean ternary, with multiple case handling, exactly as I proposed : 
`opt1, opt2, opt3 (...) ? condition`

There is also one of your optiosn (`choose note` ) which is not clear. Maybe remove it ? Or make it clear 

# 6.4 

The emit keyword was described before in the document. Is it necessary to repeat its definition ? If so, explain what this chapter brings that wasn't already documented. 

# 6.6 

I think you didn't understand my case for return statement from lambdas. We should drop this for now, and add this later if needed. For now it's useless. 

# 7.2 

Remove selector, we only keep ternary 

# 7.3 

This is good. Maybe flow documentation should come before this (right now it's weird, because you're talking about flows, but the documentation for it is below in the document). 
Also, how would you generate a flow with several members (rhythm and notes for example) ? Give more examples. The flow is a core concept of Idyl, so you can be creative for now. We will stabilize what needs to be later.  


# 8.1

There are some problems with the flow definitions. 
The `basic_pattern` is ok. 
But you must understand that by default, flow is wrapping. 
So the `verse` definition with `kick: [!] |4|` is kind of useless. It is equivalent to `kick: [!]`, since it will wrap anyway. 
About the core syntax of flows : 
- I think the entire definition of flow must be between square brackets 
- Repetition signs `|3|` are also placed inside the brackets 
- Not sure about the coma separators inside flows, makes it longer to write (think about livecoding situations)
- For example `rhythm: [! . ! ! |4| . . ! ! |2|]`
- And `notes: [ 60 64 |2| | 65 67 |4| ]` where the standalone `|` just after first repeat sign is explicitely saying that the second repeat goes back to here (not the beginning).


In general, any new feature to make flows a deeper concept is welcome : algorithmic combination, procedural generation, etc. 

For later, I also think a trait system (inspired from Rust) would be great, to allow flows to share stuff (functions, operators overload etc). You can create a subchapter about this with suggestions. 

# 9 

I think we need to think more clearly module and library system : 
- I don't want to allow to much of an OOP style 
- So maybe the modules just exposes functions (in or out of namespace) that can return values, functions, or C objects (in the case of Csound instance for example) 
- No more returning of object that has methods (to prevent long lines with several method access)

Suggestions about the syntax, and core design are welcome here. 

# 10 

Excellent for process blocs. 
Particularly for case with several process blocs, with optional duration (in case it needs to automatically schedule itself), but optional (if scheduled from outside for example). 

Cool to have `init` bloc (optional). 
Though, remove `cleanup`. Users won't handle this. 
Also, the `output` is not clear. Idyl won't be an audio engine. Just a general scheduler, talking to engines through API's (modules) and OSC (or anything else). There won't be an output. 

# 11 

Good for state persistence and voice management. 
Might be cool to add more examples, and find more complex cases of polyphonic voice management (or in general, management of several instances of something). 
Find the cases where it could be problematic, and find something to answer these cases. 





