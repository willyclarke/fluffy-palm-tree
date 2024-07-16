/******************************************************************************
 * * Filename : framerate.hpp
 * * Date     : 2024 Jul 15
 * * Author   : Willy Clarke (willy@clarke.no)
 * * Version  : Use git you GIT
 * * Copyright: W. Clarke
 * * License  : ZLIB/libPNG -> https://opensource.org/license/Zlib
 * * Descripti: Frame rate widget
 * ******************************************************************************/
#ifndef SFMLPROJECTS_INCLUDE_FRAMERATE_HPP_282D67CD_A2C2_4522_B38F_C80D51E29254
#define SFMLPROJECTS_INCLUDE_FRAMERATE_HPP_282D67CD_A2C2_4522_B38F_C80D51E29254

#include <filesystem>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

#include "wwsfmlbase.hpp"

namespace wwsfmlbase {

class frame_rate : public sf::Transformable, public ww_sfml_base {
public:
  void Init(float PosX, float PosY) {
    constexpr int NumPix{24};

    if (!Font.loadFromFile(PathToFont.string())) {
      std::cerr << __PRETTY_FUNCTION__ << " -> Could not load font from file " << PathToFont.string() << std::endl;
      std::cerr << __PRETTY_FUNCTION__
                << " -> Fonts are expected to be found in ../fonts as seen from the apps location." << std::endl;
      return;
    }

    Text.setFont(Font);
    Text.setString("Hello world");
    Text.setCharacterSize(NumPix); // in pixels, not points!
    Text.setFillColor(sf::Color::Green);
    Text.setStyle(sf::Text::Bold | sf::Text::Underlined);

    {
      sf::Vector2 Position{PosX, PosY};
      Text.setPosition(Position);
    }

    std::cerr << __PRETTY_FUNCTION__ << " -> font loaded from " << PathToFont << ". Pos: " << Text.getPosition().x
              << "," << Text.getPosition().y << std::endl;
    int Idx{};
    for (auto& T : vText) {
      T.setFont(Font);
      T.setString("Hello world" + std::to_string(Idx));
      T.setCharacterSize(NumPix); // in pixels, not points!
      T.setFillColor(sf::Color::Blue);
      T.setStyle(sf::Text::Bold | sf::Text::Underlined);
      sf::Vector2 Position{PosX, PosY + static_cast<float>((1 + Idx) * (NumPix + 2))};
      T.setPosition(Position);
      ++Idx;
    }

    TimePrv = Clock.getElapsedTime();
  }

  frame_rate() { std::cerr << __PRETTY_FUNCTION__ << std::endl; }
  virtual ~frame_rate() { std::cerr << __PRETTY_FUNCTION__ << std::endl; }

  virtual void Print() {}

  virtual void Update(sf::Event event) {
    if (event.type == sf::Event::KeyPressed) {
      auto Position            = getPosition();
      auto Rotation            = getRotation();
      auto constexpr Increment = 10.f;

      if (event.key.scancode == sf::Keyboard::Scan::A) {
        Rotation -= 1.f;
      } else if (event.key.scancode == sf::Keyboard::Scan::B) {
        Rotation += 1.f;
      } else if (event.key.scancode == sf::Keyboard::Scan::Right) {
        Position.x += Increment;
      } else if (event.key.scancode == sf::Keyboard::Scan::Left) {
        Position.x -= Increment;
      } else if (event.key.scancode == sf::Keyboard::Scan::Up) {
        Position.y -= Increment;
      } else if (event.key.scancode == sf::Keyboard::Scan::Down) {
        Position.y += Increment;
      }
      setPosition(Position);
      setRotation(Rotation);
    }

    /**
     * Calculate frame rate.
     */
    auto const TimeN1    = Clock.getElapsedTime();
    auto const DeltaTime = TimeN1 - TimePrv;
    TimePrv              = TimeN1;

    if (DeltaTime.asSeconds() > 0.f) {
      auto const FrameRate = 1.f / (DeltaTime.asSeconds());
      Text.setString(std::to_string(FrameRate));

      /* Compute running average */
      ++NumIter;
      auto const Avg = FrameRateAvg / NumIter;
      FrameRateAvg   = FrameRate + Avg;
      vText.at(0).setString(std::to_string(FrameRateAvg));

      /**
       * Update framerate displays.
       */
      for (size_t Idx = vText.size() - 1; Idx > 0; --Idx) {
        auto& T = vText.at(Idx);
        if (!(static_cast<long>(NumIter) % 60)) {
          auto const& Tp = vText.at(Idx - 1);
          T.setString(Tp.getString());
        }
      }
    }
  }

  std::filesystem::path PathToFont{};

private:
  void draw(sf::RenderTarget& Target, sf::RenderStates RenderStates) const {
    RenderStates.transform *= getTransform();

    RenderStates.texture = &Texture;

    Target.draw(Vertices, RenderStates);

    Target.draw(Text, RenderStates);
    for (auto const& T : vText) {
      Target.draw(T, RenderStates);
    }
  }

  sf::VertexArray Vertices{};
  sf::Texture     Texture{};
  sf::Font        Font{};

  sf::Text              Text{};
  std::vector<sf::Text> vText{10};
  sf::Clock             Clock{};
  sf::Time              TimePrv{};

  float NumIter{};
  float FrameRateAvg{};
};
}; // end of namespace wwsfmlbase

/**
Copyright (c) <2024> <Willy Clarke>

This software is provided ‘as-is’, without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/
#endif
