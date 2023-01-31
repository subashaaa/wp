#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <unistd.h>
#include <stdio.h>
#include "resource.h"
#define ID_TIMER 1
#define FRUIT_TIMER 2
#define SW 1920
#define SH 1080
#define GW 1290
#define GH 740
#define MW 300
#define MH 250

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

void Update_scene(RECT*);
void Draw_scene(HDC, RECT*);

void Draw_bgnd();
void Draw_pacman();

void Move_pacman_up();
void Move_pacman_down();
void Move_pacman_left();
void Move_pacman_right();

void Draw_fruit();

void Set_timers(HWND);
void Kill_timers(HWND);
void Load_bitmaps();
void Get_objects();
void Initialize_objects();

typedef struct Object_info {
    int width;
    int height;
    int x;
    int y;
    int dx;
    int dy;
} Object;

Object background, pacman, fruit;
BITMAP bmp_bgnd, bmp_pacman, bmp_fruit;
HBITMAP h_bgnd, h_pacman, h_pacman_mask, h_fruit, h_fruit_mask;
HDC hdc_mem, hdc_buffer;
HBITMAP hbm_old;

static int pacman_row, pacman_col, fruit_row;
static bool draw_fruit, pacman_fruit, pacman_dead;

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WindowProcedure2(HWND, UINT, WPARAM, LPARAM);
HWND hwnd;
HWND hwnd2;

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = "Home";
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    wincl.lpfnWndProc = WindowProcedure2;
    wincl.lpszClassName = "Game";

    if (!RegisterClassEx (&wincl))
        return 0;

    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           "Home",         /* Classname */
           _T("Text box"),       /* Title Text */
           WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, /* default window */
           (SW - MW)/2,       /* Windows decides the position */
           (SH - MH)/2,       /* where the window ends up on the screen */
           MW,                 /* The programs width */
           MH,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
    );

    ShowWindow(hwnd, nCmdShow);

    /* The class is registered, let's create the program*/


    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // also, 2nd window should have some black space on top
    // to show current score
    // and some black space on bottom
    // to show currently eaten fruit and number of lives
    HINSTANCE hinst = (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE);
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
        {
            CreateWindow("button", "Start game", WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_PUSHLIKE,
                         55,15,185,45, hwnd, (HMENU)START, hinst, NULL);
            CreateWindow("button", "Male", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | BS_RIGHTBUTTON | BS_RIGHT,
                         55,70,70,20, hwnd, (HMENU)PACMAN_MALE, hinst, NULL);
            CreateWindow("button", "Female", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | BS_RIGHTBUTTON | BS_RIGHT,
                         170,70,70,20, hwnd, (HMENU)PACMAN_FEMALE, hinst, NULL);
            CreateWindow("button", "Leaderboard", WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_PUSHLIKE,
                         55,150,185,45, hwnd, (HMENU)BOARD, hinst, NULL);

//            PAINTSTRUCT ps;
//            HDC hdc = BeginPaint(hwnd, &ps);
//            hdc_mem = CreateCompatibleDC(hdc);
//
//            h_pacman = (HBITMAP) LoadImage(NULL, "pacman_move.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
//            h_pacman_mask = (HBITMAP) LoadImage(NULL, "pacman_move_mask.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
//
//            GetObject(h_pacman, sizeof(BITMAP), &bmp_pacman);
//
//            pacman.width = bmp_pacman.bmWidth;
//            pacman.height = bmp_pacman.bmHeight;
//
//            hbm_old = (HBITMAP) SelectObject(hdc_mem, h_pacman);
//            BitBlt(hdc, 150, 150, pacman.width / 2, pacman.height / 4, hdc_mem, 1 * pacman.width / 2, 1 * pacman.height / 4, SRCAND);
//
//            hbm_old = (HBITMAP) SelectObject(hdc_mem, h_pacman_mask);
//            BitBlt(hdc, 150, 150, pacman.width / 2, pacman.height / 4, hdc_mem, 1 * pacman.width / 2, 1 * pacman.height / 4, SRCPAINT);
//
//            SelectObject(hdc_mem, hbm_old);
//            DeleteDC(hdc_mem);
//            EndPaint(hwnd, &ps);
            break;
        }
        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case START:
                {
                    hwnd2 = CreateWindowEx(
                       0,                   /* Extended possibilites for variation */
                       "Game",         /* Classname */
                       _T("Code::Blocks Template Windows App"),       /* Title Text */
                       WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, /* default window */
                       (SW - GW)/2,       /* Windows decides the position */
                       (SH - GH)/2,       /* where the window ends up on the screen */
                       GW,                 /* The programs width */
                       GH,                 /* and height in pixels */
                       hwnd,        /* The window is a child-window to desktop */
                       NULL,                /* No menu */
                       (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE),
                       NULL                 /* No Window Creation data */
                    );

                    ShowWindow(hwnd2, 1);
                    break;
                }
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

LRESULT CALLBACK WindowProcedure2(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_KEYDOWN:
        {
            switch (wParam) {
                case VK_LEFT:
                    Move_pacman_left();
                    break;
                case VK_RIGHT:
                    Move_pacman_right();
                    break;
                case VK_UP:
                    Move_pacman_up();
                    break;
                case VK_DOWN:
                    Move_pacman_down();
                    break;
            }
            break;
        }
        case WM_CREATE:
        {
            PlaySound("pacman_beginning.wav", NULL, SND_FILENAME | SND_ASYNC);
            // to-do: everything has to stop until this sound finishes
            // blocking call

            Set_timers(hwnd);

            Load_bitmaps();
            Get_objects();
            Initialize_objects();

            break;
        }
        case WM_TIMER:
        {
            RECT rect;
            HDC hdc = GetDC(hwnd);
            GetClientRect(hwnd, &rect);
            Update_scene(&rect);
            Draw_scene(hdc, &rect);
            ReleaseDC(hwnd, hdc);

            switch (wParam) {
                case FRUIT_TIMER:
                {
                    fruit_row++;
                    draw_fruit = true;
                    //KillTimer(FRUIT_TIMER);

                    break;
                }
            }
            break;
        }
        case WM_DESTROY:
            Kill_timers(hwnd);
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

void Set_timers(HWND hwnd) {
    SetTimer(hwnd, ID_TIMER, 40, NULL);
    SetTimer(hwnd, FRUIT_TIMER, 5000, NULL);
}

void Kill_timers(HWND hwnd) {
    KillTimer(hwnd, ID_TIMER);
}

void Load_bitmaps() {
    h_bgnd = (HBITMAP) LoadImage(NULL, "bgnd_w.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    h_pacman = (HBITMAP) LoadImage(NULL, "pacman_move.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    h_pacman_mask = (HBITMAP) LoadImage(NULL, "pacman_move_mask.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    h_fruit = (HBITMAP) LoadImage(NULL, "fruits.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    h_fruit_mask = (HBITMAP) LoadImage(NULL, "fruits_mask.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

void Get_objects() {
    GetObject(h_bgnd, sizeof(BITMAP), &bmp_bgnd);
    GetObject(h_pacman, sizeof(BITMAP), &bmp_pacman);
    GetObject(h_fruit, sizeof(BITMAP), &bmp_fruit);
}

void Initialize_objects() {
    background.width = bmp_bgnd.bmWidth;
    background.height = bmp_bgnd.bmHeight;
    background.dx = 0;
    background.dy = 0;
    background.x = 0;
    background.y = 0;

    pacman.width = bmp_pacman.bmWidth;
    pacman.height = bmp_pacman.bmHeight;
    pacman.dx = 10;
    pacman.dy = 10;
    pacman.x = 600;
    pacman.y = 600;

    fruit.width = bmp_fruit.bmWidth;
    fruit.height = bmp_fruit.bmHeight;
    fruit.dx = 0;
    fruit.dy = 0;
    fruit.x = 600;
    fruit.y = 450;
}

void Update_scene(RECT* rect) {

}

void Draw_bgnd() {
    hbm_old = (HBITMAP) SelectObject(hdc_mem, h_bgnd);
    BitBlt(hdc_buffer, background.x, background.y, background.width, background.height, hdc_mem, 0, 0, SRCCOPY);
}

void Draw_pacman() {
    hbm_old = (HBITMAP) SelectObject(hdc_mem, h_pacman);
    BitBlt(hdc_buffer, pacman.x, pacman.y, pacman.width / 2, pacman.height / 4, hdc_mem, pacman_col * pacman.width / 2, pacman_row * pacman.height / 4, SRCAND);

    hbm_old = (HBITMAP) SelectObject(hdc_mem, h_pacman_mask);
    BitBlt(hdc_buffer, pacman.x, pacman.y, pacman.width / 2, pacman.height / 4, hdc_mem, pacman_col * pacman.width / 2, pacman_row * pacman.height / 4, SRCPAINT);
}

void Draw_scene(HDC hdc, RECT* rect) {
    hdc_buffer = CreateCompatibleDC(hdc);
    HBITMAP hbm_buffer = CreateCompatibleBitmap(hdc, rect->right, rect->bottom);
    HBITMAP hbm_old_buffer = (HBITMAP) SelectObject(hdc_buffer, hbm_buffer);

    hdc_mem = CreateCompatibleDC(hdc);

    Draw_bgnd();

    if (pacman_dead == false) {
        Draw_pacman();
    } else {
        //Draw_pacman_death();
        //PlaySound("pacman_death.wav", NULL, SND_FILENAME | SND_ASYNC);
    }

    if ((pacman.x >= fruit.x - fruit.width / 4 && pacman.x <= fruit.x + fruit.width / 4) &&
        (pacman.y >= fruit.y - fruit.height / 16 && pacman.y <= fruit.y + fruit.height / 16)) {
        PlaySound("pacman_eat_fruit.wav", NULL, SND_FILENAME | SND_ASYNC);
        pacman_fruit = true;
        //score++
    } else {
        pacman_fruit = false;
    }

    if (draw_fruit && pacman_fruit == false) {
        Draw_fruit();
        //should disappear after moving
    }

    // Draw_ghost()
    // check for pacman_ghost collision

    BitBlt(hdc, 0, 0, rect->right, rect->bottom, hdc_buffer, 0, 0, SRCCOPY);

    SelectObject(hdc_mem, hbm_old);
    DeleteObject(hdc_mem);
    DeleteObject(hbm_buffer);

    SelectObject(hdc_buffer, hbm_old_buffer);
    DeleteObject(hdc_buffer);
    DeleteObject(hbm_old_buffer);
}

// implement walk till collision with wall
void Move_pacman_left() {
    PlaySound("pacman_chomp.wav", NULL, SND_FILENAME | SND_ASYNC | SND_NOSTOP);
    pacman_row = 0;
    if (pacman.x > 0)
        pacman.x -= pacman.dx;

    if (pacman_col == 0)
        pacman_col = 1;
    else
        pacman_col = 0;

//    mciSendString("open pacman_chomp.wav type waveaudio alias chomp", NULL, 0, NULL);
//    mciSendString("play chomp", NULL, 0, NULL);
//    mciSendString("close chomp", NULL, 0, NULL);
}

void Move_pacman_right() {
    PlaySound("pacman_chomp.wav", NULL, SND_FILENAME | SND_ASYNC | SND_NOSTOP);
    pacman_row = 1;

    if (pacman.x < 1280 - pacman.width / 2)
        pacman.x += pacman.dx;

    if (pacman_col == 0)
        pacman_col = 1;
    else
        pacman_col = 0;
}

void Move_pacman_up() {
    PlaySound("pacman_chomp.wav", NULL, SND_FILENAME | SND_ASYNC | SND_NOSTOP);
    pacman_row = 2;

    pacman.y -= pacman.dy;

    if (pacman_col == 0)
        pacman_col = 1;
    else
        pacman_col = 0;
}

void Move_pacman_down() {
    PlaySound("pacman_chomp.wav", NULL, SND_FILENAME | SND_ASYNC | SND_NOSTOP);
    pacman_row = 3;

    if (pacman.y < 700 - pacman.height / 4)
        pacman.y += pacman.dy;

    if (pacman_col == 0)
        pacman_col = 1;
    else
        pacman_col = 0;
}

void Draw_fruit() {
    hbm_old = (HBITMAP) SelectObject(hdc_mem, h_fruit);
    BitBlt(hdc_buffer, fruit.x, fruit.y, fruit.width, fruit.height / 4, hdc_mem, 0 * fruit.width, fruit_row * fruit.height / 4, SRCAND);

    hbm_old = (HBITMAP) SelectObject(hdc_mem, h_fruit_mask);
    BitBlt(hdc_buffer, fruit.x, fruit.y, fruit.width, fruit.height / 4, hdc_mem, 0 * fruit.width, fruit_row * fruit.height / 4, SRCPAINT);
}
