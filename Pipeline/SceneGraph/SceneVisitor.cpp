/*#include "Precompile.h"*/
#include "SceneVisitor.h"

#include "Pipeline/SceneGraph/Pick.h"
#include "Pipeline/SceneGraph/Scene.h"
#include "Pipeline/SceneGraph/Transform.h"
#include "Pipeline/SceneGraph/EntityInstance.h"
#include "HierarchyNodeType.h"

using namespace Helium;
using namespace Helium::SceneGraph;

TraversalAction HierarchyChildTraverser::VisitHierarchyNode(SceneGraph::HierarchyNode* node)
{
  OS_HierarchyNodeDumbPtr::Iterator itr = node->GetChildren().Begin();
  OS_HierarchyNodeDumbPtr::Iterator end = node->GetChildren().End();
  for ( ; itr != end; ++itr )
  {
    m_Children.Append( *itr );
  }

  return TraversalActions::Continue;
}

HierarchyRenderTraverser::HierarchyRenderTraverser(RenderVisitor* renderVisitor)
: m_RenderVisitor(renderVisitor)
{

}

TraversalAction HierarchyRenderTraverser::VisitHierarchyNode(SceneGraph::HierarchyNode* node)
{
  TraversalAction action;

  Matrix4 matrix = m_RenderVisitor->State().m_Matrix;

  m_RenderVisitor->State().m_Matrix = node->GetTransform()->GetGlobalTransform() * m_RenderVisitor->State().m_Matrix;

  if (node->BoundsCheck(m_RenderVisitor->State().m_Matrix))
  {
    if (node->IsVisible())
    {
      // render this node
      node->Render( m_RenderVisitor );
    }

    // keep traversing
    action = TraversalActions::Continue;
  }
  else
  {
    // not visible, prune traversal
    action = TraversalActions::Prune;
  }

  m_RenderVisitor->State().m_Matrix = matrix;

  return action;
}

HierarchyPickTraverser::HierarchyPickTraverser(PickVisitor* pickVisitor)
: m_PickVisitor (pickVisitor)
{

}

TraversalAction HierarchyPickTraverser::VisitHierarchyNode(SceneGraph::HierarchyNode* node)
{
  TraversalAction action;

  Matrix4 matrix = m_PickVisitor->State().m_Matrix;

  m_PickVisitor->State().m_Matrix = node->GetTransform()->GetGlobalTransform() * m_PickVisitor->State().m_Matrix;
  
  if (node->BoundsCheck(m_PickVisitor->State().m_Matrix))
  {
    if (node->IsVisible())
    {
      m_PickVisitor->SetCurrentObject (node, m_PickVisitor->State().m_Matrix);
      
      if (m_PickVisitor->IntersectsBox(node->GetObjectHierarchyBounds()))
      {
        // pick this node
        node->Pick( m_PickVisitor ); 
      }
    }

    // keep traversin
    action = TraversalActions::Continue;
  }
  else
  {
    // not visible or hierarchy bounding box is not intersected, prune traversal
    action = TraversalActions::Prune;
  }

  m_PickVisitor->State().m_Matrix = matrix;

  return action;
}