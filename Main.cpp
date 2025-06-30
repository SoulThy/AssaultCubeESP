#include "Bypass.h"
#include "Game.h"
#include "Esp.h"
#include <iostream>


int main() {
    Bypass bypass;

    if (!bypass.AttachToWindow("AssaultCube")) {
        std::cerr << "Could not attach to window. GetLastError = " << GetLastError() << std::endl;
        system("pause");
        return EXIT_FAILURE;
    }

    std::cout << "Attached to PID: " << bypass.GetProcessId() << std::endl;
    
    Game game(&bypass);
    Esp esp(&game);
    
    while (true) {
        game.Update();
		esp.ProcessMessages();
        esp.RefreshWindow();
        Sleep(10);
    }
    return EXIT_SUCCESS;
}

