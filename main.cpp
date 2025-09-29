/**
* Author: Nishat Shamiha
* Assignment: Ping Pong Game
* Date due: 2025-10-11, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/cs3113.h"
#include <raymath.h>

// Global Constants
constexpr int   SCREEN_WIDTH  = 1200,
                SCREEN_HEIGHT = 700,
                FPS           = 60,
                SIZE          = 200;
constexpr float MAX_AMP       = 10.0f;

constexpr Vector2 ORIGIN         = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  BASE_SIZE      = { (float) SIZE, (float) SIZE },
                  BLUE_INIT_POS  = { ORIGIN.x - 550.0f, ORIGIN.y },
                  RED_INIT_POS = { ORIGIN.x + 550.0f, ORIGIN.y },
                  BALL_INIT_POS = { ORIGIN.x, ORIGIN.y },
                  BG_INIT_POS = { ORIGIN.x, ORIGIN.y };

Vector2 gBlueWandPosition  = BLUE_INIT_POS,
        gBlueWandMovement  = { 0.0f, 0.0f },
        gBlueWandScale     = BASE_SIZE,

        gRedWandPosition = RED_INIT_POS,
        gRedWandMovement = { 0.0f, 0.0f },
        gRedWandScale    = BASE_SIZE,

        gBallPosition = BALL_INIT_POS,
        gBallMovement = { 0.0f, 0.0f },
        gBallScale    = BASE_SIZE,

        gBGPosition = BG_INIT_POS,
        gBGMovement = { 0.0f, 0.0f },
        gBGScale    = {SCREEN_WIDTH, SCREEN_HEIGHT},
        
        gBlueSparkPosition = { ORIGIN.x - 540.0f, ORIGIN.y },
        gBlueSparkScale = BASE_SIZE * 2,
        
        gRedSparkPosition = { ORIGIN.x + 540.0f, ORIGIN.y },
        gRedSparkScale = BASE_SIZE * 2;

constexpr char BG[] = "assets/pongbg.png";
constexpr char BLUE_WAND[] = "assets/bluewand.PNG";
constexpr char RED_WAND[] = "assets/redwand.PNG";
constexpr char BALL[] = "assets/spellball.PNG";
constexpr char BLUE_SPARK[] = "assets/blue_sparkle.PNG";
constexpr char RED_SPARK[] = "assets/red_sparkle.PNG";

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gAngle         = 0.0f,
          gPreviousTicks = 0.0f,
          
          gBallAngle = 0.0f,
          gBallRotation = 0.0f,
          
          gSparkTimer = 0.0f,
          sparkDuration = 0.2f; // how long the sparkles sparkle :D

bool      gBlueSparkleShown = false,
          gRedSparkleShown = false;

//variables for textures
Texture2D gBGTexture;
Texture2D gBlueWandTexture;
Texture2D gRedWandTexture;
Texture2D gBallTexture;
Texture2D gBlueSparkTexture;
Texture2D gRedSparkTexture;

Rectangle gBGArea;
Rectangle gBlueWandArea;
Rectangle gRedWandArea;
Rectangle gBallArea;
Rectangle gBlueSparkArea;
Rectangle gRedSparkArea;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, 
                 const Vector2 *positionB, const Vector2 *scaleB);

// Function Definitions
bool isColliding(const Vector2 *positionA,  const Vector2 *scaleA, 
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) - ((scaleA->y + scaleB->y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

void renderObject(const Texture2D *texture, const Vector2 *position, 
                  const Vector2 *scale)
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        *texture, 
        textureArea, destinationArea, originOffset,
        gAngle, WHITE
    );
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 2 - Wizard Pong");

    gBGTexture = LoadTexture(BG);
    gBlueWandTexture = LoadTexture(BLUE_WAND);
    gRedWandTexture  = LoadTexture(RED_WAND);
    gBallTexture = LoadTexture(BALL);
    gBlueSparkTexture = LoadTexture(BLUE_SPARK);
    gRedSparkTexture  = LoadTexture(RED_SPARK);

    gBlueWandScale = {(float)gBlueWandTexture.width * 0.5f, (float)gBlueWandTexture.height * 0.5f};
    gRedWandScale = {(float)gRedWandTexture.width * 0.5f, (float)gRedWandTexture.height * 0.5f};
    gBallScale = {(float)gBallTexture.width * 0.5f, (float)gBallTexture.height * 0.5f};

    gSparkTimer = 0.0f;

    gBallMovement = {-1.0f, -0.5f};
    gBallMovement = Vector2Normalize(gBallMovement);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;

    gBlueWandMovement = {0.0f, 0.0f};
    gRedWandMovement = {0.0f, 0.0f};

    // controls for the blue wand
    if (IsKeyDown(KEY_W)) gBlueWandMovement.y = -1.0f;
    if (IsKeyDown(KEY_S)) gBlueWandMovement.y =  1.0f;

    // controls for the red wand
    if (IsKeyDown(KEY_UP)) gRedWandMovement.y = -1.0f;
    if (IsKeyDown(KEY_DOWN)) gRedWandMovement.y =  1.0f;

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    // delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    gBlueWandMovement = {0.0f, 0.0f};
    gRedWandMovement = {0.0f, 0.0f};

    // blue movemnet
    if (IsKeyDown(KEY_W)) gBlueWandMovement.y = -1.0f;
    if (IsKeyDown(KEY_S)) gBlueWandMovement.y =  1.0f;

    // red movemnet
    if (IsKeyDown(KEY_UP)) gRedWandMovement.y = -1.0f;
    if (IsKeyDown(KEY_DOWN)) gRedWandMovement.y =  1.0f;

    // normalize
    if (Vector2Length(gBlueWandMovement) > 1.0f){
        gBlueWandMovement = Vector2Normalize(gBlueWandMovement);
    }
    if (Vector2Length(gRedWandMovement) > 1.0f){
        gRedWandMovement = Vector2Normalize(gRedWandMovement);
    }

    float wandSpeed = 300.0f;

    gBlueWandPosition.y += gBlueWandMovement.y * wandSpeed * deltaTime;
    gRedWandPosition.y  += gRedWandMovement.y  * wandSpeed * deltaTime;

    // ball movement
    if (Vector2Length(gBallMovement) == 0.0f) {
        gBallMovement = { -1.0f, 0.5f }; // mvoe left at start
        gBallMovement = Vector2Normalize(gBallMovement);
    }

    float ballSpeed = 400.0f;
    gBallPosition.x += gBallMovement.x * ballSpeed * deltaTime;
    gBallPosition.y += gBallMovement.y * ballSpeed * deltaTime;

    // ball hitting out of bounds - top or bottom
    if (gBallPosition.y - gBallScale.y / 2 < 0 || gBallPosition.y + gBallScale.y / 2 > SCREEN_HEIGHT) {
        gBallMovement.y *= -1; // move opposite way vertically
    }
    // ball hitting out of bounds - left or right
    if (gBallPosition.x - gBallScale.x / 2 < 0 || gBallPosition.x + gBallScale.x / 2 > SCREEN_WIDTH) {
        gBallPosition = BALL_INIT_POS;
        gBallMovement = { 0.0f, 0.0f };
        gBlueSparkleShown = false;
        gRedSparkleShown = false;
    }

    // collide with blue wand
    if (isColliding(&gBallPosition, &gBallScale, &gBlueWandPosition, &gBlueWandScale)) {
        gBallMovement.x = fabs(gBallMovement.x); // ball move right
        gBlueSparkleShown = true;
        gSparkTimer = 0.0f;
        gBlueSparkPosition = { gBlueWandPosition.x + 10.0f, gBlueWandPosition.y };;
    }

    // collide with red wand
    if (isColliding(&gBallPosition, &gBallScale, &gRedWandPosition, &gRedWandScale)) {
        gBallMovement.x = -fabs(gBallMovement.x); // ball move left
        gRedSparkleShown = true;
        gSparkTimer = 0.0f;
        gRedSparkPosition = gRedWandPosition;
    }

    // sparkles
    gSparkTimer += deltaTime;
    if (gSparkTimer >= sparkDuration) {
        gBlueSparkleShown = false;
        gRedSparkleShown = false;
    }

    //TODO
    // - limit the paddles from going out of bounds
    // - add multiple balls
    // - keep score
    // - add randomization to ball going in either direction
    // - increase difficulty

}

void render()
{
    BeginDrawing();
    ClearBackground(BLACK); // def to black if bg no work

    renderObject(&gBGTexture, &gBGPosition, &gBGScale); // background
    renderObject(&gBlueWandTexture, &gBlueWandPosition, &gBlueWandScale); // blue wand
    renderObject(&gRedWandTexture, &gRedWandPosition, &gRedWandScale); // red wand
    renderObject(&gBallTexture, &gBallPosition, &gBallScale); // ball

    if (gBlueSparkleShown){ // blue sparkles
        renderObject(&gBlueSparkTexture, &gBlueSparkPosition, &gBlueSparkScale);
    }

    if (gRedSparkleShown){ // red sparkles
        renderObject(&gRedSparkTexture, &gRedSparkPosition, &gRedSparkScale);
    }

    EndDrawing();
}

void shutdown() { CloseWindow(); }

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}