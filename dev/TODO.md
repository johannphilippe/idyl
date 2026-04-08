# This TODO is a list of operations for assistant to work on 

## Process block rules 

When there is one process block (only one), the rule is the following : we are in a temporal process. The program should not exit (process might be filled with user later, livecoding for example)

A process can have : a name, a duration 

When several processes are in a program, they must have either : 
- A duration : then, processes will play one after another, according to their durations 
- A name : Then process won't play, until they are fired from external command (livecoding or UDP trigger)

### Design specification — written by assistant

#### Current state

The parser already supports three `process_block` grammar forms:

1. `process: { … }` — anonymous, `name_` set to `"main"`, no duration
2. `process name, dt=expr: { … }` — named with duration expression
3. `process name: { … }` — named, no duration

The AST struct `process_block` holds `name_`, `duration_` (optional `expr_ptr`), and `body_`. However, the evaluator **ignores both `name_` and `duration_`** — it treats every process block identically: push scope, scan for temporal bindings, subscribe to scheduler, keep-alive via `active_count()` polling in the main loop. The semantic analyzer only validates that process blocks appear at global scope; it performs no name/duration checking.

#### Design

**Single process block (keep-alive mode):**

When the program contains exactly one process block (regardless of name), the runtime enters **keep-alive mode**. The main loop already does this — it spins on `scheduler.active_count() > 0`. The only change needed is: if the single process block has **no temporal subscriptions** (e.g. purely static assignments, or empty body), the program should **still stay alive** rather than exiting. This supports the livecoding scenario in which a process is initially empty and gets filled later.

Implementation:
- After `eval.run()`, check the number of process blocks encountered (the evaluator should count them during execution).
- If `process_count == 1`, enter keep-alive unconditionally (remove the `active_count() > 0` guard). The loop only exits on explicit signal (SIGINT/SIGTERM). Add signal handling with `std::signal` or `sigaction` setting an `std::atomic<bool> running` flag.
- If `process_count == 0`, exit immediately (script mode — just definitions, no execution).

**Multiple process blocks with durations (sequential mode):**

When multiple process blocks are present and they carry durations, they execute **sequentially** in source order. Each process runs for its declared duration, then its temporal subscriptions are torn down and the next process starts.

Implementation:
- The evaluator should **not** immediately start all process blocks. Instead, `eval.run()` collects all `process_block` AST nodes into an ordered vector `std::vector<std::shared_ptr<parser::process_block>>` during the top-level statement scan.
- A new method `eval.run_process(proc)` encapsulates the current process block handling logic (scope push, segment creation, scheduler subscription).
- For sequential mode, the main loop becomes:
  ```
  for each proc in processes:
      eval.run_process(proc)
      if proc.duration_:
          duration_ms = eval.eval_expr(proc.duration_).as_number()
          wait duration_ms (sleep loop checking active_count or a timer)
          unsubscribe all segments created by this process
          pop scope
      else:
          // no duration = run until its temporal sources end naturally
          wait on active_count() == 0 for this process's subscriptions
  ```
- The evaluator needs to track which subscription IDs belong to which process block, so it can selectively unsubscribe. Add a `std::vector<uint64_t> current_process_subscriptions_` that gets populated during `run_process` and cleared after teardown.
- Duration expressions are evaluated once at process start (they are not reactive).

**Multiple process blocks with names only (triggered/dormant mode):**

Named process blocks without durations are **dormant** — they are parsed, semantically analyzed, and their AST is stored, but they are not executed until triggered externally.

Implementation:
- The evaluator stores named processes in a map: `std::unordered_map<std::string, std::shared_ptr<parser::process_block>> named_processes_`.
- A `trigger_process(name)` public method on the evaluator calls `run_process` for the given name.
- External triggering can come from:
  - A UDP listener (future, out of scope for this phase)
  - A built-in function `fire("process_name")` callable from within another process block
  - An OSC message routed through the module system
- For this first implementation, support `fire("name")` as a built-in that calls `trigger_process`.

**Semantic analysis additions:**
- Validate that if multiple process blocks exist, they all have either durations or names (mixing anonymous with named is an error).
- Warn if a named process block is never referenced by `fire()`.
- Validate duration expressions resolve to `value_t::time`.

**Parser:** No grammar changes needed — the three existing forms cover all cases. The `dt=expr` syntax for duration is already parsed. Consider renaming the grammar token from `DT` to `DURATION` (or accept `dt` as the keyword) — currently the parser reuses the `DT` token which is semantically overloaded (it means both "tick rate" in temporal functions and "duration" in process blocks). This is acceptable for now but should be noted.

---

## Catch 

Catch will actually catch values emitted from a function with emit statement. 
There should be several syntax allowed : 
1. Simple catch
```idyl
process: {
    a = some_function()
    a catch stop : {
    // This block is executed when stop is triggered 
    }
}
```

```idyl
some_other_function() = cnt |> {
    init: {
        cnt = 0 
        emit incr = rnd(int(1), int(10))
        }
    cnt = cnt + incr

}
some_function() = some_other_function() * catch (incr)
again_another_function() = some_other_function() catch (incr as i) / myrand
|> {
    init: {myrand = rnd(0, i)}
}
```


Several catches should be implemented. 
Catch system will be used to catch special events (end, trigger, anything)
They will also be able to capture emitted values (from emit statement)

### end 

For temporal processes, there might be an end. 
We need to introduce an end primitive that would be emitted from functions, modules, or builtins. 
For example, Csound module will provide a way to instanciate a voice, and catch its end. 

### error 

Maybe the name is not good. Though, we might want to catch null pointers or things that could cause memory corruption or violation.

### trigger 

Of course, when a trigger is fired, it could be captured (ideal for creating nice control structures)

### Design specification — written by assistant

#### Current state

The parser supports `catch end : { … }` as a `process_body_statement`. The AST `catch_block` struct has `expression_` (the watched expression), `event_type_` (string), and `handler_` (vector of statements). However there is a **parser bug**: the grammar rule `expression catch_block { $$ = $2; }` discards the expression (`$1`) — it never assigns it to `catch_b->expression_`. The catch block is therefore created without knowing *what* it is catching on. The semantic analyzer pushes a `catch_block` scope and resolves the handler statements, but does no event type validation. The evaluator has **no** `catch_block` case in its `exec_stmt` dispatch — catch blocks are silently dropped at runtime.

#### Design

**Event model:**

Introduce a lightweight event bus on the evaluator. Events are typed values that flow from sources (temporal functions, modules, built-ins) to catch handlers.

```cpp
enum class event_type { end, error, trigger };

struct event {
    event_type type_;
    value payload_;           // the emitted value (nil for end/error without payload)
    uint64_t source_id_;      // instance ID of the source
};
```

**Event sources:**

- **`end`**: A temporal function's tick callback returns `false` (natural completion). The scheduler already supports this — when `tick_fn` returns `false`, the subscription is erased. Hook into this: before erasing, fire an `event_type::end` event for that instance. Modules can also fire `end` (e.g. Csound voice finishes) by calling an `emit_event()` function provided to them.
- **`error`**: Runtime errors that would currently crash or silently produce nil. Instead of aborting, wrap risky evaluations (division by zero, nil dereference, module call failure) in try/catch, and if a catch handler is registered, fire `event_type::error` with the error message as a string payload. If no handler is registered, fall back to current behavior (warning + nil).
- **`trigger`**: When a temporal function's output is `value_t::trigger` and the trigger fires (`trigger_ == true`), fire `event_type::trigger`. This happens every tick where the output is a fired trigger.

**Catch block binding (fixing the parser bug):**

Fix the grammar rule to assign the watched expression:

```yacc
| expression catch_block
{
    auto catch_b = std::static_pointer_cast<idyl::parser::catch_block>($2);
    catch_b->expression_ = $1;    // <-- FIX: bind the source expression
    $$ = catch_b;
}
```

**Parser extensions for new event types:**

Currently only `CATCH END` is in the grammar. Add:

```yacc
catch_block
    : CATCH END COLON LBRACE process_body_statements RBRACE
    { /* event_type_ = "end" */ }
    | CATCH ERROR COLON LBRACE process_body_statements RBRACE
    { /* event_type_ = "error" */ }
    | CATCH TRIGGER COLON LBRACE process_body_statements RBRACE
    { /* event_type_ = "trigger" */ }
    ;
```

`ERROR` needs a new token (or reuse an identifier — since `error` is a Bison reserved word, use `ERR` as the token name, mapped from the keyword `error` in the lexer, or choose a different keyword like `fail`). Alternatively, make the event type a generic `IDENTIFIER` so the parser doesn't need a token per event:

```yacc
catch_block
    : CATCH IDENTIFIER COLON LBRACE process_body_statements RBRACE
    { catch_b->event_type_ = $2; }
    | CATCH END COLON LBRACE process_body_statements RBRACE
    { catch_b->event_type_ = "end"; }
    ;
```

This keeps `end` as a keyword (since `END` is already a token) and allows arbitrary event names for future extensibility. Recommend this approach.

**Emit capture:**

The `emit` keyword inside a lambda block should propagate the emitted variable as an event payload that catch handlers can receive. When `emit x = val` executes inside a temporal function tick:

1. The value is stored in the instance state as it is today.
2. Additionally, fire an event: `{ type_: trigger, payload_: val, source_id_: instance_id }`.
3. The catch handler's scope receives the emitted variable name as a local binding.

This connects emit (already parsed and semantically validated) to the catch system.

**Evaluator implementation:**

1. Add an event registry to the evaluator:
   ```cpp
   struct catch_registration {
       std::string event_type;                    // "end", "error", "trigger"
       uint64_t source_instance_id;               // which instance to watch
       std::vector<parser::stmt_ptr> handler;     // handler statements
       std::string source_var_name;               // the variable name of the source expression
   };
   std::vector<catch_registration> catch_registrations_;
   ```

2. In `exec_stmt`, add a `case parser::node_t::catch_block`:
   - Evaluate `catch_b->expression_` to identify the source (it should resolve to a value bound to a `function_instance`).
   - Look up the instance ID from the variable name → instance mapping.
   - Register a `catch_registration` with the event type and handler.

3. Add `fire_event(event)` method on the evaluator:
   - Iterate `catch_registrations_`, match on `source_instance_id` and `event_type`.
   - For each match, push a scope, bind `payload` (or the emitted variable name), execute handler statements, pop scope.

4. Hook event sources:
   - **end**: Modify the scheduler tick lambda — when `tick_fn` returns `false`, call `fire_event({end, nil, instance_id})` before the subscription is erased. Or: add a completion callback to `function_instance` that the scheduler invokes.
   - **error**: In eval_expr, wrap risky operations. On failure, call `fire_event({error, string(message), instance_id})`. If no handler matched, produce warning.
   - **trigger**: In the tick lambda, after `tick_instance`, check if output is a fired trigger. If so, call `fire_event({trigger, output, instance_id})`.

**Semantic analysis additions:**
- Validate that `event_type_` is one of `"end"`, `"error"`, `"trigger"` (or a known user-defined event name in the future).
- Validate that `expression_` is present (non-null) on every catch block.
- Warn if a catch block's source expression doesn't resolve to a temporal function.

---

## Maybe a real integer primitive

For now, int builtin function only casts the value to int, before casting it to double. 
We might want to have a flag, to know when it has to be treated as int (useful for bitwise operations etc)
The rule is simple : every number is always float, except if explicitely written as int with int builtin. 

### Design specification — written by assistant

#### Current state

The `value` struct stores all numbers as `double number_`. The `int()` built-in function truncates the double and stores the result back as a double: effectively `value::number(std::trunc(v.as_number()))`. There is no way to distinguish an integer-typed value from a float-typed value at runtime. Bitwise operations (`&`, `|`, `^`, `<<`, `>>`, `~`) are specified in the language syntax doc but are not yet implemented in the evaluator. They would need integer semantics.

#### Design

**Approach: flag on value, not a new type enum.**

Adding a new `value_t::integer` would require changes across every switch statement on `value_t` throughout the codebase (evaluator, semantic analyzer, core, builtins, flow access). Instead, add a boolean flag `is_integer_` to the `value` struct. The type remains `value_t::number`, but the flag gates integer-specific behavior.

```cpp
struct value {
    value_t type_ = value_t::nil;
    double number_ = 0.0;
    bool trigger_   = false;
    bool is_integer_ = false;    // NEW: true when value was created via int()
    // ... rest unchanged
};
```

**Integer creation:**

- The `int()` built-in sets `is_integer_ = true` on the returned value.
- A new static constructor: `static value integer(int64_t v) { value r; r.type_ = value_t::number; r.number_ = static_cast<double>(v); r.is_integer_ = true; return r; }`.
- All other number constructors leave `is_integer_ = false` (default).

**Integer propagation rules:**

- **Arithmetic** (`+`, `-`, `*`, `/`, `%`): If **both** operands have `is_integer_ == true`, the result is integer (truncated). If either is float, the result is float. Division between two integers produces integer (truncating division), matching C semantics.
- **Comparison** (`<`, `>`, `==`, etc.): Always produces a float-typed number (0.0 or 1.0). The `is_integer_` flag does not propagate through comparisons.
- **Bitwise** (`&`, `|`, `^`, `<<`, `>>`, `~`): Require **both** operands to have `is_integer_ == true`. If not, produce a semantic error or runtime warning and return nil. Operate on `static_cast<int64_t>(number_)` and return an integer-flagged value.
- **Unary minus** (`-x`): Preserves `is_integer_`.
- **Built-in math functions** (`sin`, `cos`, `sqrt`, etc.): Always return float, even if input is integer. `floor()`, `ceil()`, `round()` return float unless explicitly wrapped in `int()`.
- **`float()` built-in**: Clears `is_integer_` flag. Useful for explicit conversion back.
- **Assignment/emit**: The flag is preserved through binding and state propagation.
- **Flow storage**: Integer values stored in flows retain their flag.

**Evaluator changes:**

1. In `eval_binop()`: check `is_integer_` on both sides. Apply integer semantics when appropriate.
2. Add cases for bitwise operators (`&`, `|`, `^`, `<<`, `>>`). These are not yet in the parser/lexer — they need new tokens.
3. Add unary `~` (bitwise NOT) operator.
4. Modify the `int()` built-in to set the flag.
5. Add `float()` built-in to clear the flag.

**Parser/lexer additions:**

The bitwise operators need lexer tokens and parser precedence rules:
- `&` (AMPERSAND), `|` (PIPE), `^` (CARET), `<<` (LSHIFT), `>>` (RSHIFT), `~` (TILDE)
- Note: `|` is already used for lambda block `|> { … }`. The lexer must disambiguate: `|>` is the pipe operator (already tokenized as `PIPE_ARROW`), while a standalone `|` followed by a non-`>` character is bitwise OR.
- Precedence (low to high): `|`, `^`, `&`, `<<`/`>>`, then arithmetic. This matches C precedence.

**Semantic analysis:**

- Warn when bitwise operators are used on non-integer values (if detectable at analysis time — may only be catchable at runtime since types are dynamic).
- The `bit(val, index)` built-in should require integer input and return an integer-flagged value.

**`as_integer()` helper:**

Add to `value`:
```cpp
int64_t as_integer() const {
    return static_cast<int64_t>(number_);
}
```

This is used internally by bitwise operations and anywhere integer semantics are needed.

**Display:**

When printing an integer-flagged value, display without decimal point: `42` instead of `42.000000`. Modify `as_string()` to check `is_integer_`.

---

## Scheduler proof 

Write a Csound module that would provide functions : 
- `handle = compile("my_orc.orc")` : Compile a Csound orchestra and return a handle (introduction of C pointer types, passed as intptr_t )
- `start(handle)` : starts performance 
- `instance = sched("instr_name", start, dur, any_other_param)`: starts an instance of a voice. The instance object should emit a stop when ending. 

The module should use the standard C++ API of Csound 6.

Module system should provide a way for module functions to emit values in time 

# Bugs 

- [FIXED] Library/module namespacing: the correct syntax is `ns = lib("mylib")` and
  `ns = module("name")`. Documentation and README updated to match.
- [FIXED] Standard modules (OSC, CSound) now require explicit `module("osc")` import.
  Built-in modules register in a catalog and are loaded lazily on first use.
- [FIXED] First-class functions in lfo: added pure `sine_shape`, `tri_shape`,
  `square_shape`, `saw_shape` helpers to stdlib. `lfo()` now uses these instead of
  calling the stateful temporal oscillators as first-class functions.

# Hot reload of process 

For live coding purposes (and some other cases), we should be able to hot reload code of a process. 
This must be done in a smart way : 
- It must not "reinit" the currently running stateful objects (keep running, with updated things)
- 

# Issues : 

- dt parameters should be able to evolve over time (like any parameter of a temporal function). For now, named params don't evolve. Same for positional arguments. 
    Example :
    ```idyl
    amp = pos(square(0.1hz, dt=10ms)) // 0-1 range lfo square
    s = lfo(1hz, amplitude=amp) // the amplitude is never updated 
    ```

# Not urgent

- Named parameter expecting number is not working when passing a constant function (which is actually a number in a way). 
    Example with lfo (stdlib) : 
    ```idyl 
    sine_wave = 0 
    s = lfo(1hz, waveform=sine_wave) // This doesn't work, because sine_wave is treated as a function
    ```

There is a workaround by calling the function with `int(sine_wave)` or `float(sine_wave)`, thus casting it to a number value.
