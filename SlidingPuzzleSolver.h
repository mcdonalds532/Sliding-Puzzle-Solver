#ifndef SLIDINGPUZZLESOLVER_H
#define SLIDINGPUZZLESOLVER_H
#include "CompareNodes.h"
#include <unordered_set>
#include <algorithm>
#include "SubGoal.h"
#include "Model.h"
#include <queue>

class SlidingPuzzleSolver
{
public:
    explicit SlidingPuzzleSolver(Model& model);
    int getStatesExplored() const;
    void solve();
private:
    int currentBoardN;                      // The dimensions of the unresolved portion of the current board.
    int statesExplored;                     // The total amount of states explored by A*.
    int n;                                  // The total dimension of the board.

    Model& model;
    std::unordered_set<int> lockedTiles;    // Tile indices stored in this set may not be moved by the algorithm.
    std::vector<SubGoal> subGoals;          // Goal steps are generated and stored.

    bool isSolvableEven(const std::string& state) const;
    bool isSolvableOdd(const std::string& state) const;
    void aStar(const SubGoal& goalState);
    void addSolutionSteps(Node& node);
    void generateSubGoals();
};

#endif //SLIDINGPUZZLESOLVER_H