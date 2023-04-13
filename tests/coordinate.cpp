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
