#include "fractal.hpp"
#include "engsupport.hpp"
#include "raylib.h"

#include <cmath>

namespace {
/**
 * Compute Zn^2 + C
 */
auto ComputeNext(Vector4 const& Current, Vector4 const& Constant) -> Vector4 {

  // Zn^2
  auto const Zr = Current.x * Current.x - Current.y * Current.y;
  auto const Zi = 2.f * Current.x * Current.y;

  // Add constant ane return
  return es::Vector(Zr, Zi, 0.f) + Constant;
}

/**
 * Return the mod squared.
 */
auto Mod2(Vector4 const& Z) -> float { return Z.x * Z.x + Z.y * Z.y; }

/**
 * Compute sequence elements until Mod exceeds threshold or max iterations.
 */
auto ComputeIterations(Vector4 const& Z0, Vector4 const& Constant, int MaxIterations = 50) -> float {
  auto Zn = Z0;
  auto Iteration{0};
  while (Mod2(Zn) < 4.f && Iteration < MaxIterations) {
    Zn = ComputeNext(Zn, Constant);
    ++Iteration;
  }

  // Create a smooth iteration count.
  auto const Mod             = std::sqrt(Mod2(Zn));
  auto const SmoothIteration = float(Iteration) - std::log2f(std::max(1.f, std::log2f(Mod)));

  return SmoothIteration;
}

};// end of anonymous namespace

namespace fluffy {
namespace fractal {
#if 0 // Code from https://www.youtube.com/watch?v=uc2yok_pLV4&t=244s
/**
 * Compute pixels color. Iterates over pixel space.
 */
auto Render(Vector4 const &RenderSize, Vector4 const &Constant) -> void {
  // Compute the size of the
  auto const Scale = 1.f / (RenderSize.y / 2.f);
  for (int Y{}; Y < RenderSize.y; ++Y) {
    for (int X{}; X < RenderSize.x; ++X) {
      // Compute the pixel coordinates.
      auto const Px = float(X - RenderSize.x / 2.f) / Scale;
      auto const Py = float(Y - RenderSize.y / 2.f) / Scale;
      // Compute the pixel color.
      auto constexpr MaxIterations = 50;
      auto const Iterations =
          ComputeIterations(es::Point(Px, Py, 0.f), Constant, MaxIterations);
      // SetPixelColor(X, Y, gradient.getColor(Iterations);
    }
  }
}
#endif

/**
 * Compute pixels color. Iterates over Canvas.
 * @RenderSize - Set up a canvas that is X units wide and Y units high.
 * @Constant - Tunable for the fractal.
 * @return - A vector with pixel colors.
 * TODO: (Willy Clarke) : Get hold of the pixelsize and set the increment
 * accordingly.
 */
auto CreateFractalVector(Vector4 const& RenderSize, Vector4 const& Constant, Vector4 const& Resolution)
    -> fluffy::fractal::config {

  auto ldaSetPixelColor = [](Vector4 const& Pos, int Iterations, int MaxIterations) -> fluffy::fractal::pixel {
    fluffy::fractal::pixel Result{};
    Result.Pos   = Pos;
    Result.Col.r = 0xFF & Iterations;
    Result.Col.g = 0xFF & (Iterations >> 8);
    Result.Col.b = 0xFF & (Iterations >> 16);
    Result.Col.a = 0xFF & int(255.f - float(255.f * float(Iterations) / float(MaxIterations)));

    return Result;
  };

  std::vector<fluffy::fractal::pixel> vPixel{};

  std::mutex io_mutex;

  struct box {
    size_t  Idx{0xFFFF};
    Vector4 LowerLeft{};
    Vector4 UpperRigth{};
  };

  auto ldaCreateFractalJuliaSet =
      [&ldaSetPixelColor, &io_mutex](
          box const& Box, Vector4 const& Resolution, Vector4 const& Constant) -> std::vector<fluffy::fractal::pixel> {
    std::vector<fluffy::fractal::pixel> vPixelCfg{};

    auto const IncrementX = 1.f / Resolution.x;
    auto const IncrementY = 1.f / Resolution.y;
    auto       X          = Box.LowerLeft.x;
    auto       Y          = Box.LowerLeft.y;
    auto       MaxRegisteredIterations{0.f};

    while (Y < Box.UpperRigth.y) {
      while (X < Box.UpperRigth.x) {

        auto const Pos = es::Point(X, Y, 0.f);

        // Compute the pixel color.
        auto constexpr MaxIterations = 500;
        auto const Iterations        = ComputeIterations(Pos, Constant, MaxIterations);
        vPixelCfg.push_back(ldaSetPixelColor(Pos, Iterations, MaxIterations));
        MaxRegisteredIterations = std::max(MaxRegisteredIterations, Iterations);

        X += IncrementX;
      }
      X = Box.LowerLeft.x;
      Y += IncrementY;
    }

    {
      std::lock_guard<std::mutex> const lk(io_mutex);
#if 0
      std::cout << "-----------------------------------------------------------"
                << std::endl;
      std::cout << "Julia: Idx: " << Box.Idx
                << "\nBox.LowerLeft: " << Box.LowerLeft
                << "\nBox.UpperRigth: " << Box.UpperRigth << "\nX final " << X
                << "\nY final " << Y << ". IncrementX: " << IncrementX
                << ". IncrementY: " << IncrementY << std::endl;
      std::cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
                << std::endl;
#endif
    }

    return vPixelCfg;
  };

  auto const Start = es::Vector(-RenderSize.x, -RenderSize.y, 0.f) * 0.5f;

  // ---
  // NOTE: Set up Nthreads and give a block of the fractal to compute per.
  // available thread.
  // ---
  auto const Nthreads = std::max<unsigned int>(std::thread::hardware_concurrency(), 1);

  auto const NumBlocksX = 1;
  auto const NumBlocksY = Nthreads;
  auto const BlockSize  = es::Vector(RenderSize.x / NumBlocksX, RenderSize.y / NumBlocksY, 0.f);

  std::vector<box> vBox{};

  for (int Jdx = 0;      //!<
       Jdx < NumBlocksY; //!<
       ++Jdx) {
    for (int Idx = 0;      //!<
         Idx < NumBlocksX; //!<
         ++Idx) {

      box Box{};
      Box.LowerLeft  = Start + es::Vector(Idx * BlockSize.x, Jdx * BlockSize.y, 0.f);
      Box.UpperRigth = Box.LowerLeft + BlockSize;
      Box.Idx        = vBox.size();
      vBox.push_back(Box);
    }
  }

  // ---
  // NOTE: Create a vector that may be used by each thread.
  // This vector will store a vector of fluffy-pixels ;-)
  // ---
  std::vector<std::vector<fluffy::fractal::pixel>> vvPixel{};

  auto vT = std::vector<std::thread>(vBox.size());

  for (size_t Idx = 0;    //!<
       Idx < vBox.size(); //!<
       ++Idx) {

    auto const& Box = vBox.at(Idx);

    auto ldaT = [&vvPixel, ldaCreateFractalJuliaSet, &io_mutex](
                    box const& Box, Vector4 const& Resolution, Vector4 const& Constant) -> void {
      std::vector<fluffy::fractal::pixel> vPixel{};
      vPixel = ldaCreateFractalJuliaSet(Box, Resolution, Constant);
      {
        std::lock_guard<std::mutex> const lk(io_mutex);
        if (!vPixel.empty()) {
          vvPixel.push_back(vPixel);
        }
      }
    };
    vT.push_back(std::thread(ldaT, Box, Resolution, Constant));
  }

  for (size_t Idx = 0;  //!<
       Idx < vT.size(); //!<
       ++Idx) {
    if (vT.at(Idx).joinable()) {
      vT.at(Idx).join();
    }
  }

  fluffy::fractal::config Config{Constant, RenderSize};
  Config.vFractalPixels  = vPixel;
  Config.vvFractalPixels = vvPixel;

#if 0
  size_t TotalPixels{};
  for (auto const &E : vvPixel) {
    TotalPixels += E.size();
  }

  std::cout << "Num threads available is " << Nthreads
            << ".\nRender size input :" << RenderSize
            << ".\nRender size output:" << Config.Dimension
            << ".\nConstant:" << Config.Constant
            << ".\nNum points:" << vPixel.size()
            << ".\nMultithreadedPixels       : " << TotalPixels
            << ".\nEstimated number of pixels: "
            << RenderSize.x * Resolution.x * RenderSize.y * Resolution.y
            << std::endl;
#endif

  return Config;
}

}; // namespace fractal
}; // namespace fluffy

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
