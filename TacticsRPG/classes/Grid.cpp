#include "Grid.h"
#include "Constants.h"
#include <cmath>

Grid::Grid(int columns, int rows): m_columns(columns),m_rows(rows),m_tiles(static_cast<size_t>(columns * rows), TileType::FLOOR) {
    for (int y = 3; y < 9; y++) {
        m_tiles[toIndex(GridPosition{7, y})] = TileType::WALL;
    }
}

size_t Grid::toIndex(const GridPosition& position) {
    return static_cast<size_t>(position.y * m_columns + position.x);
}

bool Grid::isInBounds(GridPosition position) {
    return position.x >= 0 && position.x < m_columns && position.y >= 0 && position.y < m_rows;
}

bool Grid::isWalkable(GridPosition position) {
    if (!isInBounds(position)) {
        return false;
    }
    return m_tiles[toIndex(position)] == TileType::FLOOR;
}

GridPosition Grid::screenToGridPosition(Vector2 screenPosition) {
    int x = static_cast<int>(std::floor(screenPosition.x / GameConfig::TILE_SIZE));
    int y = static_cast<int>(std::floor(screenPosition.y / GameConfig::TILE_SIZE));
    return GridPosition{x, y};
}

Vector2 Grid::gridToScreenPosition(GridPosition gridPosition) {
    float x = static_cast<float>(gridPosition.x * GameConfig::TILE_SIZE);
    float y = static_cast<float>(gridPosition.y * GameConfig::TILE_SIZE);
    return Vector2{x, y};
}

void Grid::draw() {
    for ( int y = 0; y < m_rows; y++) {
        for ( int x = 0 ; x < m_columns; x++) {
            GridPosition position{x,y};
            Vector2 screenPosition = gridToScreenPosition(position);
            Color fill = (m_tiles[toIndex(position)] == TileType::FLOOR) ? LIGHTGRAY : DARKGRAY;
            DrawRectangle(static_cast<int>(screenPosition.x), static_cast<int>(screenPosition.y), GameConfig::TILE_SIZE, GameConfig::TILE_SIZE, fill);
            DrawRectangleLines(static_cast<int>(screenPosition.x), static_cast<int>(screenPosition.y), GameConfig::TILE_SIZE, GameConfig::TILE_SIZE, GRAY);
        }
    }
}

void Grid::drawHighlightedTile(const GridPosition& position, Color color) {
    if (!isInBounds(position)) {
        return;
    }
    Vector2 screenPosition = gridToScreenPosition(position);
    DrawRectangle(static_cast<int>(screenPosition.x), static_cast<int>(screenPosition.y), GameConfig::TILE_SIZE, GameConfig::TILE_SIZE, color);
    DrawRectangleLines(static_cast<int>(screenPosition.x), static_cast<int>(screenPosition.y), GameConfig::TILE_SIZE, GameConfig::TILE_SIZE, GRAY);
}