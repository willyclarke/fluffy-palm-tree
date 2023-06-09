#include "fractal.hpp"
#include "curvesrobotics.hpp"
#include "engsupport.hpp"
#include "raylib.h"
#include "raymath.h"

#include <cmath>
#include <memory>

namespace {
/**
 * Compute Zn^2 + C
 */
auto ComputeNext(es::vector4_double const& Current, es::vector4_double const& Constant) -> es::vector4_double {

  // Zn^2
  auto const Zr = Current.x * Current.x - Current.y * Current.y;
  auto const Zi = 2.f * Current.x * Current.y;

  // Add constant and return
  return {Zr + Constant.x, Zi + Constant.y, 0., 0.};
}

/**
 * Return the mod squared.
 */
auto Mod2(es::vector4_double const& Z) -> float { return Z.x * Z.x + Z.y * Z.y; }

/**
 * Compute sequence elements until Mod exceeds threshold or max iterations.
 */
auto ComputeIterations(es::vector4_double const& Z0, es::vector4_double const& Constant, int MaxIterations = 50)
    -> float {
  auto Zn = Z0;
  auto Iteration{0};
  while (Mod2(Zn) < 4.f && Iteration < MaxIterations) {
    Zn = ComputeNext(Zn, Constant);
    ++Iteration;
  }

  return Iteration;
  // Create a smooth iteration count.
  auto const Mod             = std::sqrt(Mod2(Zn));
  auto const SmoothIteration = float(Iteration) - std::log2(std::max(1.f, std::log2f(Mod)));

  return SmoothIteration;
}

}; // end of anonymous namespace

namespace fluffy {
namespace fractal {

/**
 * @CenterX - Pixel X pos for center of screen.
 * @CenterY - Pixel Y pos for center of screen.
 * @Width -  In pixels.
 * @Height - In pixels.
 * @ResolutionX - Number of pixels in X direction per unit (m or s).
 * @ResolutionY - Number of pixels in Y direction per unit (m or s).
 */
auto ConfigurePixelCanvas(int CenterX, int CenterY, int Width, int Height, int ResolutionX, int ResolutionY)
    -> pixel_canvas {

  pixel_canvas Result{};

  int const UpperLeftX = CenterX - (Width >> 1);
  int const UpperLeftY = CenterY - (Height >> 1);

  Result.Dimension.x = Width;
  Result.Dimension.y = Height;
  Result.PosUL       = es::Point(UpperLeftX, UpperLeftY, 0.f);
  Result.PosUR       = es::Point(UpperLeftX + Width, UpperLeftY, 0.f);
  Result.PosLL       = es::Point(UpperLeftX, UpperLeftY + Height, 0.f);
  Result.PosLR       = es::Point(UpperLeftX + Width, UpperLeftY + Height, 0.f);

  Result.MhS2P = es::SetTranslation(es::Vector(UpperLeftX + Width / 2.f, UpperLeftY + Height / 2.f, 0.f));
  Result.MhS2P = Result.MhS2P * es::SetScaling(es::Vector(ResolutionX, -ResolutionY, 0.f));

  std::cout << __FUNCTION__ << "Center: " << CenterX << " " << CenterY << std::endl;
  std::cout << __FUNCTION__ << "Dimension:" << Result.Dimension << std::endl;
  std::cout << __FUNCTION__ << "UL: " << Result.PosUL << std::endl;
  std::cout << __FUNCTION__ << "UR: " << Result.PosUR << std::endl;
  std::cout << __FUNCTION__ << "LL: " << Result.PosLL << std::endl;
  std::cout << __FUNCTION__ << "LR: " << Result.PosLR << std::endl;
  std::cout << __FUNCTION__ << "MhS2P: " << Result.MhS2P << std::endl;

  // ---
  // NOTE: Set up Nthreads and give a block of the fractal to compute per.
  // available thread.
  // ---
  Result.NThreads   = std::max<unsigned int>(std::thread::hardware_concurrency(), 1);
  Result.YIncrement = float(Height) / float(Result.NThreads);

  return Result;
}

/**
 *
 */
auto GetFractalColor(double t) -> Color {
  long ColorCode = double(0xFFFFFF) * t;
  // Compute the color values
  unsigned char r = 0xFF & ColorCode;
  unsigned char g = 0xFF & (ColorCode >> 8);
  unsigned char b = 0xFF & (ColorCode >> 16);

  // Return the RGB color value
  return {r, g, b, 0xFF};
}

#if 1 // Code from https://www.youtube.com/watch?v=uc2yok_pLV4&t=244s
/**
 * Compute pixels color. Iterates over pixel space.
 * @RenderSize - Number of pixels X and Y.
 * @Constant - Fractal Constant as a complex number Real + iImg.
 */
auto Render(es::vector4_double const& RenderSize, es::vector4_double const& Constant) -> void {

  // Compute the size of the
  auto const Scale = 1.f / (RenderSize.y / 2.f);
  for (int Y{}; Y < RenderSize.y; ++Y) {
    for (int X{}; X < RenderSize.x; ++X) {

      // Compute the pixel coordinates.
      auto const Px = float(X - RenderSize.x / 2.f) * Scale;
      auto const Py = float(Y - RenderSize.y / 2.f) * Scale;

      // Compute the pixel color.
      auto constexpr MaxIterations = 500;
      auto const Iterations        = ComputeIterations({Px, Py, 0., 0.}, Constant, MaxIterations);

      auto const t = double(Iterations) / double(MaxIterations);
      DrawPixel(X, Y, GetFractalColor(t));
      // DrawPixel(X, Y, ldaGradient(Iterations, MaxIterations));
      // Color C{};
      // C.r = Y % 255;
      // C.g = X % 255;
      // C.b = Y % 100;
      // C.a = 255;
      // DrawPixel(X, Y, C);
    }
  }
}
#endif

/**
 * Render in pixel space.
 */
auto CreateFractalPixelSpace(currob::grid_cfg const&   GridCfg,
                             pixel_canvas&             PixelCanvas,
                             es::vector4_double const& Resolution,
                             es::vector4_double const& Constant,
                             Image&                    outputImage) -> void {

  auto const Zoom    = double(Resolution.x);
  bool&      PrintMe = PixelCanvas.PrintMe;

  // ---
  // Test Grid config
  // ---
  auto const PosUpperLeft  = es::VectorDouble(GridCfg.GridCenterValue.x - GridCfg.GridDimensions.x * 0.5f,
                                             GridCfg.GridCenterValue.y + GridCfg.GridDimensions.y * 0.5,
                                             0.f);
  auto const PosUpperRight = es::VectorDouble(GridCfg.GridCenterValue.x + GridCfg.GridDimensions.x * 0.5f,
                                              GridCfg.GridCenterValue.y + GridCfg.GridDimensions.y * 0.5,
                                              0.f);
  auto const PosLowerRight = es::VectorDouble(GridCfg.GridCenterValue.x + GridCfg.GridDimensions.x * 0.5f,
                                              GridCfg.GridCenterValue.y - GridCfg.GridDimensions.y * 0.5,
                                              0.f);

  auto const ExpectedNumPixels = static_cast<size_t>(PixelCanvas.Dimension.x * PixelCanvas.Dimension.y);

  if (outputImage.data == nullptr) {

    // Allocate memory for the pixel data
    std::shared_ptr<Color> spColorArray(new Color[ExpectedNumPixels], std::default_delete<Color[]>());

    if (spColorArray) {
      PixelCanvas.spColorArray = spColorArray;
      outputImage.data         = spColorArray.get();
      outputImage.width        = PixelCanvas.Dimension.x;
      outputImage.height       = PixelCanvas.Dimension.y;
      outputImage.format       = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
      outputImage.mipmaps      = 1;
      if (PrintMe) {
        std::cout << "outputImage.poutputImage.data:" << outputImage.data << std::endl;
        std::cout << "outputImage.width            :" << outputImage.width << std::endl;
        std::cout << "outputImage.height           :" << outputImage.height << std::endl;
      }
    }
  }

  if (PrintMe) {
    std::cout << "ExpectedNumPixels:" << ExpectedNumPixels << std::endl;
    std::cout << "PosUpperLeft:" << PosUpperLeft << std::endl;
    std::cout << "PosUpperRight:" << PosUpperRight << std::endl;
    std::cout << "PosLowerRight:" << PosLowerRight << std::endl;
  }

  auto const NumBlocksY = static_cast<size_t>(PixelCanvas.NThreads);
  auto const YIncrement = static_cast<double>(GridCfg.GridDimensions.y / NumBlocksY);

  struct data_fractal_gen {

    int XStart{};
    int XEnd{};
    int YStart{};
    int YEnd{};

    double             Zoom{};          //
    es::vector4_double PosUpperLeft{};  //
    es::vector4_double PosUpperRight{}; //
    es::vector4_double PosLowerRight{}; //
    es::vector4_double Constant{};      // Fractal constant
    int                PixelIdx{};      //
    size_t             Idx{};           //
    Color*             pColorArray{};   //
  };

  // ---
  // NOTE: Lambda for computing part of the fractal. Side effect is writing back to [&].
  // ---
  auto ldaJuliaSet = [](data_fractal_gen Data) -> void {
    auto PosXY = Data.PosUpperLeft;

    size_t Idx{};
    for (int Y = Data.YStart; Y < Data.YEnd; ++Y) {
      for (int X = Data.XStart; X < Data.XEnd; ++X) {

        // Compute the pixel color.
        auto constexpr MaxIterations = 500;
        auto const Iterations        = ComputeIterations(PosXY, Data.Constant, MaxIterations);

        auto const t = double(Iterations) / double(MaxIterations);

        fluffy::fractal::pixel Pixel{};
        Pixel.Pos = {double(X), double(Y), 0, 0};
        Pixel.Col = GetFractalColor(t);
        // Color& C  = Pixel.Col;
        // C.r       = Y % 255;
        // C.g       = X % 255;
        // C.b       = Y % 100;
        // C.a       = 255;

        *(Data.pColorArray + Idx) = Pixel.Col;
        ++Idx;

        PosXY.x = std::min(PosXY.x + 1. / Data.Zoom, Data.PosUpperRight.x);
      }
      PosXY.x = Data.PosUpperLeft.x;
      PosXY.y = std::max(PosXY.y - 1. / Data.Zoom, Data.PosLowerRight.y);
    }
  };

  // ---
  // NOTE: Create threads for each of the blocks that need to be computed.
  // Compute a start position for each block that can be used by each of the threads for computing part of the fractal.
  //       Since we start at the upper left Y will actually decrement, hence the minus sign.
  // ---
  auto vT       = std::vector<std::thread>{};
  auto PixelIdx = 0;

  for (size_t Idx = 0;   //!<
       Idx < NumBlocksY; //!<
       ++Idx             //!<
  ) {
    data_fractal_gen Data{};

    Data.XStart        = PixelCanvas.PosUL.x;
    Data.XEnd          = PixelCanvas.PosUL.x + PixelCanvas.Dimension.x;
    Data.YStart        = PixelCanvas.PosUL.y + PixelCanvas.YIncrement * Idx;
    Data.YEnd          = Data.YStart + PixelCanvas.YIncrement;
    Data.Zoom          = Zoom;
    Data.PosUpperLeft  = PosUpperLeft + es::VectorDouble(0., -double(YIncrement * Idx), 0.);
    Data.PosUpperRight = PosUpperRight + es::VectorDouble(0., -double(YIncrement * Idx), 0.);
    Data.PosLowerRight = es::VectorDouble(PosUpperRight.x, Data.PosUpperRight.y - YIncrement, 0.);
    Data.Constant      = Constant;
    Data.PixelIdx      = PixelIdx;
    Data.Idx           = Idx;
    if (outputImage.data)
      Data.pColorArray = (Color*)(outputImage.data) + PixelIdx;
    else
      Data.pColorArray = nullptr;

    if (PrintMe) {
      std::cout << __FUNCTION__ << "-> pData.pColorArray: " << Data.pColorArray << std::endl;
      std::cout << "Idx: " << Idx << ". XStart: " << Data.XStart << ". XEnd: " << Data.XEnd << ". Zoom:" << Data.Zoom
                << std::endl;
      std::cout << "Idx: " << Idx << ". YStart: " << Data.YStart << ". YEnd: " << Data.YEnd << std::endl;
      std::cout << "Idx: " << Idx << ". PixelIdx: " << PixelIdx << std::endl;
      std::cout << "Idx: " << Idx << " -> PosUpperLeft(" << Idx << ")=" << Data.PosUpperLeft << std::endl;
      std::cout << "Idx: " << Idx << " -> PosUpperRight(" << Idx << ")=" << Data.PosUpperRight << std::endl;
      std::cout << "Idx: " << Idx << " -> PosLowerRight(" << Idx << ")=" << Data.PosLowerRight << std::endl;
      std::cout << " ---- " << std::endl;
    }

    PixelIdx += (PixelCanvas.YIncrement * (PixelCanvas.PosUR.x - PixelCanvas.PosLL.x));
    vT.push_back(std::thread(ldaJuliaSet, Data));
  }

  for (auto& T : vT) {
    if (T.joinable())
      T.join();
  }

  PrintMe = false;

  return;
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
