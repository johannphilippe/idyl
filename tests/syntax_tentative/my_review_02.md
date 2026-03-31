# 3.1 

The following example shows a problem : 

``` idyl
// Counter
counter(trigger, dt=10ms) = count
|> {
    init: { count = 0 }
    count = count + (trigger ? 1 : 0)
}
```
There is a synchronization problem. 
counter dt is not synchronized to metronome (trigger) input. 
Maybe in such a case, there should be a system saying that the function is computed when trigger is on (since trigger is a primitive, it shouldn't be a problem). 

There is a semantic issue here. We need to find a way to express that this function is triggered. It is a special case of temporal function, or maybe even a special case of function at all. 
``` idyl
// Counter
// This would mean dt=? -> meaning expecting trigger ? 
// This is kind of ambiguous but still expressive
counter(dt=?) = count
|> {
    init: { count = 0 }
    count = count + (trigger ? 1 : 0)
}
```
Find a suggestion to solve this case 


# 3.4

Okay for `@(expression)` and `@(expression, N)` but not for `'(expression)`. This should be removed. 

# 6.3 

Ternary operator : If index is out of range, I think it would be cool if it wraps around rather than returning 0. 
Maybe there could be some optional catch to retrieve whether it was in range or not. But still, the output should always be valid (so wrap around like a modulo). 


# 7 Flows 
## 7.3 

The following example has a problem
```idyl
// Generative polyrhythm
flow polyrhythm(base_freq) = {
    rhythm_a: [!] |3|,
    rhythm_b: [!] |4|,
    frequency: [base_freq, base_freq * 1.5, base_freq * 2] |2|
}
```
How is `rhythm_a: [!] |3|` different from `rhythm_a: [!]` ? Since flows automatically wrap ? 
I think this example needs to be refactored, or explained better (if you really consider there is a difference, which I do not see now)

## new Flow features 

* Generate from bitwise operator 
```
flow bitflow(a, b)
{
    rhythm = to_trig(a & b) // meaning it will convert 1 to ! and 0 to .
}
```

* Flow interpolators 
- Linear 
- Cubic 
- ... 
(find a functional way to do it)

You are free to imagine new fun and algorithmic ways to generate and manupulate flows. 

# 9.2

the example `custom = import("my_lib.idl")` is wrong. 
The idl libraries are intended to be imported with `lib` keyword. If you think this is bad idea, say so, but we can't allow both. 

# 9.3 

We need to remove the OOP style. 
``` idyl
cs = import("csound")
orchestra = cs.compile("synth.orc")
instrument = cs.get(orchestra, "my_instr")
voice = cs.schedule(instrument, 0s, 5s) 
```
More functional, more clear. 

You are free to propose anything that would be more expressive and more compact, as long as it stays away from big OOP concepts like this. 

# 9.4 

Same comment as 9.2 : there can't be both options for idyl files : either import or lib. 

# 10.1 

Same as previously, I need you to remove all traces of big OOP compositions such as `    instr = cs.compile("synth.orc").get("my_instr")`. 

# 11.5 

The Pattern 2 could actually be implemented with a flow. Convert this to real idyl code using flows. 

Then, also remove the too many patterns focusing only about music. Sure I'm musician and working on this in order to create music. But it's not the only intentionn here. 

# Trait system 

This is a good start. I'd need more complete examples to show how it could be used. 

```
trait melody_op {
    // Overloads + operator
    op+(a, b) = [for i in 0..len(a)=> max(a,b) ]
}

flow first melody_op {
    [1, 3, 5, 7]
}

flow second implements melody_op {
    [6, 6, 5, 1]
}

third = first + second 
// third is [6, 6, 5, 7]
```

Also, of course, traits can implement functions (not only operators). Give examples of that too. 
Still keep the warning that it won't be implemented in alpha version (future feature). 

# General idea 

in for loops, replace `=>` with a single character symbol. 
Maybe `:` would be great ? 
Another idea ? 

# More 

We should find a common system to use functions as first class values : a function should be able to create a new one and return it 
So, maybe we need some other kind of lambda : flying function, with some scope capture. 

# More again 

At the end of the new syntax attempt, add a few notes about you overall opinion on the language : strengths, weaknesses, flaws, what is missing, what is consistent, what is not (etc) 