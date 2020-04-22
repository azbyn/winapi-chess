#pragma once

#include "Color.h"
#include "Paint.h"

#include <iostream>

namespace core {
class Scene;

enum class WindowMode {
    Static,
    Resizeable
};

/*
  Does all the talking to the win api. Creates a window and manages everything
  related to that.
*/
class WindowHandler {
public:
    static WindowHandler& instance() { return *theInstance; }

    static int run(Point size, const char* title,
                   Scene& initialScene,
                   int nCmdShow = SW_SHOWDEFAULT);

    WindowHandler(WindowHandler&&) = delete;
    WindowHandler(const WindowHandler&) = delete;

    WindowHandler& operator=(WindowHandler&&) = delete;
    WindowHandler& operator=(const WindowHandler&) = delete;
    ~WindowHandler() noexcept;

    const std::string& getTitle() const;
    void setTitle(const std::string& val);

    Point getSize() const;
    void setSize(Point val);

    void redraw();
    void redrawBackground();

    void quit();
    void changeScene(Scene& s);

    WindowMode getWindowMode() const { return windowMode; }
    void setWindowMode(WindowMode val);

    void getSnapshot(std::vector<uint8_t>& snap) const;
    void setSnapshot(const std::vector<uint8_t>& snap);

private:
    void updateSize();

    static LRESULT CALLBACK eventHandler(HWND hWnd, UINT uMsg,
                                         WPARAM wParam, LPARAM lParam);

    static HWND createWindow(const char* title, Rect rect,
                             BYTE type, DWORD flags);

private:

    static WindowHandler* theInstance;

    Scene* currentScene;
    std::string title;

    // Glut could save some headaches but I wanted this to work out of the box
    // in Windows
    HWND hwnd;
    Paint background, foreground;

    WindowMode windowMode;
    Paint::StretchData stretchData;

    bool hasQuit = false;

    WindowHandler(const char* title, Scene& scene,
                  Point size, HWND hwnd, const WindowDC& hdc);
    static Scene& scene() { return *instance().currentScene; }


    Point getMousePos(LPARAM lParam);
};
} // namespace core
