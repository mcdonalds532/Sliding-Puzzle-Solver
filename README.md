# Sliding-Puzzle-Solver

This program uses the A* algorithm to solve randomized solvable 3x3 to 5x5 sliding puzzles.

Visuals are implemented using the Malena framework: https://github.com/daversmith/Malena/tree/master

Note: A solution to the 5x5 puzzle is not guaranteed with this algorithm. 4x4 and 3x3 should solve with no issue.

A* chooses the best possible state to traverse to based on the lowest f(n) value of a node in the priority queue or min-heap:
1. f(n) = g(n) + h(n)
2. g(n) = the cost from the initial state to the current state
3. h(n) = Manhattan Distance OR Chebyshev Distance of target tiles to their target locations + vertical and horizontal Linear Conflict penalties

The combination of Manhattan Distance and Linear Conflict  is based on the fact that it will take a tile at least MD steps to reach its destination. The penalty of 2 applied from Linear Conflict reflects the least amount of moves to resolve the linear conflict. Hence, this simple heuristic is always admissible.

# Instructions

1. Build the project with the given CMakeLists.txt.
2. Adjust the value for n in the definition of Model's constructor to the desired n-value.
3. Adjust the dimensions of sf::VideoMode in the definition of SlidingPuzzleApp's constructor to match the dimensions of the respective image.
4. Run the program.

# Possible Improvements / Challenges

As the value of n grows, the amount of states explored increases by orders of magnitudes. As such, memory is a problem unless the heuristic can be improved. To remedy this, a potential improvement could be to use Iterative Deepening A* and prune potential nodes based on a continuously updated threshold f(n) value.

If a better heuristic is implemented, change the algorithm such that for larger n-values, the target tiles include the entire top row and left column. This ensures that the remaining n x n sub-board is solvable; by locking tiles before the entire top row and left column is complete, the remaining board is not guaranteed to be solvable. We only use this method because the current heuristic is not good enough for larger n-values.

# Video Demo

[https://www.youtube.com/watch?v=eLlc4rw3USw](url)
