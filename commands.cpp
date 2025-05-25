#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>
#include <fstream>
#include <filesystem>
#include "fastdl.h"
#include "commands.h"
#include "globs.h"

using namespace std;

bool CgeInterface::ping()
{
    cout << "Pinging \"cge7-193\"..." << endl;

    char responsebuff[1000];
    FILE *fp = popen( "ping -w 2000 79.127.217.196", "r" );
    while ( fgets( responsebuff, sizeof(responsebuff), fp) );

    int stat = pclose(fp);

    if ( stat == 1 )
    {
        return false;
    }

    if ( verbose ) cout << "\"cge7-193\" ping information:" << endl << responsebuff << endl;
    return true;
}

void CgeInterface::help()
{
    cout << "The \"cge7-193\" Multitool" << endl << "   (c) 2025 funniman.exe" << endl << endl;
    cout << "Type \"help\" - Brings up this menu." << endl;
    cout << "Type \"reset\" or \"restart\" - Restart the multitool." << endl;
    cout << "Type \"quit\" or \"exit\" - Exit the multitool." << endl;
    cout << "Type \"verbose\" - Toggle Verbose mode." << endl;
    cout << "Type \"info\" - Print \"cge7-193\" server information." << endl;
    cout << "Type \"fastdl <filepath (no quotes)>\" - Download requested file from fastdl." << endl << endl;
    cout << "fastdl quick macros:" << endl;
    cout << "   Type \"view\" - Check if view render assets have changed" << endl;
    cout << "   Type \"scrape\" - Check if known maps have changed" << endl;
}

void CgeInterface::info()
{
    cout << "This command is non-functional" << endl;
    return;

    if ( !ping() )
    {
        cout << "Failed to ping \"cge7-193\", unable to grab server info." << endl;
        return;
    }
}