#pragma once

#include "Foundation/Undo/Command.h"
#include "Foundation/Math/Matrix4.h"

namespace Helium
{
    namespace SceneGraph
    {
        class Camera;
        class Viewport;

        /// @class CameraMovedCommand
        /// @breif Used to implement a Undo/Redo queue for camera movement in the scene editor
        class CameraMovedCommand : public Undo::Command
        {
        private:
            SceneGraph::Camera*         m_Camera;
            SceneGraph::Viewport*           m_View;

            Matrix4    m_PreviousTransform;
        public:
            CameraMovedCommand(SceneGraph::Viewport* view, SceneGraph::Camera* cam );
            virtual ~CameraMovedCommand();

        public:
            void Undo();
            void Redo();
        };

    } // namespace SceneGraph
}