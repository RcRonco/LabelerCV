//
// Created by Ron Cohen on 07/08/2016.
//

#ifndef _RC_IMG_REFACTOR_H
#define _RC_IMG_REFACTOR_H

#include <boost/filesystem.hpp>
#include <stdint.h>


namespace fs = boost::filesystem;

namespace Labeler
{
    namespace ImageRefactor
    {
        void ResizeImages(fs::path path, uint32_t width, uint32_t height);
        void ResizeImages(fs::path inpath, fs::path outpath, uint32_t width, uint32_t height);
    };
};

#endif // !_RC_VIDEO_PLAYER_H
