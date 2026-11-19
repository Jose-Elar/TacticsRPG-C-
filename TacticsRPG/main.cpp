#include <optional>
#include <vector>
#include "raylib.h"
#include "Constants.h"
#include "Grid.h"
#include "Unit.h"
#include "Pathfinding.h"


std::optional<size_t> findUnitAt(const std::vector<Unit>& units, const GridPosition& position) {
    for (size_t i = 0; i < units.size(); ++i) {
        if (units[i].getPosition() == position) {
            return i; // Return the index of the unit found at the position
        }
    }
    return std::nullopt; // Return nullopt if no unit is found at the position
}

std::vector<GridPosition> getUnitPositions (const std::vector<Unit>& units) {
    std::vector<GridPosition> positions;
    for (const Unit& unit : units) {
        positions.push_back(unit.getPosition());
    }
    return positions;
}

int main(){
    InitWindow(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT, "Tactics RPG");
    SetTargetFPS(GameConfig::TARGET_FPS);

    Grid grid(GameConfig::GRID_COLUMNS, GameConfig::GRID_ROWS);

    std::vector<Unit> units;
    units.emplace_back("Knight", Team::PLAYER, GridPosition{2, 4},4);
    units.emplace_back("Archer", Team::PLAYER, GridPosition{2, 6},5);
    units.emplace_back("Goblin", Team::ENEMY, GridPosition{12, 4},4);
    units.emplace_back("Orc", Team::ENEMY, GridPosition{12, 6},3);

    std::optional<size_t> selectedUnit;
    std::vector<GridPosition> reachableTiles;

    const int panelY = GameConfig::GRID_ROWS * GameConfig::TILE_SIZE;

    while (!WindowShouldClose()) {
        std::optional<GridPosition> hoveredTile = grid.tileAt(GetMousePosition()); //Use of optional to handle the case when the mouse is out of bounds

        //Input

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hoveredTile.has_value()) {
            std::optional<size_t> clickedUnit = findUnitAt(units, hoveredTile.value());

            if (clickedUnit.has_value() && units[clickedUnit.value()].getTeam() == Team::PLAYER) {
                selectedUnit = clickedUnit;
                const Unit& unit = units[*selectedUnit];
                reachableTiles = Pathfinding::findReachableTiles(grid, unit.getPosition(), unit.getMovementRange(),getUnitPositions(units));

            } else if (selectedUnit.has_value() && Pathfinding::containsPosition(reachableTiles, *hoveredTile)) {
                units[*selectedUnit].setPosition(*hoveredTile);
                selectedUnit.reset();
                reachableTiles.clear();
            } else {
                selectedUnit.reset();
                reachableTiles.clear();
            }
            
        }

        //PathPreview
        std::vector<GridPosition> pathPreview;
        if (selectedUnit.has_value() && hoveredTile.has_value() && Pathfinding::containsPosition(reachableTiles, *hoveredTile)) {
            pathPreview = Pathfinding::findPath(grid, units[*selectedUnit].getPosition(), *hoveredTile, getUnitPositions(units));
        }

        //Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        grid.draw();


        for (const GridPosition& tile : reachableTiles) {
            grid.drawHighlightedTile(tile, SKYBLUE);
        }

        if (hoveredTile.has_value()) {
            Color highlightColor = grid.isWalkable(*hoveredTile) ? YELLOW : RED;
            grid.drawHighlightedTile(*hoveredTile, highlightColor);
        }

        if (selectedUnit.has_value()) {
            grid.drawHighlightedTile(units[*selectedUnit].getPosition(), GREEN);
        }

        if(!pathPreview.empty()){
            Vector2 previous = grid.tileCenter(units[*selectedUnit].getPosition());
            for (const GridPosition& step : pathPreview) {
                Vector2 current = grid.tileCenter(step);
                DrawLineEx(previous, current, 5.0f, PURPLE);
                previous = current;
            }
        }


        for ( Unit& unit : units) {
            unit.draw(grid);
        }

        if (selectedUnit.has_value()) {
            DrawText(TextFormat("Selected: %s", units[*selectedUnit].getName().c_str()),
                     10, panelY + 10, 20, DARKGRAY);
        } else {
            DrawText("Click a blue unit to select it", 10, panelY + 10, 20, DARKGRAY);
        }

        if (hoveredTile.has_value()) {
            DrawText(TextFormat("Tile: %d, %d", hoveredTile->x, hoveredTile->y),
                     10, panelY + 40, 20, DARKGRAY);
        } else {
            DrawText("Tile: none", 10, panelY + 40, 20, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}