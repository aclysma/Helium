/*#include "Precompile.h"*/
#include "PrimitiveCone.h"

#include "Pipeline/SceneGraph/Pick.h"

using namespace Helium;
using namespace Helium::SceneGraph;

PrimitiveCone::PrimitiveCone(ResourceTracker* m_Tracker)
: PrimitiveTemplate(m_Tracker)
{
    SetElementType( ElementTypes::Position );

    m_Length = 1.0f;
    m_Radius = 0.5f;
    m_Steps = 24;

    m_Bounds.minimum = Vector3 (-m_Radius, -m_Radius, -m_Length);
    m_Bounds.maximum = Vector3 (m_Radius, m_Radius, m_Length);
}

uint32_t PrimitiveCone::GetWireVertCount() const
{
    if (m_Steps == 0)
    {
        return m_WireVertCount = 0;
    }
    else
    {
        return m_WireVertCount = m_Steps+1+8;
    }
}

uint32_t PrimitiveCone::GetPolyVertCount() const
{
    if (m_Steps == 0)
    {
        return m_PolyVertCount = 0;
    }
    else
    {
        return m_PolyVertCount = (m_Steps*2)+4;
    }
}

void PrimitiveCone::Update()
{
    m_Bounds.minimum = Vector3 (-m_Radius, -m_Radius, -m_Length);
    m_Bounds.maximum = Vector3 (m_Radius, m_Radius, m_Length);

    SetElementCount( GetWireVertCount() + GetPolyVertCount() );
    m_Vertices.clear();

    float stepAngle = (float32_t)HELIUM_TWOPI / (float32_t)(m_Steps);


    //
    // Wire
    //

    m_Vertices.push_back(Position (0.0f, 0.0f, m_Length/2.0f));
    m_Vertices.push_back(Position ((float32_t)(cos(0.f)) * m_Radius, (float32_t)(sin(0.f)) * m_Radius, -m_Length/2.0f));

    m_Vertices.push_back(Position (0.0f, 0.0f, m_Length/2.0f));
    m_Vertices.push_back(Position ((float32_t)(cos(HELIUM_PI_2)) * m_Radius, (float32_t)(sin(HELIUM_PI_2)) * m_Radius, -m_Length/2.0f));

    m_Vertices.push_back(Position (0.0f, 0.0f, m_Length/2.0f));
    m_Vertices.push_back(Position ((float32_t)(cos(HELIUM_PI)) * m_Radius, (float32_t)(sin(HELIUM_PI)) * m_Radius, -m_Length/2.0f));

    m_Vertices.push_back(Position (0.0f, 0.0f, m_Length/2.0f));
    m_Vertices.push_back(Position ((float32_t)(cos(-HELIUM_PI_2)) * m_Radius, (float32_t)(sin(-HELIUM_PI_2)) * m_Radius, -m_Length/2.0f));

    for (int x=0; x<=m_Steps; x++)
    {
        float theta = (float32_t)(x) * stepAngle;
        m_Vertices.push_back(Position ((float32_t)(cos(theta)) * m_Radius, (float32_t)(sin(theta)) * m_Radius, -m_Length/2.0f));
    }


    //
    // Poly
    //

    m_Vertices.push_back(Position (0.0f, 0.0f, m_Length/2.0f));
    for (int x=0; x<=m_Steps; x++)
    {
        float theta = (float32_t)(x) * stepAngle;
        m_Vertices.push_back(Position ((float32_t)(cos(theta)) * m_Radius, (float32_t)(sin(theta)) * m_Radius, -m_Length/2.0f));
    }

    m_Vertices.push_back(Position (0.0f, 0.0f, -m_Length/2.0f));
    for (int x=m_Steps; x>=0; x--)
    {
        float theta = (float32_t)(x) * stepAngle;
        m_Vertices.push_back(Position ((float32_t)(cos(theta)) * m_Radius, (float32_t)(sin(theta)) * m_Radius, -m_Length/2.0f));
    }

    Base::Update();
}

void PrimitiveCone::Draw( DrawArgs* args, const bool* solid, const bool* transparent ) const
{
    if (!SetState())
        return;

    if (transparent ? *transparent : m_IsTransparent)
    {
        D3DMATERIAL9 m;
        ZeroMemory(&m, sizeof(m));

        m_Device->GetMaterial(&m);
        m.Ambient.a = m.Ambient.a < 0.0001 ? 0.5f : m.Ambient.a;
        m.Diffuse = m.Ambient;
        m_Device->SetMaterial(&m);

        m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    }

    if (solid ? *solid : m_IsSolid)
    {
        m_Device->DrawPrimitive(D3DPT_TRIANGLEFAN, (UINT)GetBaseIndex()+m_WireVertCount, m_Steps);
        m_Device->DrawPrimitive(D3DPT_TRIANGLEFAN, (UINT)GetBaseIndex()+m_WireVertCount+m_Steps+2, m_Steps);
        args->m_TriangleCount += (m_Steps*2);
    }
    else
    {
        m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex(), 4);
        m_Device->DrawPrimitive(D3DPT_LINESTRIP, (UINT)GetBaseIndex()+8, m_Steps+1);
        args->m_TriangleCount += (m_Steps*2);
    }

    if (transparent ? *transparent : m_IsTransparent)
    {
        m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    }
}

bool PrimitiveCone::Pick( PickVisitor* pick, const bool* solid ) const
{
    float stepAngle = (float32_t)HELIUM_TWOPI / (float32_t)(m_Steps);
    float sf=0, u=0, v=0;

    Vector3 v0, v1, v2;

    // cone
    for (int i=0; i<m_Steps; i++)
    {
        float theta = (float32_t)(i) * stepAngle;

        v0 = Vector3 (0.0f, 0.0f, m_Length/2.0f);
        v1 = Vector3 ((float32_t)(cos(theta)) * m_Radius, (float32_t)(sin(theta)) * m_Radius, -m_Length/2.0f);
        v2 = Vector3 ((float32_t)(cos(theta+stepAngle)) * m_Radius, (float32_t)(sin(theta+stepAngle)) * m_Radius, -m_Length/2.0f);

        if (pick->PickTriangle(v0, v1, v2))
        {
            return true;
        }
    }

    return false;
}