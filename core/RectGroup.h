#pragma once
#include "Utils.h"

#include <vector>

namespace core {
/*
  This is useful for drawing panels side by side for sub-menus and other
  rectangle based layouts.
*/
struct RectGroup {
    using Sizes = std::initializer_list<Point>;
    using Spaces = std::initializer_list<int>;

    std::vector<Rect> val;
    Rect boundingRect;

    // RectGroup() : val(), boundingRect() {}
    explicit RectGroup(size_t size) : val(size), boundingRect() {}

    RectGroup(const RectGroup& ) = delete;
    RectGroup& operator=(const RectGroup& ) = delete;

    RectGroup(RectGroup&& ) = default;
    RectGroup& operator=(RectGroup&& ) = default;

    size_t size() const { return val.size(); }

    const Rect& operator[](size_t i) const { return val[i]; }
    Rect& operator[](size_t i) { return val[i]; }
    auto begin() { return val.begin(); }
    auto begin() const { return val.begin(); }

    auto end() { return val.end(); }
    auto end() const { return val.end(); }



    /** equally spaced horizontal **/
    /*
      +--------------+
      |              |
      |+--+ +--+ +--+|
      ||  | |  | |  ||
      |+--+ +--+ +--+|
      |              |
      +--------------+
    */
    void updateEquallySpacedHori(Rect parent, Point size, Point margins);

    void updateEquallySpacedHori(Rect parent, Point size, int margin = 0) {
        updateEquallySpacedHori(parent, size, {margin, margin});
    }

    /** equally spaced vertical **/
    /*
      +-----------+
      |           |
      | +-------+ |
      | +-------+ |
      |           |
      | +-------+ |
      | +-------+ |
      |           |
      | +-------+ |
      | +-------+ |
      |           |
      +-----------+
     */
    void updateEquallySpacedVert(Rect parent, Point size, Point margins);

    void updateEquallySpacedVert(Rect parent, Point size, int margin = 0) {
        updateEquallySpacedVert(parent, size, {margin, margin});
    }

    /** centered horizontal **/
    /*
      +----------------+
      |                |
      | +-------+ +--+ |
      | |       | |  | |
      | +-------+ +--+ |
      |                |
      +----------------+
    */
    void updateCenteredHori(Rect parent, Sizes sizes, Spaces spaces,
                            Point margins);

    void updateCenteredHori(Rect parent, Sizes sizes, Spaces spaces,
                            int margin = 0) {
        updateCenteredHori(parent, sizes, spaces, {margin, margin});
    }

    void updateCenteredHori(Rect parent, Point size1, Point size2,
                            int space, Point margins) {
        updateCenteredHori(parent, {size1, size2}, {space}, margins);
    }
    void updateCenteredHori(Rect parent, Point size1, Point size2,
                            int space = 0, int margin = 0) {
        updateCenteredHori(parent, size1, size2, space, {margin, margin});
    }


    /** centered vertical **/
    /*
      +-----------+
      |           |
      | +-------+ |
      | |       | |
      | |       | |
      | +-------+ |
      |  +-----+  |
      |  |     |  |
      |  +-----+  |
      |           |
      +-----------+
    */

    void updateCenteredVert(Rect parent, Sizes sizes, Spaces spaces,
                            Point margins);

    void updateCenteredVert(Rect parent, Sizes sizes, Spaces spaces,
                            int margin = 0) {
        updateCenteredVert(parent, sizes, spaces, {margin, margin});
    }

    void updateCenteredVert(Rect parent, Point size1, Point size2,
                            int space, Point margins) {
        updateCenteredVert(parent, {size1, size2}, {space}, margins);
    }
    void updateCenteredVert(Rect parent, Point size1, Point size2,
                            int space = 0, int margin = 0) {
        updateCenteredVert(parent, size1, size2, space, {margin, margin});
    }
};

} // namespace core
