#include <windows.h>

int CALLBACK WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR     lpCmdLine,
  int       nShowCmd
){
  MessageBox(0, "hello there how are you?", "my first box!", MB_OK);
  return 0;
}
