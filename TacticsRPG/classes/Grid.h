#pragma once
#include <optional>
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

        bool isInBounds(GridPosition position) const;
        bool isWalkable(GridPosition position) const ;

        int getColumns() const { return m_columns; }
        int getRows() const { return m_rows; }

        GridPosition screenToGridPosition(Vector2 screenPosition) const ;
        Vector2 gridToScreenPosition(GridPosition gridPosition) const ;
        std::optional<GridPosition> tileAt(Vector2 screenPosition) const ;
        Vector2 tileCenter(GridPosition position) const ;



    private:
        size_t toIndex(const GridPosition& position) const ;

        std::vector<TileType> m_tiles;
        int m_columns;
        int m_rows;
};