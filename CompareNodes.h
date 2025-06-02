#ifndef COMPARENODES_H
#define COMPARENODES_H
#include "Node.h"

// Used to determine priority of the Nodes stored in the aStar algorithm.
// The Node with the lowest f-value is at the top of the min-heap.
struct CompareNodes
{
    bool operator()(Node& a, Node& b) const
    {
        return a.f() > b.f();
    }
};

#endif //COMPARENODES_H