#include "MenuScene.h"

#include "SceneCommon.h"
#include "Sprites.h"
#include "SoundManager.h"

using namespace core;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

MenuScene::MenuScene(std::initializer_list<ButtonData> buttons, Mode mode)
        : ButtonSelectorScene(buttons.size(), mode), buttons(buttons) {
    int i = 0;
    for (auto& d : this->buttons)
        d.rect_ = &getButtonRect(i++);
}

void MenuScene::updateVertSizes(Rect rect) {
    getButtonRects().updateEquallySpacedVert(rect, VertButtonSize,
                                             ButtonSpacing);
}
void MenuScene::onDrawBackground(Paint& p) {
    for (auto& r : getButtonRects())
        p.fillRect(r, ButtonCol);
}
void MenuScene::onDraw(core::Paint& p) {
    constexpr auto getPalette = [] (bool isValid) -> const auto& {
        return isValid ? sprites::ValidBtnPalette : sprites::InvalidBtnPalette;
    };

    p.setTextColor(ButtonTextCol);
    p.setFont("Arial", 32, false);

    for (auto& b : buttons) {
        std::visit(overloaded {
            [&](const ButtonData::Normal& ) {
                p.drawText(b.rect(), b.text);
            },
            [&](const ButtonData::LeftRight& lr) {
                auto val = lr.getVal();
                p.drawText(b.rect(), concat(b.text, ": ", val));
                drawLeft(p, b.rect(), sprites::Left,
                         getPalette(val != lr.min));
                drawRight(p, b.rect(), sprites::Right,
                          getPalette(val != lr.max));
            },
            [&](const ButtonData::Radio& val) {
                p.drawText(b.rect(), b.text);
                auto& sprite = sprites::getRadioBtn(val.getVal());
                drawRight(p, b.rect(), sprite, sprites::RadioBtnPalette);
            },
            [&](const ButtonData::SpriteRadio& ) {
                p.drawText(b.rect(), b.text);
            },
        }, b.data);
    }
    onDrawExtra(p);
    ButtonSelectorScene::onDraw(p);
}

void MenuScene::onButtonClick(int index, Point mousePos) {
    auto& b = getButton(index);
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, ButtonData::LeftRight>) {
            int deltaX = mousePos.x - b.rect().x0();
            if (deltaX < b.rect().height()) {
                onButtonMove(index, -1);
            } else if (deltaX > b.rect().width() - b.rect().height()) {
                onButtonMove(index, 1);
            }
            redraw();
        } else {
            ButtonSelectorScene::onButtonClick(index, mousePos);
        }
    }, b.data);
}
void MenuScene::onButtonSelected(int index) {
    auto& b = getButton(index);
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, ButtonData::LeftRight>) {
            onButtonMove(index, index);
        }
    }, b.data);
    SoundManager::playSelected();
}
void MenuScene::onButtonMove(int index, int delta) {
    auto& b = getButton(index);
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, ButtonData::LeftRight>) {
            SoundManager::playLeftRightMove();
            arg.setVal(clamp(arg.getVal() + delta, arg.min, arg.max));
        } else if constexpr (std::is_same_v<T, ButtonData::Radio> ||
                             std::is_same_v<T, ButtonData::SpriteRadio>) {
            onButtonSelected(index);
        } else {
            if (getMode() == Mode::Vertical && delta > 0) {
                onButtonSelected(index);
            } else {
                SoundManager::playInvalid();
            }
        }
    }, b.data);
}


void MenuScene::drawLeft(core::Paint& p, size_t index,
                         const core::PaletteSprite& sprite,
                         const core::Palette& palette) const {
    drawLeft(p, getButtonRect(index), sprite, palette);
}

void MenuScene::drawLeft(core::Paint& p, const core::Rect& r,
                         const core::PaletteSprite& sprite,
                         const core::Palette& palette) const {
    p.drawSprite(r.topLeft(), sprite, palette, r.height() / sprite.height());
}

void MenuScene::drawRight(core::Paint& p, const core::Rect& r,
                          const core::PaletteSprite& sprite,
                          const core::Palette& palette) const {
    p.drawSprite(r.topRight() - core::Point{r.height(), 0},
                 sprite, palette, r.height() / sprite.height());
}
void MenuScene::drawRight(core::Paint& p, size_t index,
                          const core::PaletteSprite& sprite,
                          const core::Palette& palette) const {
    drawRight(p, getButtonRect(index), sprite, palette);
}


void MenuScene::drawMouse(Paint& p, const Rect& rect) {
    p.fillRect(rect, ButtonHighlightCol);
    p.drawRectIn(rect, 3, MarginCol);
}
void MenuScene::drawKeyboard(Paint& p, const Rect& rect) {
    p.fillRect(rect, ButtonHighlightCol);
    p.drawRectIn(rect, 3, MarginCol);
}
