#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "raylib.h"

typedef struct Input {
    bool mousePressed;
    float swingDirection;
    float grappleReel;
} Input;

typedef struct Player {
    Vector2 position;
    Vector2 velocity;
    Vector2 grapplePosition; // Where is the grapple? 
    Vector2 mPosition;
    float grappleMaxLength;
    bool grappleExist; // Does the Grapple exist rn? 
} Player;

typedef struct Cubies {
    int length;
    Rectangle* data;
} Cubies;

const float PLAYER_WIDTH = 40;
const float PLAYER_HEIGHT = 60;
const float LINE_WIDTH = 5;

const float GRAPPLE_REEL_SPEED = 40;
const float SWING_SPEED = 20;

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

float PythagLength(Vector2 point1, Vector2 point2){
    float a = point1.x - point2.x;
    float b = point1.y - point2.y;
    return sqrtf(a*a + b*b);
}

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
    // printf("reached True Check %f %f %f %f \n", object.x, object.y, x, y);
    if (x > object.x && x < object.x + object.width){
        if (y > object.y && y < object.y + object.height){ 
            return true;
        }

    }
    return false;
}

Vector2 VectorAdd(Vector2 v1, Vector2 v2) {
    return (Vector2) {
        .x = v1.x + v2.x,
        .y = v1.y + v2.y,
    };
}

Vector2 VectorSub(Vector2 v1, Vector2 v2) {
    return (Vector2) {
        .x = v1.x - v2.x,
        .y = v1.y - v2.y,
    };
}

Vector2 VectorMul(Vector2 v1, float scalar) {
    return (Vector2) {
        .x = v1.x * scalar,
        .y = v1.y * scalar,
    };
}

Vector2 VectorDiv(Vector2 v1, float scalar) {
    return (Vector2) {
        .x = v1.x / scalar,
        .y = v1.y / scalar,
    };
}

void FireGrapplingHook(Player* player, Cubies cubies) {
    for (int i = 0; i < cubies.length; i++) {
        if (GrappleLocationCheck(player, cubies.data[i])) {
            (*player).grapplePosition.x = (*player).mPosition.x;
            (*player).grapplePosition.y = (*player).mPosition.y;
            (*player).grappleExist = true;
            (*player).grappleMaxLength = PythagLength((*player).position, (*player).grapplePosition);
            return;
        }
    }
}

void RetractGrapplingHook (Player* player) {
    (*player).grappleExist = false;
}

GameState Update(float delta, Player* player, Cubies cubies, Input input) {
    if ((*player).position.y > FLOOR_HEIGHT - PLAYER_HEIGHT/2) {
        return GAME_OVER;
    }
    
    for (int i = 0; i < cubies.length; i++) {
        if (CollidesWith(player, cubies.data[i])) {
            return GAME_OVER;
        }    
    }

    if (input.mousePressed) {
        if (!(*player).grappleExist) {;
            FireGrapplingHook(player, cubies);
        }
        else {
            RetractGrapplingHook(player); 
        }
    }

    if (input.grappleReel != 0){
        (*player).grappleMaxLength += GRAPPLE_REEL_SPEED * delta * input.grappleReel;
    }


    Vector2 iPosition = (*player).position;
    Vector2 iVelocity = (*player).velocity;
    iVelocity.y += delta * GRAVITY;
    if ((*player).grappleExist) {
        float grappleNextLength = PythagLength(VectorAdd((*player).position, VectorMul(iVelocity, delta)), (*player).grapplePosition);
        if (grappleNextLength >= (*player).grappleMaxLength) {
            
            Vector2 r = VectorSub((*player).position, (*player).grapplePosition);
            r = VectorDiv(r, grappleNextLength);
            float vr = iVelocity.x * r.x + iVelocity.y * r.y;
            r = VectorMul(r, vr);
            iVelocity = VectorSub(iVelocity, r);

            if (input.swingDirection != 0){
                iVelocity.x += SWING_SPEED * delta * input.swingDirection;
            }
        }
    }
    (*player).velocity = iVelocity;
    (*player).position = VectorAdd((*player).position, VectorMul(iVelocity, delta));
    if ((*player).grappleExist) {
        float grappleNextLength = PythagLength((*player).position, (*player).grapplePosition);
        if (grappleNextLength >= (*player).grappleMaxLength) {
            
            Vector2 r = VectorSub((*player).position, (*player).grapplePosition);
            (*player).position = VectorAdd((*player).grapplePosition, VectorMul(r, (*player).grappleMaxLength/grappleNextLength));
        }
    }
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
        float swingDirection = 0.0;
        if (IsKeyDown(KEY_A)) {
            swingDirection -= 1.0;
        }
        if (IsKeyDown(KEY_D)) {
            swingDirection += 1.0;
        }
        float grappleReel = 0.0;
        if (IsKeyDown(KEY_W)) {
            grappleReel -= 1.0;
        }
        if (IsKeyDown(KEY_S)) {
            grappleReel += 1.0;
        }
        Input input = (Input) {
            .mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT),
            .swingDirection = swingDirection,
            .grappleReel = grappleReel,
        }; 
        if (state == GAME_OK) {
            (*player).mPosition = GetScreenToWorld2D(GetMousePosition(), camera);
            state = Update(delta, player, cubies, input);
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
