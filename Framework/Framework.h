//----------------------------------------------------------------------------------------------------------------------
// Framework.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_FRAMEWORK_FRAMEWORK_H
#define LUNAR_FRAMEWORK_FRAMEWORK_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef LUNAR_FRAMEWORK_EXPORTS
        #define LUNAR_FRAMEWORK_API HELIUM_API_EXPORT
    #else
        #define LUNAR_FRAMEWORK_API HELIUM_API_IMPORT
    #endif
#else
    #define LUNAR_FRAMEWORK_API
#endif

#endif  // LUNAR_FRAMEWORK_FRAMEWORK_H
