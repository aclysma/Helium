#include "Texture.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_ENUMERATION( Asset::TextureFormat );
REFLECT_DEFINE_ENUMERATION( Asset::TextureFilter );
REFLECT_DEFINE_ENUMERATION( Asset::TextureCoordinateWrapMode );
REFLECT_DEFINE_OBJECT( Texture );

void Texture::PopulateComposite( Reflect::Composite& comp )
{
    comp.SetProperty( AssetProperties::FileFilter, TXT( "*.tga;*.png;*.jpg;*.bmp" ) );

    comp.AddEnumerationField( &Texture::m_WrapModeU, TXT( "m_WrapModeU" ) );
    comp.AddEnumerationField( &Texture::m_WrapModeV, TXT( "m_WrapModeV" ) );
}
