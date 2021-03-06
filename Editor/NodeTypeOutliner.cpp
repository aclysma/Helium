#include "Precompile.h"
#include "NodeTypeOutliner.h"
#include "Pipeline/SceneGraph/Scene.h"

#include "Foundation/Log.h"

#include "Editor/Controls/Tree/SortTreeCtrl.h"

using namespace Helium;
using namespace Helium::Editor;
using namespace Helium::SceneGraph;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
NodeTypeOutliner::NodeTypeOutliner( SceneGraph::SceneManager* sceneManager )
: SceneOutliner( sceneManager )
, m_InvisibleRoot( NULL )
{
    m_DisplayCounts = true; 
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
NodeTypeOutliner::~NodeTypeOutliner()
{
    DisconnectSceneListeners();
}

///////////////////////////////////////////////////////////////////////////////
// If a valid scene is specified, iterates over the node types in the scene
// and adds each one to the tree.  Also adds each instance of a given node type
// to the tree.
// 
void NodeTypeOutliner::AddNodeTypes()
{
    if ( m_CurrentScene )
    {
        if ( m_CurrentScene->GetNodeTypesByName().size() > 0 )
        {
            m_TreeCtrl->Freeze();
            bool isSortingEnabled = m_TreeCtrl->IsSortingEnabled();
            m_TreeCtrl->DisableSorting();

            // For each node type, add it to the tree
            SceneGraph::HM_StrToSceneNodeTypeSmartPtr::const_iterator itr = m_CurrentScene->GetNodeTypesByName().begin();
            SceneGraph::HM_StrToSceneNodeTypeSmartPtr::const_iterator end = m_CurrentScene->GetNodeTypesByName().end();
            for ( ; itr != end; ++itr )
            {
                const SceneNodeTypePtr& nodeType = itr->second;
                AddNodeType( nodeType );
            }

            m_TreeCtrl->EnableSorting( isSortingEnabled );
            Sort( m_InvisibleRoot );
            m_TreeCtrl->Thaw();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified node type to the tree.  Any instances that are part of 
// the node type are also added.
// 
void NodeTypeOutliner::AddNodeType( SceneGraph::SceneNodeType* nodeType )
{
    m_TreeCtrl->Freeze();
    bool isSortingEnabled = m_TreeCtrl->IsSortingEnabled();
    m_TreeCtrl->DisableSorting();

    wxTreeItemId item = AddItem( m_InvisibleRoot, 
        nodeType->GetName(), 
        nodeType->GetImageIndex(), 
        new SceneOutlinerItemData( nodeType ), 
        false, 
        false);

    HM_SceneNodeSmartPtr::const_iterator itr = nodeType->GetInstances().begin();
    HM_SceneNodeSmartPtr::const_iterator end = nodeType->GetInstances().end();
    for ( ; itr != end; ++itr )
    {
        AddInstance( itr->second );
    }

    m_TreeCtrl->EnableSorting( isSortingEnabled );
    Sort( item );
    m_TreeCtrl->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Adds an instance to the tree.  The instance will appear under the node type
// that it belongs to.
// 
void NodeTypeOutliner::AddInstance( SceneGraph::SceneNode* instance )
{
    EDITOR_SCOPE_TIMER( ("") );

    M_TreeItems::const_iterator found = m_Items.find( instance->GetNodeType() );
    if ( found != m_Items.end() )
    {
        AddItem( found->second, instance->GetName(), instance->GetImageIndex(), new SceneOutlinerItemData( instance ), instance->IsSelected() );
    }
    else
    {
        Log::Warning( TXT( "Unable to add node %s to the Type Outliner because there is no type named %s.\n" ), instance->GetName().c_str(), instance->GetNodeType()->GetName().c_str() );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Removes the specified node type from the tree.  The node types form the root
// items of the tree.  The children of the node type should have already been
// removed from the tree.
// 
void NodeTypeOutliner::RemoveNodeType( SceneGraph::SceneNodeType* nodeType )
{
    EDITOR_SCOPE_TIMER( ("") );
#ifdef _DEBUG
    // Sanity check.  The item we are deleting shouldn't have any children (they
    // should have already been removed).  If this needs to change for some 
    // reason, this function will need to remove all the children manually.
    M_TreeItems::iterator found = m_Items.find( nodeType );
    if ( found != m_Items.end() )
    {
        wxTreeItemId item = found->second;
        HELIUM_ASSERT( m_TreeCtrl->GetChildrenCount( item ) == 0 );
    }
#endif

    DeleteItem( nodeType );
}

///////////////////////////////////////////////////////////////////////////////
// Removes the specified instance from the tree.
// 
void NodeTypeOutliner::RemoveInstance( SceneGraph::SceneNode* instance )
{
    DeleteItem( instance );
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void NodeTypeOutliner::OnBeginLabelEdit( wxTreeEvent& args )
{
    Reflect::Object* found = GetTreeItemData( args.GetItem() )->GetObject();

    // If a valid Object was not found, or if the the object is not
    // a dependency node, we won't allow it's name to be changed.
    if ( !found || !found->IsClass( Reflect::GetClass<SceneGraph::HierarchyNode>() ) )
    {
        args.Veto();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Creates the tree control managed by this class.  This function should only
// be called once.  The tree control is returned so that the caller can
// insert it into the UI in the proper place.
// 
SortTreeCtrl* NodeTypeOutliner::CreateTreeCtrl( wxWindow* parent, wxWindowID id )
{
    SortTreeCtrl* tree = new SortTreeCtrl( parent, id, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_HIDE_ROOT | wxTR_EDIT_LABELS | wxTR_MULTIPLE, wxDefaultValidator, TXT( "NodeTypeOutliner" ) );
    m_InvisibleRoot = tree->AddRoot( TXT( "INVISIBLE_ROOT" ) );

    // Override dynamic GUI event handlers here
    tree->Connect( tree->GetId(), wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( NodeTypeOutliner::OnBeginLabelEdit ), NULL, this );

    return tree;
}

///////////////////////////////////////////////////////////////////////////////
// Clears out the tree.
// 
void NodeTypeOutliner::Clear()
{
    SceneOutliner::Clear();

    m_TreeCtrl->DeleteChildren( m_InvisibleRoot );
}

///////////////////////////////////////////////////////////////////////////////
// Called when the base class has finished changing the current scene.  Loads
// the node types into the tree control.
// 
void NodeTypeOutliner::CurrentSceneChanged( SceneGraph::Scene* oldScene )
{
    AddNodeTypes();
}

///////////////////////////////////////////////////////////////////////////////
// Connects all the required event listeners to the current scene.
// 
void NodeTypeOutliner::ConnectSceneListeners()
{
    SceneOutliner::ConnectSceneListeners();

    if ( m_CurrentScene )
    {
        const HM_StrToSceneNodeTypeSmartPtr& types = m_CurrentScene->GetNodeTypesByName();
        HM_StrToSceneNodeTypeSmartPtr::const_iterator itr = types.begin();
        HM_StrToSceneNodeTypeSmartPtr::const_iterator end = types.end();
        for ( ; itr != end; ++itr )
        {
            itr->second->AddNodeAddedListener( NodeTypeChangeSignature::Delegate ( this, &NodeTypeOutliner::NodeAddedToType ) );
            itr->second->AddNodeRemovedListener( NodeTypeChangeSignature::Delegate ( this, &NodeTypeOutliner::NodeRemovedFromType ) );
        }

        // Connect listeners
        m_CurrentScene->e_NodeTypeCreated.Add( NodeTypeExistenceSignature::Delegate ( this, &NodeTypeOutliner::NodeTypeAdded ) );
        m_CurrentScene->e_NodeTypeDeleted.Add( NodeTypeExistenceSignature::Delegate ( this, &NodeTypeOutliner::NodeTypeRemoved ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Disconnects all the registered event listeners from this class on the current
// scene.
// 
void NodeTypeOutliner::DisconnectSceneListeners()
{
    if ( m_CurrentScene )
    {
        const HM_StrToSceneNodeTypeSmartPtr& types = m_CurrentScene->GetNodeTypesByName();
        HM_StrToSceneNodeTypeSmartPtr::const_iterator itr = types.begin();
        HM_StrToSceneNodeTypeSmartPtr::const_iterator end = types.end();
        for ( ; itr != end; ++itr )
        {
            itr->second->RemoveNodeAddedListener( NodeTypeChangeSignature::Delegate ( this, &NodeTypeOutliner::NodeAddedToType ) );
            itr->second->RemoveNodeRemovedListener( NodeTypeChangeSignature::Delegate ( this, &NodeTypeOutliner::NodeRemovedFromType ) );
        }

        // Disconnect listeners
        m_CurrentScene->e_NodeTypeCreated.Remove( NodeTypeExistenceSignature::Delegate ( this, &NodeTypeOutliner::NodeTypeAdded ) );
        m_CurrentScene->e_NodeTypeDeleted.Remove( NodeTypeExistenceSignature::Delegate ( this, &NodeTypeOutliner::NodeTypeRemoved ) );
    }

    SceneOutliner::DisconnectSceneListeners();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a new node type is added to the scene.  Adds a root item
// to the scene to represent the new node type.
// 
void NodeTypeOutliner::NodeTypeAdded( const SceneGraph::NodeTypeExistenceArgs& args )
{
    AddNodeType( args.m_NodeType );

    args.m_NodeType->AddNodeAddedListener( NodeTypeChangeSignature::Delegate ( this, &NodeTypeOutliner::NodeAddedToType ) );
    args.m_NodeType->AddNodeRemovedListener( NodeTypeChangeSignature::Delegate ( this, &NodeTypeOutliner::NodeRemovedFromType ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a node type is removed from the scene.  Removes the 
// corresponding tree item from the view.
// 
void NodeTypeOutliner::NodeTypeRemoved( const SceneGraph::NodeTypeExistenceArgs& args )
{
    RemoveNodeType( args.m_NodeType );

    args.m_NodeType->RemoveNodeAddedListener( NodeTypeChangeSignature::Delegate ( this, &NodeTypeOutliner::NodeAddedToType ) );
    args.m_NodeType->RemoveNodeRemovedListener( NodeTypeChangeSignature::Delegate ( this, &NodeTypeOutliner::NodeRemovedFromType ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene node is added to a node type.  Adds a tree item
// representing the scene node as a child its node type.
// 
void NodeTypeOutliner::NodeAddedToType( const SceneGraph::NodeTypeChangeArgs& args )
{
    AddInstance( args.m_Node );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene node is removed from a node type.  Removes the
// corresponding tree item from the view.
// 
void NodeTypeOutliner::NodeRemovedFromType( const SceneGraph::NodeTypeChangeArgs& args )
{
    RemoveInstance( args.m_Node );
}
