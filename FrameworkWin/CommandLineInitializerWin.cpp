//----------------------------------------------------------------------------------------------------------------------
// CommandLineInitializationWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkWinPch.h"
#include "FrameworkWin/CommandLineInitializationWin.h"

#include "Platform/Windows/Windows.h"
#include "Foundation/StringConverter.h"

#include <ShellAPI.h>

using namespace Lunar;

/// @copydoc CommandLineInitializer::Initialize()
bool CommandLineInitializationWin::Initialize( String& rModuleName, DynArray< String >& rArguments )
{
    rModuleName.Clear();
    rArguments.Clear();

    // Get the module file name and full command-line string (must use the wide-character versions, as there is only
    // a CommandLineToArgvW() and not a CommandLineToArgvA() to parse the command-line string).
    LPWSTR pCommandLine = GetCommandLineW();
    HELIUM_ASSERT( pCommandLine );
    if( !pCommandLine )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "CommandLineInitializationWin::Initialize(): Failed to retrieve command-line string.\n" ) );

        return false;
    }

    WCHAR moduleFileName[ MAX_PATH ];
    DWORD moduleFileNameLength = GetModuleFileNameW( NULL, moduleFileName, MAX_PATH );
    if( moduleFileNameLength == 0 )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "CommandLineInitializationWin::Initialize(): Failed to retrieve module file name string.\n" ) );

        return false;
    }

    moduleFileName[ MAX_PATH - 1 ] = L'\0';
    moduleFileNameLength = Min< DWORD >( moduleFileNameLength, MAX_PATH - 1 );

    // If the command line contains just the module file name, don't parse it and simply store the module name
    // (if no command-line arguments are specified, GetCommandLine() may return a string containing just the module
    // name without quoting it, which would in turn cause CommandLineToArgvW() to split the module path if it
    // contains any spaces).
    if( StringCompare( pCommandLine, moduleFileName ) == 0 )
    {
        StringConverter< wchar_t, tchar_t >::Convert( rModuleName, moduleFileName );

        return true;
    }

    // Parse the command-line string.
    int argumentCount = 0;
    LPWSTR* ppArguments = CommandLineToArgvW( pCommandLine, &argumentCount );
    HELIUM_ASSERT( ppArguments );
    if( !ppArguments )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "CommandLineInitializationWin::Initialize(): Failed to parse command-line arguments.\n" ) );

        return false;
    }

    StringConverter< wchar_t, tchar_t >::Convert( rModuleName, moduleFileName );

    if( argumentCount > 1 )
    {
        rArguments.Reserve( argumentCount - 1 );

        for( int argumentIndex = 1; argumentIndex < argumentCount; ++argumentIndex )
        {
            LPWSTR pArgument = ppArguments[ argumentIndex ];
            HELIUM_ASSERT( pArgument );
            String* pConvertedArgument = rArguments.New();
            HELIUM_ASSERT( pConvertedArgument );
            StringConverter< wchar_t, tchar_t >::Convert( *pConvertedArgument, pArgument );
        }
    }

    LocalFree( ppArguments );

    return true;
}
