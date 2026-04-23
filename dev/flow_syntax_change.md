# Flow modification idea

The idea discussed is to remove the flow keyword. 
Thus, only functions. 
A function expression can return a single value, or a collection (thus, a flow). 

```idyl 
f(x) = x * x // like now, if x is a number, returns x pow 2, else, applies it to all flow 

oscil(freq, amp, dt=50ms) = [s, s::phase, amp] // returns a dynamic flow 
|> {
    s = sine(freq) * amp // no dt specified, sine is computed at parent rate 
}

multilfo(freq, amp, dt=50ms) =  // returns a multi member dynamic flow
    sine_lfo: [si, si::phase]
    sq_lfo: [sq, sq::phase]
    tri_lfo: [tri, tri::phase]
|> {
    si = sine(freq) * amp
    sq = square(freq) * amp
    tri = triangle(freq) * amp

}
```


It could also be 
```idyl
oscil(freq, amp, dt=50ms) = flow [s, s::phase, amp] // returns a dynamic flow 
|> {
    s = sine(freq) * amp // no dt specified, sine is computed at parent rate 
}
multilfo(freq, amp, dt=50ms) = flow {
    sine_lfo: [si, si::phase]
    sq_lfo: [sq, sq::phase]
    tri_lfo: [tri, tri::phase]
}
|> {
    si = sine(freq) * amp
    sq = square(freq) * amp
    tri = triangle(freq) * amp

}
```


this proposition is just a set of interrogations for now 

It must not change anything in the behavior of flows themself (they still exist, we just remove the specific syntax for flows, they can then be integrated naturally as functions outputs). They must be accessed the same way. 

1. Is it a good idea ? What would it bring ? Break ? 
2. Could we just implement the feature for flow maker functions, and not remove the flow keyword (explicit flow primitives) ? 
3. Another idea ? 

Tell me what you think about : is it a good  idea ? Pros and cons ? Modification difficulty ? Possible syntax issue ? Semantic issue ? Evaluation ? 