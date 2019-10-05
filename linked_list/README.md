# Linked Lists

## Singly linked list

A directional chain of nodes. Each node holds an item of data and a link to the next node in the chain


The first node in the chain is called the head.The link from the last node in th chain is null.This is sometimes called the tail

Can only be traversed in one direction as the links only go one way.This makes access an O(N) operation

Is unordered so data from various parts of the chain can be swapped and as long as the links remain the same then the integrity of the data structure is maintained

Nodes can be added and removed at runtime so it can be dynamically sized to suit requirements. However, node by node allocation isnt the most efficient as the locality of this memory is unspecified

Adding to and removing from the front of the list is very fast (constant time) altering other parts of the list involves iterating through to find the element. As such singly linked lists are a good simple way to implement a stack.

