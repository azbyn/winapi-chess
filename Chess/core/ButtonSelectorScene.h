#pragma once

#include "Scene.h"
#include "RectGroup.h"

namespace core {

class ButtonSelectorScene : public virtual Scene {
public:
    enum class Mode { Vertical, Horizontal };
    ButtonSelectorScene(size_t btnCount, Mode mode)
            : rects(btnCount), mode(mode) {}

    // Must be called by subclass
    void onStart() override;

    // Must be called by subclass
    void onKeyDown(char k) override;

    // Must be called by subclass
    void onLeftMouseDown(Point pos) override;
    // Must be called by subclass
    void onMouseMove(Point pos) override;

    // Must be called by subclass
    void onDraw(Paint& p) override;

    virtual void onButtonSelected(int i) = 0;

    // I couldn't find a better term, when mode == Vertical
    // if we press left arrow we call onButtonMove(-1),
    // for right arrow we call onButtonMove(1),
    // if mode == Horizontal, then if we press up arrow
    // we call onButtonMove(-1), for down arrow we call onButtonMove(1).
    //
    // You don't have to call it from the subclass.
    virtual void onButtonMove(int i, int delta);

    // Should be called by subclass, it's ok to ignore it
    // Called on click
    virtual void onButtonClick(int i, Point mousePos);

    virtual void drawMouse(Paint& p, const Rect& r) = 0;
    virtual void drawKeyboard(Paint& p, const Rect& r) = 0;

protected:
    // getButtonRects().update....() must be called from onSizeChanged
    RectGroup& getButtonRects() { return rects; }
    const Rect& getButtonRect(size_t i) const { return rects[i]; }
    Rect& getButtonRect(size_t i) { return rects[i]; }

    Mode getMode() const { return mode; }

private:
    RectGroup rects;

    Mode mode;
    bool usingKeyboard = true;
    int keyboardCursor;
    int mouseCursor;
    Point prevMousePos;

    void setMouseCursor(int val);
    void moveKeyboardCursor(int delta, Mode btnMode);

    void done() {
        onButtonSelected(usingKeyboard ? keyboardCursor : mouseCursor);
    }
};

}//namespace core
