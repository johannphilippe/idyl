The example of trigger driven dt is not clear (wrong actually), can you fix it ? 

# 7.1 
syntax description for flow is wrong, it should be :  
```
flow flow_name(params) = {
    member1: [/* something */]
    member2: [/*etc*/ ]
}
```

instead of what you wrote


# 7.4 

`to_trigger` function is not right. 
This should be : 
`to_trigger(value) = ., ! ? (value != 0)`

So we need to also say that `.` is a primitive, meaning rest, opposite of trigger. This should be in primitive section. 

# 9 

This is a good start for high-order functions & closures. 

Though, it is not clear, when you declare the returned function. Some different syntax to declare flying functions could be cool (it would need to be explicit that it is a flying function). 
Also, I think this has to figure only in a separate "Future developements" chapter, with traits. 


# 11.2

No import or lib statement inside process blocs. This should only happen in global scope. 

# 12.3 

About dynamic voices using flows, I would express it differently. 
``` idyl

flow notes = {
    pitch: [60 62 65 67]
    duration: [500s 500ms 300ms 400ms]
}

// All voices start at the same time 
process dynamic_voices_synchronous: {

    // synthesize function is called with flows, it is polyphonic by default 
    voices = synthesize(notes.pitch, notes.duration)
}

// All voices start one after another 
process dynamic_voices_synchronous: {
    init: {cnt = 0}
    metro = metronome(notes.duration[cnt])
    cnt = counter(metro) 

    voice = synthesize_on_trig(metro, notes.pitch[cnt], notes.duration[cnt])
    
    /*
        Something like the following would be great 
        metro catch ! {
            voice = synthesize(notes.pitch[cnt], notes.duration[cnt])
        }

    */

}

```
Regarding last example, I'd like you to extend the `catch` potential : create a special chapter for it, and show all features it could have (`catch !` for catch trigger, `catch end` for a function reaching end point, like voice end, `catch change` for any change in a function, and as many other relevant catch you could think of). This `end` catch also shows we probably more primitives for lifecycle management. `end` would be one. But what about others ? List it with the catch potential new features. 

# 13.5 

Add Flow functions : 
- scramble(flow) // randomly scrambles a flow 

Modify Flow functions : 
- `repeat(flow, N)` where N is 2 by default (and is optional) 

# About functions 

One important thought about functions  that came to me : That would be great if function were by default able to act both on value or flows. 
Example : 
``` 
add(a, b) 

a = add(1, 2) // = 3 

b = add([0, 3, 5], [5, 2, 0])
// b = [5, 5, 5]
```

This should be documented as well. Except if you see anything that conflicts with it ? 


# Design notes & critical assessment 

## Weaknesses 

- For loop syntax : I think we could do without imperative loops. Some functional generator would be great : show suggestions in the for loop chapter, marking explicitely (in bold) that it is a suggestion. 
- `dt=?` : Would `dt=!` be more clear ? If no, propose something (but not `dt=trigger` since it would create ambiguity about what is actually a trigger)
- OOP temptation : any suggestion to remove OOP completely, while keeping namespaces for modules and libraries ? 
- Error handling : since it must be a realtime system, I think error handling must act in two ways : warn and/or log when error occurs, abord performance if critical error (module not loaded for example) - any more suggestion ? 
- Voice pool management : I think this should stay abstracted from "voices" which is a musical concept. It should be "instances" instead. What kind of instances management pool would you suggest ? 
- No explicit type : it must stay on interpreter side 



