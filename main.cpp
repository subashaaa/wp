#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <unistd.h>
#define ID_TIMER 1
#define SW 1920
#define SH 1080
#define WW 1280
#define WH 720

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

Object background, pacman;
BITMAP bmp_bgnd, bmp_pacman;
HBITMAP h_bgnd, h_pacman, h_pacman_mask;
HDC hdc_mem, hdc_buffer;
HBITMAP hbm_old;

static int pacman_row, pacman_col;

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
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

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Code::Blocks Template Windows App"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           (SW - WW)/2,       /* Windows decides the position */
           (SH - WH)/2,       /* where the window ends up on the screen */
           WW,                 /* The programs width */
           WH,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

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


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
}

void Kill_timers(HWND hwnd) {
    KillTimer(hwnd, ID_TIMER);
}

void Load_bitmaps() {
    h_bgnd = (HBITMAP) LoadImage(NULL, "bgnd.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    h_pacman = (HBITMAP) LoadImage(NULL, "pacman_move.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    h_pacman_mask = (HBITMAP) LoadImage(NULL, "pacman_move_mask.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

void Get_objects() {
    GetObject(h_bgnd, sizeof(BITMAP), &bmp_bgnd);
    GetObject(h_pacman, sizeof(BITMAP), &bmp_pacman);
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
    Draw_pacman();

    BitBlt(hdc, 0, 0, rect->right, rect->bottom, hdc_buffer, 0, 0, SRCCOPY);

    SelectObject(hdc_mem, hbm_old);
    DeleteObject(hdc_mem);
    DeleteObject(hbm_buffer);

    SelectObject(hdc_buffer, hbm_old_buffer);
    DeleteObject(hdc_buffer);
    DeleteObject(hbm_old_buffer);
}

void Move_pacman_left() {
    pacman_row = 0;
    if (pacman.x > 0)
        pacman.x -= pacman.dx;

    if (pacman_col == 0)
        pacman_col = 1;
    else
        pacman_col = 0;
}

void Move_pacman_right() {
    pacman_row = 1;

    if (pacman.x < 1280 - pacman.width / 2)
        pacman.x += pacman.dx;

    if (pacman_col == 0)
        pacman_col = 1;
    else
        pacman_col = 0;
}

void Move_pacman_up() {
    pacman_row = 2;

    pacman.y -= pacman.dy;

    if (pacman_col == 0)
        pacman_col = 1;
    else
        pacman_col = 0;
}

void Move_pacman_down() {
    pacman_row = 3;

    if (pacman.y < 700 - pacman.height / 4)
        pacman.y += pacman.dy;

    if (pacman_col == 0)
        pacman_col = 1;
    else
        pacman_col = 0;
}
