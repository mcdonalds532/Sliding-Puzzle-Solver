#include "Model.h"

// Make sure manually change the dimensions of n based on the chosen n.
Model::Model() : n(4)
{
    initializeBoard();
    currentPivotIndex = n * n - 1;
    randomizeBoard();
    precomputeManhattanDistances();
    precomputeChebyshevDistances();
}

// Initializes the board state at the solution to ensure the board will be solvable.
void Model::initializeBoard()
{
    char c = 'a';
    std::string board;
    for(int i = 0; i < n * n; i++)
    {
        board += c;
        c++;
    }
    currentBoard = board;
    solution = board;
}

// Shuffles the board using the <random> library.
void Model::randomizeBoard()
{
    // Using <random> library to produce a unique board per instantiation.
    std::random_device randomDevice;
    std::mt19937 mersenneTwisterEngine(randomDevice());
    // Will randomize the board starting from the solution to ensure the puzzle is solvable.
    generateValidNeighbors();
    // Randomly moving 250 times from the solution to begin.
    for(int i = 0; i < 250; i++)
    {
        std::vector<int> possibleSwaps = validNeighbors[currentPivotIndex];
        const int range = static_cast<int>(possibleSwaps.size());
        std::uniform_int_distribution distribution(0, range - 1);
        const int randomNumber = distribution(mersenneTwisterEngine);
        const int randomSwapIndex = possibleSwaps[randomNumber];
        std::swap(currentBoard[currentPivotIndex], currentBoard[randomSwapIndex]);
        currentPivotIndex = randomSwapIndex;
    }
}

// Generates all valid indexed moves for a pivot index and stores it in validNeighbors.
void Model::generateValidNeighbors()
{
    for(int i = 0; i < n * n; i++)
    {
        if(i - n >= 0)
            validNeighbors[i].push_back(i - n);
        if(i + n < n * n)
            validNeighbors[i].push_back(i + n);
        if(i % n == 0)
            validNeighbors[i].push_back(i + 1);
        else if(i % n == n - 1)
            validNeighbors[i].push_back(i - 1);
        else
        {
            validNeighbors[i].push_back(i + 1);
            validNeighbors[i].push_back(i - 1);
        }
    }
}

/*
 * Precomputes the Manhattan distance (md) from all indices to all target locations./
 * md = abs(targetRow - currentRow) + abs(targetColumn - currentColumn)
 * That is, the distance between two tiles when only horizontal and vertical movements are permitted.
 */
void Model::precomputeManhattanDistances()
{
    for(int i = 0; i < n * n; i++)
    {
        manhattanDistances[i] = std::vector<int>(n * n);
        const int currentRow = i / n;
        const int currentColumn = i % n;
        for(int j = 0; j < n * n; j++)
        {
            const int targetRow = j / n;
            const int targetColumn = j % n;
            manhattanDistances[i][j] = abs(targetRow - currentRow) + abs(targetColumn - currentColumn);
        }
    }
}

/*
 * Precomputes the Chebyshev distance (md) from all indices to all target locations.
 * cd = std::max(abs(targetRow - currentRow), abs(targetColumn - currentColumn))
 * That is, the larger of the vertical and horizontal distances between two tiles.
 */
void Model::precomputeChebyshevDistances()
{
    for(int i = 0; i < n * n; i++)
    {
        chebyshevDistances[i] = std::vector<int>(n * n);
        const int currentRow = i / n;
        const int currentColumn = i % n;
        for(int j = 0; j < n * n; j++)
        {
            const int targetRow = j / n;
            const int targetColumn = j % n;
            chebyshevDistances[i][j] = std::max(abs(targetRow - currentRow), abs(targetColumn - currentColumn));
        }
    }
}

// Input: The starting index of the tile to be checked and the target index of destination/goal tile.
// Output: The Manhattan distance from starting tile to target tile.
int Model::getManhattanDistance(const int& startIndex, const int& targetIndex)
{
    return manhattanDistances[startIndex][targetIndex];
}

// Input: The starting index of the tile to be checked and the target index of destination/goal tile.
// Output: The Chebyshev distance from starting tile to target tile.
int Model::getChebyshevDistance(const int& startIndex, const int& targetIndex)
{
    return chebyshevDistances[startIndex][targetIndex];
}

/*
 * Precomputing permutations for linear conflict (lc) would very costly.
 * Definition: Two tiles i and j are in linear conflict if i and j are both in the same line, the goal positions of i and j are both in that line,
 * the position of i is to the right of j, and the goal position of j is to the left of i.
 * Source: https://mice.cs.columbia.edu/getTechreport.php?techreportID=1026&format=pdf&
 * lc = (numberOfLinearConflicts) * 2
 * Input: The row to be checked and the substring representing the elements of the row.
 * Output: The total linear conflict penalty assigned to the given state;
 */
int Model::getHorizontalLinearConflict(const std::string& elements, const int& currentRow) const
{
    int linearConflict = 0;
    std::string candidates;
    for(int i = 0; i < n; i++)
    {
        const int tileNumber = elements[i] - 'a';
        // If we are not checking the empty tile, then push the tile into its destination row.
        if(tileNumber != n * n - 1 && tileNumber / n == currentRow)
        {
            candidates += elements[i];
        }
    }

    // While the current string contains 2 or more elements, we greedily remove the element with most conflicts.
    // We check every destination row iteratively.
    // If there are two or more candidates for a row, we check for conflict.
    while(candidates.size() >= 2)
    {
        int indexToBeRemoved = -1;
        int maxConflict = 0;
        for(int i = 0; i < candidates.size(); i++)
        {
            int currentConflict = 0;
            for(int j = i + 1; j < candidates.size(); j++)
            {
                if(candidates[j] < candidates[i])
                {
                    currentConflict++;
                }
            }
            // If current conflict is greater than the current conflict, then we update the index to be removed.
            if(currentConflict > maxConflict)
            {
                maxConflict = currentConflict;
                indexToBeRemoved = i;
            }
        }
        // If the index to be removed is NOT at its default value, then we erase the index and increment the linear conflict count.
        if(indexToBeRemoved != -1)
        {
            candidates.erase(candidates.begin() + indexToBeRemoved);
            linearConflict++;
        }
        // Otherwise, we break out of the loop.
        else
            break;
    }
    // Add a penalty of 2 per linear conflict.
    return linearConflict * 2;
}

int Model::getVerticalLinearConflict(const std::string& elements, const int& currentCol) const
{
    int linearConflict = 0;
    std::string candidates;

    for(int i = 0; i < n; i++)
    {
        const int tileNumber = elements[i] - 'a';
        // If we are not checking the empty tile, then push the tile into its destination column.
        if(tileNumber != n * n - 1 && tileNumber % n == currentCol)
        {
            candidates += elements[i];
        }
    }

    // While the current string contains 2 or more elements, we greedily remove the element with most conflicts.
    // We check every destination column iteratively.
    while(candidates.size() >= 2)
    {
        int indexToBeRemoved = -1;
        int maxConflict = 0;
        for(int i = 0; i < candidates.size(); i++)
        {
            int currentConflict = 0;
            for(int j = i + 1; j < candidates.size(); j++)
            {
                if(candidates[j] < candidates[i])
                {
                    currentConflict++;
                }
            }
            // If current conflict is greater than the current conflict, then we update the index to be removed.
            if(currentConflict > maxConflict)
            {
                maxConflict = currentConflict;
                indexToBeRemoved = i;
            }
        }
        // If the index to be removed is NOT at its default value, then we erase the index and increment the linear conflict count.
        if(indexToBeRemoved != -1)
        {
            candidates.erase(candidates.begin() + indexToBeRemoved);
            linearConflict++;
        }
        // Otherwise, we break out of the loop.
        else
            break;
    }
    // Add a penalty of 2 per linear conflict.
    return linearConflict * 2;
}

const std::string& Model::getCurrentBoard() const
{
    return currentBoard;
}

int Model::getN() const
{
    return n;
}

int Model::getPivotIndex() const
{
    return currentPivotIndex;
}

std::vector<int> Model::getValidNeighbors(const int& index)
{
    return validNeighbors[index];
}

void Model::updateBoard(const std::string& newBoard)
{
    currentBoard = newBoard;
}

void Model::updatePivotIndex(const int& newIndex)
{
    currentPivotIndex = newIndex;
}

void Model::addSolutionStep(const int& step)
{
    solutionSteps.push_back(step);
}

const std::vector<int>& Model::getSolutionSteps() const
{
    return solutionSteps;
}

const std::string& Model::getSolution() const
{
    return solution;
}