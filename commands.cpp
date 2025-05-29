#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <fstream>
#include <cinttypes>
#include <filesystem>
#include <ssq/a2s.h>
#include "fastdl.h"
#include "commands.h"
#include "globs.h"

using namespace std;

bool CgeInterface::ping()
{
    cout << "Pinging \"cge7-193\"...";

    char responsebuff[1000];
    string cmd = "ping -w 2000 ";
    cmd += ip;
    FILE *fp = _popen( cmd.c_str(), "r" );
    while ( fgets( responsebuff, sizeof(responsebuff), fp) );

    int stat = _pclose(fp);

    if ( stat == 1 )
    {
        cout << " [ \033[31mFAILED\033[0m ]" << endl;
        return false;
    }

    cout << " [ \033[34mDONE\033[0m ]" << endl;

    if ( verbose ) cout << "\"cge7-193\" ping information:" << endl << responsebuff << endl;
    return true;
}

void CgeInterface::help()
{
    cout << "The \"cge7-193\" Multitool v" << verMajor << "." << verMinor << endl << "   (c) 2025 funniman.exe" << endl << endl;
    cout << "Type \"help\" - Brings up this menu." << endl;
    cout << "Type \"reset\" or \"restart\" - Restart the multitool." << endl;
    cout << "Type \"quit\" or \"exit\" - Exit the multitool." << endl;
    cout << "Type \"verbose\" - Toggle Verbose mode." << endl;
    cout << "Type \"info\" - Print \"cge7-193\" server information." << endl;
    cout << "Type \"fastdl <filepath (no quotes)>\" - Download requested file from fastdl." << endl << endl;
    cout << "fastdl quick macros:" << endl;
    cout << "   Type \"view <min/full (assumes min)>\" - Check if view render assets have changed" << endl;
    cout << "   Type \"scrape <min/full (assumes min)>\" - Check if known maps have changed" << endl;
}

const char* CgeInterface::ParseEnvirons( A2S_ENVIRONMENT environment )
{
    if ( environment == A2S_ENVIRONMENT_LINUX )
    {
        return "Linux";
    }
    else if ( environment == A2S_ENVIRONMENT_WINDOWS )
    {
        return "Windows";
    }
    else if ( environment == A2S_ENVIRONMENT_MAC )
    {
        return "MacOS";
    }
    else
    {
        return "Unknown";
    }
}

void CgeInterface::print_svr_info( const A2S_INFO *const info )
{
    bool sourceTVActive = ( info->stv_port != 0 );
    cout << "----- cge7-193 Server Information -----" << endl;
    printf( "Server Protocol    - %" PRIu8 "\n", info->protocol );
    cout << "Server Name        - \"" << info->name << "\"" << endl;
    cout << "IP                 - " << ip << endl;
    cout << "Port               - " << info->port << endl;
    cout << "SourceTV Status    - " << ( sourceTVActive ? "Online" : "Inactive" ) << endl;
    if ( sourceTVActive ) cout << "SourceTV Port      - " << info->stv_port << endl;
    cout << "Current Map        - " << ( strcmp( info->map, "" ) != 0 ? info->map : "N/A" ) << ( strcmp( info->map, "" ) != 0 ? ".bsp" : "" ) << endl;
    if ( info->bots == info->players )
        printf( "Players            - %" PRIu8 "/%" PRIu8 " - (All Bots)\n",
        info->players, info->max_players );
    else if ( info->bots > 0 )
        printf( "Players            - %" PRIu8 "/%" PRIu8 " - (%" PRIu8 " bots in-game)\n",
        info->players, info->max_players, info->bots );
    else
        printf( "Players            - %" PRIu8 "/%" PRIu8 "\n",
        info->players, info->max_players );
    cout << "Server Environment - " << ParseEnvirons( info->environment ) << endl;
    cout << "Game ID            - " << info->id << endl;
    cout << "VAC                - " << ( info->vac ? "Secured" : "Unsecured" ) << endl;
    cout << "Version            - " << info->version << endl;
}

void CgeInterface::print_svr_players( const A2S_PLAYER players[], const uint8_t player_count )
{
    cout << "--------------- Players ---------------" << endl;
    for ( uint8_t i = 0; i < player_count; ++i )
    {
        printf( "Player #%i          - \"%s\"\n", i+1, players[i].name );
        printf( "  Duration in-game - %f seconds\n", players[i].duration );
        printf( "  Score            - %" PRId32 "\n\n", players[i].score );
    }
}

void CgeInterface::info()
{
    //cout << "This command is non-functional" << endl;
    //return;

    if ( !ping() )
    {
        cout << "Connection Timeout: Host did not respond. Check your internet and try again." << endl;
        return;
    }

    cout << "Establishing connection to host...";

    WSADATA wsa_data;
    if ( WSAStartup( MAKEWORD(2, 2), &wsa_data ) != NO_ERROR )
    {
        cout << " [ \033[31mFAILED\033[0m ]" << endl;
        cerr << "WSAStartup failed with code " << WSAGetLastError() << endl;
        return;
    }

    /* Initialization */
    SSQ_SERVER *server = ssq_server_new( ip, mainPortInt );
    if ( server == NULL )
    {
        cout << " [ \033[31mFAILED\033[0m ]" << endl;
        cerr << "Memory Exhausted" << endl;
        WSACleanup();
        return;
    }
    else if ( !ssq_server_eok( server ) )
    {
        cerr << ssq_server_emsg( server ) << endl;
        ssq_server_free( server );
        WSACleanup();
        return;
    }
    ssq_server_timeout( server, ( SSQ_TIMEOUT_SELECTOR )( SSQ_TIMEOUT_RECV | SSQ_TIMEOUT_SEND ), 10000 );

    cout << " [ \033[34mDONE\033[0m ]" << endl;
    cout << "Requesting Host for info...";

    /* A2S_INFO */
    A2S_INFO *info = ssq_info( server );
    if ( ssq_server_eok( server ) )
    {
        cout << " [ \033[34mDONE\033[0m ]" << endl << endl;
        print_svr_info( info );
        ssq_info_free( info );
    }
    else
    {
        cout << " [ \033[31mFAILED\033[0m ]" << endl;
        cerr << "ssq_info: " << ssq_server_emsg( server ) << endl;
        ssq_server_eclr( server );
        ssq_server_free( server );
        WSACleanup();
        return;
    }

    cout << endl << "Requesting Host for players...";

    /* A2S_PLAYER */
    if ( info->players > 0 )
    {
        uint8_t player_count = 0;
        A2S_PLAYER *players = ssq_player( server, &player_count );
        if ( ssq_server_eok( server ) )
        {
            cout << " [ \033[34mDONE\033[0m ]" << endl << endl;
            print_svr_players( players, player_count );
            ssq_player_free( players, player_count );
        }
        else
        {
            cout << " [ \033[31mFAILED\033[0m ]" << endl;
            cerr << ssq_server_emsg( server ) << endl;
            ssq_server_eclr( server );
        }
    }

    ssq_server_free( server );
    WSACleanup();
}