#include <ssq/a2s.h>

namespace CgeInterface
{
    // Ping command
    bool ping();

    // Ping command
    bool ping_sourcetv();

    // Help command - displays command list
    void help();

    // Print Server Information
    const char* ParseEnvirons( A2S_ENVIRONMENT environment );
    void print_svr_info( const A2S_INFO *const info );
    void print_svr_players( const A2S_PLAYER players[], const uint8_t player_count );

    bool initServer();

    void pullCurrentMap();

    // Info command - displays cge server info
    void info();
}