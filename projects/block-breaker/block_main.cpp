#include "DxLib.h"

#include <algorithm>
#include <cstdlib>

namespace
{
constexpr int kScreenWidth = 1600;
constexpr int kScreenHeight = 900;
constexpr int kBlockCountX = 16;
constexpr int kBlockCountY = 3;
constexpr int kBlockWidth = 100;
constexpr int kBlockHeight = 50;
constexpr int kBarSpeed = 10;

struct Rect
{
    int x;
    int y;
    int width;
    int height;
    bool active = true;
};

struct Circle
{
    int x;
    int y;
    int radius;
};

struct Speed
{
    int x;
    int y;
};

enum class GameState
{
    Playing,
    Cleared,
    GameOver,
};

Rect blocks[kBlockCountX][kBlockCountY];
Rect bar{};
Circle ball{};
Speed ballSpeed{};
GameState gameState = GameState::Playing;

bool CircleIntersectsRect(const Rect& rect, const Circle& circle)
{
    const int closestX = std::clamp(circle.x, rect.x, rect.x + rect.width);
    const int closestY = std::clamp(circle.y, rect.y, rect.y + rect.height);
    const int distanceX = circle.x - closestX;
    const int distanceY = circle.y - closestY;

    return distanceX * distanceX + distanceY * distanceY <=
        circle.radius * circle.radius;
}

int CountRemainingBlocks()
{
    int remaining = 0;
    for (int y = 0; y < kBlockCountY; ++y)
    {
        for (int x = 0; x < kBlockCountX; ++x)
        {
            if (blocks[x][y].active)
            {
                ++remaining;
            }
        }
    }
    return remaining;
}

void InitializeGame()
{
    for (int y = 0; y < kBlockCountY; ++y)
    {
        for (int x = 0; x < kBlockCountX; ++x)
        {
            blocks[x][y] = {
                x * kBlockWidth,
                100 + y * kBlockHeight,
                kBlockWidth,
                kBlockHeight,
                true,
            };
        }
    }

    bar = {700, 700, 200, 30, true};
    ball = {800, 500, 10};
    ballSpeed = {4, -5};
    gameState = GameState::Playing;
}

void UpdateBar()
{
    if (CheckHitKey(KEY_INPUT_A) || CheckHitKey(KEY_INPUT_LEFT))
    {
        bar.x -= kBarSpeed;
    }
    if (CheckHitKey(KEY_INPUT_D) || CheckHitKey(KEY_INPUT_RIGHT))
    {
        bar.x += kBarSpeed;
    }

    bar.x = std::clamp(bar.x, 0, kScreenWidth - bar.width);
}

void ReflectAtWalls()
{
    if (ball.x - ball.radius <= 0 && ballSpeed.x < 0)
    {
        ball.x = ball.radius;
        ballSpeed.x *= -1;
    }
    else if (ball.x + ball.radius >= kScreenWidth && ballSpeed.x > 0)
    {
        ball.x = kScreenWidth - ball.radius;
        ballSpeed.x *= -1;
    }

    if (ball.y - ball.radius <= 0 && ballSpeed.y < 0)
    {
        ball.y = ball.radius;
        ballSpeed.y *= -1;
    }
}

void ReflectAtBar()
{
    if (ballSpeed.y <= 0 || !CircleIntersectsRect(bar, ball))
    {
        return;
    }

    ball.y = bar.y - ball.radius;
    const int hitOffset = ball.x - (bar.x + bar.width / 2);
    ballSpeed.x = std::clamp(hitOffset / 10, -12, 12);
    ballSpeed.y = -std::max(5, std::abs(ballSpeed.y));
}

void BreakHitBlock()
{
    for (int y = 0; y < kBlockCountY; ++y)
    {
        for (int x = 0; x < kBlockCountX; ++x)
        {
            if (!blocks[x][y].active ||
                !CircleIntersectsRect(blocks[x][y], ball))
            {
                continue;
            }

            blocks[x][y].active = false;
            ballSpeed.y *= -1;
            return;
        }
    }
}

void UpdateGame()
{
    if (gameState != GameState::Playing)
    {
        if (CheckHitKey(KEY_INPUT_SPACE))
        {
            InitializeGame();
        }
        return;
    }

    UpdateBar();
    ball.x += ballSpeed.x;
    ball.y += ballSpeed.y;

    ReflectAtWalls();
    ReflectAtBar();
    BreakHitBlock();

    if (CountRemainingBlocks() == 0)
    {
        gameState = GameState::Cleared;
    }
    else if (ball.y - ball.radius > kScreenHeight)
    {
        gameState = GameState::GameOver;
    }
}

void DrawBlocks()
{
    const unsigned int colors[kBlockCountY] = {
        GetColor(239, 83, 80),
        GetColor(255, 202, 40),
        GetColor(102, 187, 106),
    };

    for (int y = 0; y < kBlockCountY; ++y)
    {
        for (int x = 0; x < kBlockCountX; ++x)
        {
            const Rect& block = blocks[x][y];
            if (!block.active)
            {
                continue;
            }

            DrawBox(block.x, block.y, block.x + block.width,
                block.y + block.height, colors[y], TRUE);
            DrawBox(block.x, block.y, block.x + block.width,
                block.y + block.height, GetColor(20, 20, 20), FALSE);
        }
    }
}

void DrawGame()
{
    DrawBlocks();
    DrawBox(bar.x, bar.y, bar.x + bar.width, bar.y + bar.height,
        GetColor(245, 245, 245), TRUE);
    DrawCircle(ball.x, ball.y, ball.radius, GetColor(66, 165, 245), TRUE);

    DrawFormatString(20, 20, GetColor(255, 255, 255),
        "BLOCKS: %d / %d", CountRemainingBlocks(),
        kBlockCountX * kBlockCountY);
    DrawString(20, kScreenHeight - 40, "MOVE: A/D or LEFT/RIGHT",
        GetColor(200, 200, 200));

    if (gameState == GameState::Cleared)
    {
        DrawString(690, 420, "GAME CLEAR!", GetColor(255, 235, 59));
        DrawString(650, 455, "PRESS SPACE TO RESTART",
            GetColor(255, 255, 255));
    }
    else if (gameState == GameState::GameOver)
    {
        DrawString(700, 420, "GAME OVER", GetColor(239, 83, 80));
        DrawString(650, 455, "PRESS SPACE TO RESTART",
            GetColor(255, 255, 255));
    }
}
}  // namespace

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    SetMainWindowText("Block Breaker");
    SetWindowInitPosition(80, 60);
    SetGraphMode(kScreenWidth, kScreenHeight, 32);
    SetBackgroundColor(12, 18, 28);
    ChangeWindowMode(TRUE);

    if (DxLib_Init() == -1)
    {
        return -1;
    }

    SetDrawScreen(DX_SCREEN_BACK);
    InitializeGame();

    while (ProcessMessage() == 0 && !CheckHitKey(KEY_INPUT_ESCAPE))
    {
        ClearDrawScreen();
        UpdateGame();
        DrawGame();
        ScreenFlip();
    }

    DxLib_End();
    return 0;
}
