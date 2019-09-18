# Red Black Tree

Red black trees are a variety of self balancing binary tree. It gets its name from the fact that each of its nodes are assigned a colour (red or black). This color is then used to determine when a tree is "unbalanced".

There are 5 properties that define a balanced red black tree:
 1. Each node is either red or black
 2. The root is black
 3. All the leaves (null nodes) are black
 4. red nodes have black children (i.e. no consecutive red nodes)
 5. every route from root to leaf has the same number of black nodes 

When one of these properties is violated the tree is rebalanced until all are satisfied

This leads to the additional property that the distances

    root -> furthest leaf <= 2 * ( root -> closest leaf )
    
#### Algorithm complexity 

| Alg    | Avg      | Worst    |
| ------ |:---------|:-------- |
| Space  | O(n)     | O(n)     |
| Search | O(log n) | O(log n) |
| Insert | O(log n) | O(log n) |
| Delete | O(log n) | O(log n) |

#### Insertion

Initial insertions find their initial locations in the same manor as a binary search tree. All new nodes are red. Once we we find the new nodes initial position we then check if the 5 properties are satisfied.

If we find an imbalance the we solve it in several ways:

- if the new node is the root:
    - colour swap the root
- if parents aunt (grandparent's other child) is red:
    - recolour:
        - set grandparent -> red
        - aunt and parent -> black
        - run the re-balancing algorithm on the grandparent
- if parent's aunt is black
    - rotate and recolour
        - if new node Z is in the left subtree of the grandparent's (G) right subtree
            - right-left rotate around parent (P)
            - colour flip on Z (set z = red, G and P red).
            - Recurse on Z
        - if Z is in the right subtree of G's left subtree
            - left-right rotate around P
            - colour flip on Z (set z = red, G and P red).
            - Recurse on Z
        - if Z is in G' right subtree's right subtree
            - left rotate around G 
            - recolour G and P
            - recurse on P
         - if Z is in G' left subtree's left subtree
            - right rotate around G 
            - recolour G and P
            - recurse on P   
 
 
 #### Deletion 
1. Find the node to be replaced through regular BST means
2. if node to be deleted,v, has 2 non-null nodes then find it's in order successor, u, copy u's value (but not colour) to v and recursively call delete for u. From here on in we can assume v has at most 1 child
3. if v is red just delete as this wont change the black height of the tree
4. if v is black and has one red child, detach v and recolour the red child black
5. if v has one child and it is black: detach v and label the child double black, run the fix algorithm on this node. If v has no children then it is basically the same as the case on one black child, the child is just null
 
 Fixing algorithm: depending on the orientation and colours of the double black's family work through a series of cases, recursing until you reach a terminal case that leaves the tree balanced
 
 double black scenarios:
 
1. v's sibling (s) is black and at least one of s's children is red (r), then we do rotations based on the placements of s and r
   1. Left Left case: s is in parent p's left subtree and red child r is in s's left subtree
        1. right rotate about s
        2. recolour r as black
   2. Right Right: s is p's right child. r is s's right child
        1. left rotate about s
        2. recolour r as black
   3. Left Right: s is p's left child. r is s's right child
        1. right rotate about r
        2. swap colours of s and r
        3. follow Right Right steps with r playing the role of s and vice versa
   4. Right Left: s is p's right child. r is s's left child
        1. left rotate about r
        2. swap colours of s and r
        3. follow Left Left steps with r and s swapping roles
2. s is black and both s's children are black
   1. recolour s
   2. if p is red then colour p as black and tree is balanced
   3. else mark p as double black and recur with p playing the role of u/v
3. s is red
   1. flip the colours of p and s (so s always becomes black)
   2. recur. rotating based on the scenario it now matches
4. v is root
   1. just mark it black. this will just reduce the black-height of the tree by 1
 