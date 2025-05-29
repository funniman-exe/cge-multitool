namespace FastDL {
    // Main FastDL Access function
    void fastdl( const char* args, bool isTempFile, bool noVerbose );

    // FastDL Loop function
    void fastdl_loop( const char* jsonFile );

// --------------------------------------------------------------------------

    // FastDL Macro: Invokes fastdl_loop with the VIEW json
    void fastdl_macro_view_min();
    void fastdl_macro_view();

    // FastDL Macro: Invokes fastdl_loop with the KNOWN MAPS json
    void fastdl_macro_scrape();
}