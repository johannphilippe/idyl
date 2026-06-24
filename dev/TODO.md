# This TODO is a list of operations for assistant to work on 

# Hot reload of process 

For live coding purposes (and some other cases), we should be able to hot reload code of a process. 
This must be done in a smart way : 
- It must not "reinit" the currently running stateful objects (keep running, with updated things)

# TODO 

- Serial read/write utility (in utilities)
- Serial read/write module 
- MIDI module 

## Language design 

- Utility of catch expression (regarding the fact that we have "on" and "::" accessor)
- Possibility of flow arythmetics `myflow * 2`: 
    - could create a new flow where all values are multiplied by 2 (how to handle trigger ! ? )
    - Could mean something else : defined by traits for example 


## Known bugs (unsolved)

- [ascii_tool] : few stuff 
    * remove the printings 
    * Could have some specific comment triggers 
    * mirror mode is not good ! 
    * CLI option (optional, and driven by comments parsing like "vid something.mp4") to read video files (glitchy way, ASCII art degrading, and with glitchy loops etc) in the background  (not the text). This can use any linux (preferably cross platform) API to do so. 
    * CLI option to display the full code (artistically too, subject to glitches or so) in the background.
- [issue_and_question] if metro, the new durations are updated (speed), if sync it keeps at same speed
	tempo(60)
	speed = whisper.duration[0]
	m = metro(speed)

	w = whisper[m]
	on(m): {
		print("speed", speed, age)
		speed = w.duration
	}	

    However, I think we should provide a primitive that takes a flow as input and can return durations after they are elapsed 
    And another one doing the same but returning trigger 
    This pritives should be able to synchronize to clocks 
    

    Also, random temporal is not working with sync, but works with metro 

	speed = beat(rndt(1, 12, dt=3b))
	m = sync(speed)


- [issue] reset_mat process makes all process stop. It does not though if it just prints (instead of sending midi notes) 

- [potential_issue] : rcount does not reset when stopping / restarting a process (index is kept) 
- [fixed] in geneeral : issue with  trigger indexing for flows 

- [question] let's say I add a simple instruction (not temporal) to a process then hot reload : should it be triggered ? 

- [done] query on specific beat duration of main clock (or missing in documentation)

- [issue] delay with triggers ? Not sure it works correctly 
- [issue] Another example of hot reloading causing automatic stop  of process, even if there were no changes. 
Also, the duree (rndt from stdlib) causes the instrument to fire, which should not happen since the duration parameter is not a trigger
grainsharp(spike!, freq, duration=0.3s, amp=f, ch=1) =
    cs_note(cs, "grainsharp", duration, amp, freq, ch)
                                      
// metalsharp — metallic, inharmonic FM ring
metalsharp(spike!, freq, duration=0.5s, amp=f, ch=1) =
    cs_note(cs, "metalsharp", duration, amp, freq, ch)
        

// Bon del de jouer avec la vitesse, durée, et random du multiplicateur
process deltest: {                     
        m = metro(.5b)                 
        fl = superchord[rcounter(m, _) ]
        v = fl.first * rnd(8, 64)      
        print(v)                       
        duree = rndt(500, 3000, dt=.25b)                                                                                                                                                      
                                       
        //deepkick(euclid(3, 8, phase_in=phasor(4b)), amp=fff)
        //metalsharp(m, v, amp=rnd(mf, fff), ch=1, duration=duree)
        //metalsharp('(m, 100ms), '(v, 2), amp=rnd(mf, fff), ch=2, duration=duree)
        grainsharp(m, '(v, 200ms), amp=rnd(mf, fff), ch=1, duration=duree)
        grainsharp('(m, 100ms), '(v, 1), amp=rnd(mf, fff), ch=2, duration=duree)
                                       
}     

- [issue] : in some contexts of hot reload (live), changing a function call name or commenting it reloading actually stops the entire process . In this example, it is the two vapordream ...  
process test: { 
        tempo(120bpm)
        //lushpad(!, 300, amp = fff)
        a = sync(4b)
        b = sync(2b)
        c = sync(8b)
        //purecut(a, mtof(tune(c5, minor, 8, just)), amp=fff, ch=2)
        //purecut(b, mtof(tune(c7, minor, 2, just)), amp=fff, ch=1)
        print("mf: ", mf)
                
        index = nodtrrcounter(sync(8b), _)
        fl = superchord[index]
        ph = phasor(2b)
        //vapordream(euclid(3, 8, phase_in=ph), mtof(tune(a3, dorian, fl.second, just)), amp=fff, duration=.5s)
        //vapordream(euclid(3, 8, phase_in=ph), mtof(tune(a3, dorian, fl.third, just)), amp=fff, duration=.5s)
        print(fl.first, fl.second, fl.third) 
        //lushpad(c, mtof(tune(f2, dorian, fl.first, just)), amp = fff, ch=rndi(1, NCH))
        //lushpad(c, mtof(tune(f5, dorian, fl.second, just)), amp = f, ch = rndi(1, NCH))
        //lushpad(c, mtof(tune(f6, dorian, fl.third, just)), amp = ppp, ch = rndi(1, NCH))
        variation = pos(sine(0.1hz, dt=50ms))
        grainsharp(dust(variation, dt=0.01s), rnd(20, 800), amp=rnd(mf, fff), ch=rndi(1,2))  
        metalsharp(dust(variation, dt=0.01s), rnd(20, 800), amp=rnd(mf, fff), ch=rndi(1,2))  
        purecut(dust(variation, dt=0.01s), rnd(20, 5000), amp=rnd(mf, fff), ch=rndi(1, 2))  
        grainsharp(metro(1b), 500, amp=f, ch=rndi(1, 2))
}        
- [issue] hot reload can cause flow indexing to restart : 
flow superchord = {
        first: [1, 3, 7]                                          
        second: [2, 6, 4, 1]                                      
        third: [3, 5, 7, 2, 8]                                    
}       
        
        
process test: {
        tempo(120bpm)
        //lushpad(!, 300, amp = fff)
        a = sync(4b)
        b = sync(2b)
        c = sync(8b)
        //purecut(a, mtof(tune(c5, minor, 8, just)), amp=fff, ch=2)
        //purecut(b, mtof(tune(c7, minor, 2, just)), amp=fff, ch=1)
        print("mf: ", mf)
        
        fl = superchord[c]                                                                                                                                                                    
        ph = phasor(2b)
        vapordream(euclid(3, 8, phase_in=ph), mtof(tune(a3, dorian, fl.second, just)), amp=fff, duration=1s)
        vapordream(euclid(3, 8, phase_in=ph), mtof(tune(a3, dorian, fl.third, just)), amp=fff, duration=1s)
        print(fl.first, fl.second, fl.third) 
        //lushpad(c, mtof(tune(f2, dorian, fl.first, just)), amp = fff, ch=rndi(1, 2))
        //lushpad(c, mtof(tune(f5, dorian, fl.second, just)), amp = f, ch = rndi(1, 2))
        //lushpad(c, mtof(tune(f6, dorian, fl.third, just)), amp = ppp, ch = rndi(1, 2))
        variation = pos(sine(0.1hz), dt=50ms)
        grainsharp(dust(variation, dt=0.01s), rnd(20, 800), amp=rnd(mf, fff), ch=rndi(1,2))  
        metalsharp(dust(variation, dt=0.01s), rnd(20, 800), amp=rnd(mf, fff), ch=rndi(1,2))  
        purecut(dust(variation, dt=0.01s), rnd(20, 5000), amp=rnd(mf, fff), ch=rndi(1, 2))  
        grainsharp(metro(1b), 500, amp=f, ch=rndi(1, 2))
}       

The fl index is lost. Seems to only do this with trigger indexing. 

- [fixed] when "starting" a process that is synced, it might create an extra note still (that is not synced)

- [issue] hot reload : starting an already running process causes duplicates 
- [issue] changing any global clock tempo value doesn't work at runtime
- [issue] three different processes using same clock : starting or stopping a process affects others behavior (speed) : I suspect variables are shared ... 

- [todo] make an audit of every situation where dt reaction wouldn't cause update (like reactions of reactions)
- [fixed] dynamic flows do not seem to work properly  : `flow dynamicmel(deg) = [ tune(c2, minor, deg, just), tune(d3, major, deg, just) ]` : the indexing is ok, the argument is taken, 
but tune is not recomputed on each tick of deg
- [question]  ternary operator simple expression does not work so well when not assigned to an output (result)
Also, consecutive ternary without assignment don't parse. 
`(condition) ? fire_something(blabla) ; fire_something_else(blublu) // this doesn't work well`
`res = (condition) ? fire_something(blabla) ; fire_something_else(blublu) // this works well`

`
// Consecutive : doesn't parse 
(condition) ? fire_something(blabla) ; fire_something_else(blublu) 
(condition) ? fire_something(blabla) ; fire_something_else(blublu) 

`

- [fixed] hot reload not changing the values I manually changed (in euclidian rhythm)
- [fixed] hot reloading restarting flow indexing 
- [fixed] stop primitive does not seem to kill process state
- [done] delay operator ' currently delays ticks (numbers). It should be able to delay ms, b etc (all temporal primitives). 
- [bug] : pause/resume in system clock scheduler (each arpeggio inside an on block) causes duplicates of a few events. Working fine in audio clock.
- [for_later] tests/temporal/trigger_and_stop.idyl : several consecutive ternary do not parse . Also, behavior of trigger as conditions is unclear.
- [done] : loops in flows (generators) and loops in `each` do no share syntax (`each n in 1..4` against `n = 1..4`) 
- [crash] Some issues make the vm crash : this should be avoided ! 

- [implemented] repeat bars (from syntax.md) do not seem implemented (or badly, probably according to ancient design when there were no comma separators). Needs to be implemented syntactically before checking engine behavior. 
- [question_or_issue] hot reloading a repeating temporal loop (nested in a on bloc for example) will only occur when iterator repeats, is that correct behavior ? I understand it is logic on the engine side. How can we think this. 
- [addresses_at_pause_resume] stop and start from OSC (probably from idyl too) restarts counter of a temporal each loop (arpeggio for example). It it what we decided ?  Maybe there should also be a "pause" primitive and OSC action to just suspend process. 

- [seems_solved] **Double note on process restart** (`csound_module.idyl`): when a running process is stopped and restarted via OSC `start cr`, two Csound notes fire nearly simultaneously. Only audible after ~1s of Csound warmup. Does not reproduce in `csound_simple.idyl` (likely masked by the dense all-trigger rhythm). Attempted fixes: (1) `std::recursive_mutex eval_mutex_` to serialise scheduler callbacks vs main thread — notes are now sequential rather than concurrent, but both still fire; (2) `speculative_exec_ = true` during restart setup to suppress `cs_note` calls — did not resolve. Root cause not fully identified: the old process's last scheduler tick fires a note, and the setup of the new process also fires a note; serialising them makes the pair audible rather than fixing it. Further investigation needed.

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
- [done] `flow { }` as a literal expression: `name = flow { member: [...] }` — the `flow` keyword is now a type tag on a brace block, required only for named-member forms to distinguish them from future code blocks `{ }`. Simple list `[...]` was always a literal. Functions and temporal functions can return both forms. Old `flow name = { }` definition syntax is backward-compatible sugar.
- [idea] Add a concept of independant block of code : could be used as a scope, or even in ternary conditions 
```idyl
(trig) ? {/* block 1 with several statements */}; {/* block 2 */} // etc 
```

- [done] Change catch syntax so it is more consistent with the rest of emit design `catch a::sig: {/* do something */}` 

- [fixed] hot-reload of running process creates a small glitch. Sometimes, hot reload of non running process still triggers some calculation (thus, can send a note in csound_module.idyl for example)
- [feature] the `'` delay operator should be able to operate with time values as well (like `'(val, 5ms)`)
- [fixed] the start and stop process shortcut works on neovim, not in vim — root cause: old `plugin/idyl_live.vim` and `ftplugin/idyl_live.vim` from a previous plugin version were still installed; they registered a FileType autocmd that remapped `t` after `after/ftplugin` ran and never mapped `s`/`q`. Fixed by deleting legacy files and adding legacy cleanup to install.sh. Also removed `abort` from `s:ApplyIdylMappings()` and moved `s`/`q` before `<C-e>` so a failing inoremap can't block them.

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
    - `on` : 

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

```idyl
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