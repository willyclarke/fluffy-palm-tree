#ifndef SRC_FRACTAL_HPP
#define SRC_FRACTAL_HPP

#include "raylib.h"

#include <mutex>
#include <thread>
#include <vector>

namespace fluffy {
namespace fractal {
struct pixel {
  Vector4 Pos{0.f, 0.f, 0.f, 1.f}; //!< Make it a point.
  Color   Col{BLACK};
};

struct config {
  Vector4                                          Constant{-0.4f, 0.6f, 0.f, 0.f};
  Vector4                                          Dimension{2.f, 2.f, 0.f, 0.f};
  std::vector<fluffy::fractal::pixel>              vFractalPixels{};
  std::vector<std::vector<fluffy::fractal::pixel>> vvFractalPixels{};
};

auto CreateFractalVector(Vector4 const& RenderSize, Vector4 const& Constant, Vector4 const& Resolution)
    -> fluffy::fractal::config;

}; // namespace fractal
}; // namespace fluffy
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
