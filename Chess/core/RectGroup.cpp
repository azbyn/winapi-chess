#include "RectGroup.h"

namespace core {
template <bool IsVert>
void spacedImpl(RectGroup& res, Rect parent, Point size, Point margins) {
    size_t count = res.size();
    res.boundingRect = parent;

    auto space = IsVert ? (parent.height() - size.y * (int)count - margins.y*2)
                        : (parent.width()  - size.x * (int)count - margins.x*2);
    auto spacing = space / (count-1);


    auto mid = parent.middle();
    Point start = IsVert ? Point{mid.x - size.x / 2, margins.y + parent.y0()}
                         : Point{parent.x0() + margins.x, mid.y - size.y / 2};

    for (auto& v: res.val) {
        v = {start, start + size};
        if constexpr (IsVert) {
            start.y += size.y + spacing;
        } else {
            start.x += size.x + spacing;
        }
    }
}

template <bool IsVert>
void centeredImpl(RectGroup& res, Rect parent, RectGroup::Sizes sizes,
                  RectGroup::Spaces spaces, Point margins) {
    if (sizes.size() != spaces.size() + 1) {
        throw std::logic_error(
            concat("spaces.size must be sizes.size - 1: expected ",
                   sizes.size()-1, " got ", spaces.size(), "."));
    } else if (sizes.size() != res.size()) {
        throw std::logic_error(
            concat("spaces.size must be size(): expected ",
                   res.size(), " got ", spaces.size(), "."));
    }

    auto mid = parent.middle();
    Point size = {0, 0};

    for (auto s : sizes) {
        if constexpr (IsVert) {
            size.y += s.y;
            if (s.x > size.x) size.x = s.x;
        } else {
            size.x += s.x;
            if (s.y > size.y) size.y = s.y;
        }
    }
    for (auto s : spaces) {
        if constexpr (IsVert) {
            size.y += s;
        } else {
            size.y += s;
        }
    }

    int rectStart = IsVert ? mid.y - (size.y / 2) : mid.x - (size.x / 2);

    Point boundsSize = size + 2 * margins;

    res.boundingRect = Rect::fromMiddleAndSize(mid, boundsSize);

    // Initializer_lists don't have random access, so we use iterators
    auto spaceIt = spaces.begin();
    auto sizeIt  = sizes.begin();
    auto it      = res.val.begin();
    auto last    = res.val.end() - 1;

    for (;; ++spaceIt, ++sizeIt, ++it) {
        auto sz = *sizeIt;
        Point start = IsVert ? Point{mid.x - (sz.x / 2), rectStart}
                             : Point{rectStart, mid.x - (sz.x / 2)};

        *it = {start, start + sz};

        if (it >= last)
            break;
        if constexpr (IsVert) {
            rectStart += sz.y + *spaceIt;
        } else {
            rectStart += sz.x + *spaceIt;
        }
    }
}

void RectGroup::updateEquallySpacedHori(Rect parent, Point size, Point margins) {
    spacedImpl<false>(*this, parent, size, margins);
}

void RectGroup::updateEquallySpacedVert(Rect parent, Point size, Point margins) {
    spacedImpl<true>(*this, parent, size, margins);
}

void RectGroup::updateCenteredHori(Rect parent, Sizes sizes, Spaces spaces,
                                   Point margins) {
    centeredImpl<false>(*this, parent, sizes, spaces, margins);
}
void RectGroup::updateCenteredVert(Rect parent, Sizes sizes, Spaces spaces,
                                   Point margins) {
    centeredImpl<true>(*this, parent, sizes, spaces, margins);
}

} // namespace core
