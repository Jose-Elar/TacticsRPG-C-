#include "raylib.h"
#include "Constants.h"
#include "Battle.h"

int main(){
    InitWindow(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT, "Tactics RPG");
    SetTargetFPS(GameConfig::TARGET_FPS);

    Battle battle;

    while(!WindowShouldClose()){

        if (battle.isOver() && IsKeyPressed(KEY_R)){
            battle = Battle();
        }

        battle.handleInput();
        battle.update(GetFrameTime());

        BeginDrawing();
        ClearBackground(RAYWHITE);
        battle.draw();
        EndDrawing();
    }

    CloseWindow();
}