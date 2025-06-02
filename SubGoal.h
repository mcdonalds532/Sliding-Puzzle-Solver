#ifndef SUBGOAL_H
#define SUBGOAL_H
#include <vector>

struct SubGoal
{
    SubGoal();
    // Takes in the target tile numbers of the sub-goal. In any iteration larger than 3x3, these tiles are the top row and left column of an n x n subgrid.
    explicit SubGoal(const std::vector<int>& targetTileNumbers) : targetTileNumbers(targetTileNumbers), isEndingSequence(false) {}
    std::vector<int> targetTileNumbers;
    // This will be used to communicate to the solver to check if the remaining n x n sub-board is solvable.
    bool isEndingSequence;
};

#endif //SUBGOAL_H