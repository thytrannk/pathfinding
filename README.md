# Pathfinding algorithms

This repository contains C++ implementations of *pathfinding algorithms*, which solve the single-source shortest path
(SSSP) problem [1].
 
## Terminologies
* *g(n)* (*g*-cost of node *n*): cost (length) of the shortest path from the start node (source) to *n*
* *h(n)* (*h*-cost of node *n*): The heuristic value at *n*, which is a lower estimation of the cost of the shortest
path from *n* to the goal node
* *f(n)* (*f*-cost of *n*): The sum of *g(n)* and *h(n)*, which represents the estimated cost of the shortest path from
the start node to the goal node passing through *n*
* *s*: start node
* *g*: goal node 

## Algorithm descriptions
* Optimal search (algorithms to find the shortest path):
  * __Dijkstra__ [2]: This is a greedy search algorithm. At each step, the node with the minimum *g*-cost (closest
  *s*) is expanded. Unlike the rest of the algorithms below, this algorithm does **not** use any heuristic function *h*.
  * __A\*__ [3]: At each step, the node with the minimum *f*-cost is expanded.
  * __IDA\*__ (Iterative deepening A*) [4]: At each iteration, the nodes with *f*-cost being within a threshold is
  expanded in the order from low to high *f* values. At the beginning, this threshold equals to *h(s)*, and after each
  iteration, the threshold used for the next iteration is the minimum *f*-cost that exceeded the current threshold.
* Suboptimal search (algorithms to find a short path whose length is guaranteed to be within *w* times the shortest
path, where *w* is supplied by the user, to cut down run time):
  * __Weighted A\*__ [5]: At each step, the node *n* with minimum *f'(n)* is expanded, where *f'(n) = g(n) + w \* h(n)*.
  * __Optimistic search__ [6]: Similar to weighted A*, but *f'(n) = g(n) + w' \* h(n)*, where *w'* > *w*. Once a
  solution is found, a clean-up phase is done to determine if it is within *w* times the shortest path; if not, another
  solution is to be found. This algorithm can outperform weighted A*.

## Problem domain descriptions
Three problem domains are also implemented to test the algorithms. The test domains are:
* The 4 x 4 sliding tile puzzle [7]: There are 15 square tiles numbered from 1 to 15 and one empty space on a 4 × 4
  board. The goal is to slide the tiles into the empty space until the goal configuration is achieved, in which the
  empty space is at the first location of the board, followed by the tiles numbered from 1 to 15.
* 2D grid-based search: There is a 2D grid map with obstacles. The goal is to find the shortest path between two
  locations. Agents can have 8-way movement and are not allowed to cut through corners of obstacles. Vertical and
  horizontal movements have a cost of 1, while diagonal movements have a cost of sqrt(2).
* The travelling salesman problem [8]: There is a map of 19 cities uniformly placed at random in a unit square with
  their coordinates. The goal is to find the shortest route that passes each city and returns to the original city.
  

## How to build the program:
    cmake -S <path-to-source> -B <path-to-build>
    cd <path-to-build>
    make

## How to run the code:
    ./project <problem_domain> <algorithm> <print_path> <input_file_1> <input_file_2> <no_of_problems> <placement_strategy> <lookup_strategy>
where
  * `problem_domain`: *p* (for sliding tile puzzle), *g* (for 2D grid), or *s* (for travelling salesman)
  * `algorithm`: *1* for A*, *2* for Dijkstra, *3* for IDA*, *4* for optimistic search, *5* for weighted A*
  * `print_path`: either *y* (to print each step once the path to the goal is found) or *n* (not to print path)
  * `input_file_1`: the problem instance file for sliding tile puzzle; map file for 2D grid (this argument should be left blank for travelling salesman)
  * `input_file_2`: the problem instance file for 2D grid (this argument should be left blank for sliding tile puzzle and travelling salesman)
  * `no_of_problems`: maximum number of problems to solve

The following 3 input files are also included for testing:
  * *puzzle-benchmark.txt*: problem instance file for sliding tile puzzle [4]
  * *den203d.map*: map file for 2D grid [9]
  * *den203d.map.scen*: problem instance file for 2D grid [9]
  
  (for more details about the 2D grid input file, see [9])
  
The output shows the cost of the solution (length of the path found), number of nodes generated before the solution is
found, the steps to take from the start to the goal node (if the **print_path** option is enabled), and the time taken
to solve the problem.
  
### Example:

    ./project s 1 n 2
    
    Output:
    
    Problem 1
    Cost of solution: 3.907
    Number of nodes generated: 1142
    Steps:
    ASDNPRBLHOJKEQFCMIGA
    Solved in 0s.
    
    Problem 2
    Cost of solution: 4.30591
    Number of nodes generated: 901
    Steps:
    AKSNHPOMBLDIFECRQJGA
    Solved in 0s.
    
    Final results:
    Solved 2 out of 2 problems.
 
## References

[1] https://en.wikipedia.org/wiki/Shortest_path_problem#Single-source_shortest_paths
 
[2] Dijkstra, E. W. 1959. A note on two problems in connexion with graphs. Numer. Math. 1(1):269–271.
 
[3] Hart, P. E.; Nilsson, N. J.; and Raphael, B. 1968. A formal basis for the heuristic determination of minimum cost paths. IEEE transactions on Systems Science and Cybernetics 4(2):100–107.

[4] Korf, R. E. 1985. Depth-first iterative-deepening: An optimal admissible tree search. Artificial Intelligence 27(1):97 – 109.
 
[5] Pohl, I. 1970. Heuristic search viewed as path finding in a graph. Artificial intelligence 1(3-4):193–204.

[6] Thayer, J. T., and Ruml, W. 2008. Faster than weighted a*: An optimistic approach to bounded suboptimal search. In ICAPS, 355–362.
  
[7] https://en.wikipedia.org/wiki/Sliding_puzzle

[8] https://en.wikipedia.org/wiki/Travelling_salesman_problem
 
[9] Sturtevant, N. 2012. Benchmarks for grid-based pathfinding. Transactions on Computational Intelligence and AI in Games 4(2):144 – 148.