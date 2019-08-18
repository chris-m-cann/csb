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
 