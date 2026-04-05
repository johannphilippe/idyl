What's implemented (working well):

Pass 1: Symbol collection (functions, flows, lib/module imports, duplicate detection)
Pass 2: Name resolution (full recursive walk, scope stack)
Pass 3: Arity & argument validation (check_call — positional/named args, slot filling, duplicates)
Pass 4 (partial): Type inference on call arguments (infer_expr_type, expected_type_ on params)
Flow member duplicate detection
What's missing, by priority:

High priority (catches real bugs)
Temporal validation (Pass 5) — The spec's most distinctive feature has zero checking:

Lambda blocks without a time source (dt or trigger param) should error
@ memory operator used outside temporal context should warn
emit used outside a |> block should error
dt used in a non-temporal function should warn
Trigger params on non-temporal functions should warn
The scope struct already has is_temporal_function_ — it's just never set or read
Structural validation (Pass 7) — No enforcement of what goes where:

Function/flow definitions inside process blocks should error
Imports inside process blocks or function bodies should error
Invalid statement types in lambda update sections should error
Generator variable scoping — generator_expr defines a loop variable (variable_) but the resolve() case never pushes a scope or defines it. That variable will always report as undefined.

Medium priority (better diagnostics)
binary_op / unary_op operand resolution — binary_op_expr resolves by calling resolve(op_), but the binary_op case itself isn't handled — it falls through to default. You need to recurse into left_ and right_. Same for unary_op → operand_.

Flow access member validation — flow_access is resolved but member names aren't validated against the flow definition's actual members (spec §10.2.3).

check_call for module_access calls — Namespaced library calls (ns::func(args)) go through module_access which resolves the qualified name but never calls check_call for arity validation, even though library param info is available.

Unused parameter detection — The scope stack has all the info needed. After resolving a function body, any parameter symbol that was never looked up could produce an info diagnostic.

Severity levels — The spec distinguishes error/warning/info, but the current error struct has no severity field. Everything is treated as an error.

Lower priority (polish)
Circular import detection — loaded_libraries_ prevents re-parsing but doesn't detect actual circular chains (A→B→A). The spec's import stack approach isn't implemented.

Type propagation through binary/unary/ternary ops — infer_expr_type returns unknown for all operators. The spec has detailed rules for what number + time yields, etc.

define_builtins() is called twice — once in analyze() and once at the top of global_scope_pass(). The second silently overwrites; harmless but messy.