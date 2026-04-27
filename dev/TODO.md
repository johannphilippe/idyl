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

- [fixed] functional flow seems broken (see tests/temporal/functional_flow.idyl) — was treating `func2() = expr` as a constant; fixed with `has_parens_` flag
- [fixed] emit trigger behavior is weird : tests/temporal/catch_emit.idyl — bare `stop` in process catch handler fixed with `proc_stop_ctx_` mechanism
- [fixed] in tests/temporal/clock.idyl : tempo change ratio mismatch — scheduler now re-reads `dt_ms` after the tick callback so `update_dt()` from within the callback takes effect on the same reschedule
- [fixed] in tests/temporal/local_function_update.idyl : prints 500 twice — closures defined in update blocks no longer push the stale `inst->current_` scope when called from within the same tick; `currently_ticking_inst_` tracks the active instance so env_ values (freshly updated in the tick) are used instead
- [fixed] tests/temporal/proc_scheduling.idyl : bare `stop` in process catch handler now deactivates the owning instance via `proc_stop_ctx_`


- [done] Ternary operator syntax change 
```idyl
m = metro(dt=500ms)
(m) ? expr1 // I don't know if this already works

(m) ? expr1 ; expr2 

c = counter(dt=300ms)
(c) ? expr1; expr2; expr3 .... 
```
- [idea] Add a concept of independant block of code : could be used as a scope, or even in ternary conditions 
```idyl
(trig) ? {/* block 1 with several statements */}; {/* block 2 */} // etc 
```

- [done] Change catch syntax so it is more consistent with the rest of emit design `catch a::sig: {/* do something */}` 

- [bug] hot-reload of running process creates a small glitch. Sometimes, hot reload of non running process still triggers some calculation (thus, can send a note in csound_module.idyl for example)
- [feature] the `'` delay operator should be able to operate with time values as well (like `'(val, 5ms)`)
- [bug] the start and stop process shortcut works on neovim, not in vim
- [bug] when calling a flow like this `fl = melody(i)[trig]` : it is weird : 

- [fixed] trigger functions : result update on first trigger, not on second ... 
- [fixed] function with trigger parameters work fine in update bloc, but function body is updated even if trig is rest
- [fixed] hot reload works on flows, not on processes
- [in_progress] hot reload - to define properly before implementation
- [important] : serial utilities (read write) + 2 modules (serial, & MIDI)
- [later_fix] Clocks : csound_module_clock.idyl : change only occurs after 12 beats instead of 10 : investigate
- [implemented] Flows with temporal elements should update (they don't) : e.g. `flow temporal = [sine(5hz, dt=200ms), square(5hz, dt=100ms)]`
- [unnecessary] flows inlined inside functions body or assignment - seems not fully working. Also, ambiguous. Add it later ? 
- [fixed] allow .2 alias for 0.2
- [fixed] automatic nice cleanup of handles (Csound, OSC) when program catches Ctrl+c on CLI  :> modules_cleanup.md
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

## Vim Plugin 

Vim plugin insde editors/vim 
The plugin is intended for live-coding contextx. 
It is used to send code to idyl through OSC (port 9000, local IP address) using /idyl/eval address
The shortcuts to trigger evaluation are : 
- `t` in normal mode 
- Ctrl+e in edit/insert mode 

When cursor is placed in a function body, it sends the function for evaluation
Same for flow 
When cursor is placed on a process, same  