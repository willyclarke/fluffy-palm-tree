/*******************************************************************************************
 *
 *   Engineering support
 *
 *   BSD-like license that allows static linking with closed source software
 *
 *   Copyright (c) 2023 Willy Clarke (willyclarke@gmail.com)
 *
 ********************************************************************************************/
#ifndef ENGSUPPORT_HPP
#define ENGSUPPORT_HPP

#include "raylib.h"
#include <iostream>

/**
 * es - engineering support namespace
 */
namespace es // aka engineering support
{
//------------------------------------------------------------------------------
void Assert(bool Condition, char const* pCaller, int Line = 0);

/**
 * Identity matrix 4x4
 */
Matrix I();

/**
 * Compute the determinant and check it to find out if matrix is invertible.
 */
bool IsMatrixInvertible(Matrix const& In);

//------------------------------------------------------------------------------
/**
 * Return matrix 4x4 for conversion from engineering space to screen space.
 * Screen space is a floating point representation with x,y,z = 0,0,0 beeing at
 * the middle of the window.
 */
Matrix InitTranslationInv(Matrix const& M, Vector4 const& vTranslation);

/**
 * Use a Homogenous matrix to store a translation.
 */
auto SetTranslation(Vector4 const& Translation) -> Matrix;

/**
 * Set scaling for x,y,z. Use negative numbers to flip the direction.
 */
auto SetScaling(Vector4 const& Scale) -> Matrix;

/**
 * Defintions: A point in 3D space has w set to 1.
 *             A vector in 3D space has w set to 0.
 *             This implies that adding Vectors gives a new Vector.
 *             And Adding a Vector to a Point gives a new Point.
 *             And there is no meaning in adding Points since w would not be 1.
 */
Vector4 Point(float X, float Y, float Z);

/**
 * Defintions: A point in 3D space has w set to 1.
 *             A vector in 3D space has w set to 0.
 *             This implies that adding Vectors gives a new Vector.
 *             And Adding a Vector to a Point gives a new Point.
 *             And there is no meaning in adding Points since w would not be 1.
 */
Vector4 Vector(float X, float Y, float Z);

// Vector4Double, 4 components
struct vector4_double {
  double x{}; // Vector x component
  double y{}; // Vector y component
  double z{}; // Vector z component
  double w{}; // Vector w component
};

vector4_double VectorDouble(double X, double Y, double Z);
vector4_double VectorDouble(Vector4 const& V);

//------------------------------------------------------------------------------
/**
 * Return matrix 4x4 for conversion from engineering space to screen space.
 * Screen space is a floating point representation with x,y,z = 0,0,0 beeing at
 * the middle of the window.
 */
Matrix InitScaling(Matrix const& M, Vector4 const& Scale, bool Reflection = false);

/**
 * Return the result of multiplication of a Matrix and a Vector, dimension 4.
 */
Vector4 Mul(Matrix const& M, Vector4 const& V);

//------------------------------------------------------------------------------
float Dot(Vector4 const& V1, Vector4 const& V2);

//------------------------------------------------------------------------------
Vector4 Mul(Vector4 const& V1, float t);

//------------------------------------------------------------------------------
Matrix Add(Matrix const& M1, Matrix const& M2);

//------------------------------------------------------------------------------
bool Eq(Matrix const& M1, Matrix const& M2);
bool Eq(Vector4 const& V1, Vector4 const& V2);

//------------------------------------------------------------------------------
Vector4 Lerp(Vector4 const& A, Vector4 const& B, float t);

//------------------------------------------------------------------------------
Matrix Mul(Matrix const& M1, Matrix const& M2);

//------------------------------------------------------------------------------
auto DiagVector(Matrix const& MhE2P) -> Vector4;
auto DiagVectorAbs(Matrix const& MhE2P) -> Vector4;
}; // namespace es

Matrix              operator*(Matrix const& M1, Matrix const& M2);
Matrix              operator+(Matrix const& M1, Matrix const& M2);
bool                operator==(Matrix const& M1, Matrix const& M2);
bool                operator!=(Matrix const& M1, Matrix const& M2);
Vector4             operator*(Matrix const& M, Vector4 const& V);
float               operator*(Vector4 const& V1, Vector4 const& V2);
Vector4             operator*(Vector4 const& V1, float t);
Vector4             operator+(Vector4 const& V1, Vector4 const& V2);
Vector4             operator-(Vector4 const& V1, Vector4 const& V2);
bool                operator==(Vector4 const& V1, Vector4 const& V2);
es::vector4_double  operator+(es::vector4_double const& V1, es::vector4_double const& V2);
es::vector4_double& operator+=(es::vector4_double& LHS, es::vector4_double const& RHS);

std::ostream& operator<<(std::ostream& stream, const Vector4& T);
std::ostream& operator<<(std::ostream& stream, const es::vector4_double& T);
std::ostream& operator<<(std::ostream& stream, const Matrix& M);

#endif
