#include <windows.h>

LRESULT CALLBACK MainWindowCallback(
HWND window,
UINT message,
WPARAM wParam,
LPARAM lParam
){
  LRESULT result = 0;
  switch(message) {
    case WM_SIZE: {
      OutputDebugString("size\n");
    } break;
    case WM_ACTIVATEAPP: {
      OutputDebugString("activate\n");
    } break;
    case WM_CREATE: {
      OutputDebugString("Create\n");
    } break;
    case WM_DESTROY: {
      OutputDebugString("Destroy\n");
    } break;
    case WM_QUIT: {
      OutputDebugString("Quit\n");
    } break;
    case WM_CLOSE: {
      OutputDebugString("Close\n");
    } break;
    case WM_PAINT: {
      PAINTSTRUCT paint;
      HDC deviceContext = BeginPaint(window, &paint);
      int x = paint.rcPaint.left;
      int y = paint.rcPaint.top;
      int width = paint.rcPaint.right - paint.rcPaint.left;
      int height = paint.rcPaint.bottom - paint.rcPaint.top;
      static DWORD operation = WHITENESS;
      PatBlt(deviceContext, x, y, width, height, operation);
      if (operation == WHITENESS) {
        operation = BLACKNESS;
      } else {
        operation = WHITENESS;
      }

      EndPaint(window, &paint);
      OutputDebugString("paint\n");
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
  windowClass.lpfnWndProc = MainWindowCallback;
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
  for(;;) {
    BOOL messageResult = GetMessage(&message, 0, 0, 0);
    if (messageResult > 0) {
      TranslateMessage(&message);
      DispatchMessage(&message);
    } else {
      break;
    }
  }

  return 0;
}
