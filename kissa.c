#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#define BytesPerPixel 4
#define BitmapW 1024
#define BitmapH 576

#define PlayerW 16
#define PlayerH 16

#define Platforms 16
#define PlatformW 64
#define PlatformH 12

typedef struct velocity //Variables for handling object velocity
{
    int vx;
    int vy;
    int dx;
    int dy;
} velocity;

typedef struct QPCvars //Variables for QueryPerformanceCounter
{
    int State;
    int Interval;
    LARGE_INTEGER StartCount;
    LARGE_INTEGER EndCount;
    LARGE_INTEGER ElapsedCount;
    LARGE_INTEGER Frequency;
} QPCvars;

HDC DeviceContext;
static BITMAPINFO BitmapInfo = {0};
static void *BitmapMemory;

int Timer(QPCvars *Timervars)
{
    if (!Timervars->State)
    {
        QueryPerformanceCounter(&Timervars->StartCount);
        Timervars->State++;
    }
    if (Timervars->State)
    {
        QueryPerformanceCounter(&Timervars->EndCount);
        Timervars->ElapsedCount.QuadPart = (Timervars->EndCount.QuadPart - Timervars->StartCount.QuadPart) * 1000000;
    }
    if (Timervars->ElapsedCount.QuadPart > Timervars->Interval * Timervars->Frequency.QuadPart)
    {
        Timervars->State--;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void CreateBufferBitmap() //Creates buffer bitmap
{
    if (BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapW;
    BitmapInfo.bmiHeader.biHeight = BitmapH;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize = BitmapW * BitmapH * BytesPerPixel;
    BitmapMemory = VirtualAlloc(NULL, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

void DrawBackground()
{
    int BitmapArea = BitmapW * BitmapH;
    int Red = 125;
    int Green = 200;
    int Blue = 255;

    uint32_t *Pixel = (uint32_t *)BitmapMemory;
    for (int z = 0; z < BitmapH / 8; z++)
    {
        for (int z = 0; z < BitmapW * 8; z++)
        {
        *Pixel++ = (Red << 16 | Green << 8 | Blue);
        }
        Red--;
        Green--;
        Blue --;
    }

    Pixel -= BitmapArea;

    Red = 100;
    Green = 50;
    Blue = 0;

    for (int z = 0; z < 32; z++)
    {
        for (int z = 0; z < BitmapW; z++)
        {
            *Pixel++ = (Red << 16 | Green << 8 | Blue);
        }
    Red++;
    Green++;
    }

    Red = 0;
    Green = 100;
    Blue = 25;
    for (int z = 0; z < BitmapW * 8; z++)
    {
        *Pixel++ = (Red << 16 | Green << 8 | Blue);
    }
}

void MovePlatforms(POINT *PlatformLocation, QPCvars *Timervars, int *Direction)
{
    if (PlatformLocation->x < 0 && *Direction == -1)
    {
        *Direction = 1;
    }
    if (PlatformLocation->x > (BitmapW - PlatformW) && *Direction == 1)
    {
        *Direction = -1;
    }
    if (Timer(Timervars))
    {
        PlatformLocation->x += *Direction;
    }
}

void DrawPlatforms(POINT *PlatformLocation)
{
    int Red = 100;
    int Green = 50;
    int Blue = 0;

    uint32_t *PlatformPixel = (uint32_t *)BitmapMemory;
    PlatformPixel += PlatformLocation->x + PlatformLocation->y * BitmapW;

    for (int z = 0; z < PlatformH - 2; z++)
    {
        for (int z = 0; z < PlatformW; z++)
        {
            *PlatformPixel++ = (Red << 16 | Green << 8 | Blue);
        }
        Red++;
        Green++;
        PlatformPixel += BitmapW - PlatformW;
    }

    Red = 0;
    Green = 100;
    Blue = 25;
    for (int z = 0; z < 2; z++)
    {
        for (int z = 0; z < PlatformW; z++)
        {
            *PlatformPixel++ = (Red << 16 | Green << 8 | Blue);
        }
        PlatformPixel += BitmapW - PlatformW;
    }
}

void DrawPlayer(POINT *PlayerLocation, int *Direction) //Draws a cat
{
    int PlayerArea = PlayerW * PlayerH;
    int SpriteRow;
    int SpriteColumn;
    int RowBitmap;
    int SpriteBitmap[16] = {0x6777, 0x7666, 0x766E, 0x3E6C, 0x3FFC, 0x3FFC, 0x3FFC, 0x7FFC, 0x600C, 0xC01E, 0x803F, 0x803F, 0xC03F, 0x603F, 0x3033, 0x0021};
    int FurRed = 0;
    int FurGreen = 0;
    int FurBlue = 0;
    int EyeRed = 255;
    int EyeGreen = 193;
    int EyeBlue = 7;

    uint32_t *FurPixel = (uint32_t *)BitmapMemory;
    FurPixel += PlayerLocation->x + PlayerLocation->y * BitmapW;

    if (*Direction == 1)
    {
        for (SpriteRow = 0; SpriteRow < PlayerH; SpriteRow++)
        {
            for (SpriteColumn = 0; SpriteColumn < PlayerW; SpriteColumn++)
            {
                if (0x8000 & (SpriteBitmap[SpriteRow] << SpriteColumn))
                {
                    *FurPixel++ = (FurRed << 16 | FurGreen << 8 | FurBlue);
                }
                else
                {
                    FurPixel++;
                }
            }
            FurPixel += BitmapW - PlayerW;
        }

        uint32_t *EyePixel = (uint32_t *)BitmapMemory;
        EyePixel += PlayerLocation->x;
        EyePixel += PlayerLocation->y * BitmapW;
        EyePixel += BitmapW * 12 + 12;
        *EyePixel++ = (EyeRed << 16 | EyeGreen << 8 | EyeBlue);
        EyePixel += 2;
        *EyePixel++ = (EyeRed << 16 | EyeGreen << 8 | EyeBlue);
    }

    if (*Direction == -1)
    {
        for (SpriteRow = 0; SpriteRow < PlayerH; SpriteRow++)
        {
            for (SpriteColumn = 0; SpriteColumn < PlayerW; SpriteColumn++)
            {
                if (0x8000 & SpriteBitmap[SpriteRow] << ((PlayerW - 1) - SpriteColumn))
                {
                    *FurPixel++ = (FurRed << 16 | FurGreen << 8 | FurBlue);
                }
                else
                {
                    FurPixel++;
                }
            }
            FurPixel += BitmapW-PlayerW;
        }

        uint32_t *EyePixel = (uint32_t *)BitmapMemory;
        EyePixel += PlayerLocation->x;
        EyePixel += PlayerLocation->y * BitmapW;
        EyePixel += BitmapW * 12;
        *EyePixel++ = (EyeRed << 16 | EyeGreen << 8 | EyeBlue);
        EyePixel += 2;
        *EyePixel++ = (EyeRed << 16 | EyeGreen << 8 | EyeBlue);
    }
}

void DrawWindow(HWND hwnd, RECT rc) //Draws buffer bitmap to window
{
    GetClientRect(hwnd, &rc);
    int WindowW = rc.right - rc.left;
    int WindowH = rc.bottom - rc.top;

    StretchDIBits(DeviceContext, rc.left, rc.top, WindowW, WindowH, 0, 0, BitmapW, BitmapH, BitmapMemory, &BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

int DetectGroundContact(POINT ObjectLocation, POINT TargetLocation[])
{
    for (int i = 0; i < Platforms; i++)
    {
        if (ObjectLocation.y == (TargetLocation[i].y + PlatformH) && ObjectLocation.x > (TargetLocation[i].x - PlayerW) && ObjectLocation.x < (TargetLocation[i].x + PlatformW))
        {
            return 1;
        }
    }
    if (ObjectLocation.y == 40)
    {
        return 1;
    }
    return 0;
}

void ReadKeyboard(QPCvars *Timervars, POINT *ObjectLocation, velocity *ObjectV, int *Direction, int Contact)
{
    if (Timer(Timervars))
    {
        if (GetAsyncKeyState('D'))
        {
            *Direction = 1;
            if (Contact)
            {
                ObjectV->vx += 10;
            }
        }
        if (GetAsyncKeyState('A'))
        {
            *Direction = -1;
            if (Contact)
            {
                ObjectV->vx -= 10;
            }
        }
        if (GetAsyncKeyState(VK_SPACE) && Contact)
        {
            ObjectV->vy = 500;
        }
    }
}

void UpdateObjectLocation(POINT *ObjectLocation, velocity *ObjectV, QPCvars *ObjectTx, QPCvars *ObjectTy)
{
    if (ObjectV->vx)
    {
        if (!ObjectTx->State)
        {
            QueryPerformanceCounter(&ObjectTx->StartCount);
            ObjectTx->State++;
        }

        QueryPerformanceCounter(&ObjectTx->EndCount);
        ObjectTx->ElapsedCount.QuadPart = (ObjectTx->EndCount.QuadPart - ObjectTx->StartCount.QuadPart);
        ObjectV->dx = ObjectV->vx * ObjectTx->ElapsedCount.QuadPart / ObjectTx->Frequency.QuadPart;

        if (ObjectV->dx >= 1)
        {
            ObjectLocation->x++;
            ObjectTx->State--;
        }
        if (ObjectV->dx <= -1)
        {
            ObjectLocation->x--;
            ObjectTx->State--;
        }
    }

    if (ObjectV->vy)
    {

        if (!ObjectTy->State)
        {
            QueryPerformanceCounter(&ObjectTy->StartCount);
            ObjectTy->State++;
        }

        QueryPerformanceCounter(&ObjectTy->EndCount);
        ObjectTy->ElapsedCount.QuadPart = (ObjectTy->EndCount.QuadPart - ObjectTy->StartCount.QuadPart);
        ObjectV->dy = ObjectV->vy * ObjectTy->ElapsedCount.QuadPart / ObjectTy->Frequency.QuadPart;

        if (ObjectV->dy >= 1)
        {
            ObjectLocation->y++;
            ObjectTy->State--;
        }
        if (ObjectV->dy <= -1)
        {
            ObjectLocation->y--;
            ObjectTy->State--;
        }
    }
}

void Gravity(QPCvars *Timervars, POINT *ObjectLocation, velocity *ObjectV, int Contact)
{
    if (!Contact)
    {
        if (Timer(Timervars))
        {
            ObjectV->vy -= 10;
        }
    }
    if (ObjectV->vy != 0 && Contact)
    {
        ObjectV->vy = 0;
    }
}

void Friction(QPCvars *Timervars, POINT *ObjectLocation, velocity *ObjectV)
{
    if (!GetAsyncKeyState('D') && ObjectLocation->y == 40 && ObjectV->vx > 0)
    {
        if (Timer(Timervars))
        {
            ObjectV->vx -= 10;
        }
    }
    if (!GetAsyncKeyState('A') && ObjectLocation->y == 40 && ObjectV->vx < 0)
    {
        if (Timer(Timervars))
        {
            ObjectV->vx += 10;
        }
    }
}

void LimitArea(POINT *ObjectLocation, velocity *ObjectV)
{
    if (ObjectLocation->x < 0)
    {
        ObjectLocation->x++;
        ObjectV->vx = 0;
    }
    if (ObjectLocation->x > (BitmapW - PlayerW))
    {
        ObjectLocation->x--;
        ObjectV->vx = 0;
    }
    if (ObjectLocation->y == 40)
    {
        ObjectV->vy = 0;
    }
    if (ObjectLocation->y > (BitmapH - PlayerH))
    {
        ObjectLocation->y--;
        ObjectV->vy = 0;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    PAINTSTRUCT Paint;
    char WindowInfo[32];

    switch (uMsg)
    {
        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        case WM_SIZE:
        {
            GetClientRect(hwnd, &rc);
            sprintf(WindowInfo, "%ldx%ld", (rc.right - rc.left), (rc.bottom - rc.top));
            SetWindowTextA(hwnd, WindowInfo);
        }
        case WM_PAINT:
        {
            BeginPaint(hwnd, &Paint);
            DrawWindow(hwnd, rc);
            EndPaint(hwnd, &Paint);
            break;
        }
        default:
        {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
            break;
        }
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    int Running = TRUE;
    int PawContact;
    int PlayerDirection = 1;
    int PlatformDirection[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    char WindowInfo[64];
    WNDCLASS wc = {0};
    MSG msg = {0};
    QPCvars AccelerationT = {0};
    QPCvars GravityT = {0};
    QPCvars FrictionT = {0};
    QPCvars PlayerTx = {0};
    QPCvars PlayerTy = {0};
    QPCvars PlatformT[16] = {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}};
    velocity PlayerV = {0};
    POINT PlayerLocation = {0, 40};
    POINT PlatformLocation[16] = {{10, 70}, {500, 100}, {100, 130}, {70, 160}, {400, 190}, {160, 220}, {60, 250}, {200, 280}, {100, 310}, {0, 340}, {160, 370}, {450, 400}, {0, 430}, {620, 460}, {0, 490}, {100, 520}};

    AccelerationT.Interval = 8000; //microseconds
    GravityT.Interval = 8000; //microseconds
    FrictionT.Interval = 5; //microseconds

    PlatformT[0].Interval = 22000;
    PlatformT[1].Interval = 20000;
    PlatformT[2].Interval = 18000;
    PlatformT[3].Interval = 7000;
    PlatformT[4].Interval = 16000;
    PlatformT[5].Interval = 9000;
    PlatformT[6].Interval = 14000;
    PlatformT[7].Interval = 11000;
    PlatformT[8].Interval = 12000;
    PlatformT[9].Interval = 13000;
    PlatformT[10].Interval = 10000;
    PlatformT[11].Interval = 15000;
    PlatformT[12].Interval = 8000;
    PlatformT[13].Interval = 17000;
    PlatformT[14].Interval = 6000;
    PlatformT[15].Interval = 15000;


    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = L"Window Class";

    RegisterClass(&wc);
    RECT rc = {0, 0, 640, 360};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        L"WINDOW",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        0, 0, rc.right - rc.left, rc.bottom - rc.top,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (hwnd == NULL)
    {
        return 0;
    }

    DeviceContext = GetDC(hwnd);
    CreateBufferBitmap();
    QueryPerformanceFrequency(&AccelerationT.Frequency);
    GravityT.Frequency = AccelerationT.Frequency;
    PlayerTx.Frequency = AccelerationT.Frequency;
    PlayerTy.Frequency = AccelerationT.Frequency;
    for (int z = 0; z < Platforms; z++)
    {
        PlatformT[z].Frequency = AccelerationT.Frequency;
    }

    while (Running) //Run the main loop
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                Running = FALSE;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        DrawBackground();
        for (int i = 0; i < Platforms; i++)
        {
            MovePlatforms(&PlatformLocation[i], &PlatformT[i], &PlatformDirection[i]);
            DrawPlatforms(&PlatformLocation[i]);
        }
        PawContact = DetectGroundContact(PlayerLocation, PlatformLocation);
        Gravity(&GravityT, &PlayerLocation, &PlayerV, PawContact);
        ReadKeyboard(&AccelerationT, &PlayerLocation, &PlayerV, &PlayerDirection, PawContact);
        Friction(&FrictionT, &PlayerLocation, &PlayerV);
        UpdateObjectLocation(&PlayerLocation, &PlayerV, &PlayerTx, &PlayerTy);
        LimitArea(&PlayerLocation, &PlayerV);
        DrawPlayer(&PlayerLocation, &PlayerDirection);
        DrawWindow(hwnd, rc);
    }
    return 0;
}