#include "Precompile.h"
#include "FilteredDropTarget.h"

#include "Foundation/String/Wildcard.h"
#include "Foundation/String/Tokenize.h"
#include "Editor/Clipboard/ClipboardDataObject.h"
#include "Editor/Clipboard/ClipboardFileList.h"

using namespace Helium;
using namespace Helium::Editor;

FilteredDropTarget::FilteredDropTarget( const tstring& filter ) 
: DropTarget()
, m_FileFilter( filter )
{
    SetDragOverCallback( DragOverCallback::Delegate( this, &FilteredDropTarget::DragOver ) );
    SetDropCallback( DropCallback::Delegate( this, &FilteredDropTarget::Drop ) );
}

FilteredDropTarget::~FilteredDropTarget()
{
}

bool FilteredDropTarget::ValidateDrag( const Editor::DragArgs& args )
{
    ClipboardFileListPtr fileList = Reflect::SafeCast< ClipboardFileList >( args.m_ClipboardData->FromBuffer() );
    if ( !fileList )
    {
        return false;
    }

    if ( !m_FileFilter.empty() )
    {
        return true;
    }

    std::vector< tstring > extensions;
    Tokenize( m_FileFilter, extensions, TXT( ";" ) );
    if ( extensions.empty() )
    {
        return true;
    }

    for ( std::set< tstring >::const_iterator fileItr = fileList->GetFilePaths().begin(), fileEnd = fileList->GetFilePaths().end(); fileItr != fileEnd; ++fileItr )
    {
        Helium::Path path( *fileItr );
        if ( path.Get().empty() )
        {
            continue;
        }

        if ( !path.Exists() )
        {
            continue;
        }

        for ( std::vector< tstring >::const_iterator itr = extensions.begin(), end = extensions.end(); itr != end; ++itr )
        {
            const tstring& extension = *itr;
            if ( WildcardMatch( extension.c_str(), path.c_str() ) )
            {
                return true;
            }
        }
    }

    return false;
}

void FilteredDropTarget::DragOver( const Editor::DragArgs& args )
{
    if ( ValidateDrag( args ) )
    {
        args.m_Result = args.m_Default;
    }
    else
    {
        args.m_Result = wxDragNone;
    }
}

void FilteredDropTarget::Drop( const Editor::DragArgs& args )
{
    std::vector< tstring > validPaths;

    if ( !ValidateDrag( args ) )
    {
        args.m_Result = args.m_Default;
        return;
    }

    ClipboardFileListPtr fileList = Reflect::SafeCast< ClipboardFileList >( args.m_ClipboardData->FromBuffer() );
    if ( !fileList )
    {
        args.m_Result = args.m_Default;
        return;
    }

    std::vector< tstring > extensions;
    if ( !m_FileFilter.empty() )
    {
        Tokenize( m_FileFilter, extensions, TXT( ";" ) );
    }

    for ( std::set< tstring >::const_iterator fileItr = fileList->GetFilePaths().begin(), fileEnd = fileList->GetFilePaths().end(); fileItr != fileEnd; ++fileItr )
    {
        Helium::Path path( *fileItr );
        if ( path.Get().empty() )
        {
            continue;
        }

        if ( !path.Exists() )
        {
            continue;
        }

        if ( extensions.empty() )
        {
            validPaths.push_back( path.Get() );
        }
        else
        {
            for ( std::vector< tstring >::const_iterator itr = extensions.begin(), end = extensions.end(); itr != end; ++itr )
            {
                const tstring& extension = *itr;
                if ( WildcardMatch( extension.c_str(), path.c_str() ) )
                {
                    validPaths.push_back( path );
                    break;
                }
            }
        }
    }

    if ( validPaths.size() )
    {
        m_Dropped.Raise( FilteredDropTargetArgs( validPaths ) );
    }

    args.m_Result = args.m_Default;
}

void FilteredDropTarget::AddDroppedListener( const FilteredDropTargetSignature::Delegate& d )
{
    m_Dropped.Add( d );
}

void FilteredDropTarget::RemoveDroppedListener( const FilteredDropTargetSignature::Delegate& d )
{
    m_Dropped.Remove( d );
}
