#pragma once

#include "Foundation/TUID.h"

#include "Pipeline/API.h"
#include "Pipeline/SceneGraph/CreateTool.h"
#include "Pipeline/SceneGraph/EntityInstance.h"

namespace Helium
{
    namespace SceneGraph
    {
        class EntityRowInfo
        {
        public:
#pragma TODO ("Rachel WIP - Canvas Strip clean up")
            // get rid of m_OriginalValue and m_Name
            tstring m_OriginalValue;
            tstring m_Name;
            Helium::Path m_ClassPath;
            float m_Probability;

            tstring GetListName() const;
        };

        typedef std::vector< EntityRowInfo > V_EntityRowInfo;

        class PIPELINE_API EntityInstanceCreateTool : public CreateTool
        {
        public:
            Helium::Path m_ClassPath;
            static bool s_PointerVisible;
            static bool s_BoundsVisible;
            static bool s_GeometryVisible;

        public:
            REFLECT_DECLARE_ABSTRACT(SceneGraph::EntityInstanceCreateTool, SceneGraph::CreateTool);
            static void InitializeType();
            static void CleanupType();

        protected:
            Inspect::List* m_RandomEntityList;
            V_EntityRowInfo m_RandomEntityInfo;
            static std::vector< tstring > s_RandomEntities;

            Inspect::FileDialogButton* m_FileButton;
            Inspect::FileDialogButton* m_FileButtonAdd;

        public:
            EntityInstanceCreateTool(SceneGraph::Scene* scene, PropertiesGenerator* generator);
            virtual ~EntityInstanceCreateTool();

            virtual SceneGraph::TransformPtr CreateNode() HELIUM_OVERRIDE;
            virtual void CreateProperties() HELIUM_OVERRIDE;

            tstring GetEntityAsset() const;
            void SetEntityAsset(const tstring& value);
            void AddEntityAsset(const tstring& value);

            bool GetPointerVisible() const;
            void SetPointerVisible(bool show);
            bool GetBoundsVisible() const;
            void SetBoundsVisible(bool show);
            bool GetGeometryVisible() const;
            void SetGeometryVisible(bool show);
            tstring GetRandomEntity() const;
            void SetRandomEntity( const tstring& entityName );

            void OnDeleteClass( const Inspect::ButtonClickedArgs& args );
            void OnClear(const Inspect::ButtonClickedArgs& args);
            void OnNormalize(const Inspect::ButtonClickedArgs& args);
            void OnModify(const Inspect::ButtonClickedArgs& args);
        };
    }
}