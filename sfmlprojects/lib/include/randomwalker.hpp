/******************************************************************************
 * * Filename : randomwalker.hpp
 * * Date     : 2024 Jul 14
 * * Author   : Willy Clarke (willy@clarke.no)
 * * Version  : Use git you GIT
 * * Copyright: W. Clarke
 * * License  : ZLIB/libPNG -> https://opensource.org/license/Zlib
 * * Descripti: Nature of code randomwalker.
 * ******************************************************************************/
#ifndef SFMLPROJECTS_INCLUDE_RANDOMWALKER_HPP_B2FDB5EC_6B98_4510_B288_5A711BABDB47
#define SFMLPROJECTS_INCLUDE_RANDOMWALKER_HPP_B2FDB5EC_6B98_4510_B288_5A711BABDB47

#include <array>
#include <iostream>
#include <random>

#include <SFML/Graphics.hpp>

#include "wwsfmlbase.hpp"

namespace wwsfmlbase {

class random_walker : public sf::Transformable, public ww_sfml_base {
public:
  void Init(float PosX, float PosY) {

    for (auto& Cirle : WalkedPath) {
      Cirle = sf::CircleShape{};
    }

    m_Circle.setRadius(5.f);
    m_Circle.setPosition(PosX, PosY);
    m_Circle.setFillColor(sf::Color(150, 50, 250));
  }

  void Step() {
    auto                          Pos = m_Circle.getPosition();
    std::uniform_int_distribution Distribution{-1, 1};
    auto                          RandomX = Distribution(Mt);
    auto                          RandomY = Distribution(Mt);
    Pos.x += float(RandomX);
    Pos.y += float(RandomY);

    /**
     * Limit the path ...
     */
    if (Pos.x < 0.f)
      Pos.x = 0.f;
    if (Pos.x > 800.f)
      Pos.x = 800.f;
    if (Pos.y < 0.f)
      Pos.y = 0.f;
    if (Pos.y > 800.f)
      Pos.y = 800.f;

    WalkedPath.at(CurrCircle) = m_Circle;
    CurrCircle                = (CurrCircle + 1) % WalkedPath.size();

    m_Circle.setPosition(Pos);
  }

  random_walker() {
    std::cerr << __PRETTY_FUNCTION__ << std::endl;
    Init(300.f, 400.f);
  }

  virtual ~random_walker() { std::cerr << __PRETTY_FUNCTION__ << std::endl; }

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

    Step();
  }

private:
  void draw(sf::RenderTarget& Target, sf::RenderStates RenderStates) const {
    // apply the entity's transform -- combine it with the one that was passed by the caller
    RenderStates.transform *= getTransform();

    // apply the texture
    RenderStates.texture = &m_Texture;

    // you may also override states.shader or states.blendMode if you want

    // draw the vertex array
    Target.draw(m_Vertices, RenderStates);
    Target.draw(m_Circle, RenderStates);
    for (auto& Cirle : WalkedPath) {
      Target.draw(Cirle);
    }
  }

  sf::VertexArray                   m_Vertices;
  sf::Texture                       m_Texture;
  sf::CircleShape                   m_Circle{};
  std::array<sf::CircleShape, 1000> WalkedPath;
  size_t                            CurrCircle{};
  std::mt19937_64                   Mt{};
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
