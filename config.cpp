#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <direct.h>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <fstream>
#include <cinttypes>
#include <filesystem>
#include <json.hpp>

using namespace std;

using json = nlohmann::json;

#include "config.h"
#include "globs.h"

string appPath;
//int totalProfiles;

namespace ConfigInterface
{
    // JSON
    json conf;
}

namespace CurrentProfile
{
    int id;
    std::string name;
    std::string series;
    std::string game;
    std::string ip;
    uint16_t port;
    std::string fastDL;
}

bool ConfigInterface::Init( bool resetSettings )
{
    strcat( appDataPath, "\\" FUNNIMAN_SOFTWARE "\\" );

    string tmp = appDataPath;

    if ( !filesystem::exists( tmp ) )
        filesystem::create_directory( tmp );

    strcat( appDataPath, APPDATA_PATH "\\" );
    tmp += APPDATA_PATH "\\";

    if ( !filesystem::exists( tmp ) )
        filesystem::create_directory( tmp );

    tmp += configFile;

    if ( filesystem::exists( tmp ) && resetSettings )
        filesystem::remove( tmp );

    ifstream in( tmp.c_str() );
    if ( !in )
    {
        ofstream out( tmp );

        conf[ "verbose" ] = false;
        
        /*for ( int i = 1; i <= ProfileInterface::GetTotalProfiles(); i++ )
        {
            conf[ ProfileInterface::GetConfStrNested( i, "profileGame" ) ] = "TMP";
        }*/

        conf[ CurrentProfile::game ] = "TMP";

        out << setw( 4 ) << conf << endl;
        out.close();

        //cout << "Wrote config file to \"" << tmp << "\"" << endl;

        return false;
    }
    else
    {
        conf = json::parse( in );
        in.close();

        if ( CurrentProfile::game == "Team Fortress 2" )
        {
            if ( ( conf[ CurrentProfile::game ].is_null() || conf[ CurrentProfile::game ] == "TMP" ) && GetConfStr( "gamepath" ) != "" )
                conf[ CurrentProfile::game ] = GetConfStr( "gamepath" );
            else if ( conf[ CurrentProfile::game ].is_null() )
                conf[ CurrentProfile::game ] = "TMP";
        }

        ofstream out( tmp );

        out << setw( 4 ) << conf << endl;
        out.close();
            
        //cout << "Finished parsing config file at \"" << tmp << "\"" << endl;
        return true;
    }
}

int ConfigInterface::GetConfInt( string name )
{
    if ( conf[ name ].is_null() ) return 0;
    return conf[ name ].template get<int>();
}

bool ConfigInterface::GetConfBool( string name )
{
    if ( conf[ name ].is_null() ) return false;
    return conf[ name ].template get<bool>();
}

string ConfigInterface::GetConfStr( string name )
{
    if ( conf[ name ].is_null() ) return "";
    return conf[ name ].template get<string>();
}

void ConfigInterface::UpdateConfInt( string name, int val )
{
    string tmp = appDataPath;
    tmp += configFile;

    ofstream out( tmp );
    conf[name] = val;
    out << setw( 4 ) << conf << endl;
}

void ConfigInterface::UpdateConfBool( string name, bool val )
{
    string tmp = appDataPath;
    tmp += configFile;

    ofstream out( tmp );
    conf[name] = val;
    out << setw( 4 ) << conf << endl;
}

void ConfigInterface::UpdateConfStr( string name, string val )
{
    string tmp = appDataPath;
    tmp += configFile;

    ofstream out( tmp );
    conf[name] = val;
    out << setw( 4 ) << conf << endl;
}

namespace ProfileInterface {
    // JSON
    json prof;
}

bool ProfileInterface::Init( bool resetSettings )
{
    string tmp = appPath;

    tmp += profFile;

    if ( filesystem::exists( tmp ) && resetSettings )
        filesystem::remove( tmp );

    ifstream in( tmp.c_str() );
    if ( !in )
    {
        ofstream out( tmp );

        //totalProfiles = 1;
        prof[ "totalProfiles" ] = 1;
        prof[ "1" ][ "profileName" ] = "cge7-193";
        prof[ "1" ][ "profileSeries" ] = "Interloper";
        prof[ "1" ][ "profileGame" ] = "Team Fortress 2";
        prof[ "1" ][ "ip" ] = defaultIp;
        prof[ "1" ][ "port" ] = defaultMainPortInt;
        prof[ "1" ][ "fastdl" ] = defaultFastDL;

        out << setw( 4 ) << prof << endl;
        out.close();

        //cout << "Wrote config file to \"" << tmp << "\"" << endl;

        return false;
    }
    else
    {
        prof = json::parse( in );
        in.close();

        //totalProfiles = GetTotalProfiles();

        //cout << "Finished parsing config file at \"" << tmp << "\"" << endl;
        return true;
    }
}

void ProfileInterface::CreateProfile()
{
    system( "cls" );
    cout << "Please enter a name for this profile (ex: cge7-193)" << endl << ">> ";

    string tmpName;
    string tmpSeries;
    string tmpGame;
    string tmpGamePath;
    string tmpIP;
    string tmpPort;
    uint16_t tmpPortInt;
    string tmpFastDL;

    tmpName.clear();
    tmpSeries.clear();
    tmpGame.clear();
    tmpGamePath.clear();
    tmpIP.clear();
    tmpPort.clear();
    tmpFastDL.clear();

    bool illegal = true;

    while ( illegal )
    {
        illegal = false;

        getline( cin, tmpName );

        if ( tmpName == "" || tmpName == " " )
        {
            system( "cls" );
            cerr << "Profile name cannot be NULL. Please enter a proper profile name" << endl << ">> ";
            illegal = true;
        }

        for ( int i = 1; ( i < GetTotalProfiles() && illegal == false ); i++ )
        {
            if ( GetConfStrNested( i, "profileName" ) == tmpName )
            {
                system( "cls" );
                cerr << "A profile with that name already exists! Please enter a unique profile name" << endl << ">> ";
                illegal = true;
            }
        }
    }

    cout << endl << "Please enter the name of the series this profile is for (ex: Interloper)" << endl << ">> ";

    getline( cin, tmpSeries );

    cout << endl << "Please enter the game this profile is for (ex: Team Fortress 2)" << endl << ">> ";

    illegal = true;

    while ( illegal )
    {
        illegal = false;

        getline( cin, tmpGame );

        if ( tmpGame == "" || tmpGame == " " )
        {
            system( "cls" );
            cerr << "Game name cannot be NULL. Please enter a proper game name" << endl << ">> ";
            illegal = true;
        }
    }

    if ( !filesystem::is_directory( ConfigInterface::GetConfStr( tmpGame ) ) )
    {
        while( !filesystem::is_directory( tmpGamePath ) )
        {
            system( "cls" );
            cout << "Please input a valid path to your " << tmpGame << " Install folder (without quotes)" << endl << ">> ";

            tmpGamePath.clear();

            getline( cin, tmpGamePath );
        }

        ConfigInterface::UpdateConfStr( tmpGame, tmpGamePath );
    }

    cout << endl << "Please enter the IP address of the server for this profile (ex: " << defaultIp << ")" << endl << ">> ";

    illegal = true;

    while ( illegal )
    {
        illegal = false;

        getline( cin, tmpIP );

        if ( tmpIP == "" || tmpIP == " " )
        {
            system( "cls" );
            cerr << "Server IP address cannot be NULL. Please enter a proper IP address" << endl << ">> ";
            illegal = true;
        }
    }

    cout << endl << "Please enter the port of the server (ex: " << defaultMainPortInt << ")" << endl << ">> ";

    illegal = true;

    while ( illegal )
    {
        illegal = false;

        getline( cin, tmpPort );

        if ( tmpPort == "" || tmpPort == " " )
        {
            system( "cls" );
            cerr << "Server Port cannot be NULL. Please enter a proper Port" << endl << ">> ";
            illegal = true;
        }
    }

    tmpPortInt = atoi( tmpPort.c_str() );

    cout << endl << "Please enter the FastDL Web-address of the server (ex: " << defaultFastDL << ")" << endl << ">> ";

    getline( cin, tmpFastDL );

    if ( tmpFastDL == "" || tmpFastDL == " " )
        cerr << endl << "Warning! You have entered a NULL FastDL address! FastDL will be unavailable for this profile!";

    UpdateProfile( GetTotalProfiles() + 1, tmpName, tmpSeries, tmpGame, tmpIP, tmpPortInt, tmpFastDL );

    cout << endl << "Success! Would you like to load into this profile now? (y/n)" << endl << ">> ";

    string input;
    getline( cin, input );

    if ( input == "y" || input == "yes" )
    {
        forcedProfile = GetTotalProfiles();
        shouldRestart = true;
    }
}

void ProfileInterface::EditProfile()
{
    system( "cls" );

    bool illegal = true;

    string tmpName;
    string tmpSeries;
    string tmpGame;
    string tmpGamePath;
    string tmpIP;
    string tmpPort;
    uint16_t tmpPortInt;
    string tmpFastDL;

    tmpSeries.clear();
    tmpGame.clear();
    tmpGamePath.clear();
    tmpIP.clear();
    tmpPort.clear();
    tmpFastDL.clear();

    int selection = 0;

    cout << "Select a profile to edit" << endl << endl;
    while ( illegal )
    {
        for ( int i = 1; i <= ProfileInterface::GetTotalProfiles(); i++ )
        {
            cout << "[" << i << ( i == CurrentProfile::id ? " << Current Profile" : "" ) << "] " << ProfileInterface::GetConfStrNested( i, "profileName" );

            if ( ProfileInterface::GetConfStrNested( i, "profileSeries" ) != "" && ProfileInterface::GetConfStrNested( i, "profileSeries" ) != " " )
                cout << " (" << ProfileInterface::GetConfStrNested( i, "profileSeries" ) << ")";

            cout << endl;
        }

        cout << endl << ">> ";

        tmpName.clear();

        getline( cin, tmpName );

        selection = atoi( tmpName.c_str() );

        if ( selection > 0 && selection <= ProfileInterface::GetTotalProfiles() )
            illegal = false;
        else
        {
            system( "cls" );
            cerr << "\033[31mInvalid selection!\033[0m Please only enter a number from 1 to " << ProfileInterface::GetTotalProfiles() << "." << endl;
        }
    }

    cout << endl << "Please enter a new name for this profile (previous: " << ProfileInterface::GetConfStrNested( selection, "profileName" ) << ")" << endl;

    illegal = true;

    while ( illegal )
    {
        illegal = false;

        getline( cin, tmpName );

        if ( tmpName == "" || tmpName == " " )
        {
            system( "cls" );
            cerr << "Profile name cannot be NULL. Please enter a proper profile name" << endl << ">> ";
            illegal = true;
        }

        for ( int i = 1; ( i < GetTotalProfiles() && illegal == false ); i++ )
        {
            if ( GetConfStrNested( i, "profileName" ) == tmpName && GetConfStrNested( i, "profileName" ) != ProfileInterface::GetConfStrNested( selection, "profileName" ) )
            {
                cerr << "A profile with that name already exists! Please enter a unique profile name" << endl << ">> ";
                illegal = true;
            }
        }
    }

    cout << endl << "Please enter the name of the series this profile is for (previouis: " << ProfileInterface::GetConfStrNested( selection, "profileSeries" ) << ")" << endl << ">> ";

    getline( cin, tmpSeries );

    cout << endl << "Please enter the game this profile is for (previous: " << ProfileInterface::GetConfStrNested( selection, "profileGame" ) << ")" << endl << ">> ";

    getline( cin, tmpGame );

    illegal = true;

    while ( illegal )
    {
        illegal = false;

        getline( cin, tmpGame );

        if ( tmpGame == "" || tmpGame == " " )
        {
            system( "cls" );
            cerr << "Game name cannot be NULL. Please enter a proper game name" << endl << ">> ";
            illegal = true;
        }
    }

    if ( !filesystem::is_directory( ConfigInterface::GetConfStr( tmpGame ) ) )
    {
        while( !filesystem::is_directory( tmpGamePath ) )
        {
            cout << "Please input a valid path to your " << tmpGame << " Install folder (without quotes)" << endl << ">> ";

            tmpGamePath.clear();

            getline( cin, tmpGamePath );
        }

        ConfigInterface::UpdateConfStr( tmpGame, tmpGamePath );
    }

    cout << endl << "Please enter the IP address of the server for this profile (previous: " << ProfileInterface::GetConfStrNested( selection, "ip" ) << ")" << endl << ">> ";

    getline( cin, tmpIP );

    illegal = true;

    while ( illegal )
    {
        illegal = false;

        getline( cin, tmpIP );

        if ( tmpIP == "" || tmpIP == " " )
        {
            system( "cls" );
            cerr << "Server IP address cannot be NULL. Please enter a proper IP address" << endl << ">> ";
            illegal = true;
        }
    }

    cout << endl << "Please enter the port of the server (port: " << ProfileInterface::GetConfUint16Nested( selection, "port" ) << ")" << endl << ">> ";

    getline( cin, tmpPort );

    illegal = true;

    while ( illegal )
    {
        illegal = false;

        getline( cin, tmpPort );

        if ( tmpPort == "" || tmpPort == " " )
        {
            system( "cls" );
            cerr << "Server Port cannot be NULL. Please enter a proper Port" << endl << ">> ";
            illegal = true;
        }
    }

    tmpPortInt = atoi( tmpPort.c_str() );

    cout << endl << "Please enter the FastDL Web-address of the server (previous: " << ProfileInterface::GetConfStrNested( selection, "fastdl" ) << ")" << endl << ">> ";

    getline( cin, tmpFastDL );

    if ( tmpFastDL == "" || tmpFastDL == " " )
        cerr << endl << "Warning! You have entered a NULL FastDL address! FastDL will be unavailable for this profile!";

    UpdateProfile( GetTotalProfiles(), tmpName, tmpSeries, tmpGame, tmpIP, tmpPortInt, tmpFastDL );

    cout << endl << "The operation was completed successfully." << endl;

    if ( selection == CurrentProfile::id )
    {
        cout << "Press any key to restart the multitool..." << endl;
        while ( !getchar() );

        forcedProfile = CurrentProfile::id;
        shouldRestart = true;
    }
}

void ProfileInterface::DeleteProfile()
{
    string input;
    bool illegal = true;

    int selection = 0;

    cout << "Select a profile to delete" << endl << endl;
    while ( illegal )
    {
        for ( int i = 1; i <= ProfileInterface::GetTotalProfiles(); i++ )
        {
            cout << "[" << i << ( i == CurrentProfile::id ? " << Current Profile" : "" ) << "] " << ProfileInterface::GetConfStrNested( i, "profileName" );
            
            if ( ProfileInterface::GetConfStrNested( i, "profileSeries" ) != "" && ProfileInterface::GetConfStrNested( i, "profileSeries" ) != " " )
                cout << " (" << ProfileInterface::GetConfStrNested( i, "profileSeries" ) << ")";

            cout << endl;
        }

        cout << endl << ">> ";

        input.clear();

        getline( cin, input );

        selection = atoi( input.c_str() );

        if ( selection == CurrentProfile::id )
        {
            system( "cls" );
            cerr << "\033[31mInvalid selection!\033[0m You cannot delete the current profile" << endl << "Please select a valid profile to delete" << endl << endl;
        }
        else if ( selection > 0 && selection <= ProfileInterface::GetTotalProfiles() )
            illegal = false;
        else
        {
            system( "cls" );
            cerr << "\033[31mInvalid selection!\033[0m Please only enter a number from 1 to " << ProfileInterface::GetTotalProfiles() << "." << endl;
        }
    }

    cout << endl << "Are you sure you want to continue? This action cannot be undone. (y/n)" << endl << ">> ";

    getline( cin, input );

    if ( input == "y" || input == "yes" )
    {
        prof.erase( input );
        UpdateTotalProfiles( GetTotalProfiles() - 1 );
    }
}

void ProfileInterface::ListProfiles()
{
    for ( int i = 1; i <= ProfileInterface::GetTotalProfiles(); i++ )
    {
        cout << "[" << i << ( i == CurrentProfile::id ? " << Current Profile" : "" ) << "] " << ProfileInterface::GetConfStrNested( i, "profileName" );
            
        if ( ProfileInterface::GetConfStrNested( i, "profileSeries" ) != "" && ProfileInterface::GetConfStrNested( i, "profileSeries" ) != " " )
            cout << " (" << ProfileInterface::GetConfStrNested( i, "profileSeries" ) << ")";

        cout << endl;
    }
    cout << endl << "Total profiles: " << ProfileInterface::GetTotalProfiles() << endl;
}

void ProfileInterface::LoadProfile( int profileIndex )
{
    CurrentProfile::name = GetConfStrNested( profileIndex, "profileName" );
    CurrentProfile::series = GetConfStrNested( profileIndex, "profileSeries" );
    CurrentProfile::game = GetConfStrNested( profileIndex, "profileGame" );
    CurrentProfile::ip = GetConfStrNested( profileIndex, "ip" );
    CurrentProfile::port = GetConfUint16Nested( profileIndex, "port" );
    CurrentProfile::fastDL = GetConfStrNested( profileIndex, "fastdl" );
    CurrentProfile::id = profileIndex;
}

int ProfileInterface::GetTotalProfiles()
{
    if ( prof[ "totalProfiles" ].is_null() ) return 1;
    return prof[ "totalProfiles" ].template get<int>();
}

int ProfileInterface::GetConfInt( string name )
{
    if ( prof[ name ].is_null() ) return 0;
    return prof[ name ].template get<int>();
}

bool ProfileInterface::GetConfBool( string name )
{
    if ( prof[ name ].is_null() ) return false;
    return prof[ name ].template get<bool>();
}

string ProfileInterface::GetConfStr( string name )
{
    if ( prof[ name ].is_null() ) return "";
    return prof[ name ].template get<string>();
}

int ProfileInterface::GetConfIntNested( int profileIndex, string name )
{
    char index[ 12 ];
    sprintf( index, "%d", profileIndex );

    if ( prof[ index ][ name ].is_null() ) return 0;
    return prof[ index ][ name ].template get<int>();
}

uint16_t ProfileInterface::GetConfUint16Nested( int profileIndex, string name )
{
    char index[ 12 ];
    sprintf( index, "%d", profileIndex );

    if ( prof[ index ][ name ].is_null() ) return 0;
    return prof[ index ][ name ].template get<uint16_t>();
}

bool ProfileInterface::GetConfBoolNested( int profileIndex, string name )
{
    char index[ 12 ];
    sprintf( index, "%d", profileIndex );

    if ( prof[ index ][ name ].is_null() ) return false;
    return prof[ index ][ name ].template get<bool>();
}

string ProfileInterface::GetConfStrNested( int profileIndex, string name )
{
    char index[ 12 ];
    sprintf( index, "%d", profileIndex );

    if ( prof[ index ][ name ].is_null() ) return "";
    return prof[ index ][ name ].template get<string>();
}

void ProfileInterface::UpdateTotalProfiles( int val )
{
    string tmp = appPath;
    tmp += profFile;

    ofstream out( tmp );
    prof[ "totalProfiles" ] = val;
    out << setw( 4 ) << prof << endl;
}

void ProfileInterface::UpdateProfile( int profileIndex, string profileName, string profileSeries, string profileGame, string ip, uint16_t port, string fastDL )
{
    string tmp = appPath;
    tmp += profFile;

    char index[ 12 ];
    sprintf( index, "%d", profileIndex );

    ofstream out( tmp );
    prof[ "totalProfiles" ] = profileIndex;
    prof[ index ][ "profileName" ] = profileName;
    prof[ index ][ "profileSeries" ] = profileSeries;
    prof[ index ][ "profileGame" ] = profileGame;
    prof[ index ][ "ip" ] = ip;
    prof[ index ][ "port" ] = port;
    prof[ index ][ "fastdl" ] = fastDL;
    out << setw( 4 ) << prof << endl;
}