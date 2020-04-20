// #include "MainScene.h"
#include "MainMenuScene.h"

#include <signal.h>

/*
  todo:

  move log?

  animation for castling
  icon

  nicely format everything
 */

void signalHandler(int signal) {
    std::cerr << "Signal " << signal << "\n";
    ::MessageBoxA(nullptr, "Signal!", "Error", MB_OK);
    exit(-1);
}

int APIENTRY WinMain(HINSTANCE /*hCurrentInst*/, HINSTANCE /*hPreviousInst*/,
                     LPSTR /*lpszCmdLine*/, int /*nCmdShow*/) {
    signal(SIGSEGV, signalHandler);
    try {
        constexpr core::Point Size{900, 700};
        return core::WindowHandler::run(Size, "Chess",
                                        MainMenuScene::instance());

    } catch (const std::exception& e) {
        ::MessageBoxA(nullptr, e.what(), "Error", MB_OK);
        std::cerr << e.what();
        return -1;
    }
    // int nCmdShow = SW_SHOWDEFAULT);
}

#ifndef _MSC_VER
int WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/,
                    PWSTR /*pCmdLine*/, int /*nCmdShow*/) {
    return WinMain(0, 0, 0, 0);
}
#endif
int main() { return WinMain(0, 0, 0, 0); }

