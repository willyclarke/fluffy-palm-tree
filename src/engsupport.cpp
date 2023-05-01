/*******************************************************************************************
 *
 *   Engineering support
 *
 *   BSD-like license that allows static linking with closed source software
 *
 *   Copyright (c) 2023 Willy Clarke (willyclarke@gmail.com)
 *
 ********************************************************************************************/

#include "engsupport.hpp"
#include "raylib.h"

#define RAYMATH_IMPLEMENTATION // Define external out-of-line implementation
#include "raymath.h"           // Vector3, Quaternion and Matrix functionality

#include <iomanip>

/**
 * es - engineering support namespace
 */
namespace es // aka engineering support
{

/**
 * Cause a crash when condition is false.
 */
void Assert(bool Condition, char const* pCaller, int Line) {
  if (!Condition) {
    volatile char* pChar = nullptr;
    *pChar               = 't';
  }
}

/**
 * Identity matrix 4x4
 */
Matrix I() {
  Matrix M{};
  M.m0  = 1.f;
  M.m5  = 1.f;
  M.m10 = 1.f;
  M.m15 = 1.f;
  return M;
}

/**
 * Compute the Determinant of a 4x4 matrix.
 */
float Determinant(Matrix const& In) {
  float const Det = In.m0 * (In.m5 * In.m10 - In.m9 * In.m6) - In.m4 * (In.m1 * In.m10 - In.m9 * In.m2) +
                    In.m8 * (In.m1 * In.m6 - In.m5 * In.m2) -
                    In.m12 * (In.m1 * In.m6 * In.m11 - In.m1 * In.m10 * In.m7 - In.m5 * In.m2 * In.m11 +
                              In.m5 * In.m10 * In.m3 + In.m9 * In.m2 * In.m7 - In.m9 * In.m6 * In.m3);

  return Det;
}

/**
 * Return true when matrix is invertible.
 */
bool IsMatrixInvertible(Matrix const& In) { return Determinant(In) != 0.0f; }

//------------------------------------------------------------------------------
/**
 * Return matrix 4x4 for conversion from engineering space to screen space.
 * Screen space is a floating point representation with x,y,z = 0,0,0 beeing at
 * the middle of the window.
 */
Matrix InitTranslationInv(Matrix const& M, Vector4 const& vTranslation) {
  Matrix Mat = M;
  // float m0, m4, m8, m12;  // Matrix first row (4 components)
  // float m1, m5, m9, m13;  // Matrix second row (4 components)
  // float m2, m6, m10, m14; // Matrix third row (4 components)
  // float m3, m7, m11, m15; // Matrix fourth row (4 components)
  Mat.m0  = 1.f * M.m0;
  Mat.m5  = 1.f * M.m5;
  Mat.m10 = 1.f * M.m10;
  Mat.m12 = -vTranslation.x;
  Mat.m13 = -vTranslation.y;
  Mat.m14 = -vTranslation.z;
  Mat.m15 = 1.f;

  return Mat;
}

/**
 */
auto SetTranslation(Vector4 const& Translation) -> Matrix {
  auto Result = es::I();
  Result.m12  = Translation.x;
  Result.m13  = Translation.y;
  Result.m14  = Translation.z;
  return Result;
}

/**
 */
auto SetScaling(Vector4 const& Scale) -> Matrix {
  auto Result = es::I();
  Result.m0   = Scale.x;
  Result.m5   = Scale.y;
  Result.m10  = Scale.z;
  return Result;
}

/**
 * Defintions: A point in 3D space has w set to 1.
 *             A vector in 3D space has w set to 0.
 *             This implies that adding Vectors gives a new Vector.
 *             And Adding a Vector to a Point gives a new Point.
 *             And there is no meaning in adding Points since w would not be 1.
 */
Vector4 Point(float X, float Y, float Z) { return Vector4{X, Y, Z, 1.f}; }

/**
 * Defintions: A point in 3D space has w set to 1.
 *             A vector in 3D space has w set to 0.
 *             This implies that adding Vectors gives a new Vector.
 *             And Adding a Vector to a Point gives a new Point.
 *             And there is no meaning in adding Points since w would not be 1.
 */
Vector4 Vector(float X, float Y, float Z) { return Vector4{X, Y, Z, 0.f}; }

vector4_double Vector4Double(double X, double Y, double Z) { return vector4_double{X, Y, Z, 0.f}; }

/**
 * Return matrix 4x4 for conversion from engineering space to screen space.
 * Screen space is a floating point representation with x,y,z = 0,0,0 beeing at
 * the middle of the window.
 */
Matrix InitScaling(Matrix const& M, Vector4 const& Scale, bool Reflection) {
  Matrix     Mat  = M;
  auto const Sign = Reflection ? -1.f : 1.f;
  Mat.m0          = Sign * Scale.x;
  Mat.m5          = Sign * Scale.y;
  Mat.m10         = Sign * Scale.z;
  Mat.m15         = 1.f;

  return Mat;
}

/**
 * Return the result of multiplication of a Matrix and a Vector, dimension 4.
 */
Vector4 Mul(Matrix const& M, Vector4 const& V) {
  Vector4 Result{};
  Result.x = M.m0 * V.x + M.m4 * V.y + M.m8 * V.z + M.m12 * V.w;
  Result.y = M.m1 * V.x + M.m5 * V.y + M.m9 * V.z + M.m13 * V.w;
  Result.z = M.m2 * V.x + M.m6 * V.y + M.m10 * V.z + M.m14 * V.w;
  Result.w = M.m3 * V.x + M.m7 * V.y + M.m11 * V.z + M.m15 * V.w;
  return Result;
}

/**
 */
Vector4 Add(Vector4 const& V1, Vector4 const& V2) {
  return Vector4{V1.x + V2.x, V1.y + V2.y, V1.z + V2.z, std::min(1.f, V1.w + V2.w)};
}

vector4_double Add(vector4_double const& V1, vector4_double const& V2) {
  return vector4_double{V1.x + V2.x, V1.y + V2.y, V1.z + V2.z, std::min(1., V1.w + V2.w)};
}

/**
 * Dot product.
 */
float Dot(Vector4 const& V1, Vector4 const& V2) { return V1.x * V2.x + V1.y * V2.y + V1.z * V2.z + V1.w * V2.w; }

/**
 * Multiplication will keep w unchanged.
 */
Vector4 Mul(Vector4 const& V1, float c) {
  Vector4 const Result{V1.x * c, V1.y * c, V1.z * c, V1.w};
  return Result;
}

/**
 * Subtracting two vectors gives a new vector
 * Subtracting two points gives a vector
 * Subtracting vectors with point and vice versa has no meaning.
 */
Vector4 Sub(Vector4 const& V1, Vector4 const& V2) {
  return Vector4{V1.x - V2.x, V1.y - V2.y, V1.z - V2.z, V1.w - V2.w};
}

/**
 */
Matrix Add(Matrix const& M1, Matrix const& M2) {
  Matrix Result{};
  Result.m0  = M1.m0 + M2.m0;
  Result.m1  = M1.m1 + M1.m1;
  Result.m2  = M1.m2 + M2.m2;
  Result.m3  = M1.m3 + M2.m3;
  Result.m4  = M1.m4 + M2.m4;
  Result.m5  = M1.m5 + M2.m5;
  Result.m6  = M1.m6 + M2.m6;
  Result.m7  = M1.m7 + M2.m7;
  Result.m8  = M1.m8 + M2.m8;
  Result.m9  = M1.m9 + M2.m9;
  Result.m10 = M1.m10 + M2.m10;
  Result.m11 = M1.m11 + M2.m11;
  Result.m12 = M1.m12 + M2.m12;
  Result.m13 = M1.m13 + M2.m13;
  Result.m14 = M1.m14 + M2.m14;
  Result.m15 = M1.m15 + M2.m15;
  return Result;
}

/**
 */
bool Eq(Matrix const& M1, Matrix const& M2) {
  auto const Result = M1.m0 == M2.m0 &&   //!<
                      M1.m1 == M2.m1 &&   //!<
                      M1.m2 == M2.m2 &&   //!<
                      M1.m3 == M2.m3 &&   //!<
                      M1.m4 == M2.m4 &&   //!<
                      M1.m5 == M2.m5 &&   //!<
                      M1.m6 == M2.m6 &&   //!<
                      M1.m7 == M2.m7 &&   //!<
                      M1.m8 == M2.m8 &&   //!<
                      M1.m9 == M2.m9 &&   //!<
                      M1.m10 == M2.m10 && //!<
                      M1.m11 == M2.m11 && //!<
                      M1.m12 == M2.m12 && //!<
                      M1.m13 == M2.m13 && //!<
                      M1.m14 == M2.m14 && //!<
                      M1.m15 == M2.m15    //!<
      ;
  return Result;
}

/**
 */
bool Eq(Vector4 const& V1, Vector4 const& V2) {
  auto const Result = V1.x == V2.x && //!<
                      V1.y == V2.y && //!<
                      V1.z == V2.z && //!<
                      V1.w == V2.w;   //!<
  return Result;
}

/**
 * Computes the linear interpolation between A and B, if the parameter t is
 * inside [0, 1]
 */
Vector4 Lerp(Vector4 const& A, Vector4 const& B, float t) {

  if (t < 0.f || t > 1.f)
    return {};
  return A + (B - A) * t;
}

/**
 */
Matrix Mul(Matrix const& M1, Matrix const& M2) {
  Matrix Result{};
  Result.m0 = Dot(Vector4{M1.m0, M1.m4, M1.m8, M1.m12}, Vector4{M2.m0, M2.m1, M2.m2, M2.m3});
  Result.m1 = Dot(Vector4{M1.m1, M1.m5, M1.m9, M1.m13}, Vector4{M2.m0, M2.m1, M2.m2, M2.m3});
  Result.m2 = Dot(Vector4{M1.m2, M1.m6, M1.m10, M1.m14}, Vector4{M2.m0, M2.m1, M2.m2, M2.m3});
  Result.m3 = Dot(Vector4{M1.m3, M1.m7, M1.m11, M1.m15}, Vector4{M2.m0, M2.m1, M2.m2, M2.m3});

  Result.m4 = Dot(Vector4{M1.m0, M1.m4, M1.m8, M1.m12}, Vector4{M2.m4, M2.m5, M2.m6, M2.m7});
  Result.m5 = Dot(Vector4{M1.m1, M1.m5, M1.m9, M1.m13}, Vector4{M2.m4, M2.m5, M2.m6, M2.m7});
  Result.m6 = Dot(Vector4{M1.m2, M1.m6, M1.m10, M1.m14}, Vector4{M2.m4, M2.m5, M2.m6, M2.m7});
  Result.m7 = Dot(Vector4{M1.m3, M1.m7, M1.m11, M1.m15}, Vector4{M2.m4, M2.m5, M2.m6, M2.m7});

  Result.m8  = Dot(Vector4{M1.m0, M1.m4, M1.m8, M1.m12}, Vector4{M2.m8, M2.m9, M2.m10, M2.m11});
  Result.m9  = Dot(Vector4{M1.m1, M1.m5, M1.m9, M1.m13}, Vector4{M2.m8, M2.m9, M2.m10, M2.m11});
  Result.m10 = Dot(Vector4{M1.m2, M1.m6, M1.m10, M1.m14}, Vector4{M2.m8, M2.m9, M2.m10, M2.m11});
  Result.m11 = Dot(Vector4{M1.m3, M1.m7, M1.m11, M1.m15}, Vector4{M2.m8, M2.m9, M2.m10, M2.m11});

  Result.m12 = Dot(Vector4{M1.m0, M1.m4, M1.m8, M1.m12}, Vector4{M2.m12, M2.m13, M2.m14, M2.m15});
  Result.m13 = Dot(Vector4{M1.m1, M1.m5, M1.m9, M1.m13}, Vector4{M2.m12, M2.m13, M2.m14, M2.m15});
  Result.m14 = Dot(Vector4{M1.m2, M1.m6, M1.m10, M1.m14}, Vector4{M2.m12, M2.m13, M2.m14, M2.m15});
  Result.m15 = Dot(Vector4{M1.m3, M1.m7, M1.m11, M1.m15}, Vector4{M2.m12, M2.m13, M2.m14, M2.m15});
  return Result;
}

vector4_double VectorDouble(double X, double Y, double Z) { return vector4_double{X, Y, Z, 0.}; }
vector4_double VectorDouble(Vector4 const& V) { return vector4_double{V.x, V.y, V.z, V.w}; }
/**
 * Return vector containing the absolute value of the elements on the diagonal
 * of a Matrix. Can be used for pulling out resolution.
 */
auto DiagVector(Matrix const& MhE2P) -> Vector4 { return es::Vector(MhE2P.m0, MhE2P.m5, MhE2P.m10); }

/**
 * Return vector containing the absolute value of the elements on the diagonal
 * of a Matrix. Can be used for pulling out resolution.
 */
auto DiagVectorAbs(Matrix const& MhE2P) -> Vector4 {
  auto D = DiagVector(MhE2P);
  return es::Vector(std::abs(D.x), std::abs(D.y), std::abs(D.z));
}

}; // namespace es

Matrix              operator*(Matrix const& M1, Matrix const& M2) { return es::Mul(M1, M2); }
Matrix              operator+(Matrix const& M1, Matrix const& M2) { return es::Add(M1, M2); }
bool                operator==(Matrix const& M1, Matrix const& M2) { return es::Eq(M1, M2); }
bool                operator!=(Matrix const& M1, Matrix const& M2) { return !(M1 == M2); }
Vector4             operator*(Matrix const& M, Vector4 const& V) { return es::Mul(M, V); }
Vector4             operator*(Vector4 const& V1, float c) { return es::Mul(V1, c); }
Vector4             operator+(Vector4 const& V1, Vector4 const& V2) { return es::Add(V1, V2); }
es::vector4_double  operator+(es::vector4_double const& V1, es::vector4_double const& V2) { return es::Add(V1, V2); }
es::vector4_double& operator+=(es::vector4_double& LHS, es::vector4_double const& RHS) {
  LHS = es::Add(LHS, RHS);
  return LHS;
}
float   operator*(Vector4 const& V1, Vector4 const& V2) { return es::Dot(V1, V2); }
Vector4 operator-(Vector4 const& V1, Vector4 const& V2) { return es::Sub(V1, V2); }
bool    operator==(Vector4 const& V1, Vector4 const& V2) { return es::Eq(V1, V2); }

/**
 */
std::ostream& operator<<(std::ostream& stream, const Vector4& T) {
  // ---
  // NOTE: The width need to be big enough to hold a negative sign.
  // ---
  size_t const P{5};
  size_t const W{P + 5};
  stream << ((T.w != 0) ? "Point :" : "Vector:");
  stream << " " << std::fixed << std::setprecision(P) << std::setw(W) << T.x << " " << std::fixed
         << std::setprecision(P) << std::setw(W) << T.y << " " << std::fixed << std::setprecision(P) << std::setw(W)
         << T.z << " " << std::fixed << std::setprecision(P) << std::setw(W) << T.w;
  return stream;
}

/**
 */
std::ostream& operator<<(std::ostream& stream, const es::vector4_double& T) {
  // ---
  // NOTE: The width need to be big enough to hold a negative sign.
  // ---
  size_t const P{5};
  size_t const W{P + 5};
  stream << ((T.w != 0) ? "Point :" : "Vector:");
  stream << " " << std::fixed << std::setprecision(P) << std::setw(W) << T.x << " " << std::fixed
         << std::setprecision(P) << std::setw(W) << T.y << " " << std::fixed << std::setprecision(P) << std::setw(W)
         << T.z << " " << std::fixed << std::setprecision(P) << std::setw(W) << T.w;
  return stream;
}

/**
 */
std::ostream& operator<<(std::ostream& stream, const Matrix& M) {
  // ---
  // NOTE: The width need to be big enough to hold a negative sign.
  // ---
  size_t const P{5};
  size_t const W{P + 5};
  stream << "Matrix\n";
  stream << " " << std::fixed << std::setprecision(P) << std::setw(W) << M.m0 << " " << std::fixed
         << std::setprecision(P) << std::setw(W) << M.m4 << " " << std::fixed << std::setprecision(P) << std::setw(W)
         << M.m8 << " " << std::fixed << std::setprecision(P) << std::setw(W) << M.m12 << "\n";
  stream << " " << std::fixed << std::setprecision(P) << std::setw(W) << M.m1 << " " << std::fixed
         << std::setprecision(P) << std::setw(W) << M.m5 << " " << std::fixed << std::setprecision(P) << std::setw(W)
         << M.m9 << " " << std::fixed << std::setprecision(P) << std::setw(W) << M.m13 << "\n";
  stream << " " << std::fixed << std::setprecision(P) << std::setw(W) << M.m2 << " " << std::fixed
         << std::setprecision(P) << std::setw(W) << M.m6 << " " << std::fixed << std::setprecision(P) << std::setw(W)
         << M.m10 << " " << std::fixed << std::setprecision(P) << std::setw(W) << M.m14 << "\n";
  stream << " " << std::fixed << std::setprecision(P) << std::setw(W) << M.m3 << " " << std::fixed
         << std::setprecision(P) << std::setw(W) << M.m7 << " " << std::fixed << std::setprecision(P) << std::setw(W)
         << M.m11 << " " << std::fixed << std::setprecision(P) << std::setw(W) << M.m15 << "\n";
  return stream;
}
