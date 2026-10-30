#include "raylib.h"
#include "Constants.h"
#include "Grid.h"


int main(){
    InitWindow(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT, "Tactics RPG");
    SetTargetFPS(GameConfig::TARGET_FPS);

    Grid grid(GameConfig::GRID_COLUMNS, GameConfig::GRID_ROWS);

    while (!WindowShouldClose()) {
        GridPosition mouseGridPosition = grid.screenToGridPosition(GetMousePosition());

        BeginDrawing();
        ClearBackground(RAYWHITE);

        grid.draw();

        Color highlightColor = grid.isInBounds(mouseGridPosition) ? YELLOW : RED;
        grid.drawHighlightedTile(mouseGridPosition, highlightColor);

        DrawText(TextFormat("Mouse Grid Position: (%d, %d)", mouseGridPosition.x, mouseGridPosition.y), 10, GameConfig::WINDOW_HEIGHT - 110, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}