#include <windows.h>
#include "app/test.h"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR line, INT)
{
    std::vector<std::string>    singarg;
    return(singmain(singarg));
}