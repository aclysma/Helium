//----------------------------------------------------------------------------------------------------------------------
// Material.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsPch.h"
#include "Graphics/Material.h"

#include "Rendering/RConstantBuffer.h"
#include "Rendering/Renderer.h"
#include "Graphics/Texture.h"

using namespace Lunar;

L_IMPLEMENT_OBJECT( Material, Graphics, 0 );

/// Constructor.
Material::Material()
{
    MemoryZero( m_shaderVariantIndices, sizeof( m_shaderVariantIndices ) );

    for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_shaderVariantLoadIds ); ++shaderTypeIndex )
    {
        SetInvalid( m_shaderVariantLoadIds[ shaderTypeIndex ] );
        SetInvalid( m_constantBufferLoadIds[ shaderTypeIndex ] );
    }

#if L_EDITOR
    m_bLoadedOptions = false;
#endif
}

/// Destructor.
Material::~Material()
{
}

/// @copydoc GameObject::Serialize()
void Material::Serialize( Serializer& s )
{
    L_SERIALIZE_BASE( s );

    s << L_TAGGED( m_spShader );

#if L_EDITOR
    if( s.CanResolveTags() )
    {
        m_userOptions.Resize( 0 );

        Serializer::EMode serializerMode = s.GetMode();
        bool bSaving = ( serializerMode == Serializer::MODE_SAVE );
        bool bLoading = ( serializerMode == Serializer::MODE_LOAD );

        if( bSaving )
        {
            // Aggregate all user options from the resource information into the single "m_userOptions" array.
            Shader* pShader = m_spShader;
            if( pShader )
            {
                const Shader::Options& rUserOptions = pShader->GetUserOptions();

                DynArray< Name > enabledToggles;
                rUserOptions.GetOptionSetFromIndex(
                    RShader::TYPE_FIRST,
                    m_shaderVariantIndices[ 0 ],
                    enabledToggles,
                    m_userOptions );

                size_t enabledToggleCount = enabledToggles.GetSize();

                Shader::SelectPair optionPair;

                Name enabledChoice( TXT( "1" ) );
                Name disabledChoice( TXT( "0" ) );

                const DynArray< Shader::Toggle >& rUserToggles = rUserOptions.GetToggles();
                size_t userToggleCount = rUserToggles.GetSize();
                for( size_t userToggleIndex = 0; userToggleIndex < userToggleCount; ++userToggleIndex )
                {
                    optionPair.name = rUserToggles[ userToggleIndex ].name;

                    size_t enabledToggleIndex;
                    for( enabledToggleIndex = 0; enabledToggleIndex < enabledToggleCount; ++enabledToggleIndex )
                    {
                        if( enabledToggles[ enabledToggleIndex ] == optionPair.name )
                        {
                            break;
                        }
                    }

                    optionPair.choice =
                        ( enabledToggleIndex < enabledToggleCount ? enabledChoice : disabledChoice );

                    m_userOptions.Push( optionPair );
                }
            }
        }
        else if( bLoading )
        {
            DynArray< String > propertyTagNames;
            s.GetPropertyTagNames( propertyTagNames );

            Shader::SelectPair optionPair;
            optionPair.choice.Clear();

            size_t propertyTagCount = propertyTagNames.GetSize();
            for( size_t tagIndex = 0; tagIndex < propertyTagCount; ++tagIndex )
            {
                const String& rTagName = propertyTagNames[ tagIndex ];
                if( !rTagName.Contains( TXT( '.' ) ) )
                {
                    optionPair.name.Set( rTagName );
                    m_userOptions.Push( optionPair );
                }
            }
        }

        s.PushPropertyFlags( Serializer::FLAG_EDITOR_ONLY );

        size_t userOptionCount = m_userOptions.GetSize();
        for( size_t optionIndex = 0; optionIndex < userOptionCount; ++optionIndex )
        {
            Shader::SelectPair& rOptionPair = m_userOptions[ optionIndex ];
            s << Serializer::Tag( *rOptionPair.name ) << rOptionPair.choice;
        }

        // XXX TMC TODO: Replace with flexible name resolution support (a la m_userOptions above).
        s << L_TAGGED_STRUCT_DYNARRAY( m_float1Parameters );
        s << L_TAGGED_STRUCT_DYNARRAY( m_float2Parameters );
        s << L_TAGGED_STRUCT_DYNARRAY( m_float3Parameters );
        s << L_TAGGED_STRUCT_DYNARRAY( m_float4Parameters );

        s.PopPropertyFlags();

        if( bSaving )
        {
            m_userOptions.Clear();
        }
        else if( bLoading )
        {
            m_bLoadedOptions = true;
        }
    }
#endif

    // XXX TMC TODO: Replace with flexible name resolution support (a la m_userOptions above).
    s << L_TAGGED_STRUCT_DYNARRAY( m_textureParameters );
}

/// @copydoc GameObject::NeedsPrecacheResourceData()
bool Material::NeedsPrecacheResourceData() const
{
    return true;
}

/// @copydoc GameObject::BeginPrecacheResourceData()
bool Material::BeginPrecacheResourceData()
{
#if L_EDITOR
    // Convert shader options to variant indices if we just loaded a set of options.
    if( m_bLoadedOptions )
    {
        MemoryZero( m_shaderVariantIndices, sizeof( m_shaderVariantIndices ) );

        Shader* pShader = m_spShader;
        if( pShader )
        {
            const Shader::Options& rUserOptions = pShader->GetUserOptions();

            for( size_t shaderTypeIndex = 0;
                shaderTypeIndex < HELIUM_ARRAY_COUNT( m_shaderVariantIndices );
                ++shaderTypeIndex )
            {
                m_shaderVariantIndices[ shaderTypeIndex ] = static_cast< uint32_t >( rUserOptions.GetOptionSetIndex(
                    static_cast< RShader::EType >( shaderTypeIndex ),
                    m_userOptions.GetData(),
                    m_userOptions.GetSize() ) );
            }
        }

        m_userOptions.Clear();

        m_bLoadedOptions = false;
    }
#endif

    // Preload shader variant resources.
    Shader* pShader = m_spShader;
    if( !pShader )
    {
        for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_shaderVariants ); ++shaderTypeIndex )
        {
            HELIUM_ASSERT( IsInvalid( m_shaderVariantLoadIds[ shaderTypeIndex ] ) );
            m_shaderVariants[ shaderTypeIndex ].Release();
        }
    }
    else
    {
        for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_shaderVariants ); ++shaderTypeIndex )
        {
            HELIUM_ASSERT( IsInvalid( m_shaderVariantLoadIds[ shaderTypeIndex ] ) );
            m_shaderVariantLoadIds[ shaderTypeIndex ] = pShader->BeginLoadVariant(
                static_cast< RShader::EType >( shaderTypeIndex ),
                m_shaderVariantIndices[ shaderTypeIndex ] );
        }
    }

    // Preload the constant buffers for shader parameters.
    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( pRenderer )
    {
        for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_constantBuffers ); ++shaderTypeIndex )
        {
            HELIUM_ASSERT( !m_constantBuffers[ shaderTypeIndex ] );
            HELIUM_ASSERT( IsInvalid( m_constantBufferLoadIds[ shaderTypeIndex ] ) );

            size_t bufferSize = GetSubDataSize( static_cast< uint32_t >( shaderTypeIndex ) );
            if( bufferSize == 0 )
            {
                continue;
            }

            RConstantBufferPtr spBuffer = pRenderer->CreateConstantBuffer(
                bufferSize,
                RENDERER_BUFFER_USAGE_STATIC );
            if( !spBuffer )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "Material::BeginPrecacheResourceData(): Failed to allocate constant buffer of %" )
                    TPRIuSZ TXT( " bytes for parameter usage for material \"%s\".\n" ) ),
                    bufferSize,
                    *GetPath().ToString() );

                continue;
            }

            void* pBufferData = spBuffer->Map();
            if( !pBufferData )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "Material::BeginPrecacheResourceData(): Failed to map constant buffer data for " )
                    TXT( "material \"%s\".\n" ) ),
                    *GetPath().ToString() );

                continue;
            }

            size_t loadId = BeginLoadSubData( pBufferData, static_cast< uint32_t >( shaderTypeIndex ) );
            if( IsInvalid( loadId ) )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "Material::BeginPrecacheResourceData(): Failed to begin loading resource sub-data %" )
                    TPRIuSZ TXT( " for material \"%s\".\n" ) ),
                    shaderTypeIndex,
                    *GetPath().ToString() );

                spBuffer->Unmap();

                continue;
            }

            m_constantBuffers[ shaderTypeIndex ] = spBuffer;
            m_constantBufferLoadIds[ shaderTypeIndex ] = loadId;
        }
    }

    return true;
}

/// @copydoc GameObject::TryFinishPrecacheResourceData()
bool Material::TryFinishPrecacheResourceData()
{
    Shader* pShader = m_spShader;
    if( pShader )
    {
        for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_shaderVariants ); ++shaderTypeIndex )
        {
            size_t loadId = m_shaderVariantLoadIds[ shaderTypeIndex ];
            if( IsValid( loadId ) &&
                !pShader->TryFinishLoadVariant( loadId, m_shaderVariants[ shaderTypeIndex ] ) )
            {
                return false;
            }

            SetInvalid( m_shaderVariantLoadIds[ shaderTypeIndex ] );
        }
    }

    for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_constantBuffers ); ++shaderTypeIndex )
    {
        size_t loadId = m_constantBufferLoadIds[ shaderTypeIndex ];
        if( IsInvalid( loadId ) )
        {
            continue;
        }

        if( !TryFinishLoadSubData( loadId ) )
        {
            return false;
        }

        RConstantBuffer* pBuffer = m_constantBuffers[ shaderTypeIndex ];
        HELIUM_ASSERT( pBuffer );
        pBuffer->Unmap();

        SetInvalid( m_constantBufferLoadIds[ shaderTypeIndex ] );
    }

#if L_EDITOR
    // Synchronize shader constant parameters with those exposed by the shader variant resources.
    SynchronizeShaderParameters();
#endif

    return true;
}

/// @copydoc Resource::SerializePersistentResourceData()
void Material::SerializePersistentResourceData( Serializer& s )
{
    s << Serializer::WrapArray( m_shaderVariantIndices );
}

/// @copydoc Resource::GetCacheName()
Name Material::GetCacheName() const
{
    static Name cacheName( TXT( "Material" ) );

    return cacheName;
}

/// Get the name of the constant buffer used to set material parameters.
///
/// @return  Material parameter constant buffer name.
Name Material::GetParameterConstantBufferName()
{
    static Name parameterConstantBufferName( TXT( "MaterialParameters" ) );

    return parameterConstantBufferName;
}

#if L_EDITOR
/// Synchronize the shader parameter list with those provided by the selected shader variant.
///
/// @see SynchronizeFloatVectorParameters(), SynchronizeTextureParameters()
void Material::SynchronizeShaderParameters()
{
    Shader* pShader = m_spShader;
    if( !pShader )
    {
        m_float1Parameters.Clear();
        m_float2Parameters.Clear();
        m_float3Parameters.Clear();
        m_float4Parameters.Clear();
        m_textureParameters.Clear();
    }

    // Synchronize floating-point constant parameters.
    Name parameterConstantBufferName = GetParameterConstantBufferName();

    size_t existingFloat1Count = m_float1Parameters.GetSize();
    size_t existingFloat2Count = m_float2Parameters.GetSize();
    size_t existingFloat3Count = m_float3Parameters.GetSize();
    size_t existingFloat4Count = m_float4Parameters.GetSize();

    DynArray< Float1Parameter > newFloat1Parameters;
    DynArray< Float2Parameter > newFloat2Parameters;
    DynArray< Float3Parameter > newFloat3Parameters;
    DynArray< Float4Parameter > newFloat4Parameters;
    for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_shaderVariants ); ++shaderTypeIndex )
    {
        ShaderVariant* pShaderVariant = m_shaderVariants[ shaderTypeIndex ];
        if( !pShaderVariant )
        {
            continue;
        }

        const ShaderConstantBufferInfoSet* pBufferSet = pShaderVariant->GetConstantBufferInfoSet( 0 );
        if( !pBufferSet )
        {
            continue;
        }

        bool bCheckDuplicates =
            ( !newFloat1Parameters.IsEmpty() || !newFloat2Parameters.IsEmpty() || !newFloat3Parameters.IsEmpty() ||
            !newFloat4Parameters.IsEmpty() );

        const DynArray< ShaderConstantBufferInfo >& rBuffers = pBufferSet->buffers;
        size_t bufferCount = rBuffers.GetSize();
        for( size_t bufferIndex = 0; bufferIndex < bufferCount; ++bufferIndex )
        {
            const ShaderConstantBufferInfo& rBufferInfo = rBuffers[ bufferIndex ];
            if( rBufferInfo.name != parameterConstantBufferName )
            {
                continue;
            }

            const DynArray< ShaderConstantInfo >& rConstants = rBufferInfo.constants;
            size_t constantCount = rConstants.GetSize();
            for( size_t constantIndex = 0; constantIndex < constantCount; ++constantIndex )
            {
                const ShaderConstantInfo& rConstantInfo = rConstants[ constantIndex ];

                // Constants must be between 1 and 4 floating-point values.
                uint16_t constantSize = rConstantInfo.usedSize;
                if( constantSize < sizeof( float32_t ) || constantSize > sizeof( float32_t ) * 4 )
                {
                    continue;
                }

                Name constantName = rConstantInfo.name;

                size_t parameterIndex;
                if( bCheckDuplicates )
                {
                    size_t parameterCount = newFloat1Parameters.GetSize();
                    for( parameterIndex = 0; parameterIndex < parameterCount; ++parameterIndex )
                    {
                        if( newFloat1Parameters[ parameterIndex ].name == constantName )
                        {
                            break;
                        }
                    }

                    if( parameterIndex < parameterCount )
                    {
                        continue;
                    }

                    parameterCount = newFloat2Parameters.GetSize();
                    for( parameterIndex = 0; parameterIndex < parameterCount; ++parameterIndex )
                    {
                        if( newFloat2Parameters[ parameterIndex ].name == constantName )
                        {
                            break;
                        }
                    }

                    if( parameterIndex < parameterCount )
                    {
                        continue;
                    }

                    parameterCount = newFloat3Parameters.GetSize();
                    for( parameterIndex = 0; parameterIndex < parameterCount; ++parameterIndex )
                    {
                        if( newFloat3Parameters[ parameterIndex ].name == constantName )
                        {
                            break;
                        }
                    }

                    if( parameterIndex < parameterCount )
                    {
                        continue;
                    }

                    parameterCount = newFloat4Parameters.GetSize();
                    for( parameterIndex = 0; parameterIndex < parameterCount; ++parameterIndex )
                    {
                        if( newFloat4Parameters[ parameterIndex ].name == constantName )
                        {
                            break;
                        }
                    }

                    if( parameterIndex < parameterCount )
                    {
                        continue;
                    }
                }

                Simd::Vector4 newValue( 0.0f );
                for( parameterIndex = 0; parameterIndex < existingFloat1Count; ++parameterIndex )
                {
                    const Float1Parameter& rExistingParameter = m_float1Parameters[ parameterIndex ];
                    if( rExistingParameter.name == constantName )
                    {
                        newValue.SetElement( 0, rExistingParameter.value );

                        break;
                    }
                }

                if( parameterIndex >= existingFloat1Count )
                {
                    for( parameterIndex = 0; parameterIndex < existingFloat2Count; ++parameterIndex )
                    {
                        const Float2Parameter& rExistingParameter = m_float2Parameters[ parameterIndex ];
                        if( rExistingParameter.name == constantName )
                        {
                            newValue.SetElement( 0, rExistingParameter.value.GetX() );
                            newValue.SetElement( 1, rExistingParameter.value.GetY() );

                            break;
                        }
                    }

                    if( parameterIndex >= existingFloat2Count )
                    {
                        for( parameterIndex = 0; parameterIndex < existingFloat3Count; ++parameterIndex )
                        {
                            const Float3Parameter& rExistingParameter = m_float3Parameters[ parameterIndex ];
                            if( rExistingParameter.name == constantName )
                            {
                                newValue.SetElement( 0, rExistingParameter.value.GetElement( 0 ) );
                                newValue.SetElement( 1, rExistingParameter.value.GetElement( 1 ) );
                                newValue.SetElement( 2, rExistingParameter.value.GetElement( 2 ) );

                                break;
                            }
                        }

                        if( parameterIndex >= existingFloat3Count )
                        {
                            for( parameterIndex = 0; parameterIndex < existingFloat4Count; ++parameterIndex )
                            {
                                const Float4Parameter& rExistingParameter =
                                    m_float4Parameters[ parameterIndex ];
                                if( rExistingParameter.name == constantName )
                                {
                                    newValue = rExistingParameter.value;

                                    break;
                                }
                            }
                        }
                    }
                }

                if( constantSize < sizeof( float32_t ) * 2 )
                {
                    Float1Parameter* pParameter = newFloat1Parameters.New();
                    HELIUM_ASSERT( pParameter );
                    pParameter->name = constantName;
                    pParameter->value = newValue.GetElement( 0 );
                }
                else if( constantSize < sizeof( float32_t ) * 3 )
                {
                    Float2Parameter* pParameter = newFloat2Parameters.New();
                    HELIUM_ASSERT( pParameter );
                    pParameter->name = constantName;
                    pParameter->value = Simd::Vector2( newValue.GetElement( 0 ), newValue.GetElement( 1 ) );
                }
                else if( constantSize < sizeof( float32_t ) * 4 )
                {
                    Float3Parameter* pParameter = newFloat3Parameters.New();
                    HELIUM_ASSERT( pParameter );
                    pParameter->name = constantName;
                    pParameter->value =
                        Simd::Vector3( newValue.GetElement( 0 ), newValue.GetElement( 1 ), newValue.GetElement( 2 ) );
                }
                else
                {
                    Float4Parameter* pParameter = newFloat4Parameters.New();
                    HELIUM_ASSERT( pParameter );
                    pParameter->name = constantName;
                    pParameter->value = newValue;
                }
            }
        }
    }

    newFloat1Parameters.Trim();
    newFloat2Parameters.Trim();
    newFloat3Parameters.Trim();
    newFloat4Parameters.Trim();
    m_float1Parameters.Swap( newFloat1Parameters );
    m_float2Parameters.Swap( newFloat2Parameters );
    m_float3Parameters.Swap( newFloat3Parameters );
    m_float4Parameters.Swap( newFloat4Parameters );
    newFloat1Parameters.Clear();
    newFloat2Parameters.Clear();
    newFloat3Parameters.Clear();
    newFloat4Parameters.Clear();

    // Synchronize texture parameters.
    size_t existingTextureCount = m_textureParameters.GetSize();

    DynArray< TextureParameter > newTextureParameters;
    for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_shaderVariants ); ++shaderTypeIndex )
    {
        ShaderVariant* pShaderVariant = m_shaderVariants[ shaderTypeIndex ];
        if( !pShaderVariant )
        {
            continue;
        }

        const ShaderTextureInfoSet* pTextureSet = pShaderVariant->GetTextureInfoSet( 0 );
        if( !pTextureSet )
        {
            continue;
        }

        bool bCheckDuplicates = !newTextureParameters.IsEmpty();

        const DynArray< ShaderTextureInfo >& rTextureInputs = pTextureSet->inputs;
        size_t textureInputCount = rTextureInputs.GetSize();
        for( size_t textureIndex = 0; textureIndex < textureInputCount; ++textureIndex )
        {
            const ShaderTextureInfo& rTextureInfo = rTextureInputs[ textureIndex ];

            // Ignore textures prefixed with an underscore, as they are reserved for system use.
            Name textureInputName = rTextureInfo.name;
            if( !textureInputName.IsEmpty() && ( *textureInputName )[ 0 ] == TXT( '_' ) )
            {
                continue;
            }

            size_t parameterIndex;
            if( bCheckDuplicates )
            {
                size_t textureParameterCount = newTextureParameters.GetSize();
                for( parameterIndex = 0; parameterIndex < textureParameterCount; ++parameterIndex )
                {
                    if( newTextureParameters[ parameterIndex ].name == textureInputName )
                    {
                        break;
                    }
                }

                if( parameterIndex < textureParameterCount )
                {
                    continue;
                }
            }

            TextureParameter* pParameter = newTextureParameters.New();
            HELIUM_ASSERT( pParameter );
            pParameter->name = textureInputName;

            for( parameterIndex = 0; parameterIndex < existingTextureCount; ++parameterIndex )
            {
                const TextureParameter& rTextureParameter = m_textureParameters[ parameterIndex ];
                if( rTextureParameter.name == textureInputName )
                {
                    pParameter->value = rTextureParameter.value;

                    break;
                }
            }
        }
    }

    newTextureParameters.Trim();
    m_textureParameters.Swap( newTextureParameters );
    newTextureParameters.Clear();
}
#endif  // L_EDITOR

/// Serialize this struct.
///
/// @param[in] s  Serializer with which to serialize.
void Material::Float1Parameter::Serialize( Serializer& s )
{
    s << L_TAGGED( name );

    // Serialize the value as a struct with only an "x" component to mimic the serialization layout of the vector
    // float parameter structs.
    s << Serializer::Tag( TXT( "value" ) );
    s.BeginStruct();
    s << Serializer::Tag( TXT( "x" ) ) << value;
    s.EndStruct();
}

/// Serialize this struct.
///
/// @param[in] s  Serializer with which to serialize.
void Material::Float2Parameter::Serialize( Serializer& s )
{
    s << L_TAGGED( name );
    s << L_TAGGED( value );
}

/// Serialize this struct.
///
/// @param[in] s  Serializer with which to serialize.
void Material::Float3Parameter::Serialize( Serializer& s )
{
    s << L_TAGGED( name );
    s << L_TAGGED( value );
}

/// Serialize this struct.
///
/// @param[in] s  Serializer with which to serialize.
void Material::Float4Parameter::Serialize( Serializer& s )
{
    s << L_TAGGED( name );
    s << L_TAGGED( value );
}

/// Serialize this struct.
///
/// @param[in] s  Serializer with which to serialize.
void Material::TextureParameter::Serialize( Serializer& s )
{
    s << L_TAGGED( name );
    s << L_TAGGED( value );
}
