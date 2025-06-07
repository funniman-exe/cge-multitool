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

namespace ConfigInterface {
    // JSON
    json conf;
}

bool ConfigInterface::Init()
{
    strcat( appDataPath, "/funniman-software/" );
    _mkdir( appDataPath );
    strcat( appDataPath, "cge-multitool/" );
    _mkdir( appDataPath );

    string tmp = appDataPath;
    tmp += configFile;

    ifstream in( tmp.c_str() );
    if ( !in )
    {
        ofstream out( tmp );

        conf["verbose"] = false;
        conf["gamepath"] = "TMP";

        out << setw( 4 ) << conf << endl;
        out.close();

        //cout << "Wrote config file to \"" << tmp << "\"" << endl;

        return false;
    }
    else
    {
        conf = json::parse( in );
        //cout << "Finished parsing config file at \"" << tmp << "\"" << endl;
        return true;
    }
}

int ConfigInterface::GetConfInt( string name )
{
    return conf[name].template get<int>();
}

bool ConfigInterface::GetConfBool( string name )
{
    return conf[name].template get<bool>();
}

string ConfigInterface::GetConfStr( string name )
{
    return conf[name].template get<string>();
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