#pragma once

namespace GameConfig {
    const int TILE_SIZE = 48;
    const int GRID_COLUMNS = 16;
    const int GRID_ROWS = 12;

    const int UI_PANEL_HEIGHT = 120;

    const int WINDOW_WIDTH = TILE_SIZE * GRID_COLUMNS;
    const int WINDOW_HEIGHT = TILE_SIZE * GRID_ROWS + UI_PANEL_HEIGHT;

    const int TARGET_FPS = 60;
}