#include "Precompile.h"
#include "ProfileDumpCommand.h"

#include "Foundation/Log.h"
#include "Foundation/Profile.h"
#include "Foundation/File/Path.h"

#include <stdio.h>
#include <stdlib.h>

using namespace Helium;
using namespace Helium::Profile;
using namespace Helium::Editor;

static uint32_t g_Indent = 0; 

ProfileDumpCommand::ProfileDumpCommand()
: Command( TXT( "profile-dump" ), TXT( "[<INPUT>]" ), TXT( "Dump text information about a profile data file" ) )
{

}

static void PrintIndent()
{
    for(uint32_t i = 0; i < g_Indent; ++i)
    {
        Log::Print( TXT( " " ) ); 
    }
}

static void ParseAndPrintBlock(char* buffer)
{
    char* currentByte = buffer; 

    while(currentByte < buffer + PROFILE_PACKET_BLOCK_SIZE)
    {
        UberPacket* uberPacket = (UberPacket*) currentByte; 

        uint16_t cmd = uberPacket->m_Header.m_Command; 
        uint16_t size = uberPacket->m_Header.m_Size; 

        const char* packet_names[] = { "INIT       ", 
            "SCOPE_ENTER", 
            "SCOPE_EXIT ", 
            "BLOCK_END  " }; 

        if(cmd <= PROFILE_CMD_BLOCK_END)
        {
            Log::Print( TXT( "%s:" ), packet_names[cmd]); 
        }
        else
        {
            Log::Error( TXT( "UNKNOWN PROFILE TAG: 0x%.2x\n" ), cmd); 
            exit(1); 
        }

        switch(cmd)
        {
        case PROFILE_CMD_SCOPE_ENTER: 
            g_Indent++; 
            PrintIndent(); 
            Log::Print( TXT( "%s %d\n" ), 
                uberPacket->m_ScopeEnter.m_Function, 
                uberPacket->m_ScopeEnter.m_Line); 

            break; 
        case PROFILE_CMD_SCOPE_EXIT:
            PrintIndent(); 
            Log::Print( TXT( "%lld\n" ), uberPacket->m_ScopeExitPacket.m_Duration); 
            g_Indent--; 
            break; 

        default:
            Log::Print( TXT( "\n" ) );
        }

        if(cmd == PROFILE_CMD_BLOCK_END)
            break; 

        currentByte += size; 
    }
}

bool ProfileDumpCommand::Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error )
{
    tstring fileArg;

    if ( argsBegin != argsEnd )
    {
        const tstring& arg = (*argsBegin);
        ++argsBegin;

        if ( arg.length() )
        {
            fileArg = arg;
        }
    }

    const tchar_t* filename = fileArg.c_str(); 

    FILE* file = _tfopen(filename, TXT( "rb" ) );
    if(!file)
    {
        Log::Print( TXT( "Unable to open %s for reading!\n" ), filename); 
        return false;
    }

    fseek(file, 0, SEEK_END); 
    long filesize = ftell(file); 
    fseek(file, 0, SEEK_SET); 

    uint32_t blockCount = filesize / PROFILE_PACKET_BLOCK_SIZE; 

    if( filesize % PROFILE_PACKET_BLOCK_SIZE != 0)
    {
        Log::Print( TXT( "File %s is of the wrong size (not a multiple of PROFILE_PACKET_BLOCK_SIZE %d)\n" ), 
            filename, PROFILE_PACKET_BLOCK_SIZE); 
    }
    else
    {
        Log::Print( TXT( "File %s has %d blocks\n" ), filename, blockCount); 
    }

    char* buffer = new char[PROFILE_PACKET_BLOCK_SIZE]; 

    for(uint32_t i = 0; i < blockCount; ++i)
    {
        size_t bytesRead = fread(buffer, 1, PROFILE_PACKET_BLOCK_SIZE, file); 

        if(bytesRead != PROFILE_PACKET_BLOCK_SIZE)
        {
            Log::Print( TXT( "Error reading block %d from file\n" ), i); 
            exit(1); 
        }

        Log::Print( TXT( "Block %d\n" ), i); 
        ParseAndPrintBlock(buffer); 
    }

    delete[] buffer; 

    fclose(file); 
    return true;
}
