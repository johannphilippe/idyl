# ⟡ Idƴl 

A temporal language for describing time, flow, and reactive behavior. \
A language for moments, intervals, and motion. \
A functional language. 

Idƴl describes time, Xrune executes it. 

> It was never about shaping time. Some misaligned their intentions. \
> Time is not running. Though one may run without it. \
> No rush, no run is necessary. \
> Idƴl is the movement between two moments. 

## Poetics 

Idƴl is an exploration between programming language & realtime scheduling.
The interface is a functional language. 
The scheduler is efficient and non-deterministic.

The audio engine Idƴl collaborates with is its brother X̸rune.
Meanwhile, Idƴl can be a creative place for any digital artist willing to free time. 

## Semantics 

Functional language.

A simple function defined as 
``` idyl
add(a, b) = a + b
```

An temporal function defined as 
```
metro(fq, dt = 10ms) = trig ~> {phase = phasor(fq); trig = phase < phase'; phase!; }
```

Where dt argument is necessarily defined. 
`~>` operator defines an evolution bloc
`phase!` explicitely emits phase to another function it would be connected to

Better implementation would be 
```
metro(fq, dt = fq) = 1 
```

```
phasor(fq, dt = 5ms) = phase ~> {
    incr = dt<ms> / fq<ms>; // <ms> converts temporal value value 
    phase = fmod( (_ + incr)~, 1.0); 
}
```

Functions can be considered as models.
Time functions as realtime models. 

They are not yet "playing" as long as they are not instanciated or used by an instanciated function. 


Function callbacks are actually lambdas where : 
- $1 $2... are the parallel outputs 
- phase is the emitted phase by oscillator

```
oscillate(3hz, 1) : ($1 /* output 1 */, phase) -> {}
```


Modules are external audio (or else) components that will be called with 
```
// Import module from standard modules 
cs = module("csound")
osc = module("osc")

osc.init("127.0.0.1", 8000)
osc.send("/some/where/over/the/rainbow", val)
```
Idyl will provide an API to describe external modules. 
It will also provide by default a few modules : 
- Xrune audio graphs  
- Csound 
- OSC 

Modules will be more OOP : they will provide a set of methods.

## TODO 

### Add module system (like Csound module first) and introduce module object usage  
e.g. 
```
// Import module from standard modules 
use csound as cs
// Or import a custom module ? 
import mycustommodule as mcm 


metro(fq, dt = fq) = 1 ~> {interval = 1/fq; interval!}

// Then 
f = metro(3hz) : (interval) -> {inst = cs.ping(0, interval); oscillate(3hz, 1, dt = 10ms) : ($1, phase) -> { inst.freq = abs($1) * 100 + 50 } }
```

### Implement `flow` first class type 

flow is the equivalent of data structures (arrays like) 

```
flow myflow(f) = {
    rhythm = | f(x) . . f(x) |
}

flow arp(scale, offset) = {
    note = | scale[0] + offset
             scale[2] + offset
             scale[4] + offset
             . |
}

flow combine(a, b) = {
    trig = a.trig + b.trig
    note = a.note ?? b.note
}

// With repeat bars 
flow patternize(f) = {
    trig = || f(0) . f(1) |2|  f(2) f(3) . . |3| ||
}
```

### Implement a trigger first class ? 

### Implement strings ? Not sure about that 