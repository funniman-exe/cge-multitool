#include <json.hpp>

#define FUNNIMAN_SOFTWARE "funniman-software"
#define APPDATA_PATH "cge-multitool"

namespace ConfigInterface {
    // JSON
    extern json conf;
    
    // Initialization
    bool Init();

    // Read Entry
    int GetConfInt( string name );
    bool GetConfBool( string name );
    string GetConfStr( string name );

    // Update Entry
    void UpdateConfInt( string name, int val );
    void UpdateConfBool( string name, bool val );
    void UpdateConfStr( string name, string val );
}