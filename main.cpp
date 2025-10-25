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

int forcedProfile = 0;
bool shouldExit = false;
bool shouldRestart = true;
string gamePath;
bool verbose = false;

bool quitFunc = false;

const char* defaultIp = "169.150.249.133";
const char* defaultFastDL = "https://wavespray.dathost.net/fastdl/teamfortress2/679d9656b8573d37aa848d60/";
const uint16_t defaultMainPortInt = 22912;
const uint16_t defaultSourceTVInt = 22913;

const int verMajor = 1;
const int verMinor = 0;

char* appDataPath;
const char* configFile = "prefs.json";
const char* profFile = "profiles.json";

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
    else if ( strcmp( cmd, "current-map" ) == 0 || strcmp( cmd, "map" ) == 0 )
    {
        CgeInterface::pullCurrentMap();
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
        while( !filesystem::is_directory( gamePath ) )
        {
            cout << "Please input a valid path to your " << CurrentProfile::game << " Install folder (without quotes)" << endl << ">> ";

            gamePath.clear();

            getline( cin, gamePath );
        }

        ConfigInterface::UpdateConfStr( CurrentProfile::game, gamePath );
            
        return true;
    }
    else if ( strcmp( cmd, "profile" ) == 0 )
    {
        if ( strcmp( args, "create" ) == 0 )
            ProfileInterface::CreateProfile();
        else if ( strcmp( args, "edit" ) == 0 )
            ProfileInterface::EditProfile();
        else if ( strcmp( args, "delete" ) == 0 )
            ProfileInterface::DeleteProfile();
        else if ( strcmp( args, "" ) == 0 || strcmp( args, " " ) == 0 || strcmp( args, "list" ) == 0 )
            ProfileInterface::ListProfiles();
        else
            cerr << "profile -> Invalid Syntax!" << endl;
            
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
//    cout << "The current configuration is \"" << currentConfig << "\"" << endl;
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
    appDataPath = new char[ MAX_PATH ];
    appPath = argv[ 0 ];

    appPath = appPath.substr( 0, appPath.find_last_of( "\\" ) ) + "\\";

    bool resetSettings = false;
    bool resetProfiles = false;

    for ( int i = 1; i < argc; i++ )
    {
        if ( ( strcmp( argv[ i ], "-rs" ) == 0 ) || ( strcmp( argv[ i ], "--reset-settings" ) == 0 ) )
            resetSettings = true;

        if ( ( strcmp( argv[ i ], "-rp" ) == 0 ) || ( strcmp( argv[ i ], "--reset-profiles" ) == 0 ) )
            resetProfiles = true;
    }

    while ( shouldRestart )
    {
        shouldRestart = false;

        cout << "Registering Profiles...";

        if ( !ProfileInterface::Init( resetProfiles ) )
        {
            cout << " [ \033[31mFAILED\033[0m ]" << endl;
            cout << "Profile configuration file could not be found! A new one has been generated." << endl;
            ProfileInterface::LoadProfile( 1 );
        }
        else
        {
            cout << " [ \033[34mDONE\033[0m ] " << endl;

            if ( forcedProfile )
            {
                ProfileInterface::LoadProfile( forcedProfile );
                forcedProfile = 0;
            }
            else if ( ProfileInterface::GetTotalProfiles() <= 1 )
                ProfileInterface::LoadProfile( 1 );
            else
            {
                bool invalidSelection = true;

                cout << "Select a Profile" << endl << endl;
                while ( invalidSelection )
                {
                    for ( int i = 1; i <= ProfileInterface::GetTotalProfiles(); i++ )
                    {
                        cout << "[" << i << "] " << ProfileInterface::GetConfStrNested( i, "profileName" );
            
                        if ( ProfileInterface::GetConfStrNested( i, "profileSeries" ) != "" && ProfileInterface::GetConfStrNested( i, "profileSeries" ) != " " )
                            cout << " (" << ProfileInterface::GetConfStrNested( i, "profileSeries" ) << ")";

                        cout << endl;
                    }

                    cout << endl << ">> ";

                    string inputstream = "temp";
                    inputstream.clear();

                    getline( cin, inputstream );

                    int selection = atoi( inputstream.c_str() );

                    if ( selection > 0 && selection <= ProfileInterface::GetTotalProfiles() )
                    {
                        ProfileInterface::LoadProfile( selection );
                        invalidSelection = false;
                    }
                    else
                    {
                        cerr << "\033[31mInvalid selection!\033[0m Please only enter a number from 1 to " << ProfileInterface::GetTotalProfiles() << "." << endl;
                        system( "cls" );
                    }
                }
            }
        }

        cout << "Registering Filepath...";

        PWSTR *tmp = new wchar_t*;

        SHGetKnownFolderPath( FOLDERID_RoamingAppData, 0, NULL, tmp );
        wcstombs( appDataPath, *tmp, sizeof( char ) * MAX_PATH );

        delete tmp;

        if ( !ConfigInterface::Init( resetSettings ) )
        {
            cout << " [ \033[31mFAILED\033[0m ]" << endl;

            while( !filesystem::is_directory( gamePath ) )
            {
                cout << "Please input a valid path to your " << CurrentProfile::game << " Install folder (without quotes)" << endl << ">> ";

                gamePath.clear();

                getline( cin, gamePath );
            }

            ConfigInterface::UpdateConfStr( CurrentProfile::game, gamePath );
        }
        else
        {
            gamePath = ConfigInterface::GetConfStr( CurrentProfile::game );

            //cout << gamePath << endl;

            if ( !filesystem::is_directory( gamePath ) )
            {
                cout << " [ \033[31mFAILED\033[0m ]" << endl;

                while( !filesystem::is_directory( gamePath ) )
                {
                    cout << "Please input a valid path to your " << CurrentProfile::game << " Install folder (without quotes)" << endl << ">> ";

                    gamePath.clear();

                    getline( cin, gamePath );
                }

                ConfigInterface::UpdateConfStr( CurrentProfile::game, gamePath );
            }
            else
            {
                cout << " [ \033[34mDONE\033[0m ] " << endl;
            }

            verbose = ConfigInterface::GetConfBool( "verbose" );
        }

        cout << "Starting multitool..." << endl;
        //_sleep( 500 );

        system( "cls" );
        cout << "Welcome to the \"cge7-193\" Multitool! v" << verMajor << "." << verMinor << endl;
        cout << "DEDICATED TO INTERLOPER -- OCTOBER 24TH 2025" << endl;
        cout << "   (c) 2025 funniman.exe" << endl << endl;

        multitool();

        if ( shouldRestart ) system( "cls" );
    }

    cout << "\033[0m";

    return 0;
}