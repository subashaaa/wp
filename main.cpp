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

#define LEFT 3
#define RIGHT 4
#define UP 5
#define DOWN 6

#define ID_TIMER 1
#define PACMAN_TIMER 7
#define DEATH_TIMER 8
#define GREEN_GHOST_TIMER 9
#define RED_GHOST_TIMER 10
#define EAT_GHOST_TIMER 11
#define WEAK_TIMER_START 12
#define WEAK_TIMER_CHANGE_START 13
#define WEAK_TIMER_CHANGE 14
#define WEAK_TIMER_END 15
#define FRUIT_TIMER 17

#define SW 1920
#define SH 1080
#define GW 1290
#define GH 810
#define MW 300
#define MH 250
#define NW 500
#define NH 500

#define s_dimension 20
#define n_of_s_dots 70
#define b_dimension 40
#define n_of_b_dots 4

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

void Update_scene(RECT*);
void Draw_scene(HWND, HDC, RECT*);

void Draw_bgnd();
void Draw_pacman();
void Draw_lives();

void Move_pacman_up();
void Move_pacman_down();
void Move_pacman_left();
void Move_pacman_right();

void Move_ghost_up();
void Move_ghost_down();
void Move_ghost_left();
void Move_ghost_right();

void Draw_fruit();
void Draw_score();

void Set_timers(HWND);
void Kill_timers(HWND);
void Load_bitmaps();
void Get_objects();
void Initialize_objects();

void resetState();

bool pacman_colided_with_green_ghost();
bool pacman_colided_with_red_ghost();

void Uncheck(HWND, HWND);
void ButtonSwitch(HWND hwnd, HWND hwndControl);

void Draw_pacman_death();
void Draw_green_ghost();
void Draw_red_ghost();

void Draw_big_dot1(HDC hdc);
void Draw_big_dot2(HDC hdc);
void Draw_big_dot3(HDC hdc);
void Draw_big_dot4(HDC hdc);

int get_number_column(char number);
int get_number_row(char number);

typedef struct Object_info {
    int width;
    int height;
    int x;
    int y;
    int dx;
    int dy;
} Object;

bool object_can_move_right(Object& object);
bool object_can_move_left(Object& object);
bool object_can_move_up(Object& object);
bool object_can_move_down(Object& object);

int pacman_direction;
int red_ghost_direction = LEFT;
int green_ghost_direction = RIGHT;
int pacman_gender;
int gender_error;
int counter;
int score;
int numberOfLives = 3;

int callback(void *pArg, int argc, char **argv, char **imekolone);
INT_PTR CALLBACK ShowNickInput(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ShowLeaderBoard(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam);

HWND static_label;
Object background, pacman, fruit, ghost_red, ghost_green, pacman_death, ghost_weak, number, pacmanLife;
BITMAP bmp_bgnd, bmp_pacman, bmp_fruit, bmp_ghost_red, bmp_ghost_green, bmp_pacman_death, bmp_ghost_weak, bmp_number, bmp_pacman_life;
HBITMAP h_bgnd, h_pacman, h_pacman_mask, h_fruit, h_fruit_mask, h_ghost_red, h_ghost_red_mask,
        h_ghost_green, h_ghost_green_mask, h_pacman_death, h_pacman_death_mask, h_ghost_weak, h_ghost_weak_mask,
        h_number, h_number_mask, h_pacman_life, h_pacman_life_mask;
HDC hdc_mem, hdc_buffer;
HBITMAP hbm_old;
HPEN pen;
HBRUSH brush;

static int pacman_row, pacman_col, fruit_row, pacman_death_col, green_ghost_row, green_ghost_col, red_ghost_row, red_ghost_col, weak_col;

static bool draw_fruit, pacman_fruit, pacman_dead, already_ate, initial_small_dots;
static bool pacman_col_bd_1, pacman_col_bd_2, pacman_col_bd_3, pacman_col_bd_4;
static bool pacman_can_eat_ghosts, delete_green_ghost, delete_red_ghost, can_change;

int fruit_score = 10;

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WindowProcedure2(HWND, UINT, WPARAM, LPARAM);
HWND hwnd1;
HWND hwnd2;
HWND hwnd3;

POINT small_dots[n_of_s_dots] = {   {70, 155},
                                        {70, 255},
                                        {70, 360},
                                        {70, 455},
                                        {70, 555},
                                        {140, 625},
                                        {210, 625},
                                        {280, 625},
                                        {415, 625},
                                        {490, 625},
                                        {565, 625},
                                        {715, 625},
                                        {795, 625},
                                        {875, 625},
                                        {1005, 625},
                                        {1075, 625},
                                        {1145, 625},
                                        {140, 90},
                                        {210, 90},
                                        {280, 90},
                                        {415, 90},
                                        {490, 90},
                                        {565, 90},
                                        {640, 90},
                                        {715, 90},
                                        {795, 90},
                                        {875, 90},
                                        {1005, 90},
                                        {1075, 90},
                                        {1145, 90},
                                        {210, 495},
                                        {280, 495},
                                        {350, 495},
                                        {415, 495},
                                        {490, 495},
                                        {565, 495},
                                        {715, 495},
                                        {795, 495},
                                        {875, 495},
                                        {940, 495},
                                        {1005, 495},
                                        {1075, 495},
                                        {210, 225},
                                        {280, 225},
                                        {350, 225},
                                        {415, 225},
                                        {490, 225},
                                        {565, 225},
                                        {640, 225},
                                        {715, 225},
                                        {795, 225},
                                        {875, 225},
                                        {940, 225},
                                        {1005, 225},
                                        {1075, 225},
                                        {140, 360},
                                        {210, 360},
                                        {280, 360},
                                        {350, 360},
                                        {415, 360},
                                        {875, 360},
                                        {940, 360},
                                        {1005, 360},
                                        {1075, 360},
                                        {1130, 360},
                                        {1200, 155},
                                        {1200, 255},
                                        {1200, 360},
                                        {1200, 455},
                                        {1200, 555},   };

POINT big_dots[n_of_b_dots] = {   {70, 90},
                                  {1200, 90},
                                  {70, 625},
                                  {1200, 625},   };

std::map<POINT[n_of_s_dots], bool> s_dots;
std::map<POINT[n_of_b_dots], bool> b_dots;

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

    hwnd1 = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           "Home",              /* Classname */
           _T("Home"),      /* Title Text */
           WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, /* default window */
           SW/2 - MW,         /* Windows decides the position */
           SH/2 - MH,         /* where the window ends up on the screen */
           MW,                  /* The programs width */
           MH,                  /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
    );

    ShowWindow(hwnd1, nCmdShow);

    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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
                        s_dots[small_dots] = true;
                        b_dots[big_dots] = true;
                        hwnd2 = CreateWindowEx(
                           0,                   /* Extended possibilites for variation */
                           "Game",              /* Classname */
                           _T("Code::Blocks Template Windows App"),       /* Title Text */
                           WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,      /* default window */
                           (SW-GW)/2,         /* Windows decides the position */
                           (SH-GH)/2,         /* where the window ends up on the screen */
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
                    DialogBox(NULL, MAKEINTRESOURCE(IDD_BOARD), hwnd, ShowLeaderBoard);
                    break;
                }
            }
            break;
        }
        case WM_CLOSE:
            PostQuitMessage (0);
            break;
        case WM_DESTROY:{
            PostQuitMessage (0);
            break;
        }
        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

LRESULT CALLBACK WindowProcedure2(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HINSTANCE hinst = (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE);
    switch (message)
    {
        case WM_KEYDOWN:
        {
            switch (wParam) {
                case VK_LEFT:
                    if(pacman_dead == false && object_can_move_left(pacman))
                        pacman_direction = LEFT;
                    break;
                case VK_RIGHT:
                    if(pacman_dead == false && object_can_move_right(pacman))
                        pacman_direction = RIGHT;
                    break;
                case VK_UP:
                    if(pacman_dead == false && object_can_move_up(pacman))
                        pacman_direction = UP;
                    break;
                case VK_DOWN:
                    if(pacman_dead == false && object_can_move_down(pacman))
                        pacman_direction = DOWN;
                    break;
            }
            break;
        }
        case WM_CREATE:
        {
            PlaySound("pacman_beginning.wav", NULL, SND_FILENAME | SND_ASYNC);

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
            Draw_scene(hwnd, hdc, &rect);
            ReleaseDC(hwnd, hdc);

            switch (wParam) {
                case PACMAN_TIMER:
                {
                    if(pacman_direction == LEFT && object_can_move_left(pacman))
                        Move_pacman_left();
                    if(pacman_direction == RIGHT && object_can_move_right(pacman))
                        Move_pacman_right();
                    if(pacman_direction == UP && object_can_move_up(pacman))
                        Move_pacman_up();
                    if(pacman_direction == DOWN && object_can_move_down(pacman))
                        Move_pacman_down();

                    break;
                }
                case RED_GHOST_TIMER:
                {
                    if ((red_ghost_direction == LEFT || ghost_red.y == 180) && object_can_move_left(ghost_red)) {
                        red_ghost_direction = LEFT;
                        red_ghost_col = 1;
                        red_ghost_row = 1;
                        ghost_red.x -= ghost_red.dx;
                        break;
                    }

                    if ((red_ghost_direction == LEFT || red_ghost_direction == DOWN) && object_can_move_down(ghost_red)) {
                        red_ghost_direction = DOWN;
                        red_ghost_col = 0;
                        red_ghost_row = 1;
                        ghost_red.y += ghost_red.dy;
                        break;
                    }

                    if ((red_ghost_direction == RIGHT || red_ghost_direction == DOWN) && object_can_move_right(ghost_red)) {
                        red_ghost_direction = RIGHT;
                        red_ghost_col = 0;
                        red_ghost_row = 0;
                        ghost_red.x += ghost_red.dx;
                        break;
                    }

                    if ((red_ghost_direction == RIGHT || red_ghost_direction == UP) && object_can_move_up(ghost_red)) {
                        red_ghost_direction = UP;
                        red_ghost_col = 1;
                        red_ghost_row = 0;
                        ghost_red.y -= ghost_red.dy;
                        break;
                    }
                }
                case GREEN_GHOST_TIMER:
                {
                    if ((green_ghost_direction == RIGHT || ghost_green.y == 180) && object_can_move_right(ghost_green)) {
                        green_ghost_direction = RIGHT;
                        green_ghost_col = 0;
                        green_ghost_row = 0;
                        ghost_green.x += ghost_green.dx;
                        break;
                    }

                    if ((green_ghost_direction == RIGHT || green_ghost_direction == DOWN) && object_can_move_down(ghost_green)) {
                        green_ghost_direction = DOWN;
                        green_ghost_col = 0;
                        green_ghost_row = 1;
                        ghost_green.y += ghost_green.dy;
                        break;
                    }

                    if ((green_ghost_direction == LEFT || green_ghost_direction == DOWN) && object_can_move_left(ghost_green)) {
                        green_ghost_direction = LEFT;
                        green_ghost_col = 1;
                        green_ghost_row = 1;
                        ghost_green.x -= ghost_green.dx;
                        break;
                    }

                    if ((green_ghost_direction == LEFT || green_ghost_direction == UP) && object_can_move_up(ghost_green)) {
                        green_ghost_direction = UP;
                        green_ghost_col = 1;
                        green_ghost_row = 0;
                        ghost_green.y -= ghost_green.dy;
                        break;
                    }
                }
                case DEATH_TIMER:
                {
                    if (pacman_dead) {
                        if (pacman_death_col < 10)
                            pacman_death_col++;
                        counter++;
                    } else {
                        pacman_death_col = 0;
                    }

                    if (pacman_dead && counter == 10) {
                        DialogBox(NULL, MAKEINTRESOURCE(IDD_INPUT), hwnd2, ShowNickInput);
                        Kill_timers(hwnd);
                        ShowWindow(hwnd, 0);
                        resetState();
                        EndDialog(hwnd, 0);
                    }
                    break;
                }
                case EAT_GHOST_TIMER:
                {
                    delete_green_ghost = false;
                    delete_red_ghost = false;
                    ghost_green.x = 600;
                    ghost_green.y = 180;
                    ghost_red.x = 600;
                    ghost_red.y = 180;
                    pacman_can_eat_ghosts = false;

                    KillTimer(hwnd, EAT_GHOST_TIMER);
                    KillTimer(hwnd, WEAK_TIMER_CHANGE_START);
                    KillTimer(hwnd, WEAK_TIMER_CHANGE);
                    pacman_can_eat_ghosts = false;

                    can_change = false;
                    break;
                }
                case WEAK_TIMER_CHANGE:
                {
                    if (weak_col == 0)
                        weak_col = 1;
                    else if (weak_col == 1)
                        weak_col = 0;

                    break;
                }
                case WEAK_TIMER_CHANGE_START:
                {
                    SetTimer(hwnd, WEAK_TIMER_CHANGE, 300, NULL);
                    break;
                }
                case FRUIT_TIMER:
                {
                    already_ate = false;
                    pacman_fruit = false;
                    KillTimer(hwnd, FRUIT_TIMER);
                    break;
                }
                default:
                {
                    if (can_change == true) {
                        SetTimer(hwnd, EAT_GHOST_TIMER, 8000, NULL);
                        SetTimer(hwnd, WEAK_TIMER_CHANGE_START, 4000, NULL);
                        can_change = false;
                    }
                }
            }
            break;
        }
        case WM_CLOSE:
            Kill_timers(hwnd);
            ShowWindow(hwnd, 0);
            resetState();
            EndDialog(hwnd, 0);
            break;
        case WM_DESTROY:
            Kill_timers(hwnd);
            ShowWindow(hwnd, 0);
            PostQuitMessage (0);
            break;
        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

void Set_timers(HWND hwnd) {
    SetTimer(hwnd, ID_TIMER, 40, NULL);
    SetTimer(hwnd, PACMAN_TIMER, 70, NULL);
    SetTimer(hwnd, DEATH_TIMER, 130, NULL);
    SetTimer(hwnd, GREEN_GHOST_TIMER, 70, NULL);
    SetTimer(hwnd, RED_GHOST_TIMER, 70, NULL);
}

void Kill_timers(HWND hwnd) {
    KillTimer(hwnd, ID_TIMER);
    KillTimer(hwnd, PACMAN_TIMER);
    KillTimer(hwnd, DEATH_TIMER);
    KillTimer(hwnd, GREEN_GHOST_TIMER);
    KillTimer(hwnd, RED_GHOST_TIMER);
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
        MessageBox(hwnd1, "Niste odabrali gender", "Failure", MB_OK);
        gender_error = 1;
        return;
    }

    h_fruit = (HBITMAP) LoadImage(NULL, "fruits.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    h_fruit_mask = (HBITMAP) LoadImage(NULL, "fruits_mask.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    h_pacman_death = (HBITMAP) LoadImage(NULL, "pacman_death.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    h_pacman_death_mask = (HBITMAP) LoadImage(NULL, "pacman_death_mask.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    h_ghost_red = (HBITMAP) LoadImage(NULL, "red_ghost.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    h_ghost_red_mask = (HBITMAP) LoadImage(NULL, "red_ghost_mask.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    h_ghost_green = (HBITMAP) LoadImage(NULL, "green_ghost.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    h_ghost_green_mask = (HBITMAP) LoadImage(NULL, "green_ghost_mask.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    h_ghost_weak = (HBITMAP) LoadImage(NULL, "ghost_weak.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    h_ghost_weak_mask = (HBITMAP) LoadImage(NULL, "ghost_weak_mask.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    h_number = (HBITMAP) LoadImage(NULL, "number.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    h_number_mask = (HBITMAP) LoadImage(NULL, "number_mask.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    h_pacman_life = (HBITMAP) LoadImage(NULL, "pacman_life.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    h_pacman_life_mask = (HBITMAP) LoadImage(NULL, "pacman_life_mask.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

void Get_objects() {
    GetObject(h_bgnd, sizeof(BITMAP), &bmp_bgnd);
    GetObject(h_pacman, sizeof(BITMAP), &bmp_pacman);
    GetObject(h_fruit, sizeof(BITMAP), &bmp_fruit);
    GetObject(h_pacman_death, sizeof(BITMAP), &bmp_pacman_death);
    GetObject(h_pacman_death, sizeof(BITMAP), &bmp_pacman_death);
    GetObject(h_ghost_red, sizeof(BITMAP), &bmp_ghost_red);
    GetObject(h_ghost_green, sizeof(BITMAP), &bmp_ghost_green);
    GetObject(h_ghost_weak, sizeof(BITMAP), &bmp_ghost_weak);
    GetObject(h_number, sizeof(BITMAP), &bmp_number);
    GetObject(h_pacman_life, sizeof(BITMAP), &bmp_pacman_life);
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

    pacman_death.width = bmp_pacman_death.bmWidth;
    pacman_death.height = bmp_pacman_death.bmHeight;
    pacman_death.dx = 0;
    pacman_death.dy = 0;
    pacman_death.x = 0;
    pacman_death.y = 0;

    ghost_red.width = bmp_ghost_red.bmWidth / 2;
    ghost_red.height = bmp_ghost_red.bmHeight / 2;
    ghost_red.dx = 10;
    ghost_red.dy = 10;
    ghost_red.x = 600;
    ghost_red.y = 180;

    ghost_green.width = bmp_ghost_green.bmWidth / 2;
    ghost_green.height = bmp_ghost_green.bmHeight / 2;
    ghost_green.dx = 10;
    ghost_green.dy = 10;
    ghost_green.x = 600;
    ghost_green.y = 180;

    ghost_weak.width = bmp_ghost_weak.bmWidth / 2;
    ghost_weak.height = bmp_ghost_weak.bmHeight;
    ghost_weak.dx = 10;
    ghost_weak.dy = 10;

    number.width = bmp_number.bmWidth / 5;
    number.height = bmp_number.bmHeight / 2;
    number.dx = 0;
    number.dy = 0;
    number.y = 717;
    number.x = 50;

    pacmanLife.width = bmp_pacman_life.bmWidth;
    pacmanLife.height = bmp_pacman_life.bmHeight;
    pacmanLife.dx = 0;
    pacmanLife.dy = 0;
    pacmanLife.y = 717;
    pacmanLife.x = 1200;
}

void Update_scene(RECT* rect) {

}

void Draw_bgnd() {
    hbm_old = (HBITMAP) SelectObject(hdc_mem, h_bgnd);
    BitBlt(hdc_buffer, background.x, background.y, background.width, background.height, hdc_mem, 0, 0, SRCCOPY);
}

void Draw_pacman() {
    hbm_old = (HBITMAP) SelectObject(hdc_mem, h_pacman);
    BitBlt(hdc_buffer, pacman.x, pacman.y + 10, pacman.width / 2, pacman.height / 4, hdc_mem, pacman_col * pacman.width / 2, pacman_row * pacman.height / 4, SRCAND);

    hbm_old = (HBITMAP) SelectObject(hdc_mem, h_pacman_mask);
    if(pacman_direction == LEFT)
        BitBlt(hdc_buffer, pacman.x, pacman.y + 10, pacman.width / 2, pacman.height / 4, hdc_mem, pacman_col * pacman.width / 2, pacman_row * pacman.height / 4, SRCPAINT);
    else
        BitBlt(hdc_buffer, pacman.x, pacman.y + 10, pacman.width / 2, pacman.height / 4, hdc_mem, pacman_col * pacman.width / 2, pacman_row * pacman.height / 4, SRCPAINT);
}


void Draw_green_ghost() {
    if (pacman_can_eat_ghosts) {
        hbm_old = (HBITMAP) SelectObject(hdc_mem, h_ghost_weak);
        BitBlt(hdc_buffer, ghost_green.x, ghost_green.y, ghost_weak.width, ghost_weak.height, hdc_mem, ghost_weak.width * weak_col, 0 * ghost_weak.height, SRCAND);

        hbm_old = (HBITMAP) SelectObject(hdc_mem, h_ghost_weak_mask);
        BitBlt(hdc_buffer, ghost_green.x, ghost_green.y, ghost_weak.width, ghost_weak.height, hdc_mem, ghost_weak.width * weak_col, 0 * ghost_weak.height, SRCPAINT);
    } else {
        hbm_old = (HBITMAP) SelectObject(hdc_mem, h_ghost_green);
        BitBlt(hdc_buffer, ghost_green.x, ghost_green.y, ghost_green.width, ghost_green.height, hdc_mem, ghost_green.width * green_ghost_col, ghost_green.height * green_ghost_row, SRCPAINT);

        hbm_old = (HBITMAP) SelectObject(hdc_mem, h_ghost_green_mask);
        BitBlt(hdc_buffer, ghost_green.x, ghost_green.y, ghost_green.width, ghost_green.height, hdc_mem, ghost_green.width * green_ghost_col, ghost_green.height * green_ghost_row, SRCAND);
    }
}

void Draw_red_ghost() {
    if (pacman_can_eat_ghosts) {
        hbm_old = (HBITMAP) SelectObject(hdc_mem, h_ghost_weak);
        BitBlt(hdc_buffer, ghost_red.x, ghost_red.y, ghost_weak.width, ghost_weak.height, hdc_mem, ghost_weak.width * weak_col, 0 * ghost_weak.height, SRCAND);

        hbm_old = (HBITMAP) SelectObject(hdc_mem, h_ghost_weak_mask);
        BitBlt(hdc_buffer, ghost_red.x, ghost_red.y, ghost_weak.width, ghost_weak.height, hdc_mem, ghost_weak.width * weak_col, 0 * ghost_weak.height, SRCPAINT);
    } else {
        hbm_old = (HBITMAP) SelectObject(hdc_mem, h_ghost_red);
        BitBlt(hdc_buffer, ghost_red.x, ghost_red.y, ghost_red.width, ghost_red.height, hdc_mem, ghost_red.width * red_ghost_col, ghost_red.height * red_ghost_row, SRCPAINT);

        hbm_old = (HBITMAP) SelectObject(hdc_mem, h_ghost_red_mask);
        BitBlt(hdc_buffer, ghost_red.x, ghost_red.y, ghost_red.width, ghost_red.height, hdc_mem, ghost_red.width * red_ghost_col, ghost_red.height * red_ghost_row, SRCAND);
    }
}

int get_number_column(char number) {
    if(number == '0' || number == '6')
        return 0;

    if(number == '2' || number == '7')
        return 1;

    if(number == '3' || number == '8')
        return 2;

    if(number == '4' || number == '9')
        return 3;

    if(number == '5' || number == '1')
        return 4;
}
int get_number_row(char number) {
    if(number == '0' || number == '2' || number == '3' || number == '4' || number == '5')
        return 0;
    else
        return 1;
}


void Draw_score(){
    std::string displayScore = std::to_string(score);

    int shiftAmount = 0;

    for(const char& digit : displayScore) {
        hbm_old = (HBITMAP) SelectObject(hdc_mem, h_number);
        BitBlt(hdc_buffer, number.x + shiftAmount, number.y, number.width, number.height, hdc_mem, number.width * get_number_column(digit), number.height * get_number_row(digit), SRCPAINT);

        hbm_old = (HBITMAP) SelectObject(hdc_mem, h_number_mask);
        BitBlt(hdc_buffer, number.x + shiftAmount, number.y, number.width, number.height, hdc_mem, number.width * get_number_column(digit), number.height * get_number_row(digit), SRCAND);
        shiftAmount += number.width;
    }
}

void Draw_lives() {
    int shiftAmount = 0;

    for(int i; i < numberOfLives; ++i){
        hbm_old = (HBITMAP) SelectObject(hdc_mem, h_pacman_life);
        BitBlt(hdc_buffer, pacmanLife.x - shiftAmount, pacmanLife.y, pacmanLife.width, pacmanLife.height, hdc_mem, 0, 0, SRCPAINT);

        hbm_old = (HBITMAP) SelectObject(hdc_mem, h_pacman_life_mask);
        BitBlt(hdc_buffer, pacmanLife.x - shiftAmount, pacmanLife.y, pacmanLife.width, pacmanLife.height, hdc_mem, 0, 0, SRCAND);
        shiftAmount += pacmanLife.width;
    }
}

void Draw_scene(HWND hwnd, HDC hdc, RECT* rect) {
    hdc_buffer = CreateCompatibleDC(hdc);
    HBITMAP hbm_buffer = CreateCompatibleBitmap(hdc, rect->right, rect->bottom);
    HBITMAP hbm_old_buffer = (HBITMAP) SelectObject(hdc_buffer, hbm_buffer);

    hdc_mem = CreateCompatibleDC(hdc);

    Draw_bgnd();

    SelectObject(hdc, brush);
    SelectObject(hdc, pen);

    Draw_big_dot1(hdc_buffer);
    Draw_big_dot2(hdc_buffer);
    Draw_big_dot3(hdc_buffer);
    Draw_big_dot4(hdc_buffer);

    if (pacman_dead == false) {
        Draw_pacman();
        if(!delete_green_ghost)
            Draw_green_ghost();

        if(!delete_red_ghost)
            Draw_red_ghost();
    } else if (pacman_dead && !delete_green_ghost && !delete_red_ghost) {
        Draw_pacman_death();
    } else if (!pacman_dead && delete_green_ghost && delete_red_ghost) {
        Draw_pacman();
    }

    if ((pacman.x >= fruit.x - fruit.width / 4 && pacman.x <= fruit.x + fruit.width / 4) &&
        (pacman.y >= fruit.y - fruit.height / 16 && pacman.y <= fruit.y + fruit.height / 16)) {
        if (!already_ate && !pacman_dead) {
            PlaySound("pacman_eat_fruit.wav", NULL, SND_FILENAME | SND_ASYNC);
            already_ate = true;
            pacman_fruit = true;
            score += fruit_score;
            fruit_score += 10;
            SetTimer(hwnd, FRUIT_TIMER, 1500, NULL);
            RedrawWindow(static_label, NULL, NULL, RDW_ERASE);
            fruit_row = (fruit_row == 3) ? 0 : fruit_row + 1;
        }
    }

    if (pacman.y == 50 && pacman.x == 30 && !pacman_col_bd_1) {
        pacman_col_bd_1 = true;
        pacman_can_eat_ghosts = true;
        can_change = true;
        PlaySound("pacman_intermission.wav", NULL, SND_FILENAME | SND_ASYNC);
    }

    if (pacman.y == 50 && pacman.x == 1150 && !pacman_col_bd_2) {
        pacman_col_bd_2 = true;
        pacman_can_eat_ghosts = true;
        can_change = true;
        PlaySound("pacman_intermission.wav", NULL, SND_FILENAME | SND_ASYNC);
    }

    if (pacman.y == 570 && pacman.x == 30 && !pacman_col_bd_3) {
        pacman_col_bd_3 = true;
        pacman_can_eat_ghosts = true;
        can_change = true;
        PlaySound("pacman_intermission.wav", NULL, SND_FILENAME | SND_ASYNC);
    }

    if (pacman.y == 570 && pacman.x == 1150 && !pacman_col_bd_4) {
        pacman_col_bd_4 = true;
        pacman_can_eat_ghosts = true;
        can_change = true;
        PlaySound("pacman_intermission.wav", NULL, SND_FILENAME | SND_ASYNC);
    }

    if (pacman_fruit == false) {
        Draw_fruit();
    }

    if (((pacman_colided_with_green_ghost() && !delete_green_ghost) ||
         (pacman_colided_with_red_ghost() && !delete_red_ghost)) &&
         !pacman_can_eat_ghosts) {
        if(numberOfLives > 1){
            numberOfLives--;
            pacman.x = 600;
            pacman.y = 570;
        } else {
            pacman_death.x = pacman.x;
            pacman_death.y = pacman.y;
            pacman.x++;
            pacman.y++;
            pacman_dead = true;
            PlaySound("pacman_death.wav", NULL, SND_FILENAME | SND_ASYNC);
        }
    } else if (pacman_colided_with_green_ghost() && pacman_can_eat_ghosts && !delete_green_ghost) {
        weak_col = 0;
        delete_green_ghost = true;
        score = score + 200;
    } else if (pacman_colided_with_red_ghost() && pacman_can_eat_ghosts && !delete_red_ghost) {
        weak_col = 0;
        delete_red_ghost = true;
        score = score + 200;
    }
    Draw_score();
    Draw_lives();

    Draw_score();
    Draw_lives();

    BitBlt(hdc, 0, 0, rect->right, rect->bottom, hdc_buffer, 0, 0, SRCCOPY);

    SelectObject(hdc_mem, hbm_old);
    DeleteObject(hdc_mem);
    DeleteObject(hbm_buffer);

    SelectObject(hdc_buffer, hbm_old_buffer);
    DeleteObject(hdc_buffer);
    DeleteObject(hbm_old_buffer);
}

void Move_pacman_left() {
    printf("%d\n", pacman.x);
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
    printf("%d\n", pacman.x);
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
    printf("%d\n", pacman.x);
    PlaySound("pacman_chomp.wav", NULL, SND_FILENAME | SND_ASYNC | SND_NOSTOP);
    pacman_row = 2;

    pacman.y -= pacman.dy;

    if (pacman_col == 0)
        pacman_col = 1;
    else
        pacman_col = 0;
}

void Move_pacman_down() {
    printf("%d\n", pacman.x);
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
    lvItem.pszText = strcat(strcat(argv[1], " "), argv[2]);
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

                    sprintf(sql, "SELECT * FROM leaderboard ORDER BY score ASC");

                    status = sqlite3_exec(db, sql, callback, 0, &err);
                    if(status == SQLITE_OK) {
                        MessageBox(hdlg, "Prikaz leaderboarda uspjesan", "Success", MB_OK);
                        sqlite3_free(err);
                    } else {
                        MessageBox(hdlg, "Greska", "Failure", MB_OK);
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

INT_PTR CALLBACK ShowNickInput(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam){
    int status;
    char* err = 0;
    switch(message) {
	    case WM_INITDIALOG: {
            sqlite3_open("baza", &db);
            return TRUE;
	    }
	    case WM_COMMAND: {
	        switch(LOWORD(wParam))
			{
                case IDC_CONFIRM:
                {
                    char sql[200], temp_nick[200];

                    temp_nick[0] = 0;

                    HWND hwnd_nick = GetDlgItem(hdlg, IDC_INPUT_NICK);
                    GetWindowText(hwnd_nick, temp_nick, sizeof(temp_nick));
                    if(temp_nick[0] == 0) {
                        MessageBox(hdlg, "Potrebno je unijeti nick", "Greska", MB_OK);
                        break;
                    }

                    sprintf(sql, "INSERT INTO leaderboard (nick, score) VALUES ('%s', '%d')", temp_nick, score);

                    status = sqlite3_exec(db, sql, 0, 0, &err);

                    if(status == SQLITE_OK) {
                        MessageBox(hdlg, "Vas rezultat je unesen", "Success", MB_OK);
                        sqlite3_free(err);
                        sqlite3_close(db);
                        EndDialog(hdlg, 0);
                        break;
                    } else if (status == SQLITE_CONSTRAINT) {
                        MessageBox(hdlg, "Nick vec postoji", "Failure", MB_OK);
                        break;
                    } else {
                        MessageBox(hdlg, "Greska", "Failure", MB_OK);
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

bool object_can_move_right(Object& object){
    if(pacman_dead)
        return false;

    if(object.y == 310 && object.x > 819 && object.x >= 1190)
        object.x = 0;

    if(object.y == 570 && object.x > 359 && object.x < 820)
        return true;

    if(object.y == 570 && object.x > 29 && object.x < 230)
        return true;

    if(object.y == 570 && object.x > 949 && object.x < 1150)
        return true;

    if(object.y == 440 && object.x > 159 && object.x < 1020)
        return true;

    if(object.y == 180 && object.x > 159 && object.x < 1020)
        return true;

    if(object.y == 50 && object.x > 359 && object.x < 820)
        return true;

    if(object.y == 50 && object.x > 29 && object.x < 230)
        return true;

    if(object.y == 50 && object.x > 949 && object.x < 1150)
        return true;

    if(object.y == 310 && object.x >= 0 && object.x < 360)
        return true;

    if(object.y == 310 && object.x > 819 && object.x < 1200)
        return true;

    return false;
}

bool object_can_move_left(Object& object){
    if(pacman_dead)
        return false;

    if(object.y == 310 && object.x == 0 && object.x < 361)
        object.x = 1190;

    if(object.y == 570 && object.x > 360 && object.x < 821)
        return true;

    if(object.y == 570 && object.x > 30 && object.x < 231)
        return true;

    if(object.y == 570 && object.x > 950 && object.x < 1151)
        return true;

    if(object.y == 440 && object.x > 160 && object.x < 1021)
        return true;

    if(object.y == 180 && object.x > 160 && object.x < 1021)
        return true;

    if(object.y == 50 && object.x > 360 && object.x < 821)
        return true;

    if(object.y == 50 && object.x > 30 && object.x < 231)
        return true;

    if(object.y == 50 && object.x > 950 && object.x < 1151)
        return true;

    if(object.y == 310 && object.x >= 0 && object.x < 361)
        return true;

    if(object.y == 310 && object.x > 820 && object.x <= 1190)
        return true;

    return false;
}

bool object_can_move_up(Object& object) {
    if(pacman_dead)
        return false;

    if(object.x == 360 && object.y < 571 && object.y > 50)
        return true;

    if(object.x == 820 && object.y < 571 && object.y > 50)
        return true;

    if(object.x == 1150 && object.y < 571 && object.y > 50)
        return true;

    if(object.x == 30 && object.y < 571 && object.y > 50)
        return true;

    if(object.x == 160 && object.y < 441 && object.y > 180)
        return true;

    if(object.x == 1020 && object.y < 441 && object.y > 180)
        return true;

    if(object.x == 230 && object.y < 181 && object.y > 50)
        return true;

    if(object.x == 950 && object.y < 181 && object.y > 50)
        return true;

    if(object.x == 230 && object.y < 571 && object.y > 440)
        return true;

    if(object.x == 950 && object.y < 571 && object.y > 440)
        return true;

    return false;
}

bool object_can_move_down(Object& object) {
    if(pacman_dead)
        return false;

    if(object.x == 360 && object.y < 570 && object.y > 49)
        return true;

    if(object.x == 820 && object.y < 570 && object.y > 49)
        return true;

    if(object.x == 1150 && object.y < 570 && object.y > 49)
        return true;

    if(object.x == 30 && object.y < 570 && object.y > 49)
        return true;

    if(object.x == 160 && object.y < 440 && object.y > 179)
        return true;

    if(object.x == 1020 && object.y < 440 && object.y > 179)
        return true;

    if(object.x == 230 && object.y < 180 && object.y > 49)
        return true;

    if(object.x == 950 && object.y < 180 && object.y > 49)
        return true;

    if(object.x == 230 && object.y < 570 && object.y > 439)
        return true;

    if(object.x == 950 && object.y < 570 && object.y > 439)
        return true;

    return false;
}

bool pacman_colided_with_green_ghost() {
    if(pacman.x == ghost_green.x && pacman.y == ghost_green.y)
        return true;

    return false;
}

bool pacman_colided_with_red_ghost() {
    if(pacman.x == ghost_red.x && pacman.y == ghost_red.y)
        return true;

    return false;
}

void Draw_pacman_death() {
    hbm_old = (HBITMAP) SelectObject(hdc_mem, h_pacman_death);
    BitBlt(hdc_buffer, pacman_death.x, pacman_death.y, pacman_death.width / 11, pacman_death.height, hdc_mem, pacman_death_col * pacman_death.width / 11, 0 * pacman_death.height, SRCAND);

    hbm_old = (HBITMAP) SelectObject(hdc_mem, h_pacman_death_mask);
    BitBlt(hdc_buffer, pacman_death.x, pacman_death.y, pacman_death.width / 11, pacman_death.height, hdc_mem, pacman_death_col * pacman_death.width / 11, 0 * pacman_death.height, SRCPAINT);
}

void Draw_big_dot1(HDC hdc) {
    if (pacman_col_bd_1 == false) {
        Ellipse(hdc, big_dots[0].x - b_dimension / 2, big_dots[0].y + b_dimension / 2, big_dots[0].x + b_dimension / 2, big_dots[0].y - b_dimension / 2);
    }
}

void Draw_big_dot2(HDC hdc) {
    if (pacman_col_bd_2 == false) {
        Ellipse(hdc, big_dots[1].x - b_dimension / 2, big_dots[1].y + b_dimension / 2, big_dots[1].x + b_dimension / 2, big_dots[1].y - b_dimension / 2);
    }
}

void Draw_big_dot3(HDC hdc) {
    if (pacman_col_bd_3 == false) {
        Ellipse(hdc, big_dots[2].x - b_dimension / 2, big_dots[2].y + b_dimension / 2, big_dots[2].x + b_dimension / 2, big_dots[2].y - b_dimension / 2);
    }
}

void Draw_big_dot4(HDC hdc) {
    if (pacman_col_bd_4 == false) {
        Ellipse(hdc, big_dots[3].x - b_dimension / 2, big_dots[3].y + b_dimension / 2, big_dots[3].x + b_dimension / 2, big_dots[3].y - b_dimension / 2);
    }
}

void resetState() {
    pacman_direction = 0;
    red_ghost_direction = LEFT;
    green_ghost_direction = RIGHT;
    score = 0;
    numberOfLives = 3;

    pacman.x = 600;
    pacman.y = 570;

    ghost_green.x = 600;
    ghost_green.y = 180;

    ghost_red.x = 600;
    ghost_red.y = 180;

    pacman_row = 0;
    pacman_col = 0;
    fruit_row = 0;
    pacman_death_col = 0;
    green_ghost_row = 0;
    green_ghost_col = 0;
    red_ghost_row = 0;
    red_ghost_col = 0;
    weak_col = 0;

    draw_fruit = false;
    pacman_fruit = false;
    pacman_dead = false;
    already_ate = false;
    initial_small_dots = false;
    pacman_col_bd_1 = false;
    pacman_col_bd_2 = false;
    pacman_col_bd_3 = false;
    pacman_col_bd_4 = false;
    pacman_can_eat_ghosts = false;
    delete_green_ghost = false;
    delete_red_ghost = false;
    can_change= false;

    fruit_score = 10;

    counter = 0;
}

//void Draw_small_dots(HDC hdc) {
//
//}

//void Draw_big_dots(HDC hdc) {
//    static int i = 0;
//    for (auto& dot : b_dots) {
////        if (pacman.x >= big_dots[i].x - 10 && pacman.x <= big_dots[i].x + 10) {
////            dot.second = false;
////            printf("pojeo\n");
////        }
//        if (pacman.x == 50 || pacman.x == 1150) {
//            //if (pacman.y == 90 || pacman.y == 625) {
//                printf("pojeo\n");
//           // }
//        } else {
//            Ellipse(hdc, big_dots[i].x - b_dimension / 2, big_dots[i].y + b_dimension / 2, big_dots[i].x + b_dimension / 2, big_dots[i].y - b_dimension / 2);
//        }
////        if (dot.second == true) {
////            Ellipse(hdc, big_dots[i].x - b_dimension / 2, big_dots[i].y + b_dimension / 2, big_dots[i].x + b_dimension / 2, big_dots[i].y - b_dimension / 2);
////        }
//        if (i < n_of_b_dots) {
//            ++i;
//        } else {
//            i = 0;
//        }
//    }
//}

//POINT big_dots[n_of_b_dots] = {   {70, 90},
//                                  {1200, 90},
//                                  {70, 625},
//                                  {1200, 625},   };
