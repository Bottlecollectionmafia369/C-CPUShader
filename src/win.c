//Initialize the window and handle rendering in multiple threads

#include "defines.h"
#include "win.h"

static int         g_width   = 0;
static int         g_height  = 0;
static const char *g_title   = NULL;
static UINT32     *g_pixels  = NULL;
static vec4_t     *g_fpixels = NULL;
static BITMAPINFO  g_bmi;
static HWND        g_hwnd    = NULL;
static int         g_is_open = 0;
static ULONGLONG   g_start_time = 0;
static uint        g_frame_counter = 0;
static bool        g_tempolatal_accumulation = false;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            StretchDIBits(hdc,
                0, 0, g_width, g_height,
                0, 0, g_width, g_height,
                g_pixels, &g_bmi, DIB_RGB_COLORS, SRCCOPY);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            g_is_open = 0;
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcA(hwnd, msg, wp, lp);
}

void window_create(const char *title, int width, int height)
{
    g_width  = width;
    g_height = height;
    g_title  = title;
    g_pixels = (UINT32 *)malloc(width * height * sizeof(UINT32));
    g_fpixels = (vec4_t *)malloc(width * height * sizeof(vec4_t));
    g_start_time = GetTickCount64();

    g_bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    g_bmi.bmiHeader.biWidth       = width;
    g_bmi.bmiHeader.biHeight      = height;
    g_bmi.bmiHeader.biPlanes      = 1;
    g_bmi.bmiHeader.biBitCount    = 32;
    g_bmi.bmiHeader.biCompression = BI_RGB;

    HINSTANCE hInst = GetModuleHandle(NULL);

    WNDCLASSA wc    = {0};
    wc.lpfnWndProc  = WndProc;
    wc.hInstance    = hInst;
    wc.hCursor      = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = title;
    RegisterClassA(&wc);

    RECT rect = { 0, 0, width, height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    g_hwnd = CreateWindowA(
        title, title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL, NULL, hInst, NULL);

    g_is_open = 1;
}

void update_pixel(int x, int y)
{
    vec4_t color = g_fpixels[y * g_width + x];

    color.x = fmaxf(0.0f, fminf(1.0f, color.x));
    color.y = fmaxf(0.0f, fminf(1.0f, color.y));
    color.z = fmaxf(0.0f, fminf(1.0f, color.z));
    color.w = fmaxf(0.0f, fminf(1.0f, color.w));

    int r = (int)(color.x * 255.0f);
    int g = (int)(color.y * 255.0f);
    int b = (int)(color.z * 255.0f);
    g_pixels[y * g_width + x] = (UINT32)((r & 0xFF) << 16 | (g & 0xFF) <<  8 | (b & 0xFF));
}

typedef struct {
    int        y_start;
    int        y_end;
    uint       current_frame;
    ULONGLONG  current_time;
    RenderFunc render;
} Worker;

static int      g_num_threads = 0;
static Worker  *g_workers     = NULL;
static HANDLE  *g_threads     = NULL;
static HANDLE   g_work_sem;
static HANDLE   g_done_sem;
static volatile int g_shutdown = 0;

static DWORD WINAPI worker_thread(LPVOID arg)
{
    Worker *w = (Worker *)arg;
    while (1) {
        WaitForSingleObject(g_work_sem, INFINITE);
        if (g_shutdown) break;

        for (int y = w->y_start; y < w->y_end; y++) {
            for (int x = 0; x < g_width; x++){
                vec4_t color = w->render(vec2(x, y), vec2(g_width, g_height), (float)(w->current_time - g_start_time) * 0.001f, w->current_frame);

                if(g_tempolatal_accumulation){
                    vec4_t old_color = g_fpixels[y * g_width + x];
                    float weight = 1.0 / (w->current_frame + 1);
                    color = v4_add(v4_mul1(old_color, 1.0 - weight), v4_mul1(color, weight));
                }

                g_fpixels[y * g_width + x] = color;
                update_pixel(x, y);
            }
        }

        ReleaseSemaphore(g_done_sem, 1, NULL); 
    }
    return 0;
}

static void pool_create(int num_threads, RenderFunc render)
{
    g_num_threads = num_threads;
    g_workers     = (Worker *)malloc(num_threads * sizeof(Worker));
    g_threads     = (HANDLE *)malloc(num_threads * sizeof(HANDLE));
    g_work_sem    = CreateSemaphore(NULL, 0, num_threads, NULL);
    g_done_sem    = CreateSemaphore(NULL, 0, num_threads, NULL);

    int rows = g_height / num_threads;

    for (int i = 0; i < num_threads; i++) {
        g_workers[i].y_start = i * rows;
        g_workers[i].y_end   = (i == num_threads - 1) ? g_height : (i + 1) * rows;
        g_workers[i].render  = render;
        g_threads[i] = CreateThread(NULL, 0, worker_thread, &g_workers[i], 0, NULL);
    }
}

static void pool_render_frame()
{
    for (int i = 0; i < g_num_threads; i++) {
        g_workers[i].current_frame = g_frame_counter;
        g_workers[i].current_time = GetTickCount64();
    }

    ReleaseSemaphore(g_work_sem, g_num_threads, NULL);

    for (int i = 0; i < g_num_threads; i++)
        WaitForSingleObject(g_done_sem, INFINITE);
}

static void pool_destroy()
{
    g_shutdown = 1;
    ReleaseSemaphore(g_work_sem, g_num_threads, NULL);
    WaitForMultipleObjects(g_num_threads, g_threads, TRUE, INFINITE);
    for (int i = 0; i < g_num_threads; i++) CloseHandle(g_threads[i]);
    CloseHandle(g_work_sem);
    CloseHandle(g_done_sem);
    free(g_workers);
    free(g_threads);
}

void window_run(RenderFunc render, int num_threads, bool temporal_accumulation)
{
    pool_create(num_threads, render);

    g_tempolatal_accumulation = temporal_accumulation;
    ULONGLONG last = GetTickCount64();

    while (g_is_open) {
        ULONGLONG render_start = GetTickCount64();

        pool_render_frame();

        InvalidateRect(g_hwnd, NULL, FALSE);
        
        MSG msg;
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }

        //Update title with FPS and render time
        ULONGLONG current = GetTickCount64();
        if (current - last >= 100) 
        {
            char title[256];

            float ms = current - render_start;
            sprintf(title, "%s | Rendering : %.1ffps, %.1fms | Time : %.2fs", g_title, 1000.0f / ms, ms, (current - g_start_time) * 0.001f);

            SetWindowTextA(g_hwnd, title);
            last = current;
        }
        
        g_frame_counter ++;
    }

    pool_destroy();
    free(g_pixels);
    free(g_fpixels);
}