//----------------------------------------------------------------------------------------------------------------------
// World.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the number of layers currently active in this world.
    ///
    /// @return  Layer count.
    ///
    /// @see GetLayer()
    size_t World::GetLayerCount() const
    {
        return m_layers.GetSize();
    }

    /// Get the ID of the main graphics scene view.
    ///
    /// @return  Main graphics scene view ID.
    uint32_t World::GetMainSceneViewId() const
    {
        return m_mainSceneViewId;
    }
}
