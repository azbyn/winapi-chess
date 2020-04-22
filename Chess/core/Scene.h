#pragma once

#include "WindowHandler.h"

namespace core {

/*
  A scene tells the WindowHandler what to draw and responds
  to events like key presses.

  Examples of scenes: The main menu, the game itself, etc.

  Note: The background doesn't get reset on scene change,
  so we can draw scenes on top of each other.
  This can be useful for pop-up menus.

  It doesn't make sense to have multiple instances of the same scene.
  So it's useful to make this a singleton;
 */
class Scene {
public:
    Scene() = default;
    Scene(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene& operator=(Scene&&) = delete;

    // Provided for convenience
    static void quit() {
        WindowHandler::instance().quit();
    }
    static void redraw() {
        WindowHandler::instance().redraw();
    }
    static void redrawBackground() {
        WindowHandler::instance().redrawBackground();
    }

    // Called when this scene gets selected
    virtual void onStart() {}

    // Called when this scene gets deselected or the window is closed
    virtual void onStop() {}

    virtual void onDraw(Paint& p) = 0;
    virtual void onDrawBackground(Paint& p) = 0;

    // Should return true if the key is captured
    // Only used for comunication between the class and subclass
    virtual void onKeyDown(char /*key*/) { }
    virtual void onKeyUp(char /*key*/) { }

    virtual void onMouseMove(Point /*pos*/) {}

    virtual void onRightMouseUp(Point /*pos*/) {}
    virtual void onRightMouseDown(Point /*pos*/) {}
    virtual void onLeftMouseUp(Point /*pos*/) {}
    virtual void onLeftMouseDown(Point /*pos*/) {}

    //gets called after onSelected
    virtual void onSizeChanged(Point /*newSize*/) {}

    virtual ~Scene() noexcept = default;
};


} // namespace core
