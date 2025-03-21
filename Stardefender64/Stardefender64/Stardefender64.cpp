#include "raylib.h"
#include <math.h>
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_BALLOONS 10
#define MAX_TOWERS 5
#define MAX_PROJECTILES 50

typedef struct {
    Vector2 position;
    float speed;
    bool active;
} Balloon;

typedef struct {
    Vector2 position;
    bool active;
} Tower;

typedef struct {
    Vector2 position;
    Vector2 direction;
    float speed;
    bool active;
} Projectile;

Balloon balloons[MAX_BALLOONS];
Tower towers[MAX_TOWERS];
Projectile projectiles[MAX_PROJECTILES];

Vector2 target = { SCREEN_WIDTH - 50, SCREEN_HEIGHT / 2 }; // End goal
int score = 0, lives = 3, wave = 1;

void SpawnBalloon() {
    for (int i = 0; i < MAX_BALLOONS; i++) {
        if (!balloons[i].active) {
            balloons[i].position = (Vector2){ 50, rand() % SCREEN_HEIGHT };
            balloons[i].speed = 1.0f + (wave * 0.2f);
            balloons[i].active = true;
            break;
        }
    }
}

void SpawnProjectile(Vector2 start, Vector2 direction) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!projectiles[i].active) {
            projectiles[i].position = start;
            projectiles[i].direction = direction;
            projectiles[i].speed = 4.0f;
            projectiles[i].active = true;
            break;
        }
    }
}

void UpdateGame() {
    // Update Balloons
    for (int i = 0; i < MAX_BALLOONS; i++) {
        if (balloons[i].active) {
            Vector2 diff = { target.x - balloons[i].position.x, target.y - balloons[i].position.y };
            float length = sqrtf(diff.x * diff.x + diff.y * diff.y);
            Vector2 direction = { diff.x / length, diff.y / length };

            balloons[i].position.x += direction.x * balloons[i].speed;
            balloons[i].position.y += direction.y * balloons[i].speed;

            if (length < 10) { // Reached target
                balloons[i].active = false;
                lives--;
            }
        }
    }

    // Update Projectiles
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            projectiles[i].position.x += projectiles[i].direction.x * projectiles[i].speed;
            projectiles[i].position.y += projectiles[i].direction.y * projectiles[i].speed;

            if (projectiles[i].position.x < 0 || projectiles[i].position.x > SCREEN_WIDTH ||
                projectiles[i].position.y < 0 || projectiles[i].position.y > SCREEN_HEIGHT) {
                projectiles[i].active = false;
            }
        }
    }

    // Check for collisions
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            for (int j = 0; j < MAX_BALLOONS; j++) {
                if (balloons[j].active) {
                    float dist = sqrtf(powf(projectiles[i].position.x - balloons[j].position.x, 2) +
                        powf(projectiles[i].position.y - balloons[j].position.y, 2));
                    if (dist < 10) { // Hit
                        projectiles[i].active = false;
                        balloons[j].active = false;
                        score += 10;
                    }
                }
            }
        }
    }

    // Tower Shooting
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (towers[i].active) {
            for (int j = 0; j < MAX_BALLOONS; j++) {
                if (balloons[j].active) {
                    Vector2 diff = { balloons[j].position.x - towers[i].position.x, balloons[j].position.y - towers[i].position.y };
                    float length = sqrtf(diff.x * diff.x + diff.y * diff.y);
                    if (length < 100) { // Tower range
                        Vector2 direction = { diff.x / length, diff.y / length };
                        SpawnProjectile(towers[i].position, direction);
                        break;
                    }
                }
            }
        }
    }
}

void DrawGame() {
    BeginDrawing();
    ClearBackground(SKYBLUE);

    // Draw path
    DrawRectangle(100, 0, 100, SCREEN_HEIGHT, DARKGRAY);
    DrawRectangle(600, 0, 100, SCREEN_HEIGHT, DARKGRAY);

    DrawCircleV(target, 10, RED);
    for (int i = 0; i < MAX_BALLOONS; i++) {
        if (balloons[i].active) DrawCircleV(balloons[i].position, 10, BLUE);
    }
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (towers[i].active) DrawRectangleV(towers[i].position, (Vector2) { 20, 20 }, DARKGREEN);
    }
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) DrawCircleV(projectiles[i].position, 5, ORANGE);
    }

    DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);
    DrawText(TextFormat("Lives: %d", lives), 10, 40, 20, BLACK);
    DrawText(TextFormat("Wave: %d", wave), 10, 70, 20, BLACK);

    EndDrawing();
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fieldrunner-Style Game");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            for (int i = 0; i < MAX_TOWERS; i++) {
                if (!towers[i].active) {
                    towers[i].position = GetMousePosition();
                    towers[i].active = true;
                    break;
                }
            }
        }

        if (GetRandomValue(0, 100) < 3) SpawnBalloon();
        UpdateGame();
        DrawGame();
    }

    CloseWindow();
    return 0;
}
