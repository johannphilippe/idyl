# This TODO is a list of operations for assistant to work on 

# Hot reload of process 

For live coding purposes (and some other cases), we should be able to hot reload code of a process. 
This must be done in a smart way : 
- It must not "reinit" the currently running stateful objects (keep running, with updated things)


# TODO 

- Serial read/write utility (in utilities)
- Serial read/write module 
- MIDI module 

## Next 

- [essential] hot reload - to define properly before implementation
- [important] automatic nice cleanup of handles (Csound, OSC) when program catches Ctrl+c on CLI  :> modules_cleanup.md
- [important] : serial utilities (read write) + 2 modules (serial, & MIDI)
- [fix] Clocks : csound_module_clock.idyl : change only occurs after 12 beats instead of 10 : investigate
- [implemented] Flows with temporal elements should update (they don't) : e.g. `flow temporal = [sine(5hz, dt=200ms), square(5hz, dt=100ms)]`
- [unnecessary] flows inlined inside functions body or assignment - seems not fully working. Also, ambiguous. Add it later ? 
- [fixed] (YET TO CHECK) clock are not well designed (kind of useless) & temporal conversions introduce issues (bpm are not treated as bpm by default etc)
- [fixed] (YET TO CHECK) basic metronome only sends triggers ! . A trigger, if not fired, should always be a rest (case of `metro(dt=100ms)=!`, if called or evaluated, return should be rest when not fired)
- [fixed] Doc ch 9 show erratic examples of functions defined inside process blocs
- [later] List of features in syntax.md (traits for example)
- [later] Audio base runtime option (either system_clock scheduler, either audio clock scheduler with low buffer size - 16 or 32 samples) (for buffer rate accuracy, with pure audio synchronization) (not urgent)
- [later] Closures : or better way to get full flexibility over function manipulation
- [later] traits : a way to implement new features to some flows 
- [unnecessary]Reintroduce function definition inside lambdas init scope   
- [unnecessary] on keyword : `on(trig): {// Do something}` where the bloc is only computed when the trigger happens : ternary are ok for this `_; do_something ? condition`

- [semantics] modify a few keywords so they match idyl philosophy : 
    - `process` : `cycle`, `verse`, `weave` or `tide` (last is favorite)
    - `dt` could alias  `drift` (though, it would explicitly say we're drifting, but it's fun)
    - `start` : `dawn`, `rise`, `wake`, `spark` (dawn)
    - `stop`: `ease`, `dim`, `wane` , 
    - `catch`: `meet`, `hear`, `heed` (favoritee hear)

for example, rise a tide, then wane it. inside the tide, everything can drift.
And we can hear some signals.

## Flows 

Currently, flows seem static
The language allows flows to take parameters though : 
`flow mypattern(i) = [30+i 32-i 34*i 36]`

Though, it doesn't work at runtime. 

This change needs to make sure that : 
- flows using parameters are resolved, statically & dynamically (temporally) with their parameters

For now, the syntax (grammar) is flexible enough to allow everything we need 

```idyl```
lib("stdlib")
flow pattern(i) = [40+i 50+i 60+i]

flow complex_pattern(i) = {
    notes = [40+i 50+i 60+i]
}

flow incr = [0 12 -12]

// Calling with static i parameter
process static: {
    m = metro(dt=1s)
    p = pattern(12) 
    print(p[m]) // prints 52, 62, 72

    print(pattern(1)[m]) // prints 41 51 61 

    pat = complex_pattern(5)
    print(pat[m].notes) // prints 45 55 65 
    print(pat.notes[m]) // same
}

process dynamic: {
    m1 = metro(dt=3s)
    m2 = metro(dt=300ms)

    i = incr[m1]

    print(pattern(i)[m2]) // when i changes, the flow members must be recomputed 

    // Same for the other syntaxes 

}

```


It involves to treat flows that have parameters differently than others : 
- If any of the parameters is dynamic : recompute when the smallest parameters dt ticks 
- If all static : compute once 