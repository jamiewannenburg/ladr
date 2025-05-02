#include "signal_util.h"
#include <string.h>  /* For strcmp */

#if defined(_WIN32) || defined(WIN32) || defined(__MINGW32__) || defined(__MINGW64__)

/* Windows implementation of custom signal handling */

static void (*custom_signal_handler)(int) = NULL;

/* Custom Windows message for simulating SIGUSR1 */
#define WM_CUSTOM_SIGUSR1 (WM_USER + 1)

/* Window procedure to handle custom messages */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_CUSTOM_SIGUSR1) {
        if (custom_signal_handler) {
            custom_signal_handler(SIGUSR1);
        }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/* Hidden window for message handling */
static HWND message_window = NULL;

/* Thread for message processing */
static HANDLE message_thread = NULL;

/* Thread function to create a hidden window and process messages */
DWORD WINAPI MessageThreadProc(LPVOID lpParameter)
{
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "SignalUtilMessageWindow";
    
    if (!RegisterClassEx(&wc)) {
        return 1;
    }
    
    message_window = CreateWindowEx(
        0, wc.lpszClassName, "SignalUtil", 
        0, 0, 0, 0, 0, 
        HWND_MESSAGE, NULL, wc.hInstance, NULL
    );
    
    if (!message_window) {
        return 1;
    }
    
    /* Message loop */
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}

/* Register a custom signal handler for SIGUSR1 */
void register_custom_signal_handler(void (*handler)(int))
{
    /* Store the handler */
    custom_signal_handler = handler;
    
    /* Create message window if not already done */
    if (!message_thread) {
        message_thread = CreateThread(
            NULL, 0, MessageThreadProc, NULL, 0, NULL
        );
    }
    
    /* Also register the standard signal handler for compatibility */
    signal(SIGUSR1, handler);
}

/* Structure to pass to EnumWindowsProc */
typedef struct {
    DWORD target_pid;
    BOOL found;
} EnumWindowsData;

/* Callback function for EnumWindows */
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    EnumWindowsData* data = (EnumWindowsData*)lParam;
    DWORD window_pid = 0;
    char class_name[256];
    
    GetWindowThreadProcessId(hwnd, &window_pid);
    if (window_pid == data->target_pid) {
        GetClassName(hwnd, class_name, sizeof(class_name));
        if (strcmp(class_name, "SignalUtilMessageWindow") == 0) {
            /* Found the window we're looking for */
            SendMessage(hwnd, WM_CUSTOM_SIGUSR1, 0, 0);
            data->found = TRUE;
            return FALSE; /* Stop enumerating */
        }
    }
    return TRUE; /* Continue enumerating */
}

/* Send a custom signal to a process */
int send_custom_signal(int pid, int sig)
{
    if (sig == SIGUSR1) {
        /* Find the window from the target process */
        EnumWindowsData data = { (DWORD)pid, FALSE };
        
        /* Enumerate all top-level windows to find one with matching process ID */
        EnumWindows(EnumWindowsProc, (LPARAM)&data);
        
        return 0; /* Success even if window wasn't found */
    }
    else {
        /* For other signals, use the standard Windows mechanism */
        return raise(sig); /* Can only signal self, not other processes */
    }
}

#endif /* Windows implementation */ 