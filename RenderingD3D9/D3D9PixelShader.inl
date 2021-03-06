//----------------------------------------------------------------------------------------------------------------------
// D3D9PixelShader.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the Direct3D pixel shader.
    ///
    /// @return  Direct3D pixel shader instance.
    IDirect3DPixelShader9* D3D9PixelShader::GetD3DShader() const
    {
        return ( m_bStaging ? NULL : static_cast< IDirect3DPixelShader9* >( m_pShaderData ) );
    }
}
