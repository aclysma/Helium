//----------------------------------------------------------------------------------------------------------------------
// RVertexShader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_R_VERTEX_SHADER_H
#define LUNAR_RENDERING_R_VERTEX_SHADER_H

#include "Rendering/RShader.h"

namespace Lunar
{
    class Renderer;

    L_DECLARE_RPTR( RVertexDescription );
    L_DECLARE_RPTR( RVertexInputLayout );

    /// Vertex shader interface.
    class LUNAR_RENDERING_API RVertexShader : public RShader
    {
    public:
        /// @name Type Information
        //@{
        EType GetType() const;
        //@}

        /// @name Vertex Input Layout Caching
        //@{
        void CacheDescription( Renderer* pRenderer, RVertexDescription* pDescription );
        RVertexInputLayout* GetCachedInputLayout() const;
        //@}

    protected:
        /// Most recently used vertex description.
        RVertexDescriptionPtr m_spCachedDescription;
        /// Input layout associated with the most recently used vertex description.
        RVertexInputLayoutPtr m_spCachedInputLayout;

        /// @name Construction/Destruction
        //@{
        RVertexShader();
        virtual ~RVertexShader() = 0;
        //@}
    };
}

#endif  // LUNAR_RENDERING_R_VERTEX_SHADER_H
