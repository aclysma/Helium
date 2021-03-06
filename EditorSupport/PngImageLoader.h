//----------------------------------------------------------------------------------------------------------------------
// PngImageLoader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_EDITOR_SUPPORT_PNG_IMAGE_LOADER_H
#define LUNAR_EDITOR_SUPPORT_PNG_IMAGE_LOADER_H

#include "EditorSupport/EditorSupport.h"

#if L_EDITOR

namespace Helium
{
    class Stream;
}

namespace Lunar
{
    class Image;

    /// PNG image loading support.
    class LUNAR_EDITOR_SUPPORT_API PngImageLoader
    {
    public:
        /// @name Loading
        //@{
        static bool Load( Image& rImage, Stream* pSourceStream );
        //@}
    };
}

#endif  // L_EDITOR

#endif  // LUNAR_EDITOR_SUPPORT_PNG_IMAGE_LOADER_H
