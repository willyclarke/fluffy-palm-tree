#ifndef SRC_FRACTAL_HPP
#define SRC_FRACTAL_HPP

#include "curvesrobotics.hpp"
#include "engsupport.hpp"
#include "raylib.h"

#include <mutex>
#include <thread>
#include <vector>

namespace fluffy {
namespace fractal {
struct pixel {
  es::vector4_double Pos{0.f, 0.f, 0.f, 1.f}; //!< Make it a point.
  Color              Col{BLACK};
};

struct pixel_canvas {
  Vector4 Dimension{};
  Vector4 PosUL{};
  Vector4 PosUR{};
  Vector4 PosLL{};
  Vector4 PosLR{};
  int     ResolutionX{100}; //!< Pixel per unit X direction.
  int     ResolutionY{100}; //!< Pixel per unit Y direction.
  int     NThreads{1};      //!< Number of threads to use for rendering.
  int     YIncrement{};     //!< When we have x threads, each thread will deal with a sub block of height YIncrement.
  Matrix  MhS2P{};          //!< Homogenous matrix to go from Screen to pixel, screen center is at 0,0,0.
  bool    PrintMe{true};
};

struct config {
  es::vector4_double                  Constant{-0.4f, 0.6f, 0.f, 0.f};
  es::vector4_double                  Dimension{2.f, 2.f, 0.f, 0.f};
  std::vector<fluffy::fractal::pixel> vFractalPixels{};
  Image                               iMage{};
  pixel_canvas                        PixelCanvas{};
};

auto ConfigurePixelCanvas(int CenterX, int CenterY, int Width, int Height, int ResolutionX, int ResolutionY)
    -> pixel_canvas;
auto GetFractalColor(double t) -> Color;
auto Render(es::vector4_double const& RenderSize, es::vector4_double const& Constant) -> void;
auto CreateFractalPixelSpace(currob::grid_cfg const&              GridCfgInput,
                             pixel_canvas&                        PixelCanvas,
                             int                                  screenWidth,
                             int                                  screenHeigth,
                             es::vector4_double const&            Resolution,
                             es::vector4_double const&            Constant,
                             std::vector<fluffy::fractal::pixel>& vFractalPixels,
                             Image&                               outputImage) -> void;
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
