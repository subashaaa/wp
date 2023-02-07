#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <bits/stdc++.h>
#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include "sqlite3.h"
#include "resource.h"
#include <commctrl.h>
#include <commdlg.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#define ID_TIMER 1

#define PACMAN_TIMER 7
#define LEFT 3
#define RIGHT 4
#define UP 5
#define DOWN 6

#define SW 1920
#define SH 1080
#define GW 1290
#define GH 860
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

bool pacman_can_move_right();
bool pacman_can_move_left();
bool pacman_can_move_up();
bool pacman_can_move_down();

void Uncheck(HWND, HWND);
void ButtonSwitch(HWND hwnd, HWND hwndControl);

typedef struct Object_info {
    int width;
    int height;
    int x;
    int y;
    int dx;
    int dy;
} Object;

int pacman_direction;
int pacman_gender;
int gender_error;
int score;

int callback(void *pArg, int argc, char **argv, char **imekolone);
INT_PTR CALLBACK ShowLeaderBoard(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam);

Object background, pacman, fruit;
BITMAP bmp_bgnd, bmp_pacman, bmp_fruit;
HBITMAP h_bgnd, h_pacman, h_pacman_mask, h_fruit, h_fruit_mask;
HDC hdc_mem, hdc_buffer;
HBITMAP hbm_old;

static int pacman_row, pacman_col, fruit_row;
static bool draw_fruit, pacman_fruit, pacman_dead, already_ate;

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WindowProcedure2(HWND, UINT, WPARAM, LPARAM);
HWND hwnd;
HWND hwnd2;

sqlite3 *db;
static HWND listHandle;

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = "Home";
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    if (!RegisterClassEx (&wincl))
        return 0;

    wincl.lpfnWndProc = WindowProcedure2;
    wincl.lpszClassName = "Game";

    if (!RegisterClassEx (&wincl))
        return 0;

    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           "Home",              /* Classname */
           _T("Text box"),      /* Title Text */
           WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, /* default window */
           (SW - MW)/2,         /* Windows decides the position */
           (SH - MH)/2,         /* where the window ends up on the screen */
           MW,                  /* The programs width */
           MH,                  /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
    );

    ShowWindow(hwnd, nCmdShow);

    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // also, 2nd window should have some black space on top
    // to show current score
    // and some black space on bottom
    // to show currently eaten fruit and number of lives
    HINSTANCE hinst = (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE);
    switch (message)
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
            break;
        }
        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case START:
                {
                    if (gender_error == 0) {
                        hwnd2 = CreateWindowEx(
                           0,                   /* Extended possibilites for variation */
                           "Game",              /* Classname */
                           _T("Code::Blocks Template Windows App"),       /* Title Text */
                           WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,      /* default window */
                           (SW - GW)/2,         /* Windows decides the position */
                           (SH - GH)/2,         /* where the window ends up on the screen */
                           GW,                  /* The programs width */
                           GH,                  /* and height in pixels */
                           hwnd,                /* The window is a child-window to desktop */
                           NULL,                /* No menu */
                           (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE),
                           NULL                 /* No Window Creation data */
                        );
                        ShowWindow(hwnd2, 1);
                    } else {
                        EndDialog(hwnd2, 0);
                    }
                    break;
                }
                case PACMAN_MALE:
                {
                    Uncheck(hwnd, (HWND)lParam);
                    ButtonSwitch(hwnd, (HWND)lParam);
                    pacman_gender = 1;
                    break;
                }
                case PACMAN_FEMALE:
                {
                    Uncheck(hwnd, (HWND)lParam);
                    ButtonSwitch(hwnd, (HWND)lParam);
                    pacman_gender = 2;
                    break;
                }
                case BOARD:
                {
                    printf("djes");
                    DialogBox(NULL, MAKEINTRESOURCE(IDD_BOARD), hwnd, ShowLeaderBoard);
                    break;
                }
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage (0);
            break;
        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

LRESULT CALLBACK WindowProcedure2(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_KEYDOWN:
        {
            switch (wParam) {
                case VK_LEFT:
                    if(pacman_can_move_left())
                        pacman_direction = LEFT;
                    break;
                case VK_RIGHT:
                    if(pacman_can_move_right())
                        pacman_direction = RIGHT;
                    break;
                case VK_UP:
                    if(pacman_can_move_up())
                        pacman_direction = UP;
                    break;
                case VK_DOWN:
                    if(pacman_can_move_down())
                        pacman_direction = DOWN;
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
                case PACMAN_TIMER:
                {
                    if(pacman_direction == LEFT && pacman_can_move_left())
                        Move_pacman_left();
                    if(pacman_direction == RIGHT && pacman_can_move_right())
                        Move_pacman_right();
                    if(pacman_direction == UP && pacman_can_move_up())
                        Move_pacman_up();
                    if(pacman_direction == DOWN && pacman_can_move_down())
                        Move_pacman_down();

                    break;
                }
            }
            break;
        }
        case WM_DESTROY:
            Kill_timers(hwnd);
            PostQuitMessage (0);
            break;
        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

void Set_timers(HWND hwnd) {
    SetTimer(hwnd, ID_TIMER, 40, NULL);
    SetTimer(hwnd, PACMAN_TIMER, 50, NULL);
}

void Kill_timers(HWND hwnd) {
    KillTimer(hwnd, ID_TIMER);
    KillTimer(hwnd, PACMAN_TIMER);
}

void Load_bitmaps() {
    if (pacman_gender == 1) {
        h_bgnd = (HBITMAP) LoadImage(NULL, "bgnd.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        h_pacman = (HBITMAP) LoadImage(NULL, "pacman_move.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        h_pacman_mask = (HBITMAP) LoadImage(NULL, "pacman_move_mask.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    } else if (pacman_gender == 2) {
        h_bgnd = (HBITMAP) LoadImage(NULL, "bgnd_w.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        h_pacman = (HBITMAP) LoadImage(NULL, "pacwoman_move.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        h_pacman_mask = (HBITMAP) LoadImage(NULL, "pacwoman_move_mask.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    } else {
        MessageBox(hwnd, "Niste odabrali gender", "Failure", MB_OK);
        gender_error = 1;
        return;
    }

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
    pacman.y = 570;

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
    if(pacman_direction == LEFT)
        BitBlt(hdc_buffer, pacman.x, pacman.y, pacman.width / 2, pacman.height / 4, hdc_mem, pacman_col * pacman.width / 2, pacman_row * pacman.height / 4, SRCPAINT);
    else
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
        if (already_ate == false) {
            PlaySound("pacman_eat_fruit.wav", NULL, SND_FILENAME | SND_ASYNC);
            already_ate = true;
            pacman_fruit = true;
            score += 10;
            fruit_row++;
        }
    }

    if (pacman_fruit == false) {
        Draw_fruit();
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

void Move_pacman_left() {
    PlaySound("pacman_chomp.wav", NULL, SND_FILENAME | SND_ASYNC | SND_NOSTOP);
    pacman_row = 0;
    if (pacman.x > 0)
        pacman.x -= pacman.dx;

    if (pacman_col == 0)
        pacman_col = 1;
    else
        pacman_col = 0;
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

bool pacman_can_move_right(){
    if(pacman.y == 570 && pacman.x > 359 && pacman.x < 820)
        return true;

    if(pacman.y == 570 && pacman.x > 29 && pacman.x < 230)
        return true;

    if(pacman.y == 570 && pacman.x > 949 && pacman.x < 1150)
        return true;

    if(pacman.y == 440 && pacman.x > 159 && pacman.x < 1020)
        return true;

    if(pacman.y == 180 && pacman.x > 159 && pacman.x < 1020)
        return true;

    if(pacman.y == 50 && pacman.x > 359 && pacman.x < 820)
        return true;

    if(pacman.y == 50 && pacman.x > 29 && pacman.x < 230)
        return true;

    if(pacman.y == 50 && pacman.x > 949 && pacman.x < 1150)
        return true;

    if(pacman.y == 310 && pacman.x > 29 && pacman.x < 360)
        return true;

    if(pacman.y == 310 && pacman.x > 819 && pacman.x < 1150)
        return true;

    return false;
}

bool pacman_can_move_left(){
    if(pacman.y == 570 && pacman.x > 360 && pacman.x < 821)
        return true;

    if(pacman.y == 570 && pacman.x > 30 && pacman.x < 231)
        return true;

    if(pacman.y == 570 && pacman.x > 950 && pacman.x < 1151)
        return true;

    if(pacman.y == 440 && pacman.x > 160 && pacman.x < 1021)
        return true;

    if(pacman.y == 180 && pacman.x > 160 && pacman.x < 1021)
        return true;

    if(pacman.y == 50 && pacman.x > 360 && pacman.x < 821)
        return true;

    if(pacman.y == 50 && pacman.x > 30 && pacman.x < 231)
        return true;

    if(pacman.y == 50 && pacman.x > 950 && pacman.x < 1151)
        return true;

    if(pacman.y == 310 && pacman.x > 30 && pacman.x < 361)
        return true;

    if(pacman.y == 310 && pacman.x > 820 && pacman.x < 1151)
        return true;

    return false;
}

bool pacman_can_move_up() {
    if(pacman.x == 360 && pacman.y < 571 && pacman.y > 50)
        return true;

    if(pacman.x == 820 && pacman.y < 571 && pacman.y > 50)
        return true;

    if(pacman.x == 1150 && pacman.y < 571 && pacman.y > 50)
        return true;

    if(pacman.x == 30 && pacman.y < 571 && pacman.y > 50)
        return true;

    if(pacman.x == 160 && pacman.y < 441 && pacman.y > 180)
        return true;

    if(pacman.x == 1020 && pacman.y < 441 && pacman.y > 180)
        return true;

    if(pacman.x == 230 && pacman.y < 181 && pacman.y > 50)
        return true;

    if(pacman.x == 950 && pacman.y < 181 && pacman.y > 50)
        return true;

    if(pacman.x == 230 && pacman.y < 571 && pacman.y > 440)
        return true;

    if(pacman.x == 950 && pacman.y < 571 && pacman.y > 440)
        return true;

    return false;
}

bool pacman_can_move_down() {
    if(pacman.x == 360 && pacman.y < 570 && pacman.y > 49)
        return true;

    if(pacman.x == 820 && pacman.y < 570 && pacman.y > 49)
        return true;

    if(pacman.x == 1150 && pacman.y < 570 && pacman.y > 49)
        return true;

    if(pacman.x == 30 && pacman.y < 570 && pacman.y > 49)
        return true;

    if(pacman.x == 160 && pacman.y < 440 && pacman.y > 179)
        return true;

    if(pacman.x == 1020 && pacman.y < 440 && pacman.y > 179)
        return true;

    if(pacman.x == 230 && pacman.y < 180 && pacman.y > 49)
        return true;

    if(pacman.x == 950 && pacman.y < 180 && pacman.y > 49)
        return true;

    if(pacman.x == 230 && pacman.y < 570 && pacman.y > 439)
        return true;

    if(pacman.x == 950 && pacman.y < 570 && pacman.y > 439)
        return true;

    return false;
}

void ButtonSwitch(HWND hwnd, HWND hwndControl)
{
    SendMessage(hwndControl, BM_SETCHECK, !SendMessage(hwndControl, BM_GETCHECK, 0, 0), 0);
    if(SendMessage(hwndControl, BM_GETCHECK, 0, 0))
    {
        HWND parent = GetWindow(hwnd, GW_OWNER);
        SendMessage(parent, WM_USER, 0, 0);
    }
}

void Uncheck(HWND hwnd, HWND hwnd_of_current_button)
{
    HWND hwnd_male = GetDlgItem(hwnd, PACMAN_MALE);
    if(hwnd_male != hwnd_of_current_button)
    {
        if(SendMessage(hwnd_male, BM_GETCHECK, 0, 0))
        {
            SendMessage(hwnd_male, BM_SETCHECK, !SendMessage(hwnd_male, BM_GETCHECK, 0, 0), 0);
        }
    }

    HWND hwnd_female = GetDlgItem(hwnd, PACMAN_FEMALE);
    if(hwnd_female != hwnd_of_current_button)
    {
        if(SendMessage(hwnd_female, BM_GETCHECK, 0, 0))
        {
            SendMessage(hwnd_female, BM_SETCHECK, !SendMessage(hwnd_female, BM_GETCHECK, 0, 0), 0);
        }
    }
}

int callback(void *pArg, int argc, char **argv, char **imekolone) {
    LVITEM lvItem;

    memset(&lvItem, 0, sizeof(lvItem));
    lvItem.mask = LVIF_TEXT;
    lvItem.pszText = strcat(argv[0], strcat(strcat(argv[1], " "), argv[2]));
    lvItem.iSubItem = 0;
    ListView_InsertItem(listHandle, &lvItem);

    return(0);
}

INT_PTR CALLBACK ShowLeaderBoard(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam){
    int status;
    char* err = 0;
    switch(message) {
	    case WM_INITDIALOG: {
            sqlite3_open("baza", &db);
            listHandle = GetDlgItem(hdlg, IDC_BOARD);
            HINSTANCE hinst = (HINSTANCE)GetWindowLong(hdlg, GWLP_HINSTANCE);

            return TRUE;
	    }
	    case WM_COMMAND: {
	        switch(LOWORD(wParam))
			{
                case IDC_LOAD:
                {
                    char sql[200];

                    sprintf(sql, "SELECT * FROM leaderboard ORDER BY score DESC");

                    status = sqlite3_exec(db, sql, callback, 0, &err);
                    if(status == SQLITE_OK) {
                        MessageBox(hdlg, "Prikaz leaderboarda uspješan", "Success", MB_OK);
                        sqlite3_free(err);
                    }
                    else {
                        MessageBox(hdlg, "Greška", "Failure", MB_OK);
                        sqlite3_free(err);
                        sqlite3_close(db);
                        EndDialog(hdlg, 0);
                        break;
                    }
                    break;
                }
			}
			return TRUE;
	    }
	    case WM_CLOSE: {
            EndDialog(hdlg, 0);
            return TRUE;
	    }
        default:
            return FALSE;
    }
}
