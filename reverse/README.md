# Reverse

A simple algorithm for reversing the order of a collection of elements.

involves traversing the list from both directions simultaneously and swapping each pair of elements in turn 

i.e. for sequence 1 2 3 4

- get iterators pointing to the first and last elements

```
1      2      3     4
^first              ^last
```

- swap the values
```
4      2      3     1
^first              ^last
```
- move first up the list and last down
```
4      2      3       1
       ^first ^last
```
- and swap those values
```
4      3      2       1
       ^first ^last
```
- repeat until pos(first) >= pos(last)
```
4      3      2       1 // all done now that first has passed last
       ^last  ^first    
```