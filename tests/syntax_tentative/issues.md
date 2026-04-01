# Critical issues 

1. Ternary operator 

The collision with function parameters is a problem when using ternary in function call parameters.
I suggest replacing the ternary operator separator from `,` to `;`. 

2. Lambda block syntax ambiguity 

The grammar rule will be that a lambda is : 
- `|>` followed by opening curly bracket, init block, expressions, then closing curly bracket.

3. |N| and | in Flow definitions conflicts with bitwise OR 

This is true, and a big problem. We should switch all `|2| |` expression to `|2||` (no space). This should avoid parsing issues. 

4. State Emission 

For now, I have the feeling emit statement is not used and documented proprely. 
The functions defined inside lambda (update bloc, even in init sections) are always visible to the whole function body. 
The emit keyword aim is to propagate these functions outside of the function body (to a parent scope). 
This is not used yet. But it should be documented properly. 

# Moderate issues 

5. Trigger parameter postfix conflicts with trigger litteral 

The difference is parsed with the context : 
- identifier with `!` `ident!` without space means trigger parameter 
- `!` alone means trigger. 

6. Flow member access ambiguity with module functions 

I think we should figure something out to make this right. 
Suggestion : 
- For modules and libraries in namespaces : `osc::send()` with `::` instead

7. Default parameter conflict 

I don't see the problem : context is clearly different 

8. Process block scope rules unclear 

`import` and `lib` statements must never be called inside a block. Only at global scope. 
This should be fixed in examples. 

# Minor issues 

9. Init block structure 

Init block ends when its curly bracket is closed. This is clear enough for me. If you think it is not : document it explicitely. 

10. Flow keyword scope unclear 

keyword `flow` means : defining a flow. 
A function returning a flow must be called a flow (as it is actually a flow). 



