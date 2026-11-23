
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

const int W = 900;
const int H = 600;

const int GROUND = 535;
const float GRAVITY = 0.45f;

float ballX = 450;
float ballY = 500;
float ballRadius = 22;
float ballAngle = 0;

float startX = 450;
float startY = 500;
float targetX = 450;
float aimOffset = 0;

float shotTime = 0;
float fallVelocity = 2.5f;
float bounceVelocity = -7.0f;

int gameState = 0;
// 0 = ready, 1 = shooting, 2 = falling, 3 = bouncing

int score = 0;
int attempts = 0;

bool paused = false;

const COLORREF WHITE = RGB(255, 245, 220);

// Draw line
void drawLine(HDC hdc, int x1, int y1,
              int x2, int y2,
              COLORREF color, int width) {

    HPEN pen = CreatePen(PS_SOLID, width, color);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

// Draw court
void drawCourt(HDC hdc) {

    POINT court[4] = {
        {245, 195},
        {655, 195},
        {820, 550},
        {80, 550}
    };

    HBRUSH wood =
        CreateSolidBrush(RGB(190, 125, 65));

    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, wood);

    HPEN border = CreatePen(PS_SOLID, 3, WHITE);
    HPEN oldPen = (HPEN)SelectObject(hdc, border);

    Polygon(hdc, court, 4);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    DeleteObject(wood);
    DeleteObject(border);

    // Court boundary
    drawLine(hdc, 245, 195, 655, 195, WHITE, 3);
    drawLine(hdc, 655, 195, 820, 550, WHITE, 3);
    drawLine(hdc, 820, 550, 80, 550, WHITE, 3);
    drawLine(hdc, 80, 550, 245, 195, WHITE, 3);

    // Center line
    drawLine(hdc, 450, 195, 450, 550, WHITE, 3);

    // Center circle
    HPEN pen = CreatePen(PS_SOLID, 3, WHITE);
    HPEN old = (HPEN)SelectObject(hdc, pen);

    HBRUSH hollow = (HBRUSH)SelectObject(
        hdc, GetStockObject(HOLLOW_BRUSH));

    Ellipse(hdc, 395, 410, 505, 500);

    SelectObject(hdc, hollow);
    SelectObject(hdc, old);
    DeleteObject(pen);

    // Key area
    POINT key[4] = {
        {370, 195},
        {530, 195},
        {555, 325},
        {345, 325}
    };

    pen = CreatePen(PS_SOLID, 3, WHITE);
    old = (HPEN)SelectObject(hdc, pen);

    hollow = (HBRUSH)SelectObject(
        hdc, GetStockObject(HOLLOW_BRUSH));

    Polygon(hdc, key, 4);

    SelectObject(hdc, hollow);
    SelectObject(hdc, old);
    DeleteObject(pen);

    // Free throw circle
    pen = CreatePen(PS_SOLID, 3, WHITE);
    old = (HPEN)SelectObject(hdc, pen);

    hollow = (HBRUSH)SelectObject(
        hdc, GetStockObject(HOLLOW_BRUSH));

    Ellipse(hdc, 385, 275, 515, 365);

    SelectObject(hdc, hollow);
    SelectObject(hdc, old);
    DeleteObject(pen);

    // Three-point arc
    POINT arc[81];

    for (int i = 0; i <= 80; i++) {

        double angle =
            (20.0 + i * 140.0 / 80.0)
            * 3.14159265 / 180.0;

        arc[i].x =
            450 + (int)(300 * cos(angle));

        arc[i].y =
            195 + (int)(285 * sin(angle));
    }

    pen = CreatePen(PS_SOLID, 3, WHITE);
    old = (HPEN)SelectObject(hdc, pen);

    Polyline(hdc, arc, 81);

    SelectObject(hdc, old);
    DeleteObject(pen);
}

// Draw connected basketball hoop
void drawBasket(HDC hdc) {

    // Main support
    drawLine(hdc, 450, 25, 450, 100,
             RGB(100, 100, 100), 12);

    // Support arm
    drawLine(hdc, 450, 105, 450, 125,
             RGB(90, 90, 90), 8);

    // Backboard
    RECT board = {375, 100, 525, 135};

    HBRUSH boardBrush =
        CreateSolidBrush(RGB(245, 245, 245));

    FillRect(hdc, &board, boardBrush);
    DeleteObject(boardBrush);

    HPEN boardPen =
        CreatePen(PS_SOLID, 3, RGB(30, 30, 30));

    HPEN oldPen = (HPEN)SelectObject(hdc, boardPen);

    HBRUSH hollow = (HBRUSH)SelectObject(
        hdc, GetStockObject(HOLLOW_BRUSH));

    Rectangle(hdc, 375, 100, 525, 135);
    Rectangle(hdc, 425, 115, 475, 135);

    SelectObject(hdc, hollow);
    SelectObject(hdc, oldPen);
    DeleteObject(boardPen);

    // Metal mounting brackets
    drawLine(hdc, 435, 135, 425, 160,
             RGB(100, 100, 100), 5);

    drawLine(hdc, 465, 135, 475, 160,
             RGB(100, 100, 100), 5);

    // Circular rim
    HPEN rimPen =
        CreatePen(PS_SOLID, 5, RGB(230, 55, 25));

    oldPen = (HPEN)SelectObject(hdc, rimPen);

    hollow = (HBRUSH)SelectObject(
        hdc, GetStockObject(HOLLOW_BRUSH));

    Ellipse(hdc, 405, 155, 495, 180);

    SelectObject(hdc, hollow);
    SelectObject(hdc, oldPen);
    DeleteObject(rimPen);

    // Net
    for (int i = 0; i <= 8; i++) {

        int x1 = 410 + i * 10;
        int x2 = 420 + i * 7;

        drawLine(hdc, x1, 177, x2, 225,
                 RGB(245, 245, 245), 2);
    }

    drawLine(hdc, 420, 190, 480, 190, WHITE, 1);
    drawLine(hdc, 425, 205, 475, 205, WHITE, 1);
    drawLine(hdc, 430, 220, 470, 220, WHITE, 1);
}

// Draw basketball
void drawBall(HDC hdc) {

    int x = (int)ballX;
    int y = (int)ballY;
    int r = (int)ballRadius;

    // Shadow
    int shadowWidth = r + 12;

    HBRUSH shadow =
        CreateSolidBrush(RGB(105, 75, 45));

    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, shadow);

    Ellipse(hdc,
        x - shadowWidth,
        GROUND + 3,
        x + shadowWidth,
        GROUND + 9);

    SelectObject(hdc, oldBrush);
    DeleteObject(shadow);

    // Ball body
    HBRUSH orange = CreateSolidBrush(
        RGB(235, 105, 25));

    HPEN seam = CreatePen(
        PS_SOLID, 2, RGB(40, 25, 15));

    oldBrush = (HBRUSH)SelectObject(hdc, orange);
    HPEN oldPen = (HPEN)SelectObject(hdc, seam);

    Ellipse(hdc, x-r, y-r, x+r, y+r);

    // Rotating seams
    double angle =
        ballAngle * 3.14159265 / 180.0;

    for (int i = 0; i < 2; i++) {

        double a = angle + i * 1.570796;

        int x1 = x - (int)(r * cos(a));
        int y1 = y - (int)(r * sin(a));

        int x2 = x + (int)(r * cos(a));
        int y2 = y + (int)(r * sin(a));

        MoveToEx(hdc, x1, y1, NULL);
        LineTo(hdc, x2, y2);
    }

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);

    DeleteObject(seam);
    DeleteObject(orange);
}

// Draw game information
void drawInformation(HDC hdc) {

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, WHITE);

    char info[100];

    sprintf(info, "SCORE: %d     SHOTS: %d",
            score, attempts);

    TextOutA(hdc, 20, 15,
             "BASKETBALL GAME", 15);

    TextOutA(hdc, 20, 40,
             info, (int)strlen(info));

    TextOutA(hdc, 20, 65,
        "ARROWS: MOVE  ENTER: SHOOT  A/D: AIM",
        39);

    TextOutA(hdc, 20, 85,
        "+/-: SIZE  SPACE: PAUSE  R: RESET",
        35);

    if (paused) {
        SetTextColor(hdc, RGB(255, 230, 50));
        TextOutA(hdc, 400, 290, "PAUSED", 6);
    }
}

// Draw complete scene
void drawScene(HDC hdc) {

    RECT area = {0, 0, W, H};

    HBRUSH bg =
        CreateSolidBrush(RGB(25, 75, 55));

    FillRect(hdc, &area, bg);
    DeleteObject(bg);

    drawCourt(hdc);
    drawBasket(hdc);
    drawBall(hdc);
    drawInformation(hdc);
}

// Start shooting
void shootBall() {

    if (gameState != 0 || paused)
        return;

    startX = ballX;
    startY = ballY;

    targetX = 450 + aimOffset;

    shotTime = 0;
    gameState = 1;

    attempts++;
}

// Update animation
void updateGame() {

    if (paused)
        return;

    // Rotation transformation
    ballAngle += 5;

    if (ballAngle >= 360)
        ballAngle -= 360;

    // Shooting phase
    if (gameState == 1) {

        shotTime += 0.018f;

        if (shotTime > 1.0f)
            shotTime = 1.0f;

        // Horizontal translation
        ballX = startX +
                (targetX - startX) * shotTime;

        // Curved ball trajectory
        ballY = startY
                - 1173 * shotTime
                + 858 * shotTime * shotTime;

        if (shotTime >= 1.0f) {

            // Check actual target alignment
            if (fabs(targetX - 450) < 25) {
                score += 2;
            }

            // Ball comes below the rim
            ballX = targetX;
            ballY = 185;

            fallVelocity = 2.5f;
            gameState = 2;
        }
    }

    // Ball falling through and below the net
    else if (gameState == 2) {

        fallVelocity += GRAVITY;
        ballY += fallVelocity;

        if (ballY >= GROUND - ballRadius) {

            ballY = GROUND - ballRadius;

            bounceVelocity = -7.0f;
            gameState = 3;
        }
    }

    // Ground bounce
    else if (gameState == 3) {

        ballY += bounceVelocity;
        bounceVelocity += GRAVITY;

        if (ballY >= GROUND - ballRadius) {

            ballY = GROUND - ballRadius;

            // Reset ball after bounce
            ballX = 450;
            ballY = 500;

            aimOffset = 0;
            gameState = 0;
        }
    }
}

// Reset game
void resetGame() {

    ballX = 450;
    ballY = 500;

    ballRadius = 22;
    ballAngle = 0;

    aimOffset = 0;
    shotTime = 0;

    gameState = 0;
    paused = false;

    score = 0;
    attempts = 0;
}

// Window procedure
LRESULT CALLBACK WindowProc(
    HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

    switch (msg) {

    case WM_CREATE:
        SetTimer(hwnd, 1, 16, NULL);
        return 0;

    case WM_TIMER:
        updateGame();
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;

    case WM_KEYDOWN:

        if (wp == VK_SPACE) {
            paused = !paused;
        }

        if (!paused && gameState == 0) {

            // Translation
            if (wp == VK_LEFT && ballX > 110)
                ballX -= 12;

            if (wp == VK_RIGHT && ballX < 790)
                ballX += 12;

            if (wp == VK_UP && ballY > 280)
                ballY -= 12;

            if (wp == VK_DOWN && ballY < 510)
                ballY += 12;

            // Aim
            if (wp == 'A' && aimOffset > -100)
                aimOffset -= 10;

            if (wp == 'D' && aimOffset < 100)
                aimOffset += 10;
        }

        // Shoot
        if (wp == VK_RETURN) {
            shootBall();
        }

        // Scaling
        if (wp == VK_OEM_PLUS || wp == VK_ADD) {
            if (ballRadius < 40)
                ballRadius += 2;
        }

        if (wp == VK_OEM_MINUS || wp == VK_SUBTRACT) {
            if (ballRadius > 12)
                ballRadius -= 2;
        }

        // Reset
        if (wp == 'R') {
            resetGame();
        }

        InvalidateRect(hwnd, NULL, FALSE);
        return 0;

    case WM_PAINT: {

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT client;
        GetClientRect(hwnd, &client);

        int saved = SaveDC(hdc);

        SetMapMode(hdc, MM_ANISOTROPIC);

        SetWindowExtEx(hdc, W, H, NULL);

        SetViewportExtEx(
            hdc, client.right, client.bottom, NULL);

        drawScene(hdc);

        RestoreDC(hdc, saved);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wp, lp);
}

// Main function
int WINAPI WinMain(
    HINSTANCE instance, HINSTANCE,
    LPSTR, int show) {

    WNDCLASSA wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = "BasketballWindow";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground =
        (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassA(&wc);

    RECT rect = {0, 0, W, H};

    AdjustWindowRect(
        &rect, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hwnd = CreateWindowA(
        "BasketballWindow",
        "Basketball Game",
        WS_OVERLAPPEDWINDOW,
        100, 50,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL, NULL, instance, NULL
    );

    if (!hwnd)
        return 0;

    ShowWindow(hwnd, show);

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
