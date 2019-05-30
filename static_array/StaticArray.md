# Static Array

A static array is a list of items of the same type stored in a contiguous block of memory.

It is stored on the stack so size of thing its storing and how many elements are in the array must be known at compile time

As it is on the stack you cannot dynamically resize it at runtime.So no elements can be appended or removed from the array.

As it is a contiguous memory block you have random access so indexing into it is very fast. An index is just a memory offset from the start of the array

Iterating a static array is also very fast as computer knows exactly what memory you will need access to so can optimise heavily.
