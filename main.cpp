#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <direct.h>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <fstream>
#include <filesystem>
#include <shlobj.h>
#include <json.hpp>

using namespace std;

using json = nlohmann::json;

#include "fastdl.h"
#include "commands.h"
#include "config.h"
#include "globs.h"

bool shouldExit = false;
bool shouldRestart = true;
string gamePath;
bool verbose = false;

bool quitFunc = false;

const char* ip = "79.127.217.197";
const char* mainPort = "22912";
const char* sourceTV = "22913";
const uint16_t mainPortInt = 22912;
const uint16_t sourceTVInt = 22913;

const int verMajor = 0;
const int verMinor = 4;

char* appDataPath;
char* appDataPathRaw;
const char* configFile = "prefs.json";

bool parse( const char* cmd, const char* args )
{
    if ( strcmp( cmd, "cls" ) == 0 )
    {
        system("cls");
        return true;
    }
    else if ( strcmp( cmd, "help" ) == 0 )
    {
        CgeInterface::help();
        return true;
    }
    else if ( strcmp( cmd, "exit" ) == 0 || strcmp( cmd, "quit" ) == 0 )
    {
        shouldExit = true;
        return true;
    }
    else if ( strcmp( cmd, "reset" ) == 0 || strcmp( cmd, "restart" ) == 0 )
    {
        shouldRestart = true;
        return true;
    }
    else if ( strcmp( cmd, "info" ) == 0 )
    {
        CgeInterface::info();
        return true;
    }
    else if ( strcmp( cmd, "fastdl" ) == 0 )
    {
        FastDL::fastdl( args, false, false );
        return true;
    }
    else if ( strcmp( cmd, "view" ) == 0 )
    {
        if ( strcmp( args, "full" ) == 0 )
            FastDL::fastdl_macro_view();
        else if ( strcmp( args, "" ) == 0 || strcmp( args, " " ) == 0 || strcmp( args, "min" ) == 0 )
            FastDL::fastdl_macro_view_min();
        else
            cerr << "fastdl -> macro (view) -> Invalid Syntax!" << endl;
            
        return true;
    }
    else if ( strcmp( cmd, "scrape" ) == 0 )
    {
        if ( strcmp( args, "full" ) == 0 )
            FastDL::fastdl_macro_scrape();
        else if ( strcmp( args, "" ) == 0 || strcmp( args, " " ) == 0 || strcmp( args, "min" ) == 0 )
            FastDL::fastdl_macro_scrape_min();
        else
            cerr << "fastdl -> macro (scrape) -> Invalid Syntax!" << endl;
            
        return true;
    }
    else if ( strcmp( cmd, "gamepath" ) == 0 )
    {
        gamePath.clear();

        while( !filesystem::is_directory( gamePath ) )
        {
            cout << "Please input a valid path to your TF2 Install folder (without quotes)" << endl << ">> ";

            gamePath.clear();

            getline( cin, gamePath );
        }

        ConfigInterface::UpdateConfStr( "gamepath", gamePath );
        chdir( gamePath.c_str() );
            
        return true;
    }
    else if ( strcmp( cmd, "verbose" ) == 0 )
    {
        verbose = !verbose;
        cout << ( verbose ? "Verbose Logging is now enabled." : "Verbose Logging is now disabled" ) << endl;
        ConfigInterface::UpdateConfBool( "verbose", verbose );
        return true;
    }
    else
    {
        return false;
    }
}

void multitool()
{
    cout << "Type \"help\" for a list of valid commands." << endl;

    while( !shouldExit && !shouldRestart )
    {
        cout << endl << ">> ";

        string inputstream = "temp";
        string second = "temp";

        inputstream.clear();
        second.clear();

        getline( cin, inputstream );

        string::size_type pos = inputstream.find( ' ', 0 );
        if ( pos != string::npos )
        {
            second = inputstream.substr( pos+1 );
            inputstream = inputstream.substr( 0, pos );
        }

        bool response = parse( inputstream.c_str(), second.c_str() );
        if ( !response ) cerr << "Invalid command: \"" << inputstream.c_str() << "\"" << endl;
    }
}

int main( int argc, char *argv[] )
{
    appDataPath = new char[MAX_PATH];

    while ( shouldRestart )
    {
        shouldRestart = false;

        cout << "Registering Filepath...";

        PWSTR *tmp = new wchar_t*;

        SHGetKnownFolderPath( FOLDERID_RoamingAppData, 0, NULL, tmp );
        wcstombs( appDataPath, *tmp, sizeof( char ) * MAX_PATH );

        delete tmp;

        if ( !ConfigInterface::Init() )
        {
            cout << " [ \033[31mFAILED\033[0m ]" << endl;

            while( !filesystem::is_directory( gamePath ) )
            {
                cout << "Please input a valid path to your TF2 Install folder (without quotes)" << endl << ">> ";

                gamePath.clear();

                getline( cin, gamePath );
            }

            ConfigInterface::UpdateConfStr( "gamepath", gamePath );
        }
        else
        {
            gamePath = ConfigInterface::GetConfStr( "gamepath" );

            cout << gamePath << endl;

            if ( !filesystem::is_directory( gamePath ) )
            {
                cout << " [ \033[31mFAILED\033[0m ]" << endl;

                while( !filesystem::is_directory( gamePath ) )
                {
                    cout << "Please input a valid path to your TF2 Install folder (without quotes)" << endl << ">> ";

                    gamePath.clear();

                    getline( cin, gamePath );
                }
            }
            else
            {
                cout << " [ \033[34mDONE\033[0m ] " << endl;
            }

            verbose = ConfigInterface::GetConfBool( "verbose" );
        }

        chdir( gamePath.c_str() );

        cout << "Starting multitool..." << endl;
        _sleep( 500 );

        system("cls");
        cout << "Welcome to the \"cge7-193\" Multitool! v" << verMajor << "." << verMinor << endl << "   (c) 2025 funniman.exe" << endl << endl;

        multitool();
    }

    cout << "\033[0m";

    return 0;
}