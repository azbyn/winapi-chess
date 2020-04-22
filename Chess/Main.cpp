#include "MainMenuScene.h"

#include <signal.h>

/*
  todo:

  move log?
  release

  animation for castling

  nicely format everything
 */
using namespace core;
void signalHandler(int signal) {
    std::cerr << "Signal " << signal << "\n";
    ::MessageBoxA(nullptr, concat("Signal ", signal, "!").c_str(),
                  "Error", MB_OK);
    exit(-1);
}

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int nCmdShow) {
    signal(SIGSEGV, signalHandler);
    try {
        constexpr Point Size{900, 700};
        return WindowHandler::run(Size, "Chess", MainMenuScene::instance(),
            nCmdShow);

    } catch (const std::exception& e) {
        ::MessageBoxA(nullptr, e.what(), "Error", MB_OK);
        std::cerr << e.what();
        return -1;
    }
}

#ifndef _MSC_VER
int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int nCmdShow) {
    return WinMain(0, 0, 0, nCmdShow);
}
#endif
int main() {
    return WinMain(0, 0, 0, SW_SHOWDEFAULT);
}

