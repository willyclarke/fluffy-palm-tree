/******************************************************************************
 * * Filename : wwsfmllib.hpp
 * * Date     : 2024 Jul 15
 * * Author   : Willy Clarke (willy@clarke.no)
 * * Version  : Use git you GIT
 * * Copyright: W. Clarke
 * * License  : ZLIB/libPNG -> https://opensource.org/license/Zlib
 * * Descripti: Interface to Library functions.
 * ******************************************************************************/

#include "wwsfmlbase.hpp"

#include <filesystem>
#include <memory>

namespace wwsfmlbase {

/**
 * Forward declarations.
 */
class frame_buffer;
class frame_rate;
class random_walker;
class ww_sfml_base;

/**
 * Create a frame buffer to show individual pixels.
 * @param: SizeH - Horisontal size.
 * @param: SizeV - Vertical size.
 * @return: shared_ptr to base object.
 */
std::shared_ptr<ww_sfml_base> CreateFrameBuffer(float SizeH, float SizeV);

/**
 * Create a column of framerate texts. Each row shows the average over X frames.
 * @param: PosX - Position along the X-axis. i.e. horisontal positon.
 * @param: PosY - Position along the Y-axis. i.e. vertical position.
 * @return: shared_ptr to base object.
 */
std::shared_ptr<ww_sfml_base> CreateFrameRate(float PosX, float PosY, std::filesystem::path const& PathToFont);

/**
 * Create a random walker as described in the first chapter of 'Nature of code'.
 * @return: shared_ptr to base object.
 */
std::shared_ptr<ww_sfml_base> CreateRandomWalker();

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
