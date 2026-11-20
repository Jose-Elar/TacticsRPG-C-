#include "raylib.h"
#include "Constants.h"
#include "Battle.h"

int main(){
    InitWindow(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT, "Tactics RPG");
    SetTargetFPS(GameConfig::TARGET_FPS);

    Battle battle;

    while(!WindowShouldClose()){
        float deltaTime = GetFrameTime();

        battle.handleInput();
        battle.update(deltaTime);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        battle.draw();
        EndDrawing();
    }

    CloseWindow();
}