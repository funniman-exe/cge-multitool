#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <direct.h>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <fstream>
#include <filesystem>
#include "fastdl.h"
#include "commands.h"
#include "globs.h"

using namespace std;

bool shouldExit = false;
bool shouldRestart = true;
char* gamePath;
bool verbose = false;

bool quitFunc = false;

const char* ip = "79.127.217.197";
const char* mainPort = "22912";
const char* sourceTV = "22913";
uint16_t mainPortInt = 22912;
uint16_t sourceTVInt = 22913;

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
    else if ( strcmp( cmd, "verbose" ) == 0 )
    {
        verbose = !verbose;
        cout << ( verbose ? "Verbose Logging is now enabled." : "Verbose Logging is now disabled" ) << endl;
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
        FastDL::fastdl_macro_view();
        return true;
    }
    else if ( strcmp( cmd, "scrape" ) == 0 )
    {
        FastDL::fastdl_macro_scrape();
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
    gamePath = new char[MAX_PATH];

    while ( shouldRestart )
    {
        shouldRestart = false;

        cout << "Registering Filepath...";

        char path[MAX_PATH];
        GetModuleFileNameA( nullptr, path, MAX_PATH );
        string path_str(path);
        strcpy( gamePath, path_str.substr( 0, path_str.find_last_of( "\\/" ) ).c_str() );
        chdir( gamePath );

        cout << " [ DONE ] " << endl;

        _sleep( 3000 );
        cout << "Starting multitool... " << endl;
        _sleep( 1000 );

        system("cls");
        cout << "Welcome to the \"cge7-193\" Multitool!" << endl << "   (c) 2025 funniman.exe" << endl << endl;

        multitool();
    }

    delete[] gamePath;

    return 0;
}