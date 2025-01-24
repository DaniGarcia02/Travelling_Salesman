# GraphApplication

This is an exercise made at University, it is an application that can read graph that represent locations and I had to implement de searching algorithms to find the optimal path from the starting node to the ending node while traversing through some intermediate nodes.

The application was provided to me, I implemented the searching algorithms.

## Dijkstra
Finds the sortest path between a specified node and all the other nodes in the graph.
## Greedy
Always takes the best local option at any point, because of this it does not always find the shortest path.
## Backtracking
### First implementation
A pure backtracking algorithm where the nodes are traversed using DFS until a solution is found, then when we reach a point that surpasses the current best solution in length we prune it.
### Second Implementation
Instead of traversing every node, first we apply Dijkstra to find the shortest subtracks form every point that needs to be visited and we do the baacktracking algorithm with those subtracks instead of doing it with every node.
## Branch and Bound
### First Implementation
It's a BFS with priority for the shortest edge, we prune when a path is longer than the current best. It doesn't traverse every node, it uses subpaaths the same way as the second implementation of backtracking does.
### Second Implementation
Here every partial solution has an upper and lower bound that are calculated doing the sum the worst and best possible subtracks respectively from every node that remains unvisited, we order the tracks by the lower bound and prune when the lower bound is higher than the upper bound, we know we found a solution when the lower and upper bound are equal.
### Third implementation
This is a more sophisticated version of the second implementation, instead of calculating the bounds using every posible subpath from the unvisited nodes, we only calculate them with subpaths that can actually be used in the current partial solution, this way we order in a better way the partial solutions and are also able to prune better.
## Randomized algorithms
Not yet implementd.
