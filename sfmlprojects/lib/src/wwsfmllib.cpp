/******************************************************************************
 * * Filename : wwsfmlib.cpp
 * * Date     : 2024 Jul 15
 * * Author   : Willy Clarke (willy@clarke.no)
 * * Version  : Use git you GIT
 * * Copyright: W. Clarke
 * * License  : ZLIB/libPNG -> https://opensource.org/license/Zlib
 * * Descripti: Interface / Utilities for the various sfml widgets.
 * ******************************************************************************/

#include "../include/framebuffer.hpp"
#include "../include/framerate.hpp"
#include "../include/randomwalker.hpp"
#include "../include/wwsfmlbase.hpp"

namespace wwsfmlbase {
/**
 */
std::shared_ptr<ww_sfml_base> CreateFrameBuffer(float SizeH, float SizeV) {
  std::shared_ptr<wwsfmlbase::ww_sfml_base> pFrameBuffer = std::make_shared<wwsfmlbase::frame_buffer>(SizeH, SizeV);
  return pFrameBuffer;
}

/**
 */
std::shared_ptr<ww_sfml_base> CreateFrameRate(float PosX, float PosY, std::filesystem::path const& PathToFont) {
  std::shared_ptr<wwsfmlbase::frame_rate> pFrameRate = std::make_shared<wwsfmlbase::frame_rate>();
  if (pFrameRate) {
    pFrameRate->PathToFont = PathToFont;
    pFrameRate->Init(PosX, PosY);
    std::shared_ptr<wwsfmlbase::ww_sfml_base> pFR = std::dynamic_pointer_cast<wwsfmlbase::ww_sfml_base>(pFrameRate);
    return pFR;
  }
  return {};
}

/**
 */
std::shared_ptr<ww_sfml_base> CreateRandomWalker() {
  std::shared_ptr<wwsfmlbase::ww_sfml_base> pRandomWalker = std::make_shared<wwsfmlbase::random_walker>();
  return pRandomWalker;
}
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
