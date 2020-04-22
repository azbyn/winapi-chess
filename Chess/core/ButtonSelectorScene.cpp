#include "ButtonSelectorScene.h"

namespace core {

void ButtonSelectorScene::onStart() {
    keyboardCursor = 0;
    mouseCursor = -1;
    prevMousePos = { -10, -10 };
}

void ButtonSelectorScene::onKeyDown(char k) {
    switch (k) {
    case VK_SPACE:
    case VK_RETURN:
        if (usingKeyboard) {
            done();
        }
        else {
            usingKeyboard = true;
        }
        break;
    case 'A': case VK_LEFT:
        moveKeyboardCursor(-1, Mode::Horizontal);
        break;
    case 'W': case VK_UP:
        moveKeyboardCursor(-1, Mode::Vertical);
        break;
    case 'D': case VK_RIGHT:
        moveKeyboardCursor(1, Mode::Horizontal);
        break;
    case 'S': case VK_DOWN:
        moveKeyboardCursor(1, Mode::Vertical);
        break;
    default:
        return;
    }
    redraw();
}

void ButtonSelectorScene::onButtonClick(int, Point) {
    done();
}
void ButtonSelectorScene::onLeftMouseDown(Point pos) {
    onMouseMove(pos);
    if (mouseCursor >= 0) onButtonClick(mouseCursor, pos);
}

void ButtonSelectorScene::onMouseMove(Point pos) {
    auto delta = (prevMousePos - pos).length2();
    if (delta == 0) return;

    prevMousePos = pos;
    if (rects.boundingRect.contains(pos)) {
        int i = 0;
        for (auto& r : rects) {
            if (r.contains(pos)) {
                setMouseCursor(i);
                // Assume they don't overlap
                return;
            }
            ++i;
        }
    }
    setMouseCursor(-1);
}
void ButtonSelectorScene::setMouseCursor(int val) {
    bool prev = usingKeyboard;
    if (prev && val == -1) return;
    usingKeyboard = false;
    if (prev || mouseCursor != val) {
        mouseCursor = val;
        redraw();
    }
}

void ButtonSelectorScene::onButtonMove(int index, int delta) {
    if (mode == Mode::Vertical && delta > 0) onButtonSelected(index);
}
void ButtonSelectorScene::moveKeyboardCursor(int delta, Mode btnMode) {
    if (!usingKeyboard) {
        usingKeyboard = true;
        return;
    }
    if (btnMode != mode) {
        onButtonMove(keyboardCursor, delta);
        return;
    }
    keyboardCursor += delta;
    keyboardCursor = clamp(keyboardCursor, 0, int(rects.size()) - 1);
}

void ButtonSelectorScene::onDraw(Paint& p) {
    if (usingKeyboard) {
        drawKeyboard(p, rects[keyboardCursor]);
    }
    else if (mouseCursor >= 0) {
        drawMouse(p, rects[mouseCursor]);
    }
}
}
