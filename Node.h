#ifndef NODE_H
#define NODE_H
#include <stack>
#include <string>
#include <utility>

// The Node struct represents a state of the board.
struct Node
{
    Node() = default;
    Node(std::string state, const int& pivotSquare, const int& g, const int& h) : pivotSquare(pivotSquare), g(g), h(h), state(std::move(state)) {}
    Node(std::string state, const int& pivotSquare, const std::stack<int>& path, const int& g, const int& h) : pivotSquare(pivotSquare), g(g), h(h), state(std::move(state)), path(path) {}
    // The current pivot square of the state.
    int pivotSquare{};
    // The path cost from the starting Node to the current Node.
    int g{};
    // A heuristic/estimate of the cost from the current state to the goal state.
    int h{};
    // Function returns the total estimated cost of the given state.
    [[nodiscard]] int f() const {return g + h;}
    // String representing the state of the board.
    std::string state;
    // Nodes hold the previous pivot indices representing their path in a stack.
    std::stack<int> path;
};

#endif //NODE_H