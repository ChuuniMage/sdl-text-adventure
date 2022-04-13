Idiom: Struct of Pointers to Structs.

Handling a lot of state at once is difficult.

This is why we put lots of related state into structs.

When a struct gets big, then working on all of it at once becomes a bit expensive.

This is why we pass a pointer to the struct.

When we have

It is even more difficult when you are passing in many states at once into a function.

The solution: A struct of pointers to other structs.

Sub-problems: Ergonomics with repeated access of nested properties.
