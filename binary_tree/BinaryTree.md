# Binary tree

A binary tree is just a tree with at most 2 children per node. When most prople talk abouta binary tree they specifically mean a binary search tree. This is a binary tree with the additional property that children to the left of a node are less than it and children to the right of a node a re greater than it. This results in the every node to the left of a subtree being less than those in the right subtree.

This also leads to fast lookups O(log(n)) 

there are a bunch of ways to traverse tree. including:

### Depth first searches

#### In order traversal
 as it sounds, this leads to visiting all nodes in order.
 
 1. visit left nod
 2. visit root node
 3. visit right node
 
#### Pre order traversal
 
 
 1. visit root
 2. visit left
 3. visit right
 
 
#### Post order traversal
 
 1. visit left
 2. visit right 
 3. visit root
 
 
### Breadth first search
 traverses a tree level-by-level. cant be done recusively like the others. need to use a queue to store and process the nodes on any given level
 
 
### Deleting from a binary tree
 
 There are 3 distinct possibilities:
 
 - Node to be deleted is a leaf node
   - then just delete
 - Node only has 1 child
    - then copy child to node then delete chile
 - node has 2 children
    - then need to contains the inorder successor node the end of the left subtree and copy value over before deleting the successor node