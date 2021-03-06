#pragma once

// Include our Windows header data first to make sure everything is set up properly before the Direct3D includes pull it
// in.
#include "Platform/Windows/Windows.h"

#include <d3d9.h>
#include <d3dx9.h>

#include "Pipeline/API.h"
#include "Pipeline/SceneGraph/Pick.h"
#include "Pipeline/SceneGraph/SceneNodeType.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PickHit;
        typedef Helium::SmartPtr< PickHit > PickHitPtr;
        typedef std::vector< PickHitPtr > V_PickHitSmartPtr;

        class PIPELINE_API HierarchyNodeType : public SceneNodeType
        {
        protected:
            // members
            bool m_Visible;
            bool m_Selectable;

        public:
            // materials
            D3DMATERIAL9 m_WireMaterial;
            D3DMATERIAL9 m_SolidMaterial;

        public:
            REFLECT_DECLARE_ABSTRACT( HierarchyNodeType, SceneGraph::SceneNodeType );
            static void InitializeType();
            static void CleanupType();

        public:
            HierarchyNodeType( SceneGraph::Scene* scene, const Reflect::Class* instanceClass );

            virtual void Create();
            virtual void Delete();

            bool IsVisible() const;
            void SetVisible(bool value);

            bool IsSelectable() const;
            void SetSelectable( bool value );

            virtual bool IsTransparent();
        };

        typedef std::map< tstring, HierarchyNodeType* > M_HierarchyNodeTypeDumbPtr;
    }
}