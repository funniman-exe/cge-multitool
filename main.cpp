#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>

#ifdef _WIN32
    #include <direct.h>
    #include <winsock2.h>
    #include <windows.h>
    #include <shlobj.h>
#endif /* _WIN32 */

#include <fstream>
#include <filesystem>
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
bool shouldSwitchProfiles = false;

string gamePath;

bool verbose = false;
int defaultProfile = 1;
bool useProfileSelector = true;

bool quitFunc = false;

const char* defaultIp = "169.150.249.133";
const char* defaultFastDL = "https://wavespray.dathost.net/fastdl/teamfortress2/679d9656b8573d37aa848d60/";
const uint16_t defaultMainPortInt = 22912;
const uint16_t defaultSourceTVInt = 22913;

const int verMajor = 1;
const int verMinor = 2;

//char* appDataPath;
const char* configFile = "prefs.json";
const char* profFile = "profiles.json";

void clrScr()
{
#ifdef _WIN32
    system( "cls" );
#else
    system( "clear" );
#endif /* _WIN32 */
}

bool parse( const char* cmd, const char* args )
{
	if ( strcmp( cmd, "cls" ) == 0 )
		clrScr();
	else if ( strcmp( cmd, "help" ) == 0 )
		CgeInterface::help();
	else if ( strcmp( cmd, "credits" ) == 0 )
		CgeInterface::credits();
	else if ( strcmp( cmd, "exit" ) == 0 || strcmp( cmd, "quit" ) == 0 )
		shouldExit = true;
	else if ( strcmp( cmd, "reset" ) == 0 || strcmp( cmd, "restart" ) == 0 )
		shouldRestart = true;
	else if ( strcmp( cmd, "info" ) == 0 )
		CgeInterface::info();
	else if ( strcmp( cmd, "fastdl" ) == 0 )
	{
		if ( CurrentProfile::fastDL == "" )
		{
			cerr << "The current profile has no fastdl set, so you cannot use this command." << endl;
			return true;
		}

		FastDL::fastdl( args, false, false );
	}
	else if ( strcmp( cmd, "current-map" ) == 0 || strcmp( cmd, "map" ) == 0 )
	{
		if ( CurrentProfile::fastDL == "" )
		{
			cerr << "The current profile has no fastdl set, so you cannot use this command." << endl;
			return true;
		}

		CgeInterface::pullCurrentMap();
	}
	else if ( strcmp( cmd, "view" ) == 0 )
	{
		if ( CurrentProfile::name != "cge7-193" )
		{
			cerr << "This command is only supported when using the \"cge7-193\" profile." << endl;
			return true;
		}

		if ( CurrentProfile::fastDL == "" )
		{
			cerr << "The current profile has no fastdl set, so you cannot use this command." << endl;
			return true;
		}

		if ( strcmp( args, "full" ) == 0 )
			FastDL::fastdl_macro_view();
		else if ( strcmp( args, "" ) == 0 || strcmp( args, " " ) == 0 || strcmp( args, "min" ) == 0 )
			FastDL::fastdl_macro_view_min();
		else
			cerr << "fastdl -> macro (view) -> Invalid Syntax!" << endl;
	}
	else if ( strcmp( cmd, "scrape" ) == 0 )
	{
		if ( CurrentProfile::name != "cge7-193" )
		{
			cerr << "This command is only supported when using the \"cge7-193\" profile." << endl;
			return true;
		}

		if ( CurrentProfile::fastDL == "" )
		{
			cerr << "The current profile has no fastdl set, so you cannot use this command." << endl;
			return true;
		}

		if ( strcmp( args, "full" ) == 0 )
			FastDL::fastdl_macro_scrape();
		else if ( strcmp( args, "" ) == 0 || strcmp( args, " " ) == 0 || strcmp( args, "min" ) == 0 )
			FastDL::fastdl_macro_scrape_min();
		else
			cerr << "fastdl -> macro (scrape) -> Invalid Syntax!" << endl;
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
	}
	else if ( strcmp( cmd, "profile" ) == 0 )
	{
		if ( strcmp( args, "create" ) == 0 )
			ProfileInterface::CreateProfile();
		else if ( strcmp( args, "edit" ) == 0 )
			ProfileInterface::EditProfile();
		else if ( strcmp( args, "delete" ) == 0 )
			ProfileInterface::DeleteProfile();
        else if ( strcmp( args, "switch" ) == 0 )
        {
            if ( ProfileInterface::GetTotalProfiles() <= 1 )
            {
                cout << "You need multiple profiles to launch the profile selector." << endl;
                return true;
            }

            shouldRestart = true;
            shouldSwitchProfiles = true;
        }
		else if ( strcmp( args, "" ) == 0 || strcmp( args, " " ) == 0 || strcmp( args, "list" ) == 0 )
			ProfileInterface::ListProfiles();
		else
			cerr << "profile -> Invalid Syntax!" << endl;
	}
    else if ( strcmp( cmd, "default" ) == 0 )
    {
        defaultProfile = CurrentProfile::id;
        cout << "The default profile is now " << "[" << defaultProfile << "] " << ProfileInterface::GetConfStrNested( defaultProfile, "profileName" );

        if ( ProfileInterface::GetConfStrNested( defaultProfile, "profileSeries" ) != "" && ProfileInterface::GetConfStrNested( defaultProfile, "profileSeries" ) != " " )
                            cout << " (" << ProfileInterface::GetConfStrNested( defaultProfile, "profileSeries" ) << ")";
        cout << endl;
        ConfigInterface::UpdateConfInt( "defaultprofile", defaultProfile );
    }
    else if ( strcmp( cmd, "selector" ) == 0 )
	{
		useProfileSelector = !useProfileSelector;
		cout << ( useProfileSelector ? "The profile selector will now launch on startup if multiple profiles are present." : "The profile selector will no longer launch on startup." ) << endl;
		ConfigInterface::UpdateConfBool( "profileselector", useProfileSelector );
	}
	else if ( strcmp( cmd, "verbose" ) == 0 )
	{
		verbose = !verbose;
		cout << ( verbose ? "Verbose Logging is now enabled." : "Verbose Logging is now disabled." ) << endl;
		ConfigInterface::UpdateConfBool( "verbose", verbose );
	}
	else
		return false;

	return true;
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

        getline( cin >> ws, inputstream );

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
 //   appDataPath = new char[ MAX_PATH ];
    appPath = argv[ 0 ];

#ifdef _WIN32
    appPath = appPath.substr( 0, appPath.find_last_of( "\\" ) ) + "\\";
#else
    appPath = appPath.substr( 0, appPath.find_last_of( "/" ) ) + "/";
#endif /* _WIN32 */

    bool resetSettings = false;
    bool resetProfiles = false;

    for ( int i = 1; i < argc; i++ )
    {
        if ( ( strcmp( argv[ i ], "-rs" ) == 0 ) || ( strcmp( argv[ i ], "--reset-settings" ) == 0 ) )
            resetSettings = true;

        if ( ( strcmp( argv[ i ], "-rp" ) == 0 ) || ( strcmp( argv[ i ], "--reset-profiles" ) == 0 ) )
            resetProfiles = true;
        
        if ( ( strcmp( argv[ i ], "-fp" ) == 0 ) || ( strcmp( argv[ i ], "--forced-profile" ) == 0 ) )
            forcedProfile = atoi( argv[ ++i ] );
    }

    while ( shouldRestart )
    {
        shouldRestart = false;

        cout << "Registering Configs...";

 //       PWSTR *tmp = new wchar_t*;

 //       SHGetKnownFolderPath( FOLDERID_RoamingAppData, 0, NULL, tmp );
 //       wcstombs( appDataPath, *tmp, sizeof( char ) * MAX_PATH );

 //       delete tmp;

        bool validInstallPath = ConfigInterface::Init( resetSettings );

        if ( !validInstallPath )
        {
            cout << " [ \033[31mFAILED\033[0m ]" << endl;
            cout << "Prefrences file could not be found! A new one has been generated." << endl;
        }
        else
        {
            cout << " [ \033[34mDONE\033[0m ] " << endl;
        }

        verbose = ConfigInterface::GetConfBool( "verbose" );
        defaultProfile = ConfigInterface::GetConfInt( "defaultprofile" );
        useProfileSelector = ConfigInterface::GetConfBool( "profileselector" );

        cout << "Registering Profiles...";

        if ( !ProfileInterface::Init( resetProfiles ) )
        {
            cout << " [ \033[31mFAILED\033[0m ]" << endl;

			if ( resetProfiles )
				cout << "You have passed the reset-profiles switch." << endl << "  Profiles have been reset to their defaults." << endl;
			else
            	cout << "Profile configuration file could not be found! A new one has been generated." << endl;
			
			cout << "Starting default profile..." << endl;
            ProfileInterface::LoadProfile( 1 );
        }
        else
        {
            cout << " [ \033[34mDONE\033[0m ] " << endl;

            if ( forcedProfile )
            {
				cout << "You have passed the forced-profile switch." << endl << "  Profile #" << forcedProfile << " will now be loaded." << endl;
                ProfileInterface::LoadProfile( forcedProfile );
                forcedProfile = 0;
            }
            else if ( ProfileInterface::GetTotalProfiles() <= 1 )
			{
				cout << "Starting default profile..." << endl;
                ProfileInterface::LoadProfile( 1 );
			}
            else
            {
                bool invalidSelection = true;

                if ( !useProfileSelector && !shouldSwitchProfiles )
                {
                    cout << "Starting default profile...";
                    if ( defaultProfile > 0 && defaultProfile <= ProfileInterface::GetTotalProfiles() )
                    {
                        cout << endl;
                        ProfileInterface::LoadProfile( defaultProfile );
                        invalidSelection = false;
                    }
                    else
                    {
                        cout << " [ \033[31mFAILED\033[0m ]" << endl;
                        cerr << "The Profile ID assigned as the default profile is malformed or does not exist." << endl;
                        defaultProfile = 1;
                        ConfigInterface::UpdateConfInt( "defaultprofile", defaultProfile );
                    }
                }

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
                        clrScr();
                    }
                }
            }
        }

        cout << "Registering Filepath...";
        if ( !validInstallPath )
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
        }

        cout << "Starting multitool..." << endl;
        //sleep( 500 );

        clrScr();
        cout << "Welcome to the \"cge7-193\" Multitool! v" << verMajor << "." << verMinor << endl;
        cout << "DEDICATED TO INTERLOPER -- OCTOBER 24TH 2025" << endl;
        cout << "   (c) 2026 funniman.exe" << endl << endl;

        multitool();

        if ( shouldRestart ) clrScr();
    }

    cout << "\033[0m";

    return 0;
}