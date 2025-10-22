#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <direct.h>
#include <string>
#include <windows.h>
#include <fstream>
#include <filesystem>
#include <picosha2.h>
#include <json.hpp>

using namespace std;

using json = nlohmann::json;

#include "fastdl.h"
#include "config.h"
#include "globs.h"

void FastDL::fastdl( const char* args, bool isTempFile, bool noVerbose )
{
    if ( strcmp( args, "" ) == 0 || strcmp( args, " " ) == 0 )
    {
        cerr << "fastdl -> Invalid Syntax!" << endl;
        return;
    }

    if ( strstr( args, "\\" ) != NULL )
    {
        cerr << "fastdl -> Please do not use backslashes, only use normal slashes." << endl;
        return;
    }

    char responsebuff[128];
    char* cmdHead = new char[512];
    if ( !verbose ) strcpy( cmdHead, "curl -s -I " );
    if ( verbose ) strcpy( cmdHead, "curl -I " );
    strcat( cmdHead, CurrentProfile::fastDL.c_str() );
    strcat( cmdHead, args );
    char* cmdMain = new char[512];
    if ( !verbose ) strcpy( cmdMain, "curl -s " );
    if ( verbose ) strcpy( cmdMain, "curl " );
    strcat( cmdMain, CurrentProfile::fastDL.c_str() );
    strcat( cmdMain, args );
    strcat( cmdMain, " --create-dirs --output \"" );
    strcat( cmdMain, gamePath.c_str() );

    if ( !isTempFile ) strcat( cmdMain, "/tf/download/" );
    if ( isTempFile ) strcat( cmdMain, "/tf/download/cge-temp/" );
    strcat( cmdMain, args );
    strcat( cmdMain, "\"" );

    //cout << cmdHead << endl;
    //cout << cmdMain << endl;

    if ( !noVerbose ) cout << "Validating HTTP Request Header...";
    if ( verbose && !noVerbose ) cout << endl;

    FILE *fp = _popen( cmdHead, "r" );
    string result;

    if ( fgets( responsebuff, sizeof(responsebuff), fp ) != nullptr )
    {
        result = responsebuff;
        result.erase( result.find_last_not_of( " \n\r\t" ) + 1 );
    }

    _pclose( fp );
    delete[] cmdHead;

    if ( strstr( result.c_str(), "200 OK" ) == NULL )
    {
        if ( !verbose && !noVerbose ) cout << " [ \033[31mFAILED\033[0m ]" << endl;
        cerr << "Invalid Request! ";

        if ( strstr( result.c_str(), "404 NOT FOUND" ) != NULL )
        {
            cerr << "Returned 404 on requested path \"" << args << "\". Check your spelling and try again" << endl;
        }
        else if ( strstr( result.c_str(), "403 FORBIDDEN" ) != NULL )
        {
            cerr << "Returned 403 on requested path \"" << args << "\". Ensure you specified an actual file, not a folder" << endl;
        }
        else if ( strstr( result.c_str(), "429" ) != NULL )
        {
            cerr << "You are being Rate Limited by the host! Please wait a minute or so before continuing" << endl;
        }
        else if ( strstr( result.c_str(), "Could not resolve host" ) != NULL )
        {
            cerr << "Could not resolve the fastdl host! Check your internet and try again." << endl << "If the issue persists, contact @funniman.exe" << endl;
        }
        else
        {
            cerr << "Uncaught Error! The following is what was printed by cURL: " << endl << "  " << result << endl;
        }

        delete[] cmdMain;
        return;
    }

    if ( !verbose && !noVerbose ) cout << " [ \033[34mOK\033[0m ]" << endl;
    if ( !noVerbose ) cout << "Downloading file from fastdl...";
    if ( verbose && !noVerbose ) cout << endl;

    system( cmdMain );

    if ( !verbose && !noVerbose ) cout << " [ \033[34mDONE\033[0m ]" << endl;
    if ( !isTempFile && !noVerbose ) cout << "Success! Downloaded file to \"" << gamePath << "/tf/download/" << args << "\"" << endl;
    if ( isTempFile && !noVerbose ) cout << "Success! Downloaded file to \"" << gamePath << "/tf/download/cge-temp/" << args << "\"" << endl;

    delete[] cmdMain;
}

void FastDL::fastdl_loop( const char* jsonFile )
{
    //cout << "Type 'q' to exit the loop" << endl;
    cout << "To exit the loop, you must quit the application" << endl;

    string targetPath = appDataPath;
    targetPath += jsonFile;

    string cmd = "curl -s https://raw.githubusercontent.com/funniman-exe/funniman-exe.github.io/refs/heads/main/ftp/interloper/";
    cmd += jsonFile;
    cmd += " --create-dirs --output ";
    cmd += targetPath;

    string cmdHead = "curl -s -I https://raw.githubusercontent.com/funniman-exe/funniman-exe.github.io/refs/heads/main/ftp/interloper/";
    cmdHead += jsonFile;

    while ( true )
    {
        cout << "Validating HTTP Request Header...";

        FILE *fp = _popen( cmdHead.c_str(), "r" );
        string result;

        char responsebuff[128];

        if ( fgets( responsebuff, sizeof(responsebuff), fp ) != nullptr )
        {
            result = responsebuff;
            result.erase( result.find_last_not_of( " \n\r\t" ) + 1 );
        }

        _pclose( fp );

        if ( strstr( result.c_str(), "200 OK" ) == NULL )
        {
            cout << " [ \033[31mFAILED\033[0m ]" << endl;
            cerr << "Invalid Request! ";

            if ( strstr( result.c_str(), "404 NOT FOUND" ) != NULL )
            {
                cerr << "Returned 404 on requested json \"" << jsonFile << "\". Please contact funniman.exe" << endl;
            }
            else if ( strstr( result.c_str(), "403 FORBIDDEN" ) != NULL )
            {
                cerr << "Returned 403 on requested json \"" << jsonFile << "\". Please contact funniman.exe" << endl;
            }
            else if ( strstr( result.c_str(), "429" ) != NULL )
            {
                cerr << "You are being Rate Limited by the host! Please wait a minute or so before continuing" << endl;
            }
            else if ( strstr( result.c_str(), "Could not resolve host" ) != NULL )
            {
                cerr << "Could not resolve the host! Check your internet and try again." << endl << "If the issue persists, contact @funniman.exe" << endl;
            }
            else
            {
                cerr << "Uncaught Error! The following is what was printed by cURL: " << endl << "  " << result << endl;
            }

            return;
        }

        cout << " [ \033[34mOK\033[0m ]" << endl;
        cout << "Retrieving file \"" << jsonFile << "\" from host...";

        system( cmd.c_str() );

        cout << " [ \033[34mDONE\033[0m ]" << endl;

        ifstream f( targetPath.c_str() );
        json data = json::parse( f );

        for ( int i = 0; i < data["count"].template get<int>(); i++ )
        {
            string viewFileEntry = "asset_";

            char tm_a[5];
            itoa( i + 1, tm_a, 10 );

            //cout << tm_a << endl;

            viewFileEntry += tm_a;

            //cout << viewFileEntry << endl;

            string viewFile( data[viewFileEntry.c_str()].template get<string>() );

            string tmp = gamePath;
            string tmp2 = gamePath;

            tmp += "/tf/download/";
            tmp2 += "/tf/download/cge-temp/";
            tmp += viewFile;
            tmp2 += viewFile;

            if ( filesystem::exists( tmp ) )
            {
                fastdl( viewFile.c_str(), true, true );
                cout << "Comparing file \"" << viewFile << "\"...";

                ifstream file( tmp, ios::binary );
                ifstream file2( tmp2, ios::binary );

                if ( file.is_open() && file2.is_open() )
                {
                    vector<unsigned char> hash( picosha2::k_digest_size );
                    vector<unsigned char> hash2( picosha2::k_digest_size );

                    picosha2::hash256( file, hash.begin(), hash.end() );
                    picosha2::hash256( file2, hash2.begin(), hash2.end() );

                    string result = picosha2::bytes_to_hex_string( hash.begin(), hash.end() );
                    string result2 = picosha2::bytes_to_hex_string( hash2.begin(), hash2.end() );

                    file.close();
                    file2.close();

                    cout << " [ \033[34mDONE\033[0m ] " << endl;

                    cout << "Original File's SHA256:    " << result << endl << "New File's SHA256:         " << result2 << endl;

                    if ( result == result2 )
                    {
                        cout << "File \"" << viewFile << "\" is identical." << endl;
                        remove( tmp2.c_str() );
                    }
                    else
                    {
                        cout << "\033[34mTHIS FILE DIFFERS!! THIS FILE DIFFERS!!" << endl << "THIS FILE DIFFERS!! THIS FILE DIFFERS!!" << endl << "THIS FILE DIFFERS!! THIS FILE DIFFERS!!" << endl;
                        cout << "\033[0mFile \"" << viewFile << "\" differs! Updating local file..." << endl;
                        remove( tmp.c_str() );
                        remove( tmp2.c_str() );
                        fastdl( viewFile.c_str(), false, true );
                        _sleep( 3000 );
                    }
                }
                else
                {
                    cout << " [ \033[31mFAILED\033[0m ] " << endl;
                    cerr << "Unable to open file \"" << viewFile << "\"!" << endl;
                    if ( file.is_open() ) file.close();
                    if ( file2.is_open() ) file2.close();
                }
            }
            else
            {
                cout << "\033[31mFile \"" << viewFile << "\" does not exist locally, downloading...\033[0m" << endl;
                fastdl( viewFile.c_str(), false, true );
            }

            cout << endl;
        }

        f.close();
    }
}

// macros

void FastDL::fastdl_macro_view()
{
    fastdl_loop( "view-files.json" );
}

void FastDL::fastdl_macro_view_min()
{
    fastdl_loop( "view-files-min.json" );
}

void FastDL::fastdl_macro_scrape()
{
    fastdl_loop( "known-maps.json" );
}

void FastDL::fastdl_macro_scrape_min()
{
    fastdl_loop( "known-maps-min.json" );
}