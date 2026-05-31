1. Diff process issuee

When I start the following process in a live coding situation, it works fine 
When I comment out one of the two vapordream instances, almost all the other elements stop (only remaining working instances are the other vapordream, and the regular grainshap at line 31 or 32). I works again if I stop / start. 

``` idyl
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
        vapordream(euclid(3, 8, phase_in=ph), mtof(tune(a3, dorian, fl.second, just)), amp=fff, duration=1s)
        vapordream(euclid(3, 8, phase_in=ph), mtof(tune(a3, dorian, fl.third, just)), amp=fff, duration=1s)                                                                                 
        print(fl.first, fl.second, fl.third) 
        //lushpad(c, mtof(tune(f2, dorian, fl.first, just)), amp = fff, ch=rndi(1, NCH))
        //lushpad(c, mtof(tune(f5, dorian, fl.second, just)), amp = f, ch = rndi(1, NCH))
        //lushpad(c, mtof(tune(f6, dorian, fl.third, just)), amp = ppp, ch = rndi(1, NCH))
        variation = pos(sine(0.1hz, dt=50ms))
        grainsharp(dust(variation, dt=0.01s), rnd(20, 800), amp=rnd(mf, fff), ch=rndi(1,2))  
        metalsharp(dust(variation, dt=0.01s), rnd(20, 800), amp=rnd(mf, fff), ch=rndi(1,2))  
        purecut(dust(variation, dt=0.01s), rnd(20, 5000), amp=rnd(mf, fff), ch=rndi(1, 2))  
        m = metro(1b) 
        grainsharp(m, 500, amp=f, ch=rndi(1, 2))
        grainsharp('(m, 200ms), 500, amp=f, ch=rndi(1, 2))
}         
     
```

2. Another issue with hot reloading 

Another example of hot reloading causing automatic stop  of process (or process instances of temporal functions), even if there were no changes. 
Also, the duree variable/function (rndt from stdlib) causes the instrument to fire, which should not happen since the duration parameter is not a trigger
``` idyl
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
``` 

3. Hot reload can cause flow indexation to restart (in the case of trigger indexing)


``` idyl 
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
``` 


The fl index is lost. Seems to only do this with trigger indexing. 




4. (Not the most urgent) Delay with triggers 

It appears to me that the delay operator `'`  is not operating well with triggers. Can we check with a series of automatic tests to see if behaves as it should (If second parameter is number, it is number of ticks, if it is temporal value, it takes it explicitely for the correct amount of delay)


