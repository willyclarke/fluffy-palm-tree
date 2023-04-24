#ifndef SRC_CURVESROBOTICS_HPP
#define SRC_CURVESROBOTICS_HPP

/**
 * Data structs for the curves robotics app.
 *
 * Copyright (c) 2023 Willy Clarke
 *
 * MIT License - see bottom of file.
 */

#include "raylib.h"

#include <mutex>
#include <string>
#include <vector>

namespace currob {
/**
 * Hold pixel position as integers, X and Y.
 */
struct pixel_pos {
  int         X{};
  int         Y{};
  Color       color{LIGHTGRAY};
  std::string TxtTagX{}; //!< Text for x markers
  std::string TxtTagY{}; //!< Text for y markers
};

/**
 * Grid configuration.
 * Store a vector of pixel_pos which is the screen X and Y pixel positions.
 * The GridCentre and GridDimensions are used to keep information about size
 * so that it can be passed around to the various routines that need it.
 */
struct grid_cfg {
  float TickDistance{0.1f};

  std::vector<pixel_pos> vGridLines{};
  std::vector<pixel_pos> vGridSubDivider{};

  /**
   */
  Vector4 GridScreenCentre{0.f, 0.f, 0.f, 1.f}; //!< Make it a Point.

  /**
   */
  Vector4 GridCenterValue{0.f, 0.f, 0.f, 1.f}; //!< Make it a Point.

  /**
   * @- x is GridLength
   * @- y is GridHeight
   * @- Defined as Homogenous vector, w=0.
   */
  Vector4 GridDimensions{8.f, 6.f, 0.f, 0.f};
};

}; // namespace currob

#endif
/**
MIT License

Copyright (c) 2023 Willy Clarke

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
