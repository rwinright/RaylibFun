#include "raylib.h"
#include <vector>

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
        void Update(float dt) override
        {
            Move(dt);
        }

        void Move(float dt)
        {
            if (playerNum == 1)
                pos.y += (IsKeyDown(KEY_S) - IsKeyDown(KEY_W)) * paddleSpeed * dt;
            else
                pos.y += (IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP)) * paddleSpeed * dt;
            
            //Map controllers
            if (IsGamepadAvailable(playerNum))
                pos.y += GetGamepadAxisMovement(playerNum, GAMEPAD_AXIS_LEFT_Y) * paddleSpeed * dt;
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
                    if (pos.x - ballRadius <= paddle->pos.x + paddle->width &&
                        pos.x + ballRadius >= paddle->pos.x &&
                        pos.y - ballRadius <= paddle->pos.y + paddle->height &&
                        pos.y + ballRadius >= paddle->pos.y)
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

    /*

    //Instantiate game objects
    Ball ball{
        Vector2{
            static_cast<float>(GetScreenWidth()) / 2,
            static_cast<float>(GetScreenHeight()) / 2
        },
        5
    };

    entities.push_back(&ball);

    Paddle p1
    {
        Vector2{
            50.f,
            static_cast<float>((GetScreenHeight() / 2) - 50),
        },
        1
    };

    entities.push_back(&p1);
    
    Paddle p2
    {
        Vector2{
            static_cast<float>(GetScreenWidth() - 40),
            static_cast<float>(GetScreenHeight() / 2 - 50),
        },
        2
    };

    entities.push_back(&p2);
    */

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

    while (!WindowShouldClose())
    {

        float dt = GetFrameTime();

        GameState gameState = GameState::Menu;

        BeginDrawing();
            ClearBackground(BLACK);

            switch (gameState)
            {
            case GameState::Menu:
                DrawText("Press Start to Begin Play", GetScreenWidth()/2, GetScreenHeight()/2, 30, WHITE);
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
                break;
            case GameState::Pause:
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