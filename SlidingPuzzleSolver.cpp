#include "SlidingPuzzleSolver.h"

SlidingPuzzleSolver::SlidingPuzzleSolver(Model& model) : model(model), statesExplored(0)
{
    n = model.getN();
    currentBoardN = n;
    generateSubGoals();
}

void SlidingPuzzleSolver::generateSubGoals()
{
    int currentRow = 0;
    int currentColumn = 0;

    // Conditionally creates sub-goals if given a 5x5.
    if(n >= 5)
    {
        // Breaking the problem into parts. This is good for space complexity but does not guarantee a solution.
        // The reason why this method does not guarantee a solution is that the tiles are locked in place once in their positions.
        // Hence, the parity of the remaining board may also be locked in place, leading to an unresolvable n x n sub-board.
        const SubGoal fiveByFiveSubGoalOne({currentRow * n + currentColumn});
        subGoals.push_back(fiveByFiveSubGoalOne);

        const SubGoal fiveByFiveSubGoalTwo({currentRow * n + currentColumn + 1});
        subGoals.push_back(fiveByFiveSubGoalTwo);

        const SubGoal fiveByFiveSubGoalThree({currentRow * n + currentColumn + 2});
        subGoals.push_back(fiveByFiveSubGoalThree);

        // We group the final two elements of a row together in a step, as locking the second-most right element before the final element creates a situation where a linear conflict is guaranteed.
        const SubGoal fiveByFiveSubGoalFour({currentRow * n + currentColumn + 3, currentRow * n + currentColumn + 4});
        subGoals.push_back(fiveByFiveSubGoalFour);

        const SubGoal fiveByFiveSubGoalFive({(currentRow + 1) * n + currentColumn});
        subGoals.push_back(fiveByFiveSubGoalFive);

        const SubGoal fiveByFiveSubGoalSix({(currentRow + 2) * n + currentColumn});
        subGoals.push_back(fiveByFiveSubGoalSix);

        // Likewise, grouping the final two elements of a column.
        SubGoal fiveByFiveSubGoalSeven({(currentRow + 3 ) * n + currentColumn, (currentRow + 4) * n + currentColumn});
        fiveByFiveSubGoalSeven.isEndingSequence = true;
        subGoals.push_back(fiveByFiveSubGoalSeven);

        // Test out these grouping solutions if you want, but they almost never lead to a solution. This is due to memory problems and a weak heuristic; if a stronger heuristic is used, this could be possible.
        // The alternative for n x n boards where n >= 5 is to use Iterative Deepening A* and prune the heaps based on a continuously updated threshold value for f(n). This guarantees a solution with linear memory at the sacrifice of runtime.

        // Grouping the sub-goal together. This guarantees that the remaining 4x4 will be solvable BUT memory will likely run out.
        // SubGoal fiveByFiveSubGoal(
        //     {currentRow * n + currentColumn, currentRow * n + currentColumn + 1, currentRow * n + currentColumn + 2, currentRow * n + currentColumn + 3, currentRow * n + currentColumn + 4,
        //         (currentRow + 1) * n + currentColumn, (currentRow + 2) * n + currentColumn, (currentRow + 3) * n + currentColumn, (currentRow + 4) * n + currentColumn});
        // fiveByFiveSubGoal.isEndingSequence = true;
        // subGoals.push_back(fiveByFiveSubGoal);

        // Grouping the row and columns together. This does not necessarily guarantee the remaining 4x4 will be solvable BUT it is less likely to run out of memory.
        // SubGoal fiveByFiveSubGoalRow({currentRow * n + currentColumn, currentRow * n + currentColumn + 1, currentRow * n + currentColumn + 2, currentRow * n + currentColumn + 3, currentRow * n + currentColumn + 4});
        // subGoals.push_back(fiveByFiveSubGoalRow);
        // SubGoal fiveByFiveSubGoalColumn({(currentRow + 1) * n + currentColumn, (currentRow + 2) * n + currentColumn, (currentRow + 3) * n + currentColumn, (currentRow + 4) * n + currentColumn});
        // fiveByFiveSubGoalColumn.isEndingSequence = true;
        // subGoals.push_back(fiveByFiveSubGoalColumn);

        currentRow++;
        currentColumn++;
    }

    // Conditionally creates sub-goals if given a 4x4 or 5x5.
    if(n >= 4)
    {
        const SubGoal fourByFourSubGoalOne({currentRow * n + currentColumn});
        subGoals.push_back(fourByFourSubGoalOne);

        const SubGoal fourByFourSubGoalTwo({currentRow * n + currentColumn + 1});
        subGoals.push_back(fourByFourSubGoalTwo);

        const SubGoal fourByFourSubGoalThree({currentRow * n + currentColumn + 2, currentRow * n + currentColumn + 3});
        subGoals.push_back(fourByFourSubGoalThree);

        const SubGoal fourByFourSubGoalFour({(currentRow + 1) * n + currentColumn});
        subGoals.push_back(fourByFourSubGoalFour);

        SubGoal fourByFourSubGoalFive({(currentRow + 2) * n + currentColumn, (currentRow + 3) * n + currentColumn});
        fourByFourSubGoalFive.isEndingSequence = true;
        subGoals.push_back(fourByFourSubGoalFive);

        currentRow++;
        currentColumn++;
    }

    // The remaining 3x3 does not necessarily need to be broken down anymore as it is fairly cheap to compute (maximum around 30000 states traversed but on average, this is around <2000).
    const SubGoal threeByThreeSubGoal({
        currentRow * n + currentColumn, currentRow * n + currentColumn + 1, currentRow * n + currentColumn + 2,
        (currentRow + 1) * n + currentColumn, (currentRow + 1) * n + currentColumn + 1, (currentRow + 1) * n + currentColumn + 2,
        (currentRow + 2) * n + currentColumn, (currentRow + 2) * n + currentColumn + 1, (currentRow + 2) * n + currentColumn + 2,});

    subGoals.push_back(threeByThreeSubGoal);

    // Note: We could consider making sub-goals for 3x3 and then 2x2 but at this stage in the puzzle, guaranteeing the remaining puzzle can be solved is more important as computations are orders of magnitude cheaper.
}

void SlidingPuzzleSolver::solve()
{
    // Iterate through the sub-goals.
    for(const auto& subGoal : subGoals)
    {
        // Call aStar on sub-goal. aStar will verify that the remaining board is solvable if the goal state's isEndingSequence is set to true.
        aStar(subGoal);
        // Then lock the tiles.
        for(const auto& targetTileNumber : subGoal.targetTileNumbers)
            lockedTiles.insert(targetTileNumber);
    }
    // Add the final movement to the path.
    model.addSolutionStep(model.getPivotIndex());
}

void SlidingPuzzleSolver::aStar(const SubGoal& goalState)
{
    /*
     * Declare a priority queue, with lowest f(n) = g(n) + h(n) at the top.
     * g(n) = Distance from starting state to current state.
     * h(n) is a combination of the following heuristics:
     * 1. Manhattan distance of target tile to its target location
     *    ALTERNATIVE: Chebyshev distance is the greater of the absolute horizontal and vertical distances multiplied by 2.
     * 2. Linear conflict penalty of remaining board state.
    */
    std::priority_queue<Node, std::vector<Node>, CompareNodes> minHeap;

    // Unordered map stores the states already visited in the current iteration/goal-step.
    std::unordered_map<std::string, Node> visitedStates;

    // Current board.
    std::string currentBoard = model.getCurrentBoard();
    // Current pivot index.
    int pivotIndex = model.getPivotIndex();

    // Initialize starting node, add its state to the visited states, and add it to the priority queue.
    Node startingNode(currentBoard, pivotIndex, 0, 0);
    statesExplored++;
    visitedStates[currentBoard] = startingNode;
    minHeap.push(startingNode);

    // Traverse various states while the heap is not empty.
    while(!minHeap.empty())
    {
        // Retrieve the state with the lowest score and pop it.
        Node currentNode = minHeap.top();
        minHeap.pop();
        // For every target placement and target tile number pair, check if it has been reached.
        for(int i = 0; i < goalState.targetTileNumbers.size(); i++)
        {
            // If the state has been reached, we continue.
            if(currentNode.state[goalState.targetTileNumbers[i]] - 'a' == goalState.targetTileNumbers[i])
            {
                // If the final state is reached, we then check if the shrunken board is solvable if the current board is greater than a 3x3.
                if(i == goalState.targetTileNumbers.size() - 1)
                {
                    // Otherwise, we simply store the steps and return.
                    if(currentBoardN <= 3 || !goalState.isEndingSequence)
                    {
                        addSolutionSteps(currentNode);
                        model.updateBoard(currentNode.state);
                        model.updatePivotIndex(currentNode.pivotSquare);
                        return;
                    }
                    else
                    {
                        // Flatten the remaining state, then pass it into the function.
                        std::string remainingState;
                        std::unordered_set<int> currentTiles;
                        for(int targetTileNumber : goalState.targetTileNumbers)
                        {
                            currentTiles.insert(targetTileNumber);
                        }
                        for(const char& c : currentNode.state)
                        {
                            if(lockedTiles.find(c - 'a') == lockedTiles.end() && currentTiles.find(c - 'a') == currentTiles.end())
                                remainingState += c;
                        }
                        // Odd and even boards have different checks.
                        if(currentBoardN % 2 == 1)
                        {
                            if(isSolvableOdd(remainingState))
                            {
                                currentBoardN--;
                                addSolutionSteps(currentNode);
                                model.updateBoard(currentNode.state);
                                model.updatePivotIndex(currentNode.pivotSquare);
                                return;
                            }
                        }
                        else
                        {
                            if(isSolvableEven(remainingState))
                            {
                                currentBoardN--;
                                addSolutionSteps(currentNode);
                                model.updateBoard(currentNode.state);
                                model.updatePivotIndex(currentNode.pivotSquare);
                                return;
                            }
                        }
                    }
                }
            }
            // Otherwise, we break and move onto the next step.
            else
                break;
        }
        // Traverse the neighboring tiles.
        std::vector<int> validNeighborIndices = model.getValidNeighbors(currentNode.pivotSquare);
        for(const auto& neighbor : validNeighborIndices)
        {
            if(lockedTiles.find(neighbor) == lockedTiles.end())
            {
                std::string currentState = currentNode.state;
                pivotIndex = currentNode.pivotSquare;
                std::swap(currentState[pivotIndex], currentState[neighbor]);
                int heuristic = 0;

                // The first heuristic: the Manhattan distance of the target tile from its target placement.
                // Alternatively, Chebyshev distance can be used: https://www.researchgate.net/publication/301536229_A_Comparative_Study_of_Three_Heuristic_Functions_Used_to_Solve_the_8-Puzzle
                for(int i = 0; i < goalState.targetTileNumbers.size(); i++)
                {
                    if(currentState[i] != 'a' + n * n - 1)
                        heuristic += model.getChebyshevDistance(currentState.find('a' + goalState.targetTileNumbers[i]), goalState.targetTileNumbers[i]) * 2;
                        // heuristic += model.getManhattanDistance(currentState.find('a' + goalState.targetTileNumbers[i]), goalState.targetTileNumbers[i]);
                }

                // The second heuristic: the total linear conflict of the remaining board.
                for(int i = 0; i < n; i++)
                {
                    // Pass in the current row as a flattened string.
                    heuristic += model.getHorizontalLinearConflict(currentState.substr(i * n, n), i);
                    // Pass in the current column as a flattened string.
                    std::string flattenedVertical;
                    for(int j = 0; j < n; j++)
                    {
                        flattenedVertical += currentState[j * n + i];
                    }
                    heuristic += model.getVerticalLinearConflict(flattenedVertical, i);
                }

                // Initialize a new node with the same path, an incremented g-value, and the heuristic we just calculated.
                Node newNode(currentState, neighbor, currentNode.path, currentNode.g + 1, heuristic);

                // If the node's state has not been visited, or has a lower g-value, we continue.
                if(visitedStates.find(currentState) == visitedStates.end() || visitedStates[currentState].g > newNode.g)
                {
                    statesExplored++;
                    // Insert the current pivot index in the path so we can re-trace it later.
                    newNode.path.push(pivotIndex);
                    // Map the current state to the new node/replace the old node with the new one that has a better g-value.
                    visitedStates[currentState] = newNode;
                    // Push it into the heap.
                    minHeap.push(newNode);
                }
            }
        }
    }
}

// Checks if a state is solvable for a given n x n where n is even.
// If even number of columns:
// 1. If the blank row is 1 from the bottom (1-indexed), the number of inversions must be even for it to be solvable.
// 2. If the blank row is 2 from the bottom (1-indexed), the number of inversions must be odd for it to be solvable.
bool SlidingPuzzleSolver::isSolvableEven(const std::string& state) const
{
    int inversions = 0;
    int blankRowFromBottom;
    for(int i = 0; i < state.size(); i++)
    {
        if(state[i] != 'a' + n * n - 1)
        {
            for(int j = i + 1; j < state.size(); j++)
            {
                if(state[j] != 'a' + n * n - 1 && state[i] > state[j])
                    inversions++;
            }
        }
        else
        {
            blankRowFromBottom = currentBoardN - (i / currentBoardN);
        }
    }
    if(blankRowFromBottom % 2 == 1)
        return inversions % 2 == 0;
    return inversions % 2 == 1;
}

// Checks if a state is solvable for a given n x n where n is odd.
// If odd number of columns: the parity MUST be even.
bool SlidingPuzzleSolver::isSolvableOdd(const std::string& state) const {
    int inversions = 0;
    for(int i = 0; i < state.size(); i++)
    {
        if(state[i] != 'a' + n * n - 1)
        {
            for(int j = i + 1; j < state.size(); j++)
            {
                if(state[j] != 'a' + n * n - 1 && state[i] > state[j])
                    inversions++;
            }
        }
    }
    return inversions % 2 == 0;
}

// Add the solution steps to the model. These are used to draw the solution.
void SlidingPuzzleSolver::addSolutionSteps(Node& node)
{
    std::vector<int> steps;
    while(!node.path.empty())
    {
        steps.push_back(node.path.top());
        node.path.pop();
    }
    std::reverse(steps.begin(), steps.end());
    for(const auto& step : steps)
    {
        model.addSolutionStep(step);
    }
}

// Returns the total amount of states explored. Note that states explored does not equal opened states. Rather, this represents the amount of states we stored in the heap.
int SlidingPuzzleSolver::getStatesExplored() const
{
    return statesExplored;
}