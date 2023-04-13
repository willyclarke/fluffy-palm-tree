/**
 * Test code for matrix conversion to and from pixelspace.
 *
 * MIT License - see at bottom of file.
 * Copyright (c) 2023 Willy Clarke
 */

#include <catch2/catch_test_macros.hpp>

#include "../src/engsupport.hpp"
#include "raylib.h"
#include "raymath.h"

unsigned int Factorial(unsigned int number) { return number <= 1 ? number : Factorial(number - 1) * number; }

TEST_CASE("Factorials are computed", "[factorial]") {
  REQUIRE(Factorial(1) == 1);
  REQUIRE(Factorial(2) == 2);
  REQUIRE(Factorial(3) == 6);
  REQUIRE(Factorial(10) == 3628800);
}

/**
 * Test if a matrix is invertible.
 */
TEST_CASE("Invert a Matrix", "[Linear algebra]") {

  Matrix A{1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 8.f, 7.f, 6.f, 5.f, 4.f, 3.f, 2.f};
  Matrix B{-2.f, 1.f, 2.f, 3.f, 3.f, 2.f, 1.f, -1.f, 4.f, 3.f, 6.f, 5.f, 1.f, 2.f, 7.f, 8.f};
  Matrix Expect{20.f, 22.f, 50.f, 48.f, 44.f, 54.f, 114.f, 108.f, 40.f, 58.f, 110.f, 102.f, 16.f, 26.f, 46.f, 42.f};

  auto const M = A * B;
  REQUIRE(M == Expect);

  auto BIsInvertible = es::IsMatrixInvertible(B);
  REQUIRE(true == BIsInvertible);

  if (BIsInvertible) {
    auto InvB   = MatrixInvert(B);
    auto TstInv = B * InvB;
    REQUIRE(TstInv == es::I());
  }
}

/**
 */
TEST_CASE("Lerp between two points", "[engsupport]") {
  Vector4 Start = es::Vector(1.f, 0.f, 0.f);
  Vector4 End   = es::Vector(1.f, 1.f, 0.f);
  REQUIRE((es::Lerp(Start, End, 0.f) == Start) == true);
  REQUIRE((es::Lerp(Start, End, 0.5f) == (Start + (End - Start) * 0.5f)) == true);
  REQUIRE((es::Lerp(Start, End, 0.75f) == (Start + (End - Start) * 0.75f)) == true);
  REQUIRE((es::Lerp(Start, End, 0.5f) == es::Vector(1.f, 0.5f, 0.f)) == true);
  REQUIRE((es::Lerp(Start, End, 1.f) == End) == true);

  // for (int Idx = 0; Idx < 100; ++Idx) {
  //   float const c = float(Idx) / 100.f;
  //   std::cout << "Lerp at c=" << c << " = " << es::Lerp(Start, End, c) << std::endl;
  // }
}

/**
 * Test changes from coordinate system to screen coordinates.
 */
TEST_CASE("Test3dScreenCalculations2", "[engsupport]") {
  // Given the engineering input
  auto const Eo             = es::Point(1.f, 1.f, 0.f);       //!< Engineering value at Screen Origo
  auto const Pe0            = es::Point(0.f, 0.f, 0.f);       //!< Point Engineering 0
  auto const Pe1            = Eo;                             //!< Point Engineering 1
  auto const So             = es::Point(0.f, 0.f, 0.f);       //!< Screen Origo
  auto const PixPerUnit     = es::Point(100.f, 100.f, 0.f);   //!< Pixel Per Unit
  auto const ScreenSize     = es::Point(1280.f, 1024.f, 0.f); //!< Screen size
  auto const ScreenSizeHalf = ScreenSize * 0.5f;              //!< Screen half size

  // ---
  // NOTE: The concept is to have the screen centre as Screen Origo aka 0,0,0 in x,y,z.
  //       And to have a mapping from Engineering value to Screen value
  //       so that it is possible to have the screen center become engineering
  //       value, say, 3,3,0.
  //       These matrix transforms should do a coordinate system transform
  //       from Engineering -> Screen -> Pixel.
  // ---

  auto const MhE2S = MatrixInvert(es::SetTranslation(Eo));

  // ---
  // NOTE: Test that the homogenous matrix get set up correctly.
  // ---
  {
    auto const Ps = MhE2S * Pe0;
    // std::cout << "Ps :" << Ps << std::endl;
    REQUIRE(es::Vector(MhE2S.m12, MhE2S.m13, MhE2S.m14) == (es::Vector(Eo.x, Eo.y, Eo.z) * -1.f));
    REQUIRE(Ps == es::Point(-1.f, -1.f, -0.f));
  }

  // ---
  // NOTE: Test that the Point engineering 1 is translated to Screen origo.
  // ---
  {
    auto const Ps = MhE2S * Pe1;
    // std::cout << "Ps :" << Ps << std::endl;
    REQUIRE(Ps == So);
  }

  // Compute the scaling into pixel space - aka Matrix Pixel Scale = MhS2P
  // That gives the Pixel Point Pp.
  // auto const MhS2P = es::InitScaling({}, es::Point(100.f, 100.f, 100.f));
  auto MhS2P             = es::SetTranslation(ScreenSizeHalf);
  auto constexpr Reflect = true;
  MhS2P                  = es::InitScaling(MhS2P, PixPerUnit, Reflect);

  // ---
  // NOTE: Convert from Screen coordinate to Pixel coordinate.
  // ---
  auto const PixelPos = MhS2P * So;
  REQUIRE(PixelPos == ScreenSizeHalf);

  auto const PixelPosPe0 = MhS2P * MhE2S * Pe0;
  auto const PixelPosPe1 = MhS2P * MhE2S * Pe1;

  // ---
  // NOTE: Compute the total homogenous matrix for coordinate system transforms
  //       from engineering space to pixelspace.
  // ---
  auto       MhE2P        = MhS2P * MhE2S;
  auto const PixelPosPe0_ = MhE2P * Pe0;
  auto const PixelPosPe1_ = MhE2P * Pe1;
  REQUIRE(PixelPosPe0 == PixelPosPe0_);
  REQUIRE(PixelPosPe1 == PixelPosPe1_);
}

/**
 * Test changes from coordinate system to screen coordinates.
 * This test uses InitTranslationInv which should be removed. FIXME: (Willy Clarke)
 */
TEST_CASE("Test3dScreenCalculations", "[engsupport]") {
  // Given the engineering input
  auto const Pe = es::Point(0.f, 0.f, 0.f);

  // ---
  // NOTE: The concept is to have the screen centre as Screen Origo aka 0,0,0 in x,y,z.
  //       And to have a mapping from Engineering value to Screen value
  //       so that it is possible to have the screen center become engineering
  //       value, say, 3,3,0.
  //       These matrix transforms should do a coordinate system transform
  //       from Engineering -> Screen -> Pixel.
  // ---

  auto const MhE2S = es::InitTranslationInv({}, es::Point(1.f, 1.f, 1.f));
  auto const Ps    = MhE2S * Pe;
  REQUIRE(es::Point(MhE2S.m12, MhE2S.m13, MhE2S.m14) == es::Point(-1.f, -1.f, -1.f));
  REQUIRE(Ps == es::Point(-1.f, -1.f, -1.f));

  // Compute the scaling into pixel space - aka Matrix Pixel Scale = MhS2P
  // That gives the Pixel Point Pp.
  auto const MhS2P = es::InitScaling({}, es::Point(100.f, 100.f, 100.f));
  auto const Pps   = MhS2P * Ps;
  REQUIRE(Pps == es::Point(-100.f, -100.f, -100.f));

  // Compute the translation onto the screen based on 0,0 beeing top left of
  // screen
  // Matrix Translation - aka MhE2P
  auto const MhE2P = es::InitTranslationInv({}, es::Point(-1280.f / 2.f, -1024.f / 2.f, 0.f));

  auto const Pp = MhE2P * Pps;
  REQUIRE(Pp == es::Point(1280.f / 2.f, 1024.f / 2.f, 0.f));
}

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
