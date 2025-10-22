#include <json.hpp>

#define FUNNIMAN_SOFTWARE "funniman-software"
#define APPDATA_PATH "cge-multitool"
extern std::string appPath;
//extern int totalProfiles;

namespace CurrentProfile
{
    extern int id;
    extern std::string name;
    extern std::string series;
    extern std::string game;
    extern std::string ip;
    extern uint16_t port;
    extern std::string fastDL;
}

namespace ConfigInterface
{
    // JSON
    extern json conf;
    
    // Initialization
    bool Init( bool resetSettings );

    // Read Entry
    int GetConfInt( string name );
    bool GetConfBool( string name );
    std::string GetConfStr( string name );

    // Update Entry
    void UpdateConfInt( string name, int val );
    void UpdateConfBool( string name, bool val );
    void UpdateConfStr( string name, string val );
}

namespace ProfileInterface
{
    // JSON
    extern json prof;
    
    // Initialization
    bool Init( bool resetSettings );

    void CreateProfile();
    void EditProfile();
    void DeleteProfile();
    void ListProfiles();

    // Read Entry
    void LoadProfile( int profileIndex );
    int GetTotalProfiles();

    int GetConfInt( string name );
    bool GetConfBool( string name );
    string GetConfStr( string name );
    int GetConfIntNested( int profileIndex, string name );
    uint16_t GetConfUint16Nested( int profileIndex, string name );
    bool GetConfBoolNested( int profileIndex, string name );
    std::string GetConfStrNested( int profileIndex, string name );

    // Update Entry
    void UpdateTotalProfiles( int val );
    void UpdateProfile( int profileIndex, string profileName, string profileSeries, string profileGame, string ip, uint16_t port, string fastDL );
}