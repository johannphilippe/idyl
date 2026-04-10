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

- [important] on keyword : `on(trig): {// Do something}` where the bloc is only computed when the trigger happens 
- List of features in syntax.md (traits for example)
- Audio base runtime (for buffer rate accuracy, with pure audio synchronization) (not urgent)

## Test drifts 

- Compare csound_module scheduling to Csound OSC scheduling  