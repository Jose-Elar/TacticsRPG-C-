#pragma once

#include <vector>
#include "GridPosition.h"
#include "raylib.h"



enum class TileType {
    FLOOR,
    WALL
};

class Grid {
    public:
        Grid(int colums, int rows);

        void draw() ;
        void drawHighlightedTile(const GridPosition& position, Color color) ;

        bool isInBounds(GridPosition position);
        bool isWalkable(GridPosition position);

        GridPosition screenToGridPosition(Vector2 screenPosition);
        Vector2 gridToScreenPosition(GridPosition gridPosition);


    private:
        size_t toIndex(const GridPosition& position) ;

        std::vector<TileType> m_tiles;
        int m_columns;
        int m_rows;
};