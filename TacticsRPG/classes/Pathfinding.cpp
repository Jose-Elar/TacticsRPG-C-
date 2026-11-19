#include "Pathfinding.h"
#include "Grid.h"
#include <algorithm>
#include <queue>

static const GridPosition DIRECTIONS[] =
    {
        GridPosition{0, -1}, // Up
        GridPosition{0, 1},  // Down
        GridPosition{-1, 0}, // Left
        GridPosition{1, 0}   // Right
};

static size_t toIndex(const Grid &grid, const GridPosition &position)
{
    return static_cast<size_t>(position.y * grid.getColumns() + position.x);
}

static GridPosition positionOf(const Grid &grid, size_t index){
    size_t colums = static_cast<size_t>(grid.getColumns());
    return GridPosition{static_cast<int>(index % colums), static_cast<int>(index / colums)};
}

static int heuristic (GridPosition a, GridPosition b) { //Absolute value of the difference between the two positions, used for A* pathfinding
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

namespace Pathfinding
{
    bool containsPosition(const std::vector<GridPosition> &positions, GridPosition position)
    {
        return std::find(positions.begin(), positions.end(), position) != positions.end();
    }

    std::vector<GridPosition> findReachableTiles(const Grid &grid, GridPosition start, int range, const std::vector<GridPosition> &blockedTiles)
    {

        std::vector<GridPosition> reachable;
        std::vector<int> distance(static_cast<size_t>(grid.getColumns() * grid.getRows()), -1);
        std::queue<GridPosition> frontier;

        distance[toIndex(grid, start)] = 0;
        frontier.push(start);

        while (!frontier.empty())
        {
            GridPosition current = frontier.front();
            frontier.pop();

            int currentDistance = distance[toIndex(grid, current)];
            if (currentDistance == range)
            { // Max Range Reached
                continue;
            }

            for (const GridPosition &direction : DIRECTIONS)
            { // Look at all directions from this space
                GridPosition next{current.x + direction.x, current.y + direction.y};

                // All Checks of occupied Tiles
                if (!grid.isWalkable(next))
                {
                    continue;
                }
                if (containsPosition(blockedTiles, next))
                {
                    continue;
                }
                // If its already been visited
                size_t nextIndex = toIndex(grid, next);
                if (distance[nextIndex] != -1)
                {
                    continue;
                }

                // New reachable tile found
                distance[nextIndex] = currentDistance + 1;
                frontier.push(next);
                reachable.push_back(next);
            }
        }
        return reachable;
    }

    std::vector<GridPosition> findPath(const Grid& grid, GridPosition start, GridPosition goal, const std::vector<GridPosition>& blockedTiles) {

        if((start == goal) || !grid.isWalkable(goal) || containsPosition(blockedTiles, goal)){ //Basic Cases that instantly return an empty path
            return {};
        }

        using Entry = std::pair<int, size_t>; //Pair of cost and position
        std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> open; // Min-heap priority queue for A* search

        const size_t tileCount = static_cast<size_t>(grid.getColumns() * grid.getRows());
        std::vector<int> gScore(tileCount, -1); // Cost from start to this tile, starts a s -1 to indicate unvisited
        std::vector<GridPosition> cameFrom(tileCount, start);

        gScore[toIndex(grid, start)] = 0;
        open.push(Entry{heuristic(start, goal), toIndex(grid, start)}); //Push the start tile into the open set with its heuristic cost

        while(!open.empty()){
            Entry entry = open.top();//Copy the top entry from the priority queue
            open.pop(); //Remove the top entry from the priority queue

            size_t currentIndex = entry.second; //Get the index of the current tile
            GridPosition current = positionOf(grid, currentIndex); //Convert the index back to a GridPosition

            if (current == goal){//Reconstruct path, as goal reached
                std::vector<GridPosition> path;
                GridPosition step = goal; //Start from the goal and work backwards to the start
                while (step != start) {
                    path.push_back(step);
                    step = cameFrom[toIndex(grid, step)];
                }
                std::reverse(path.begin(), path.end()); //Reverse the path to get it from start to goal
                return path;
            }

            if(entry.first > gScore[currentIndex] + heuristic(current, goal)){ //If the current entry's cost is greater than the actual cost to reach this tile, skip it
                continue;
            }
            for (const GridPosition& direction : DIRECTIONS) { //Check all directions as in BFS
                GridPosition next{current.x + direction.x, current.y + direction.y};

                if (!grid.isWalkable(next) || containsPosition(blockedTiles, next)) {
                    continue;
                }

                size_t nextIndex = toIndex(grid, next);
                int newCost = gScore[currentIndex] + 1; //Cost to reach the next tile is the cost to reach the current tile + 1

                if(gScore[nextIndex] == -1 || newCost < gScore[nextIndex]){ //If the next tile has not been visited or the new cost is lower than the previous cost
                    gScore[nextIndex] = newCost; //Update the cost to reach the next tile
                    cameFrom[nextIndex] = current; //Update the path to reach the next tile
                    open.push(Entry{newCost + heuristic(next, goal), nextIndex}); //Push the next tile into the open set with its total cost (g + h)
                }
            }
        

        }
        return {}; //Return an empty path if no path is found
    }

}


