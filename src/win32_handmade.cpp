#include <windows.h>

#define internal static 
#define local_persist static 
#define global_variable static 

global_variable bool Running;
global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable HBITMAP BitmapHandle;
global_variable HDC BitmapDeviceContext;

// DIB = device independent bitmap
internal void Win32ResizeDIBSection(int width, int height) {
  if(BitmapHandle) {
    DeleteObject(BitmapHandle);
  } 
  if (!BitmapDeviceContext) {
    BitmapDeviceContext = CreateCompatibleDC(0);
  }

  BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
  BitmapInfo.bmiHeader.biWidth = width;
  BitmapInfo.bmiHeader.biHeight = height;
  BitmapInfo.bmiHeader.biPlanes = 1;
  BitmapInfo.bmiHeader.biBitCount = 32;
  BitmapInfo.bmiHeader.biCompression = BI_RGB;


  BitmapHandle = CreateDIBSection(
    BitmapDeviceContext,
    &BitmapInfo,
    DIB_RGB_COLORS,
    &BitmapMemory,
    0,
    0
  );
}

internal void Win32UpdateWindow(HDC deviceContext, int x, int y, int width, int height) {
  StretchDIBits(
    deviceContext,
    x,
    y,
    width,
    height,
    x,
    y,
    width,
    height,
    &BitmapMemory,
    &BitmapInfo,
    DIB_RGB_COLORS,
    SRCCOPY
  );
}

LRESULT CALLBACK Win32MainWindowCallback(HWND window,UINT message,WPARAM wParam,LPARAM lParam)
{
  LRESULT result = 0;
  switch(message) {
    case WM_SIZE: {
      RECT clientRect;
      GetClientRect(window, &clientRect);
      int width = clientRect.right - clientRect.left;
      int height = clientRect.bottom - clientRect.top;
      Win32ResizeDIBSection(width, height);
    } break;
    case WM_ACTIVATEAPP: {
      OutputDebugString("activate\n");
    } break;
    case WM_CREATE: {
      OutputDebugString("Create\n");
    } break;
    case WM_DESTROY: {
      Running = false;
      OutputDebugString("Destroy\n");
    } break;
    case WM_CLOSE: {
      Running = false;
      OutputDebugString("Close\n");
    } break;
    case WM_PAINT: {
      PAINTSTRUCT paint;
      HDC deviceContext = BeginPaint(window, &paint);
      int x = paint.rcPaint.left;
      int y = paint.rcPaint.top;
      int width = paint.rcPaint.right - paint.rcPaint.left;
      int height = paint.rcPaint.bottom - paint.rcPaint.top;
      Win32UpdateWindow(deviceContext, x, y, width, height);
      EndPaint(window, &paint);
    } break;
    default: {
      result = DefWindowProc(window, message, wParam, lParam);
    } break;
  }
  return result;

}

int CALLBACK WinMain(
  HINSTANCE instance,
  HINSTANCE prevInstance,
  LPSTR     cmdLine,
  int       showCmd
){
  // MessageBox(0, "hello there how are you?", "my first box!", MB_OK);
  WNDCLASS windowClass = {};
  windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  windowClass.hInstance = instance;
  windowClass.lpszClassName = "my window";
  windowClass.lpfnWndProc = Win32MainWindowCallback;
  // windowClass.hIcon = ;
  // windowClass.hCursor = ;

  RegisterClass(&windowClass);
  HWND windowHandle = CreateWindowEx(
    // DWORD     dwExStyle,
    0,
    // LPCSTR    lpClassName,
    windowClass.lpszClassName,
    // LPCSTR    lpWindowName,
    "my window",
    // DWORD     dwStyle, // does it need visible?
    WS_OVERLAPPEDWINDOW|WS_VISIBLE,
    // int       X,
    CW_USEDEFAULT,
    // int       Y,
    CW_USEDEFAULT,
    // int       nWidth,
    CW_USEDEFAULT,
    // int       nHeight,
    CW_USEDEFAULT,
    // HWND      hWndParent,
    0,
    // HMENU     hMenu,
    0,
    // HINSTANCE hInstance,
    instance,
    // LPVOID    lpParam
    0
  );
  MSG message;
  Running = true;
  while(Running) {
    BOOL messageResult = GetMessage(&message, 0, 0, 0);
    if (messageResult > 0) {
      TranslateMessage(&message);
      DispatchMessage(&message);
    } else {
      Running = false;
    }
  }

  return 0;
}
