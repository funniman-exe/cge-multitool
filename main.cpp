#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>
#include <fstream>
#include <filesystem>
#include "picosha2.h"
#include "json.hpp"

using namespace std;

using json = nlohmann::json;

bool shouldExit = false;
bool shouldRestart = true;
char* gamePath;
bool verbose = false;

bool quitFunc = false;

bool ping()
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

void help()
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

void info()
{
    cout << "This command is non-functional" << endl;
    return;

    if ( !ping() )
    {
        cout << "Failed to ping \"cge7-193\", unable to grab server info." << endl;
        return;
    }
}

void fastdl( const char* args, bool isTempFile, bool noVerbose )
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

    string tmp_a;
    tmp_a += gamePath;
    tmp_a += "/tf/download/";
    if ( isTempFile ) tmp_a += "cge-temp/";
    tmp_a += args;
    size_t last_slash_pos = tmp_a.rfind( '/' );

    if ( last_slash_pos != std::string::npos )
    {
        tmp_a.erase( last_slash_pos + 1 );
    }

    if ( !filesystem::is_directory( tmp_a ) ) mkdir( tmp_a.c_str() );

    tmp_a.clear();

    char responsebuff[128];
    char* cmdHead = new char[512];
    if ( !verbose ) strcpy( cmdHead, "curl -s -I https://wavespray.dathost.net/fastdl/teamfortress2/679d9656b8573d37aa848d60/" );
    if ( verbose ) strcpy( cmdHead, "curl -I https://wavespray.dathost.net/fastdl/teamfortress2/679d9656b8573d37aa848d60/" );
    strcat( cmdHead, args );
    char* cmdMain = new char[512];
    if ( !verbose ) strcpy( cmdMain, "curl -s https://wavespray.dathost.net/fastdl/teamfortress2/679d9656b8573d37aa848d60/" );
    if ( verbose ) strcpy( cmdMain, "curl https://wavespray.dathost.net/fastdl/teamfortress2/679d9656b8573d37aa848d60/" );
    strcat( cmdMain, args );
    if ( !isTempFile ) strcat( cmdMain, " --output tf/download/" );
    if ( isTempFile ) strcat( cmdMain, " --output tf/download/cge-temp/" );
    strcat( cmdMain, args );

    //cout << cmdHead << endl;
    //cout << cmdMain << endl;

    if ( !noVerbose ) cout << "Validating HTTP Request Header...";
    if ( verbose && !noVerbose ) cout << endl;

    FILE *fp = popen( cmdHead, "r" );
    string result;

    if ( fgets( responsebuff, sizeof(responsebuff), fp ) != nullptr )
    {
        result = responsebuff;
        result.erase( result.find_last_not_of( " \n\r\t" ) + 1 );
    }

    pclose( fp );
    delete[] cmdHead;

    if ( strstr( result.c_str(), "200 OK" ) == NULL )
    {
        if ( !verbose && !noVerbose ) cout << " [ FAILED ]" << endl;
        cerr << "Invalid Request! ";

        if ( strstr( result.c_str(), "404 NOT FOUND" ) != NULL )
        {
            cerr << "Returned 404 on requested path \"" << args << "\". Check your spelling and try again" << endl;
        }
        else if ( strstr( result.c_str(), "403 FORBIDDEN" ) != NULL )
        {
            cerr << "Returned 403 on requested path \"" << args << "\". Ensure you specified an actual file, not a folder" << endl;
        }
        else if ( strstr( result.c_str(), "Could not resolve host" ) != NULL )
        {
            cerr << "Could not resolve the fastdl host! Check your internet and try again." << endl;
        }
        else
        {
            cerr << "Uncaught Error! The following is what was printed by cURL: " << endl << "  " << result << endl;
        }

        delete[] cmdMain;
        return;
    }

    if ( !verbose && !noVerbose ) cout << " [ OK ]" << endl;
    if ( !noVerbose ) cout << "Downloading file from fastdl...";
    if ( verbose && !noVerbose ) cout << endl;

    system( cmdMain );

    if ( !verbose && !noVerbose ) cout << " [ DONE ]" << endl;
    if ( !isTempFile && !noVerbose ) cout << "Success! Downloaded file to \"" << gamePath << "/tf/download/" << args << "\"" << endl;
    if ( isTempFile && !noVerbose ) cout << "Success! Downloaded file to \"" << gamePath << "/tf/download/cge-temp/" << args << "\"" << endl;

    delete[] cmdMain;
}

void fastdl_loop( const char* jsonFile )
{
    //cout << "Type 'q' to exit the loop" << endl;
    cout << "To exit the loop, you must quit the application" << endl;

    string targetPath = "cge-multitool/";
    targetPath += jsonFile;

    string cmd = "curl -s https://raw.githubusercontent.com/funniman-exe/funniman-exe.github.io/refs/heads/main/ftp/interloper/";
    cmd += jsonFile;
    cmd += " --output ";
    cmd += targetPath;

    //cout << cmd << endl;

    while ( true )
    {
        system( cmd.c_str() );

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

                    cout << " [ DONE ] " << endl;

                    cout << "Original File's SHA256:    " << result << endl << "New File's SHA256:         " << result2 << endl;

                    if ( result == result2 )
                    {
                        cout << "File \"" << viewFile << "\" is identical." << endl;
                        remove( tmp2.c_str() );
                    }
                    else
                    {
                        cout << "File \"" << viewFile << "\" differs! Updating local file..." << endl;
                        remove( tmp.c_str() );
                        remove( tmp2.c_str() );
                        fastdl( viewFile.c_str(), false, true );
                    }
                }
                else
                {
                    cout << " [ FAILED ] " << endl;
                    cerr << "Unable to open file \"" << viewFile << "\"!" << endl;
                    if ( file.is_open() ) file.close();
                    if ( file2.is_open() ) file2.close();
                }
            }
            else
            {
                cout << "File \"" << viewFile << "\" does not exist locally, downloading..." << endl;
                fastdl( viewFile.c_str(), false, true );
            }

            cout << endl;
        }

        f.close();
    }
}

void view()
{
    fastdl_loop( "view-files.json" );
}

void scrape()
{
    fastdl_loop( "known-maps.json" );
}

bool parse( const char* cmd, const char* args )
{
    if ( strcmp( cmd, "cls" ) == 0 )
    {
        system("cls");
        return true;
    }
    else if ( strcmp( cmd, "help" ) == 0 )
    {
        help();
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
        info();
        return true;
    }
    else if ( strcmp( cmd, "fastdl" ) == 0 )
    {
        fastdl( args, false, false );
        return true;
    }
    else if ( strcmp( cmd, "view" ) == 0 )
    {
        view();
        return true;
    }
    else if ( strcmp( cmd, "scrape" ) == 0 )
    {
        scrape();
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