#ifdef _WIN32

#include <windows.h>
#include "app/test.h"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR line, INT)
{
    std::vector<std::string>    singarg;
    return(singmain(singarg));
}

#else

#include "app/test.h"

int main(int argc, char **argv)
{
    std::vector<std::string>    singarg;
    return(singmain(singarg));
}

#endif