#include "raylib.h"
#include <vector>
#include <string>

//ope
using namespace std;

//Handle menu systems
enum class GameState {
    Menu,
    Play,
    Pause
};

int main()
{
    InitWindow(800, 600, "Pong");
    SetWindowState(FLAG_VSYNC_HINT);
    SetTargetFPS(60);

    /*
        The same thing as an abstract class.
        Just provides an interface saying that derived classes need these methods implemented.
    */
    class Entity {
    public:
        Vector2 pos = Vector2();
        virtual void Draw() = 0;
        virtual void Update(float dt) = 0;

        virtual ~Entity() {}
    };
    
    /*
    * Create Paddle class
    */
    class Paddle : public Entity
    {
    private:
        int paddleSpeed = 100;
        int playerNum;
    public:
        int height = 100;
        int width = 10;
        Rectangle CollisionBounds;
        void Update(float dt) override
        {
            Move(dt);
        }

        void Move(float dt)
        {
            //Confine to bounds.
            if (pos.y < 0)
                pos.y = 1;
            if (pos.y + height > GetScreenHeight())
                pos.y = GetScreenHeight() - height - 1;
            
            if (playerNum == 1)
                pos.y += (IsKeyDown(KEY_S) - IsKeyDown(KEY_W)) * paddleSpeed * dt;
            else
                pos.y += (IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP)) * paddleSpeed * dt;
            
            //Map controllers
            if (IsGamepadAvailable(playerNum))
                pos.y += GetGamepadAxisMovement(playerNum, GAMEPAD_AXIS_LEFT_Y) * paddleSpeed * dt;

            CollisionBounds = { pos.x, pos.y, (float)width, float(height) };
        }

        void Draw() override
        {
            DrawRectangle(
                pos.x,
                pos.y,
                width,
                height,
                WHITE
            );
        }

        Paddle(Vector2 _paddlePos, int _player)
        {
            pos = _paddlePos;
            playerNum = _player;
            CollisionBounds = {pos.x, pos.y, (float)width, (float)height};
        }
    };


    class Ball : public Entity {

    private:
        int ballRadius;
        Vector2 ballSpeed{ 150, 150 };
    public:
        void Update(float dt) override
        {
            pos.x += ballSpeed.x * dt;
            pos.y += ballSpeed.y * dt;

            if (pos.y > GetScreenHeight() || pos.y <= 0)
            {
                ballSpeed.y *= -1;
            }
            else if (pos.x > GetScreenWidth() || pos.x <= 0)
            {
                ballSpeed.x *= -1;
            }
        }

        void ChangeDirection()
        {
            ballSpeed.x *= -1;
        }

        void CheckCollision(vector<Entity*> &entities)
        {
            for (const auto& ent2 : entities)
            {
                if (auto paddle = dynamic_cast<Paddle*>(ent2))
                {
                    if (CheckCollisionCircleRec(pos, ballRadius, paddle->CollisionBounds))
                    {
                        ChangeDirection();
                    }
                }
            }
        }

        void Draw() override
        {
            DrawCircle(
                pos.x,
                pos.y,
                ballRadius,
                WHITE
            );
        }

        Ball(Vector2 _ballPos, int _ballRadius)
        {
            pos = _ballPos;
            ballRadius = _ballRadius;
        }
    };

    vector<Entity*> entities;

    auto ball = new Ball(
        Vector2{
                static_cast<float>(GetScreenWidth()) / 2,
                static_cast<float>(GetScreenHeight()) / 2
        },
        5
    );

    auto p1 = new Paddle(
        Vector2{
            50.f,
            static_cast<float>((GetScreenHeight() / 2) - 50),
        },
        1
    );

    auto p2 = new Paddle(
        Vector2{
            static_cast<float>(GetScreenWidth() - 40),
            static_cast<float>(GetScreenHeight() / 2 - 50),
        },
        2
    );

    entities.push_back(ball);
    entities.push_back(p1);
    entities.push_back(p2);

    GameState gameState = GameState::Menu;

    while (!WindowShouldClose())
    {

        float dt = GetFrameTime();

        string startText = "Press Start to Begin Play";
        int startTextWidth = MeasureText(startText.c_str(), 30) /2;

        string pauseText = "Press Start to Resume";
        int pauseTextWidth = MeasureText(startText.c_str(), 30) / 2;

        BeginDrawing();
            ClearBackground(BLACK);

            switch (gameState)
            {
            case GameState::Menu:
                DrawText(startText.c_str(), (GetScreenWidth() / 2) - startTextWidth, GetScreenHeight() / 2, 30, WHITE);
                if (IsKeyDown(KEY_ENTER) || IsKeyDown(GAMEPAD_BUTTON_MIDDLE_RIGHT))
                {
                    gameState = GameState::Play;
                }
                break;
            case GameState::Play: //Draw gameplay
                for (const auto& ent : entities)
                {
                    ent->Update(dt);
                    ent->Draw();

                    //Syntax to get the ball and cast it to a variable if it's compatible.
                    if (auto ball = dynamic_cast<Ball*>(ent))
                    {
                        ball->CheckCollision(entities);
                    }
                }

                //Show pause screen
                if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(GAMEPAD_BUTTON_MIDDLE_RIGHT))
                {
                    gameState = GameState::Pause;
                }

                break;
            case GameState::Pause:
                //Draw Pause Menu
                DrawText(pauseText.c_str(), (GetScreenWidth() / 2) - pauseTextWidth, GetScreenHeight() / 2, 30, WHITE);
                if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(GAMEPAD_BUTTON_MIDDLE_RIGHT))
                {
                    gameState = GameState::Play;
                }
                break;
            default:
                break;
            }
            
        EndDrawing();
    }

    for (const auto& ent : entities)
    {
        delete ent;
    }

    CloseWindow();
}