#pragma once

namespace element {
    struct startup_settings {
        //TODO: here will appear startup settings
        
    };

    class init {
        public:
            static void setup_engine(const startup_settings& settings);
            static void cleanup();
    };
} // namespace elemen 