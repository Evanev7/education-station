#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "raylib.h"

typedef struct Player {
    Vector2 position;
    Vector2 velocity;
    bool grappleExist; // Does the Grapple exist rn? 
    Vector2 grapplePosition; // Where is the grapple? 
    Vector2 mPosition; 
} Player;

typedef struct Cubies {
    int length;
    Rectangle* data;
} Cubies;

const float PLAYER_WIDTH = 40;
const float PLAYER_HEIGHT = 60;
const float LINE_WIDTH = 5;

void DrawPlayer(Player *player) {
    Vector2 pos = (*player).position; 
    DrawRectangle(pos.x-PLAYER_WIDTH/2-1, pos.y-PLAYER_HEIGHT/2-1, PLAYER_WIDTH+2, PLAYER_HEIGHT+2, BLACK);
    DrawRectangleLinesEx(
        (Rectangle) {.x=pos.x-LINE_WIDTH-PLAYER_WIDTH/2, .y=pos.y-PLAYER_HEIGHT/2-LINE_WIDTH, .width = PLAYER_WIDTH+2*LINE_WIDTH, .height = PLAYER_HEIGHT+2*LINE_WIDTH},
        LINE_WIDTH, 
        PINK
    );
    DrawCircle(pos.x, pos.y, 1, RED);
}

const float GRAVITY = 98.696;

/* Player Position, Player Velocity, Gravity
Update Player Position based on the player's velocity and how much time has elapsed since the last update
Update Player velocity based on gravity and how they're currently travelling


If player position = floor height, run "game over"
floor height - 10 < position < floor height + 10
*/
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;
const float FLOOR_HEIGHT = 450;

typedef enum GameState {
    GAME_OK,
    GAME_OVER,
} GameState;

/* Define the function
If player position >< cube  
*/


bool CollidesWith(Player* player, Rectangle object) {
    float x = (*player).position.x;
    float y = (*player).position.y;
    if (x + PLAYER_WIDTH/2 > object.x && x - PLAYER_WIDTH/2 < object.x + object.width) {
        if (y + PLAYER_HEIGHT/2 > object.y && y - PLAYER_HEIGHT/2 < object.y + object.height) {
            return true;
        }
    }
    return false;    
}

bool GrappleLocationCheck(Player* player, Rectangle object){
    float x = (*player).mPosition.x;
    float y = (*player).mPosition.y;
    printf("reached True Check %f %f %f %f \n", object.x, object.y, x, y);
    if (x > object.x && x < object.x + object.width){
        if (y > object.y && y < object.y + object.height){ 
            return true;
        }

    }
    return false;
}

void FireGrapplingHook(Player* player, Cubies cubies) {
    for (int i = 0; i < cubies.length; i++) {
        if (GrappleLocationCheck(player, cubies.data[i])) {
            (*player).grapplePosition.x = (*player).mPosition.x;
            (*player).grapplePosition.y = (*player).mPosition.y;
            (*player).grappleExist = true;
            return;
        }
    }
}

void RetractGrapplingHook (Player* player) {
    (*player).grappleExist = false;
}

GameState Update(float delta, Player* player, Cubies cubies, bool mPressed) {
    if ((*player).position.y > FLOOR_HEIGHT - PLAYER_HEIGHT/2) {
        return GAME_OVER;
    }
    
    for (int i = 0; i < cubies.length; i++) {
        if (CollidesWith(player, cubies.data[i])) {
            return GAME_OVER;
        }    
    }

    if (mPressed) { printf("reached");
        if (!(*player).grappleExist) {;
            FireGrapplingHook(player, cubies);
        }
        else { printf("reached 3");
            RetractGrapplingHook(player); 
        }
    }

    Vector2 iPosition = (*player).position;
    Vector2 iVelocity = (*player).velocity;
    (*player).position = (Vector2) {
        .x = iPosition.x + iVelocity.x * delta, // x, v = x/t, v * t has units of x
        .y = iPosition.y + iVelocity.y * delta,
    };
    (*player).velocity = (Vector2) {
        .x = iVelocity.x,
        .y = iVelocity.y + delta * GRAVITY,
    };
    return GAME_OK;
}


int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Drowning Point");
    SetTargetFPS(60);

    Player* player = &(Player) {
        .position = (Vector2) { 
            .x = 0, 
            .y = 0,
        },
        .velocity = (Vector2) {
            .x = 100,
            .y = -200,
        },
        .grappleExist = false,
        .grapplePosition = (Vector2) {
            .x = 0,
            .y = 0,

        },
        .mPosition = GetMousePosition(),
    };
    Camera2D camera = (Camera2D) {
        .offset = (Vector2) {.x = SCREEN_WIDTH/2, .y = SCREEN_HEIGHT/2},
        .target = (Vector2) {.x = 0, .y = 0},
        .rotation = 0.,
        .zoom = 1.0, 
    };
    Rectangle cubie = (Rectangle) {
        .x = -10,
        .y = 100,
        .height = 20,
        .width = 20,
    };
    Rectangle cubie2 = (Rectangle) {
        .x = 500,
        .y = -200,
        .height = 20,
        .width = 20,
    };
    Cubies cubies = (Cubies) {
        .length = 2,
        .data = malloc(sizeof(Rectangle) * 2),
    };
    cubies.data[0] = cubie;
    cubies.data[1] = cubie2;
    GameState state = GAME_OK;
    while (!WindowShouldClose()) {
        double delta = GetFrameTime();
        bool mPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT); 
        if (state == GAME_OK) {
            (*player).mPosition = GetScreenToWorld2D(GetMousePosition(), camera);
            state = Update(delta, player, cubies, mPressed);
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode2D(camera);
                DrawPlayer(player);
                DrawRectangleRec(cubie, BLACK);
                DrawRectangleRec(cubie2, BLACK);
                if ((*player).grappleExist) {
                    DrawLineEx((*player).position, (*player).grapplePosition, 10, PINK);
                }
            EndMode2D();
            if (state == GAME_OVER) {
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color) {.r = 127, .g = 127, .b = 127, .a = 80});
            }
        EndDrawing();
    }
    free(cubies.data);
    CloseWindow();
    return 0;
}

/* Print an unknown variable to a string buffer
        int len = snprintf(NULL, 0, "%f", delta);
        if (len < 0) {
            exit(len);
        }
        char* res = malloc(len+1);
        snprintf(res, len+1, "%f", delta);
*/
