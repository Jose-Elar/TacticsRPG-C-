#pragma once

#include <vector>
#include "GridPosition.h"

class Grid;

namespace Pathfinding {
    bool containsPosition(const std::vector<GridPosition>& positions, GridPosition position);

    std::vector<GridPosition> findReachableTiles( const Grid& grid, GridPosition start, int range, const std::vector<GridPosition>& blockedTiles);

    std::vector<GridPosition> findPath(const Grid& grid, GridPosition start, GridPosition goal, const std::vector<GridPosition>& blockedTiles);
}