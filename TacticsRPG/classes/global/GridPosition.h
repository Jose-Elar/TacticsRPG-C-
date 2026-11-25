#pragma once

#include <cstdlib>

struct GridPosition {
    int x;
    int y;

    bool operator==(const GridPosition& other) const {
        return x == other.x && y == other.y;
    }

    int distanceTo(GridPosition other) const { //Number of Grid Gaps between 2 lines ignoring walls
        return std::abs(x - other.x) + std::abs(y - other.y);
    }
};