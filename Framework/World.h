//----------------------------------------------------------------------------------------------------------------------
// World.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_FRAMEWORK_WORLD_H
#define LUNAR_FRAMEWORK_WORLD_H

#include "Framework/Framework.h"
#include "Engine/GameObject.h"

#include "Platform/Math/Simd/Quat.h"
#include "Platform/Math/Simd/Vector3.h"
#include "Graphics/GraphicsScene.h"

namespace Lunar
{
    class Entity;

    HELIUM_DECLARE_PTR( GraphicsScene );
    HELIUM_DECLARE_PTR( Layer );

    /// World instance.
    ///
    /// A world contains a discrete group of entities that can be simulated within an application environment.  Multiple
    /// world instances can exist at the same time, allowing the use of specific worlds for special-case scenarios, such
    /// as rendering scenes outside the game world to a texture or editor preview windows.
    class LUNAR_FRAMEWORK_API World : public GameObject
    {
        L_DECLARE_OBJECT( World, GameObject );

    public:
        /// @name Construction/Destruction
        //@{
        World();
        virtual ~World();
        //@}

        /// @name World Initialization
        //@{
        virtual bool Initialize();
        virtual void Shutdown();
        //@}

        /// @name World Updating
        //@{
        virtual void UpdateGraphicsScene();
        //@}

        /// @name GameObject Interface
        //@{
        virtual void PreDestroy();
        //@}

        /// @name Entity Creation
        //@{
        virtual Entity* CreateEntity(
            Layer* pLayer, const GameObjectType* pType, const Simd::Vector3& rPosition = Simd::Vector3( 0.0f ),
            const Simd::Quat& rRotation = Simd::Quat::IDENTITY, const Simd::Vector3& rScale = Simd::Vector3( 1.0f ),
            Entity* pTemplate = NULL, Name name = NULL_NAME, bool bAssignInstanceIndex = true );
        virtual bool DestroyEntity( Entity* pEntity );
        //@}

        /// @name Layer Registration
        //@{
        virtual bool AddLayer( Layer* pLayer );
        virtual bool RemoveLayer( Layer* pLayer );

        inline size_t GetLayerCount() const;
        Layer* GetLayer( size_t index ) const;
        //@}

        /// @name Scene Access
        //@{
        GraphicsScene* GetGraphicsScene() const;
        inline uint32_t GetMainSceneViewId() const;
        //@}

    private:
        /// Active layers.
        DynArray< LayerPtr > m_layers;

        /// Graphics scene instance.
        GraphicsScenePtr m_spGraphicsScene;
        /// Main scene view ID.
        uint32_t m_mainSceneViewId;
    };
}

#include "Framework/World.inl"

#endif  // LUNAR_FRAMEWORK_WORLD_H
