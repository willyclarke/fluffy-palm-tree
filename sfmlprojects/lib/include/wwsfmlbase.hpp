/******************************************************************************
 * * Filename : wwsfmlbase.hpp
 * * Date     : 2024 Jul 14
 * * Author   : Willy Clarke (willy@clarke.no)
 * * Version  : Use git you GIT
 * * Copyright: W. Clarke
 * * License  : ZLIB/libPNG -> https://opensource.org/license/Zlib
 * * Descripti: Base classes for sfml programming. See below for full license text.
 * ******************************************************************************/
#ifndef SFMLPROJECTS_INCLUDE_WWSFMLBASE_HPP_94E3CB58_C00F_48AE_93FE_8E71D1DAB680
#define SFMLPROJECTS_INCLUDE_WWSFMLBASE_HPP_94E3CB58_C00F_48AE_93FE_8E71D1DAB680

#include <iostream>

#include <SFML/Graphics.hpp>

/**
 * The intent for the base class is to be able to create a vector of shared_ptr
 * so that the update loop easily can do an Update and a draw.
 *
 * Example:
 * Create a vector like so:
   std::vector<std::shared_ptr<wwsfmlbase::ww_sfml_base>> vSfmlBaseObjects{};
 * and a shared_ptr with a proper object like so:
 *
  std::shared_ptr<wwsfmlbase::ww_sfml_base> pFrameBuffer =
      std::make_shared<wwsfmlbase::frame_buffer>(WindowSize.x, WindowSize.y);
 *
*/
namespace wwsfmlbase {
class ww_sfml_base : public sf::Drawable {
public:
  ww_sfml_base() { std::cerr << __PRETTY_FUNCTION__ << std::endl; }
  virtual ~ww_sfml_base() { std::cerr << __PRETTY_FUNCTION__ << std::endl; }
  virtual void Init(float _W, float _H)  = 0;
  virtual void Update(sf::Event Ev = {}) = 0;
  virtual void Print()                   = 0;

private:
  virtual void draw(sf::RenderTarget& target, sf::RenderStates RenderStates) const = 0;
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
