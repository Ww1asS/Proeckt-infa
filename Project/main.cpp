#include <raylib.h>

int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI); // Поддержка Retina
    InitWindow(800, 450, "Raylib Test");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
