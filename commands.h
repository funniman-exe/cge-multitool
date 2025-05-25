#include <ssq/a2s.h>

namespace CgeInterface {
    // Ping command
    bool ping();

    // Ping command
    bool ping_sourcetv();

    // Help command - displays command list
    void help();

    // Print Server Information
    const char* ParseEnvirons( A2S_ENVIRONMENT environment );
    void print_svr_info( const A2S_INFO *const info );

    // Info command - displays cge server info
    // TODO: FINISH ME
    void info();
}