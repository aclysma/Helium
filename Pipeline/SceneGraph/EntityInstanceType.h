#pragma once

#include "InstanceType.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PIPELINE_API EntityInstanceType : public InstanceType
        {
        private:
            // materials
            D3DMATERIAL9 m_Material;

        public:
            REFLECT_DECLARE_ABSTRACT( SceneGraph::EntityInstanceType, SceneGraph::InstanceType );
            static void InitializeType();
            static void CleanupType();

            EntityInstanceType( SceneGraph::Scene* scene, const Reflect::Class* instanceClass );
            ~EntityInstanceType();

            virtual void Reset() HELIUM_OVERRIDE;
            virtual void Create() HELIUM_OVERRIDE;
            virtual void Delete() HELIUM_OVERRIDE;

            virtual void AddInstance(SceneNodePtr n) HELIUM_OVERRIDE;
            virtual void PopulateManifest( Asset::SceneManifest* manifest ) const HELIUM_OVERRIDE;
        };
    }
}