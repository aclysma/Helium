/*#include "Precompile.h"*/
#include "JointTransform.h"

#include "Foundation/Math/EulerAngles.h"
#include "Foundation/Undo/PropertyCommand.h"

#include "Pipeline/SceneGraph/Color.h"
#include "Pipeline/SceneGraph/PrimitiveAxes.h"
#include "Pipeline/SceneGraph/PrimitiveRings.h"
#include "Pipeline/SceneGraph/HierarchyNodeType.h"
#include "Pipeline/SceneGraph/Scene.h"

using namespace Helium;
using namespace Helium::SceneGraph;

static D3DMATERIAL9 g_JointTransformMaterial;

REFLECT_DEFINE_OBJECT( JointTransform );

void JointTransform::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &JointTransform::m_SegmentScaleCompensate, TXT( "m_SegmentScaleCompensate" ) );
}

void JointTransform::InitializeType()
{
    Reflect::RegisterClassType< JointTransform >( TXT( "SceneGraph::JointTransform" ) );

    ZeroMemory(&g_JointTransformMaterial, sizeof(g_JointTransformMaterial));
    g_JointTransformMaterial.Ambient = SceneGraph::Color::DARKGREEN;
    g_JointTransformMaterial.Diffuse = SceneGraph::Color::BLACK;
    g_JointTransformMaterial.Specular = SceneGraph::Color::BLACK;
}

void JointTransform::CleanupType()
{
    Reflect::UnregisterClassType< JointTransform >();
}

JointTransform::JointTransform()
: m_SegmentScaleCompensate( false )
{
}

JointTransform::~JointTransform()
{
}

void JointTransform::Initialize()
{
    Base::Initialize();

    SceneGraph::PrimitiveRings* rings = static_cast< SceneGraph::PrimitiveRings* >( m_Owner->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointRings ) );
    m_ObjectBounds.minimum = Vector3(-rings->m_Radius, -rings->m_Radius, -rings->m_Radius);
    m_ObjectBounds.maximum = Vector3(rings->m_Radius, rings->m_Radius, rings->m_Radius);
}

int32_t JointTransform::GetImageIndex() const
{
    return -1; // Helium::GlobalFileIconsTable().GetIconID( TXT( "joint" ) );
}

tstring JointTransform::GetApplicationTypeName() const
{
    return TXT( "Joint" );
}

void JointTransform::Render( RenderVisitor* render )
{
    RenderEntry* entry = render->Allocate(this);

    entry->m_Location = render->State().m_Matrix.Normalized();
    entry->m_Center = m_ObjectBounds.Center();

    if (IsSelected() && m_Owner->IsFocused() )
    {
        entry->m_Draw = &JointTransform::DrawSelected;
    }
    else
    {
        entry->m_Draw = &JointTransform::DrawNormal;
    }

    // don't call Base here, it will draw big ass axes
    SceneGraph::HierarchyNode::Render( render );
}

void JointTransform::DrawNormal( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const SceneGraph::HierarchyNode* node = Reflect::AssertCast<SceneGraph::HierarchyNode>( object );

    const JointTransform* joint = Reflect::AssertCast<JointTransform>( node );

    joint->SetMaterial( g_JointTransformMaterial );

    node->GetOwner()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointAxes )->Draw( args );
    node->GetOwner()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointRings )->Draw( args );
}

void JointTransform::DrawSelected( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const SceneGraph::HierarchyNode* node = Reflect::AssertCast<SceneGraph::HierarchyNode>( object );

    const JointTransform* joint = Reflect::AssertCast<JointTransform>( node );

    joint->SetMaterial( g_JointTransformMaterial );

    node->GetOwner()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointAxes )->Draw( args );
    node->GetOwner()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointRings )->Draw( args );
}

bool JointTransform::Pick( PickVisitor* pick )
{
    pick->SetCurrentObject (this, pick->State().m_Matrix.Normalized());

    return pick->PickPoint(Vector3::Zero, static_cast< SceneGraph::PrimitiveAxes* >( m_Owner->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointAxes ) )->m_Length);
}