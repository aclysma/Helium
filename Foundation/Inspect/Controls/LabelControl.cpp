#include "Foundation/Inspect/Controls/LabelControl.h"
#include "Foundation/Inspect/Container.h"
#include "Foundation/Inspect/DataBinding.h"

using namespace Helium;
using namespace Helium::Inspect;

REFLECT_DEFINE_OBJECT( Inspect::Label );

Label::Label()
: a_Ellipsize( true )
{
    a_ProportionalWidth.Set( 1.f/3.f );
}

bool Label::Process(const tstring& key, const tstring& value)
{
    bool handled = false;

    if ( Base::Process(key, value) )
    {
        return true;
    }

    if ( key == LABEL_ATTR_TEXT )
    {
        Bind( new StringFormatter<tstring>( new tstring( value ), true ) );
        return true;
    }

    return false;
}
