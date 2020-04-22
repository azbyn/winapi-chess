#pragma once

#include "core/ButtonSelectorScene.h"
#include "Sprites.h"

#include <variant>

class MenuScene : public core::ButtonSelectorScene {
public:
    class ButtonData {
        struct Normal {};
        struct Radio {
            using GetVal = bool (*)();
            GetVal getVal;
        };
        struct SpriteRadio {
            // all the drawaing should be done in the subclass
        };
        struct LeftRight {
            using GetVal = int (*)();
            using SetVal = void (*)(int);
            int min, max;
            GetVal getVal;
            SetVal setVal;
        };
        using Data = std::variant<Normal, Radio, SpriteRadio, LeftRight>;

    public:
        static ButtonData makeNormal(std::string_view text) {
            return {text, Normal{}};
        }
        static ButtonData makeLeftRight(std::string_view text,
                                        int min, int max,
                                        LeftRight::GetVal getVal,
                                        LeftRight::SetVal setVal) {
            return {text, LeftRight{min, max, getVal, setVal}};
        }
        static ButtonData makeRadio(std::string_view text,
                                    Radio::GetVal getVal) {
            return {text, Radio{getVal}};
        }

        static ButtonData makeSpriteRadio(std::string_view text) {
            return {text, SpriteRadio{}};
        }

        core::Rect& rect() { return *rect_; }
        const core::Rect& rect() const { return *rect_; }

    private:
        std::string text;
        core::Rect* rect_ = nullptr;
        Data data;

        ButtonData() {}
        ButtonData(std::string_view text, Data data)
                : text(text), data(data) {}

        friend class MenuScene;
    };

    MenuScene(std::initializer_list<ButtonData> buttons, Mode mode);

    void drawLeft(core::Paint& p, size_t index,
                  const core::PaletteSprite& sprite,
                  const core::Palette& palette) const;

    void drawLeft(core::Paint& p, const core::Rect& r,
                  const core::PaletteSprite& sprite,
                  const core::Palette& palette) const;

    void drawRight(core::Paint& p, const core::Rect& r,
                   const core::PaletteSprite& sprite,
                   const core::Palette& palette) const;

    void drawRight(core::Paint& p, size_t index,
                   const core::PaletteSprite& sprite,
                   const core::Palette& palette) const;

    // Should be called from onSizeChanged
    void updateVertSizes(core::Rect r);

    // If you want to override this use onDrawExtra
    // only override in special cases
    void onDraw(core::Paint& p) override;
    virtual void onDrawExtra(core::Paint& /*p*/) {}
    // Must be called by subclass
    void onDrawBackground(core::Paint& p) override;

    // Must be called by subclass
    void onButtonSelected(int i) override;

    void onButtonMove(int i, int delta) final;
    void onButtonClick(int i, core::Point mousePos) final;

    void drawMouse(core::Paint& p, const core::Rect& r) override;
    void drawKeyboard(core::Paint& p, const core::Rect& r) override;

protected:
    ButtonData& getButton(int i) { return buttons[i]; }
    const ButtonData& getButton(int i) const { return buttons[i]; }

private:
    std::vector<ButtonData> buttons;
};

