#include <windows.h>
#include <stdint.h>
#define internal static 
#define local_persist static 
#define global_variable static 

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

struct Win32OffScreenBuffer{
  BITMAPINFO Info;
  void *Memory;
  int Width;
  int Height;
  int Pitch;
  int BytesPerPixel;
};

global_variable bool GlobalRunning;
global_variable Win32OffScreenBuffer GlobalBackBuffer;

struct Win32WindowDimension{
  int Width;
  int Height;
};
Win32WindowDimension Win32GetWindowDimension(HWND window) {
    RECT clientRect;
    GetClientRect(window, &clientRect);
    Win32WindowDimension dimension;
    dimension.Width = clientRect.right - clientRect.left;
    dimension.Height = clientRect.bottom - clientRect.top;
    return dimension;
}

internal void RenderWeirdThing(Win32OffScreenBuffer buffer, int offsetX, int offsetY) {
  uint8 *row = (uint8 *)buffer.Memory;

  for(int y = 0; y < buffer.Height; ++y) {
    uint32 *pixel = (uint32 *)row;
    for(int x = 0; x < buffer.Width; ++x) {
    // pixel in memory = BB GG RR XX
    // 0xxxRRGGBB
      uint8 blue = x + offsetX;
      uint8 green = y + offsetY;
      *pixel++ = ((green << 8) | blue);
    }
    row += buffer.Pitch;
  }
}

// DIB = device independent bitmap
internal void Win32ResizeDIBSection(Win32OffScreenBuffer *buffer, int width, int height) {

  if(buffer->Memory) {
    VirtualFree(buffer->Memory, 0, MEM_RELEASE);
  }

  buffer->Width = width;
  buffer->Height = height;
  buffer->BytesPerPixel = 4;


  buffer->Info.bmiHeader.biSize = sizeof(buffer->Info.bmiHeader);
  buffer->Info.bmiHeader.biWidth = buffer->Width;
  buffer->Info.bmiHeader.biHeight = -buffer->Height;
  buffer->Info.bmiHeader.biPlanes = 1;
  buffer->Info.bmiHeader.biBitCount = 32;
  buffer->Info.bmiHeader.biCompression = BI_RGB;

  int bitmapMemorySize = buffer->BytesPerPixel * (buffer->Width * buffer->Height);
  buffer->Memory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
  buffer->Pitch = buffer->Width * buffer->BytesPerPixel;
}

internal void 
Win32DisplayBufferInWindow(HDC deviceContext, 
                            int windowWidth,
                            int windowHeight,
                            Win32OffScreenBuffer buffer) {
                            // int x, int y, int width, int height) {
  StretchDIBits(
    deviceContext,
    0, 0, windowWidth, windowHeight,
    0, 0, buffer.Width, buffer.Height,
    buffer.Memory,
    &buffer.Info,
    DIB_RGB_COLORS,
    SRCCOPY
  );
}

LRESULT CALLBACK Win32MainWindowCallback(HWND window,UINT message,WPARAM wParam,LPARAM lParam)
{
  LRESULT result = 0;
  switch(message) {
    case WM_SIZE: {
          } break;
    case WM_ACTIVATEAPP: {
      OutputDebugString("activate\n");
    } break;
    case WM_CREATE: {
      OutputDebugString("Create\n");
    } break;
    case WM_DESTROY: {
      GlobalRunning = false;
      OutputDebugString("Destroy\n");
    } break;
    case WM_CLOSE: {
      GlobalRunning = false;
      OutputDebugString("Close\n");
    } break;
    case WM_PAINT: {
      PAINTSTRUCT paint;
      HDC deviceContext = BeginPaint(window, &paint);
      int x = paint.rcPaint.left;
      int y = paint.rcPaint.top;
      int width = paint.rcPaint.right - paint.rcPaint.left;
      int height = paint.rcPaint.bottom - paint.rcPaint.top;
      RECT clientRect;
      GetClientRect(window, &clientRect);
      Win32WindowDimension dimension = Win32GetWindowDimension(window);
      Win32DisplayBufferInWindow(deviceContext, dimension.Width, dimension.Height, GlobalBackBuffer);
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
  WNDCLASS windowClass = {};
  Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);

  // if you take off CS_OWNDC it causes a memory leak!
  windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  windowClass.hInstance = instance;
  windowClass.lpszClassName = "my window";
  windowClass.lpfnWndProc = Win32MainWindowCallback;
  // windowClass.hIcon = ;
  // windowClass.hCursor = ;

  RegisterClass(&windowClass);
  HWND window = CreateWindowEx(
    0,
    windowClass.lpszClassName,
    "my window",
    WS_OVERLAPPEDWINDOW|WS_VISIBLE,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    0,
    0,
    instance,
    0
  );
  MSG message;
  int xOffset = 0;
  int yOffset = 0;
  GlobalRunning = true;
  while(GlobalRunning) {
    while(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
      if (message.message == WM_QUIT) {
        GlobalRunning = false;
      }
      TranslateMessage(&message);
      DispatchMessage(&message);
    }
    RenderWeirdThing(GlobalBackBuffer, xOffset, yOffset);
    HDC deviceContext = GetDC(window);
    Win32WindowDimension dimension = Win32GetWindowDimension(window);
    Win32DisplayBufferInWindow(deviceContext, dimension.Width, dimension.Height, GlobalBackBuffer);
    ++xOffset;
    ++yOffset;
  }

  return 0;
}
