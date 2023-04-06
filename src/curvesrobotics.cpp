
/*******************************************************************************************
 * Famous Curves for Robotics in C++
 *
 * Display of curves as suggested by Markus Buchholz on Medium.
 * https://medium.com/@markus-x-buchholz/famous-curves-for-robotics-in-c-55aa916d1191
 *
 * Copyright (c) 2023 Willy Clarke
 *
 * MIT License
 *
 *******************************************************************************************/

#include "engsupport.hpp"
#include "raylib.h"

#define RAYMATH_IMPLEMENTATION // Define external out-of-line implementation
#include "raymath.h"           // Vector3, Quaternion and Matrix functionality

#include <algorithm>

#define _USE_MATH_DEFINES
#include <cmath>

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {
/**
 * Hold pixel position as integers, X and Y.
 */
struct pixel_pos {
  int X{};
  int Y{};
  Color color{LIGHTGRAY};
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
  Vector4 GridOrigo{0.f, 0.f, 0.f, 1.f}; //!< Make it a Point.

  /**
   * x is GridLength
   * y is GridHeight
   */
  Vector4 GridDimensions{8.f, 6.f, 0.f, 0.f};
};

//------------------------------------------------------------------------------
struct data {

  // Declare the function pointer
  auto(*UpdateDrawFramePointer)(data *) -> void;
  std::vector<std::string> vHelpTextPage{};
  std::string WikipediaLink{};

  int screenWidth = 1280;
  int screenHeight = 768;

  enum class pages { PageAsteroid, PageFourier, PageFractal, PageHelp };
  pages PageNum{};

  int Key{};
  int KeyPrv{};
  bool TakeScreenshot{};
  bool StopUpdate{};
  bool ShowGrid{true};
  float Xcalc{};
  int n{5}; //!< Fourier series number of terms.
  float dt{};
  float t{};
  std::vector<Vector4> vTrendPoints{};
  size_t CurrentTrendPoint{};
  size_t NumTrendPoints{};
  grid_cfg GridCfg{};

  Matrix MhE2P{}; //!< Homogenous matrix for conversion from engineering space
                  //!< to pixelspace.

  Matrix MhE2PInv{}; //!< Homogenous matrix for conversion from pixel
                     //!< space to engineering space.

  Matrix MhG2E{}; //!< Homogenous matrix for conversion from grid
                  //!< space to engineering space.

  Matrix MhG2EInv{}; //!< Homogenous matrix for conversion from grid
                     //!< space to engineering space.

  Vector4 vEngOffset{}; //!< Position of figure in engineering space.
  Vector4 vPixelsPerUnit{100.f, 100.f, 100.f, 0.f};

  Vector4 MousePosEng{};
  Vector4 MousePosGrid{};
  struct mouse_input {
    bool MouseButtonPressed{};
    bool MouseButtonDown{};
    bool MouseButtonReleased{};
    bool MouseButtonUp{};
  };
  mouse_input MouseInput{};
};

/*
 * Create lines and ticks for a grid in engineering units.
 */
auto GridCfgInPixels(Matrix const &MhE2P, //!< Homogenous matrix from
                     grid_cfg const &GridCfg) -> grid_cfg {

  auto Result = GridCfg;
  Result.vGridLines.clear();
  Result.vGridSubDivider.clear();

  auto const GridLength = GridCfg.GridDimensions.x;
  auto const GridHeight = GridCfg.GridDimensions.y;
  auto const GridScreenXCentre = GridCfg.GridScreenCentre.x;
  auto const GridScreenYCentre = GridCfg.GridScreenCentre.y;
  auto const GridOrigoX = GridCfg.GridOrigo.x;
  auto const GridOrigoY = GridCfg.GridOrigo.y;
  auto TickDistance = GridCfg.TickDistance;

  auto const GridXLowerLeft = GridScreenXCentre - GridLength / 2.f;
  auto const GridYLowerLeft = GridScreenYCentre - GridHeight / 2.f;
  // ---
  // NOTE: Make and draw a grid.
  // ---
  struct grid_point {
    float fromX{};
    float fromY{};
    float toX{};
    float toY{};
    bool TagX{};
    bool TagY{};
  };

  const float NumTicksX = GridLength / TickDistance;
  const float NumTicksY = GridHeight / TickDistance;
  std::vector<grid_point> vGridPoint{};
  std::vector<grid_point> vGridSubDivider{};

  //!< Vertical left
  vGridPoint.push_back(grid_point{GridXLowerLeft, GridYLowerLeft,
                                  GridXLowerLeft, GridYLowerLeft + GridHeight});

  //!< Horizontal lower
  vGridPoint.push_back(grid_point{GridXLowerLeft, GridYLowerLeft,
                                  GridXLowerLeft + GridLength, GridYLowerLeft});

  //!< Vertical rigth
  vGridPoint.push_back(grid_point{GridXLowerLeft + GridLength, GridYLowerLeft,
                                  GridXLowerLeft + GridLength,
                                  GridYLowerLeft + GridHeight});

  //!< Horizontal upper
  vGridPoint.push_back(grid_point{GridXLowerLeft, GridYLowerLeft + GridHeight,
                                  GridXLowerLeft + GridLength,
                                  GridYLowerLeft + GridHeight});

  //!< Center Horizontal
  vGridPoint.push_back(grid_point{
      GridXLowerLeft, GridYLowerLeft + (GridHeight / 2.f),
      GridXLowerLeft + GridLength, GridYLowerLeft + (GridHeight / 2.f)});

  //!< Center Vertical
  vGridPoint.push_back(grid_point{
      GridXLowerLeft + GridLength / 2.f, GridYLowerLeft,
      GridXLowerLeft + GridLength / 2.f, GridYLowerLeft + GridHeight});

  // ---
  // NOTE: Create ticks along the horizontal axis.
  // ---
  for (size_t Idx = 0; Idx < int(NumTicksX); ++Idx) {
    auto const PosX0 = GridXLowerLeft + float(Idx) * TickDistance;
    auto const PosX1 = PosX0;
    auto const PosY0 = GridYLowerLeft + GridHeight / 2.f;
    auto const PosY1 = PosY0 + TickDistance / 2.f;

    // ---
    // NOTE: Tag at each major divider to ease text placement.
    // ---
    auto const TagX = (Idx && !(Idx % 5));
    auto const TagY{false};
    vGridPoint.push_back(grid_point{PosX0, PosY0, PosX1, PosY1, TagX, TagY});

    // ---
    // NOTE: Create the vGridSubDivider horizontally.
    // ---
    if (Idx && !(Idx % 5)) {
      auto const PosSudividerY0 = GridYLowerLeft;
      auto const PosSudividerY1 = GridYLowerLeft + GridHeight;
      vGridSubDivider.push_back(
          grid_point{PosX0, PosSudividerY0, PosX1, PosSudividerY1});
    }
  }

  // ---
  // NOTE: Create ticks along the vertical axis.
  // ---
  for (size_t Idx = 0; Idx < int(NumTicksY); ++Idx) {
    float const PosX0 = GridXLowerLeft + GridLength / 2.f;
    float const PosX1 = PosX0 + TickDistance / 2.f;
    float const PosY0 = GridYLowerLeft + float(Idx) * TickDistance;
    float const PosY1 = PosY0;
    vGridPoint.push_back(grid_point{PosX0, PosY0, PosX1, PosY1});

    // ---
    // NOTE: Tag at each major divider to ease text placement.
    // ---
    auto const TagX{false};
    auto const TagY = (Idx && !(Idx % 5));
    vGridPoint.push_back(grid_point{PosX0, PosY0, PosX1, PosY1, TagX, TagY});

    // ---
    // NOTE: Create the vGridSubDivider vertically.
    // ---
    if (Idx && !(Idx % 5)) {
      auto const PosSudividerX0 = GridXLowerLeft;
      auto const PosSudividerX1 = GridXLowerLeft + GridLength;
      vGridSubDivider.push_back(
          grid_point{PosSudividerX0, PosY0, PosSudividerX1, PosY1});
    }
  }

  Result.GridScreenCentre.x = GridXLowerLeft + GridLength / 2.f;
  Result.GridScreenCentre.y = GridYLowerLeft + GridHeight / 2.f;
  Result.GridDimensions.x = GridLength;
  Result.GridDimensions.y = GridHeight;

  auto const MhG2E =
      es::SetTranslation(es::Vector(GridOrigoX, GridOrigoY, 0.f));

  // ---
  // NOTE: Create major dividers.
  // ---
  for (auto const &Elem : vGridPoint) {
    auto const ToPixel = MhE2P * es::Point(Elem.toX, Elem.toY, 0.f);
    auto const FromPixel = MhE2P * es::Point(Elem.fromX, Elem.fromY, 0.f);

    // ---
    // NOTE: Convert the floating point indicators.
    // ---
    auto ldaFloat2Str = [](float In) -> std::string {
      std::string Result{};
      struct converted_text {
        char Conv[10]{};
      };

      converted_text C{};

      auto const Status =
          std::snprintf(C.Conv, sizeof(converted_text), "%.1f", In);

      if (Status) {
        Result = std::string(C.Conv);
      }
      return Result;
    };

    // ---
    // NOTE: Create the axis tag based on the setup from the grid.
    // ---
    auto const &TagX =
        Elem.TagX ? ldaFloat2Str((MhG2E * es::Point(Elem.fromX, 0.f, 0.f)).x)
                  : "";
    auto const &TagY =
        Elem.TagY ? ldaFloat2Str((MhG2E * es::Point(0.f, Elem.fromY, 0.f)).y)
                  : "";
    pixel_pos PixelPos = {int(ToPixel.x), int(ToPixel.y), DARKGRAY, TagX, TagY};

    Result.vGridLines.push_back(PixelPos);
    Result.vGridLines.push_back({int(FromPixel.x), int(FromPixel.y), DARKGRAY});
  }

  // ---
  // NOTE: Create the minor dividers.
  // ---
  for (auto const &Elem : vGridSubDivider) {
    auto const ToPixel = MhE2P * es::Point(Elem.toX, Elem.toY, 0.f);
    auto const FromPixel = MhE2P * es::Point(Elem.fromX, Elem.fromY, 0.f);
    Result.vGridLines.push_back({int(ToPixel.x), int(ToPixel.y)});
    Result.vGridLines.push_back({int(FromPixel.x), int(FromPixel.y)});
  }

  return Result;
};

/**
 * Initialize the matrix to convert from engineering basis to screen basis.
 * The screen center is used as the reference point.
 *
 * @OrigoScreen - the X, Y, Z values in engineering space at center of screen.
 * @vPixelsPerUnit - The number of pixels per unit, i.e 100 pixels equals 1m.
 * @ScreenCenterInPixels - The coordinates for the centre of the screen in
 * pixels.
 */
auto InitEng2PixelMatrix(Vector4 const &OrigoScreen,
                         Vector4 const &vPixelsPerUnit,
                         Vector4 const &ScreenPosInPixels) -> Matrix {

  // ---
  // Flip because pixel coord increases when moving down.
  // ---
  constexpr float Flip = -1.f;
  constexpr float NoFlip = 1.f;

  // ---
  // Create a Homogenous matrix that converts from engineering unit to screen.
  // ---
  auto Hes = es::I();
  Hes.m12 = ScreenPosInPixels.x + OrigoScreen.x * vPixelsPerUnit.x;
  Hes.m13 = ScreenPosInPixels.y + OrigoScreen.y * vPixelsPerUnit.y;
  Hes.m14 = ScreenPosInPixels.z + OrigoScreen.z * vPixelsPerUnit.z;

  // Flip and scale to pixel value.
  Hes.m0 = NoFlip * vPixelsPerUnit.x;
  Hes.m5 = Flip * vPixelsPerUnit.y;
  Hes.m10 = NoFlip * vPixelsPerUnit.z;

  return Hes;
}

// ---
// NOTE: Lamda to draw a box in engineering units.
// @MhE2P - Homogenous matrix to convert from engineering to pixelspace
// @Pos - Lower Left X, Lower Left Y
// @Dim - Length, Height
// ---
auto ldaDrawBox = [](Matrix const &MhE2P, Vector4 const &Pos,
                     Vector4 const &Dim, Color Col = BLUE,
                     float Alpha = 1.f) -> void {
  Color C = Col;
  C.a = 0xFF & int(float(int(Col.a) * Alpha * 255.f / 255.f));

  auto const PixPosStrt = MhE2P * Pos;
  auto const PixPosEnd = MhE2P * (Pos + Dim);
  DrawLine(PixPosStrt.x, PixPosStrt.y, 0, 0, VIOLET); //!< Debug help line.
  DrawLine(PixPosEnd.x, PixPosEnd.y, 0, 0, ORANGE);   //!< Debug help line.
  DrawLine(PixPosStrt.x, PixPosStrt.y, PixPosEnd.x, PixPosStrt.y, C);
  DrawLine(PixPosEnd.x, PixPosStrt.y, PixPosEnd.x, PixPosEnd.y, C);
  DrawLine(PixPosStrt.x, PixPosStrt.y, PixPosStrt.x, PixPosEnd.y, C);
  DrawLine(PixPosStrt.x, PixPosEnd.y, PixPosEnd.x, PixPosEnd.y, C);
};

// ---
// NOTE: Lamda to write/draw text placed in engineering units.
// ---
auto ldaDrawText = [](Matrix const &MhE2P, Vector4 const &Pos,
                      std::string const &Text, int FontSize = 20,
                      Color Col = BLUE, float AlphaText = 1.f,
                      float AlphaBox = 1.f) -> void {
  auto const PixelPos = MhE2P * Pos;

  // DrawLine(PixelPos.x, PixelPos.y, 0, 0, BLUE); //!< Debug help line.
  DrawText(Text.c_str(), PixelPos.x, PixelPos.y, FontSize, Col);
};

// ---
// NOTE: Lamda to draw a point. Actually it draws a small circle.
// ---
auto ldaDrawPoint = [](Matrix const &MhE2P, Vector4 const &Pos,
                       Vector4 const &m2Pixel, bool Print = false,
                       Color Col = BLUE, float Alpha = 1.f) -> void {
  auto PixelPos = MhE2P * Pos;
  DrawPixel(PixelPos.x, PixelPos.y, ColorAlpha(RED, Alpha));
  constexpr float Radius = 0.01f;
  DrawCircleLines(PixelPos.x, PixelPos.y, Radius * m2Pixel.x,
                  ColorAlpha(Col, Alpha));
  if (Print) {
    DrawLine(PixelPos.x, PixelPos.y, 0, 0, BLUE);
    DrawText(std::string("CurvePoint x/y: " + std::to_string(PixelPos.x) +
                         " / " + std::to_string(PixelPos.y))
                 .c_str(),
             140, 70, 20, BLUE);
  }
};

/**
 * Draw a circle with Radius - go figure.
 */
auto ldaDrawCircle = [](Matrix const &MhE2P, Vector4 const &Centre,
                        float Radius, Color Col = BLUE) -> void {
  auto CurvePoint = MhE2P * Centre;
  // Use MhE2P.m5 for scaling/zoom factor.
  DrawCircleLines(CurvePoint.x, CurvePoint.y, Radius * MhE2P.m5,
                  Fade(Col, 0.9f));
};

/**
 * Draw a circle with Radius - filled gradient version.
 */
auto ldaDrawCircleG = [](Matrix const &MhE2P, Vector4 const &Centre,
                         float Radius, Color Col = BLUE) -> void {
  auto CurvePoint = MhE2P * Centre;
  // Use MhE2P.m5 for scaling/zoom factor.
  DrawCircleGradient(CurvePoint.x, CurvePoint.y, Radius * MhE2P.m5,
                     Fade(Col, 0.3f), Col);
};

/**
 * Function to draw a line between two points.
 */
auto ldaDrawLine = [](Matrix const &MhE2P, Vector4 const &From,
                      Vector4 const &To, Color Col = BLUE) -> void {
  auto F = MhE2P * From;
  auto T = MhE2P * To;
  DrawLine(F.x, F.y, T.x, T.y, BLUE);
};

/**
 * Function to show the grid.
 */
auto ldaShowGrid = [](data *pData) -> void {
  for (size_t Idx = 0; Idx < pData->GridCfg.vGridLines.size(); Idx += 2) {
    auto const &Elem0 = pData->GridCfg.vGridLines[Idx];
    auto const &Elem1 = pData->GridCfg.vGridLines[Idx + 1];

    DrawLine(Elem0.X, Elem0.Y, Elem1.X, Elem1.Y, Fade(Elem0.color, 0.3f));

    if (!Elem0.TxtTagX.empty())
      DrawText(Elem0.TxtTagX.c_str(), Elem0.X - 1, Elem0.Y + 8, 10, DARKGRAY);

    if (!Elem0.TxtTagY.empty())
      DrawText(Elem0.TxtTagY.c_str(), Elem0.X - 20, Elem0.Y - 10, 10, DARKGRAY);
  }
};

auto UpdateDrawFrameFourier(data *pData) -> void;
auto UpdateDrawFrameFractal(data *pData) -> void;
auto UpdateDrawFrameAsteroid(data *pData) -> void;
auto UpdateDrawFrameHelp(data *pData) -> void;

/**
 * Keyboard input handling common to all the drawing routines.
 */
auto HandleInput(data *pData) -> bool {

  auto const MousePos = GetMousePosition();
  pData->MousePosEng = pData->MhE2PInv * es::Point(MousePos.x, MousePos.y, 0.f);
  pData->MousePosGrid = pData->MhG2E * pData->MousePosEng;
  ldaDrawText(
      pData->MhE2P, es::Point(0.f, -1.f, 0.f),
      std::string("MousePosGrid:" + std::to_string(pData->MousePosGrid.x) +
                  " " + std::to_string(pData->MousePosGrid.y))
          .c_str());

  pData->MouseInput.MouseButtonUp = IsMouseButtonUp(0);
  pData->MouseInput.MouseButtonDown = IsMouseButtonDown(0);
  pData->MouseInput.MouseButtonPressed = IsMouseButtonPressed(0);
  pData->MouseInput.MouseButtonReleased = IsMouseButtonReleased(0);

  DrawText(std::string("Use arrow keys. Zoom: " +
                       std::to_string(pData->vPixelsPerUnit.x) +
                       // ". CurrentTrendPoint :" +
                       // std::to_string(pData->CurrentTrendPoint) +
                       ". Mouse: " + std::to_string(MousePos.x) + " " +
                       std::to_string(MousePos.y) +
                       ". Mouse Eng: " + std::to_string(pData->MousePosEng.x) +
                       " " + std::to_string(pData->MousePosEng.y))
               .c_str(),
           140, 10, 20, BLUE);

  bool InputChanged{};

  constexpr float MinPixelPerUnit = 50.f;
  auto const PixelPerUnitPrv = pData->vPixelsPerUnit;

  if (pData->Key) {
    if (KEY_G == pData->Key) {
      pData->ShowGrid = !pData->ShowGrid;
      InputChanged = true;
    } else if (KEY_DOWN == pData->Key) {

      auto &vPPU = pData->vPixelsPerUnit;
      vPPU.x = std::max(vPPU.x - 10.f, MinPixelPerUnit);
      vPPU.y = std::max(vPPU.y - 10.f, MinPixelPerUnit);
      vPPU.z = std::max(vPPU.z - 10.f, MinPixelPerUnit);

      InputChanged = true;
    } else if (KEY_UP == pData->Key) {

      auto &vPPU = pData->vPixelsPerUnit;
      vPPU.x = std::max(vPPU.x + 10.f, MinPixelPerUnit);
      vPPU.y = std::max(vPPU.y + 10.f, MinPixelPerUnit);
      vPPU.z = std::max(vPPU.z + 10.f, MinPixelPerUnit);

      InputChanged = true;
    } else if (KEY_LEFT == pData->Key) {
      --pData->n;
      InputChanged = true;
    } else if (KEY_RIGHT == pData->Key) {
      ++pData->n;
      InputChanged = true;
    } else if (KEY_SPACE == pData->Key) {
      InputChanged = true;
      pData->StopUpdate = !pData->StopUpdate;
    } else if (KEY_A == pData->Key) {
      pData->UpdateDrawFramePointer = &UpdateDrawFrameAsteroid;
      InputChanged = true;
    } else if (KEY_F == pData->Key) {
      pData->UpdateDrawFramePointer = &UpdateDrawFrameFourier;
      InputChanged = true;
    } else if (KEY_R == pData->Key) {
      pData->UpdateDrawFramePointer = &UpdateDrawFrameFractal;
      InputChanged = true;
    } else if (KEY_L == pData->Key) {
      if (!pData->WikipediaLink.empty())
        OpenURL(pData->WikipediaLink.c_str());
    } else if (KEY_F1 == pData->Key) {
      pData->UpdateDrawFramePointer = &UpdateDrawFrameHelp;
      InputChanged = true;
    } else if (KEY_F2 == pData->Key) {
      pData->TakeScreenshot = true;
    }

    pData->KeyPrv = pData->Key;
  }

  if (InputChanged) {
    pData->Xcalc = 0.0;
    pData->CurrentTrendPoint = 0;

    pData->MhE2P = InitEng2PixelMatrix(
        pData->vEngOffset, pData->vPixelsPerUnit,
        {pData->screenWidth / 2.f, pData->screenHeight / 2.f, 0.f, 0.f});
    pData->MhE2PInv = MatrixInvert(pData->MhE2P);

    pData->GridCfg.GridDimensions.x = pData->GridCfg.GridDimensions.x *
                                      PixelPerUnitPrv.x /
                                      pData->vPixelsPerUnit.x;
    pData->GridCfg.GridDimensions.y = pData->GridCfg.GridDimensions.y *
                                      PixelPerUnitPrv.y /
                                      pData->vPixelsPerUnit.y;

    pData->GridCfg = GridCfgInPixels(pData->MhE2P, pData->GridCfg);
  }

  if (false && pData->MouseInput.MouseButtonReleased) {
    pData->GridCfg.GridOrigo.x = pData->MousePosEng.x;
    pData->GridCfg.GridOrigo.y = pData->MousePosEng.y;
    pData->GridCfg = GridCfgInPixels(pData->MhE2P, pData->GridCfg);
  }

  return InputChanged;
}

/**
 * Draw an animation of n - terms of a Fourier square wave.
 */
auto UpdateDrawFrameFourier(data *pData) -> void {

  if (data::pages::PageFourier != pData->PageNum) {
    pData->WikipediaLink = "https://en.wikipedia.org/wiki/Square_wave";
    pData->PageNum = data::pages::PageFourier;
  }

  BeginDrawing();
  ClearBackground(RAYWHITE);

  DrawText(std::string("Num terms: " + std::to_string(pData->n) +
                       ". Key:" + std::to_string(pData->KeyPrv) +
                       ". Time:" + std::to_string(pData->Xcalc))
               .c_str(),
           140, 40, 20, BLUE);

  {
    ldaDrawText(pData->MhE2P,
                es::Point(pData->GridCfg.GridScreenCentre.x -
                              pData->GridCfg.GridDimensions.x / 2.f,
                          -(pData->GridCfg.GridDimensions.y / 2.f * 1.05f),
                          0.f),
                pData->WikipediaLink);

    auto const BoxPosition =
        es::Point(pData->GridCfg.GridScreenCentre.x -
                      21.f * pData->GridCfg.GridDimensions.x / 40.f,
                  -(pData->GridCfg.GridDimensions.y / 2.f * 1.15f), 0.f);
    auto const BoxDimension =
        es::Vector(5.f / 8.f * pData->GridCfg.GridDimensions.x,
                   pData->GridCfg.GridDimensions.y / 15.f, 0.f);

    if (pData->MousePosEng.x > BoxPosition.x &&
        pData->MousePosEng.x < (BoxPosition.x + BoxDimension.x) &&
        pData->MousePosEng.y > BoxPosition.y &&
        pData->MousePosEng.y < (BoxPosition.y + BoxDimension.y)) {

      ldaDrawBox(pData->MhE2P, BoxPosition, BoxDimension);

      if (pData->MouseInput.MouseButtonReleased)
        if (!pData->WikipediaLink.empty())
          OpenURL(pData->WikipediaLink.c_str());
    }
  }

  HandleInput(pData);

  // ---
  // NOTE: Draw the grid.
  // ---
  if (pData->ShowGrid) {
    ldaShowGrid(pData);
  }

  auto const Frequency = 2.0;
  auto const Omegat = M_2_PI * Frequency * pData->t;
  auto const Radius = 4.f / M_PI;
  auto Centre = es::Point(pData->GridCfg.GridScreenCentre.x -
                              pData->GridCfg.GridDimensions.x / 2.f - Radius,
                          0.f, 0.f);

  auto Ft =
      Centre + es::Vector(Radius * cosf(Omegat), Radius * sinf(Omegat), 0.f);

  ldaDrawCircle(pData->MhE2P, Centre, Radius);

  // Draw the outer circle line
  ldaDrawLine(pData->MhE2P, Centre, Ft);

  // ---
  // Create the Fourier series.
  // ---
  auto Ftp = Ft;
  for (int Idx = 1; Idx < pData->n; ++Idx) {
    auto nthTerm = 1.f + Idx * 2.f;
    auto Ftn = Ftp + es::Vector(Radius / nthTerm * cosf(nthTerm * Omegat),
                                Radius / nthTerm * sinf(nthTerm * Omegat), 0.f);
    ldaDrawLine(pData->MhE2P, Ftp, Ftn);
    ldaDrawCircle(pData->MhE2P, Ftn, Radius / nthTerm);
    Ftp = Ftn;
  }

  auto GridStart = es::Point(0.f, 0.f, 0.f);
  pData->Xcalc += pData->dt;

  // ---
  // NOTE: Reset X value axis plots
  // ---
  auto const GridRight =
      pData->GridCfg.GridScreenCentre.x + pData->GridCfg.GridDimensions.x / 2.f;

  if (pData->Xcalc > GridRight) {
    auto const GridLeft = -GridRight;
    pData->Xcalc = GridLeft;
    pData->CurrentTrendPoint = 0;
  }

  auto AnimationPoint = GridStart + es::Vector(pData->Xcalc, Ftp.y, 0.f);

  // Draw the actual trend
  pData->vTrendPoints[pData->CurrentTrendPoint] = (AnimationPoint);

  for (size_t Idx = 0; Idx < pData->CurrentTrendPoint; ++Idx) {
    ldaDrawPoint(pData->MhE2P, pData->vTrendPoints[Idx],
                 {pData->MhE2P.m0, pData->MhE2P.m5, 0.f, 0.f});
  }

  // Draw the inner circle line
  ldaDrawLine(pData->MhE2P, Ft, Ftp);
  // Draw the connecting line
  ldaDrawLine(pData->MhE2P, Ftp, AnimationPoint);

  ++pData->CurrentTrendPoint;

  EndDrawing();

  if (pData->TakeScreenshot) {
    pData->TakeScreenshot = false;
    auto const FileName = std::string(__FUNCTION__) + ".png";
    TakeScreenshot(FileName.c_str());
  }
}

/**
 * Draw a Fractal.
 */
auto UpdateDrawFrameFractal(data *pData) -> void {

  if (data::pages::PageFractal != pData->PageNum) {
    pData->WikipediaLink = "https://en.wikipedia.org/wiki/Fractal";
    pData->PageNum = data::pages::PageFractal;
  }

  BeginDrawing();
  ClearBackground(RAYWHITE);

  DrawText(std::string("Num terms: " + std::to_string(pData->n) +
                       ". Key:" + std::to_string(pData->KeyPrv) +
                       ". Time:" + std::to_string(pData->Xcalc))
               .c_str(),
           140, 40, 20, BLUE);

  {
    ldaDrawText(pData->MhE2P,
                es::Point(pData->GridCfg.GridScreenCentre.x -
                              pData->GridCfg.GridDimensions.x / 2.f,
                          -(pData->GridCfg.GridDimensions.y / 2.f * 1.05f),
                          0.f),
                pData->WikipediaLink);

    auto const BoxPosition =
        es::Point(pData->GridCfg.GridScreenCentre.x -
                      21.f * pData->GridCfg.GridDimensions.x / 40.f,
                  -(pData->GridCfg.GridDimensions.y / 2.f * 1.15f), 0.f);
    auto const BoxDimension =
        es::Vector(5.f / 8.f * pData->GridCfg.GridDimensions.x,
                   pData->GridCfg.GridDimensions.y / 15.f, 0.f);

    if (pData->MousePosEng.x > BoxPosition.x &&
        pData->MousePosEng.x < (BoxPosition.x + BoxDimension.x) &&
        pData->MousePosEng.y > BoxPosition.y &&
        pData->MousePosEng.y < (BoxPosition.y + BoxDimension.y)) {

      ldaDrawBox(pData->MhE2P, BoxPosition, BoxDimension);

      if (pData->MouseInput.MouseButtonReleased)
        if (!pData->WikipediaLink.empty())
          OpenURL(pData->WikipediaLink.c_str());
    }
  }

  // ---
  // NOTE: Get mouse click position inside the grid to move center to
  //       the point that was clicked.
  // ---
  {
    auto const &GridC = pData->GridCfg.GridScreenCentre;
    auto const &GridD = pData->GridCfg.GridDimensions;
    auto const GridP = GridC - GridD * (1.f / 2.f);

    ldaDrawBox(pData->MhE2P,
               es::Point(pData->MousePosEng.x, pData->MousePosEng.y, 0.f),
               GridD, RED);

    if (pData->MousePosEng.x > (GridP.x) &&
        pData->MousePosEng.x < (GridP.x + GridD.x) &&
        pData->MousePosEng.y > (GridP.y) &&
        pData->MousePosEng.y < (GridP.y + GridD.y)) {

      ldaDrawBox(pData->MhE2P, GridP, GridD, ORANGE);

      if (pData->MouseInput.MouseButtonReleased) {
        pData->GridCfg.GridOrigo =
            pData->GridCfg.GridOrigo + pData->MousePosEng;
        pData->GridCfg = GridCfgInPixels(pData->MhE2P, pData->GridCfg);
        pData->MhG2E = es::SetTranslation(pData->MousePosGrid);
        pData->MhG2EInv = MatrixInvert(pData->MhG2E);
      }
    }
  }

  HandleInput(pData);

  // ---
  // NOTE: Draw the grid.
  // ---
  if (pData->ShowGrid) {
    ldaShowGrid(pData);
  }

  EndDrawing();

  if (pData->TakeScreenshot) {
    pData->TakeScreenshot = false;
    auto const FileName = std::string(__FUNCTION__) + ".png";
    TakeScreenshot(FileName.c_str());
  }
}

/**
 * Draw an animation of the Asteroid parametric equation.
 * link: https://en.wikipedia.org/wiki/Astroid
 */
auto UpdateDrawFrameAsteroid(data *pData) -> void {

  if (data::pages::PageAsteroid != pData->PageNum) {
    pData->WikipediaLink = "https://en.wikipedia.org/wiki/Astroid";
    pData->PageNum = data::pages::PageAsteroid;
  }

  BeginDrawing();
  ClearBackground(WHITE);

  DrawText(std::string("Asteriode. Key:" + std::to_string(pData->KeyPrv) +
                       ". Time:" + std::to_string(pData->Xcalc))
               .c_str(),
           140, 40, 20, BLUE);

  {
    auto const PosTxt =
        es::Point(pData->GridCfg.GridScreenCentre.x -
                      pData->GridCfg.GridDimensions.x / 2.f,
                  -(pData->GridCfg.GridDimensions.y / 2.f * 1.05f), 0.f);

    ldaDrawText(pData->MhE2P, PosTxt, pData->WikipediaLink, 20, GREEN, 0.7f,
                0.05f);

    {
      auto const BoxPosition =
          es::Point(pData->GridCfg.GridScreenCentre.x -
                        21.f * pData->GridCfg.GridDimensions.x / 40.f,
                    -(pData->GridCfg.GridDimensions.y / 2.f * 1.15f), 0.f);
      auto const BoxDimension =
          es::Vector(5.f / 8.f * pData->GridCfg.GridDimensions.x,
                     pData->GridCfg.GridDimensions.y / 15.f, 0.f);

      if (pData->MousePosEng.x > BoxPosition.x &&
          pData->MousePosEng.x < (BoxPosition.x + BoxDimension.x) &&
          pData->MousePosEng.y > BoxPosition.y &&
          pData->MousePosEng.y < (BoxPosition.y + BoxDimension.y)) {

        ldaDrawBox(pData->MhE2P, BoxPosition, BoxDimension);

        if (pData->MouseInput.MouseButtonReleased)
          if (!pData->WikipediaLink.empty())
            OpenURL(pData->WikipediaLink.c_str());
      }
    }
  }

  HandleInput(pData);

  // ---
  // NOTE: Draw the grid.
  // ---
  if (pData->ShowGrid) {
    ldaShowGrid(pData);
  }

  auto constexpr Radius = 1.f;
  auto const t = pData->t;

  // ---
  // NOTE: The formula to compute the Asteroide.
  // ---
  auto const x = Radius / 4.f * (3.f * cos(t) + cos(3.f * t));
  auto const y = Radius / 4.f * (3.f * sin(t) - sin(3.f * t));

  // ---
  // NOTE: Follow along the fixed circle.
  // ---
  auto const FixedCx = Radius * cos(t);
  auto const FixedCy = Radius * sin(t);

  auto GridStart = es::Point(0.f, 0.f, 0.f);

  auto AnimationSmallCircle =
      GridStart + es::Vector(3.f / 4.f * FixedCx, 3.f / 4.f * FixedCy, 0.f);

  auto constexpr DotSize = 0.025f;

  // Draw the small circle.
  ldaDrawCircle(pData->MhE2P, AnimationSmallCircle, Radius / 4.f);
  ldaDrawCircleG(pData->MhE2P, AnimationSmallCircle, DotSize);

  // Draw the fixed circle.
  ldaDrawCircle(pData->MhE2P, GridStart, Radius);

  pData->Xcalc += pData->dt;

  // ---
  // NOTE: Reset X value axis plots
  // ---
  if (pData->Xcalc > 2.f * M_PI) {
    pData->Xcalc = 0.f;
    pData->CurrentTrendPoint = 0;
  }

  auto AnimationPoint = GridStart + es::Vector(x, y, 0.f);

  // Draw the actual trend
  pData->vTrendPoints[pData->CurrentTrendPoint] = AnimationPoint;

  for (size_t Idx = 0;
       Idx < std::min(pData->vTrendPoints.size(), pData->NumTrendPoints);
       ++Idx) {

    // ---
    // NOTE: Compute the Alpha channel.
    // Split into two sections.
    // a: From CurrentTrendPoint+1 to NumTrendPoints.
    // b: From 0 to CurrentTrendPoint.
    //    90    180  270  360
    // ----|----|----|----|----|
    //       ^
    //       CurrentTrendPoint
    //       t0
    auto Alpha = 0.f;
    auto t0 = 0.f;
    if (Idx < pData->CurrentTrendPoint) { //!< segment a
      t0 = float(pData->NumTrendPoints - pData->CurrentTrendPoint) /
           float(pData->NumTrendPoints);
    } else { //!< segment b -> do nothing
    }
    auto const t = float(Idx) / float(pData->NumTrendPoints) + t0;
    Alpha = es::Lerp(es::Vector(0.f, 0.f, 0.f), es::Vector(1.f, 0.f, 0.f), t).x;

    ldaDrawPoint(pData->MhE2P, pData->vTrendPoints[Idx],
                 {pData->MhE2P.m0, pData->MhE2P.m5, 0.f, 0.f}, false,
                 Idx < pData->CurrentTrendPoint ? BLUE : RED, Alpha);
  }

  ldaDrawLine(pData->MhE2P, AnimationPoint, AnimationSmallCircle);
  ldaDrawCircleG(pData->MhE2P, AnimationPoint, DotSize, ORANGE);

  ++pData->CurrentTrendPoint;

  pData->NumTrendPoints =
      std::max(pData->CurrentTrendPoint, pData->NumTrendPoints);

  EndDrawing();

  if (pData->TakeScreenshot) {
    pData->TakeScreenshot = false;
    auto const FileName = std::string(__FUNCTION__) + ".png";
    TakeScreenshot(FileName.c_str());
  }
}

/**
 * Display a page with some help text.
 */
auto UpdateDrawFrameHelp(data *pData) -> void {

  if (data::pages::PageHelp != pData->PageNum) {
    pData->WikipediaLink = "";
    pData->PageNum = data::pages::PageHelp;
  }

  BeginDrawing();
  ClearBackground(LIGHTGRAY);

  constexpr auto TextOffsetY = 25u;
  constexpr auto TextPosY = 40u;
  auto TextIdx = 0u;

  DrawText("Available pages", 40, TextPosY + (TextIdx * TextOffsetY), 20, BLUE);

  auto ldaDisplayHelpText = [&](std::string HelpText) -> unsigned int {
    ++TextIdx;

    DrawText(HelpText.c_str(), 40, TextPosY + (TextIdx * TextOffsetY), 20,
             BLUE);
    return TextIdx;
  };

  for (auto E : pData->vHelpTextPage) {
    ldaDisplayHelpText(E);
  }

  HandleInput(pData);

  EndDrawing();
}
}; // namespace

/**
 *
 */
auto main(int argc, char const *argv[]) -> int {

  // ---
  // NOTE: Poor mans testing framework.
  // ---
  if (argc > 1) {
    es::TestHomogenousMatrix();
    es::Test3dCalucations();
    es::Test3dScreenCalculations();
    es::TestLerp();
    es::TestInvert();
    return 0;
  }

  data Data{};
  Data.vTrendPoints.resize(size_t(Data.screenWidth * Data.screenHeight));
  auto pData = &Data;

  // Initialization
  // ---
  InitWindow(Data.screenWidth, Data.screenHeight,
             "Fluffy's adventures with Raylib");

  Data.vHelpTextPage.push_back("F1 - This help page");
  Data.vHelpTextPage.push_back("F2 - ScreenShot");
  Data.vHelpTextPage.push_back("a -  Asteriode");
  Data.vHelpTextPage.push_back("f -  Fourier square wave");
  Data.vHelpTextPage.push_back("g -  toggle Grid");
  Data.vHelpTextPage.push_back("l -  open current page's web Link");
  Data.vHelpTextPage.push_back("r -  fRactal");

  // ---
  SetTargetFPS(60); // Set our game to run at X frames-per-second

  // ---
  // NOTE: Move all points around by setting the engineering offset which will
  // be added to the offset of the screen position in pixels.
  // ---
  Data.vEngOffset = es::Point(0.f, 0.f, 0.f);

  // ---
  // NOTE: All three xyz values are needed in vPixelsPerUnit to ensure
  //       that the conversion matrix is invertible.
  // ---
  Data.vPixelsPerUnit = es::Point(100.f, 100.f, 100.f);

  // ---
  // NOTE: Set up Homogenous matrix for conversion to pixel space.
  // ---
  Data.MhE2P = InitEng2PixelMatrix(
      Data.vEngOffset, Data.vPixelsPerUnit,
      {Data.screenWidth / 2.f, Data.screenHeight / 2.f, 0.f, 0.f});

  Data.MhG2E = es::SetTranslation(es::Vector(0.f, 0.f, 0.f));
  Data.MhG2EInv = MatrixInvert(Data.MhG2E);
  std::cout << Data.MhG2E;
  std::cout << Data.MhG2EInv << std::endl;
  std::cout << Data.MhG2E * Data.MhG2EInv << std::endl;

  if (es::IsMatrixInvertible(Data.MhE2P)) {
    Data.MhE2PInv = MatrixInvert(Data.MhE2P);
    std::cout << "MatrixInvert: " << Data.MhE2PInv << std::endl;
    std::cout << "Matrix      : " << Data.MhE2P << std::endl;

    auto const OrigoScreenInPixels =
        es::Point(Data.screenWidth / 2.f, Data.screenHeight / 2.f, 0.f);

    std::cout << "Pixel Pos: " << OrigoScreenInPixels << std::endl;
    std::cout << "Engin Pos: " << Data.MhE2PInv * OrigoScreenInPixels
              << std::endl;

  } else {
    std::cerr << "The Homogenous matrix MhE2P is not invertible." << std::endl;
    std::cout << Data.MhE2P << std::endl;
    std::cerr << "Will not be able to convert to engineering pos from PixelPos."
              << std::endl;
    return 1;
  }

  // ---
  // NOTE: Construct the grid pattern.
  // ---
  Data.GridCfg = GridCfgInPixels(Data.MhE2P, Data.GridCfg);

  Data.UpdateDrawFramePointer = UpdateDrawFrameHelp;

  // ---
  // Main game loop
  // ---
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    DrawFPS(10, 10);
    Data.dt = 1. / 60.f; // / GetFPS();
    if (!Data.StopUpdate)
      Data.t = GetTime();
    Data.Key = GetKeyPressed();

    (*Data.UpdateDrawFramePointer)(pData);
  }

  // ---
  // De-Initialization
  // ---
  CloseWindow(); // Close window and OpenGL context

  return 0;
}
