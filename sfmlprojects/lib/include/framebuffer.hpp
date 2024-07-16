/******************************************************************************
 * * Filename : framebuffer.hpp
 * * Date     : 2024 Jul 14
 * * Author   : Willy Clarke (willy@clarke.no)
 * * Version  : Use git you GIT
 * * Copyright: W. Clarke
 * * License  : ZLIB/libPNG -> https://opensource.org/license/Zlib
 * * Descripti: Implements a SFML frame buffer and fills it with some colors.
 * ******************************************************************************/
#ifndef SFMLPROJECTS_INCLUDE_FRAMEBUFFER_8919A6D9_B7DC_49CA_83BE_AC3BC1BF4A6F
#define SFMLPROJECTS_INCLUDE_FRAMEBUFFER_8919A6D9_B7DC_49CA_83BE_AC3BC1BF4A6F

#include <iomanip>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "wwsfmlbase.hpp"

namespace wwsfmlbase {
class frame_buffer : public sf::Transformable, public ww_sfml_base {
public:
  // add functions to play with the entity's geometry / colors / texturing...

  void Init(float Win, float Hin) {

    size_t const NumElems = size_t(Win) * size_t(Hin);
    m_Vertices.resize(NumElems);
    W = Win;
    H = Hin;

    for (size_t Y = 0;  //!<
         Y < size_t(H); //!<
         ++Y)           //!<
    {
      for (size_t X = 0;  //!<
           X < size_t(W); //!<
           ++X)           //!<
      {
        if ((X * Y) < m_Vertices.getVertexCount())
          m_Vertices[X + Y * size_t(W)] =
              sf::Vertex(sf::Vector2f(float(X), float(Y)),
                         sf::Color(static_cast<char>(Y % 256), static_cast<char>(X % 256), 128),
                         sf::Vector2f(0.f, 0.f));
      }
    }
  }

  frame_buffer(float Win, float Hin) : W(Win), H(Hin) { Init(W, H); }
  frame_buffer() { Init(W, H); }
  virtual ~frame_buffer() { std::cerr << __PRETTY_FUNCTION__ << std::endl; }

  virtual void Print() {
    auto const pMatrix = getTransform().getMatrix();
    std::cout << "FrameBuffer.getTransform().getMatrix() gives\n" << std::endl;
    std::cout << " [ 0]:" << std::fixed << std::setprecision(3)                //!<
              << std::setfill(' ') << std::left << std::setw(10) << pMatrix[0] //!<
              << " [ 1]:" << std::setw(10) << pMatrix[1]                       //!<
              << " [ 2]:" << std::setw(10) << pMatrix[2]                       //!<
              << " [ 3]:" << std::setw(10) << pMatrix[3] << "\n"               //!<
              << " [ 4]:" << std::setw(10) << pMatrix[4]                       //!<
              << " [ 5]:" << std::setw(10) << pMatrix[5]                       //!<
              << " [ 6]:" << std::setw(10) << pMatrix[6]                       //!<
              << " [ 7]:" << std::setw(10) << pMatrix[7] << "\n"               //!<
              << " [ 8]:" << std::setw(10) << pMatrix[8]                       //!<
              << " [ 9]:" << std::setw(10) << pMatrix[9]                       //!<
              << " [10]:" << std::setw(10) << pMatrix[10]                      //!<
              << " [11]:" << std::setw(10) << pMatrix[11] << "\n"              //!<
              << " [12]:" << std::setw(10) << pMatrix[12]                      //!<
              << " [13]:" << std::setw(10) << pMatrix[13]                      //!<
              << " [14]:" << std::setw(10) << pMatrix[14]                      //!<
              << " [15]:" << std::setw(10) << pMatrix[15]                      //!<
              << "\n"
              << std::endl;
    ;
  }

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
  }

private:
  virtual void draw(sf::RenderTarget& target, sf::RenderStates RenderStates) const {
    // apply the entity's transform -- combine it with the one that was passed by the caller
    RenderStates.transform *= getTransform(); // getTransform() is defined by sf::Transformable

    // apply the texture
    RenderStates.texture = &m_Texture;

    // you may also override states.shader or states.blendMode if you want

    // draw the vertex array
    target.draw(m_Vertices, RenderStates);
  }

  float           W{1080.f};
  float           H{768.f};
  sf::VertexArray m_Vertices{sf::Points, size_t(W* H)};
  sf::Texture     m_Texture;
};
}; // namespace wwsfmlbase

#endif
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
