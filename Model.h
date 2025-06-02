#ifndef MODEL_H
#define MODEL_H
#include <unordered_map>
#include <iostream>
#include <string>
#include <random>
#include <vector>

class Model
{
public:
    Model();

    int getHorizontalLinearConflict(const std::string& elements, const int& currentRow) const;
    int getVerticalLinearConflict(const std::string& elements, const int& currentCol) const;
    int getManhattanDistance(const int& startIndex, const int& targetIndex);
    int getChebyshevDistance(const int& startIndex, const int& targetIndex);

    std::vector<int> getValidNeighbors(const int& index);
    const std::vector<int>& getSolutionSteps() const;
    const std::string& getCurrentBoard() const;
    const std::string& getSolution() const;
    int getPivotIndex() const;
    int getN() const;

    void updateBoard(const std::string& newBoard);
    void updatePivotIndex(const int& newIndex);
    void addSolutionStep(const int& step);

private:
    int n;                      // Represents the dimension of the grid (n * n).
    int currentPivotIndex;      // Represents the index/location of the current empty space.
    std::string currentBoard;   // Represents the current board state; Use strings to flatten the array of integers; Works for up to 5x5 boards.
    std::string solution;       // Represents the solution board (goal state).

    std::unordered_map<int, std::vector<int>> validNeighbors;           // Maps a specific pivot index to all its valid neighboring indices.
    std::unordered_map<int, std::vector<int>> manhattanDistances;       // Array containing md's where array[i] represents the tile's md from the ith tile.
    std::unordered_map<int, std::vector<int>> chebyshevDistances;       // Array containing cd's where array[i] represents the tile's cd from the ith tile.
    std::vector<int> solutionSteps;                                     // Vector of integers represents the tile being swapped with.

    void generateValidNeighbors();
    void precomputeManhattanDistances();
    void precomputeChebyshevDistances();
    void initializeBoard();
    void randomizeBoard();
};

#endif //MODEL_H