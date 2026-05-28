cmake_minimum_required(VERSION 3.22)
project(SlopAudioConnect VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
find_package(CURL REQUIRED)

FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.12
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(JUCE nlohmann_json)

juce_add_gui_app(SlopAudioConnect
    PRODUCT_NAME     "SlopAudio Connect"
    COMPANY_NAME     "SlopSmith"
    BUNDLE_ID        "com.slopsmith.slopaudio-connect"
    VERSION          "0.1.0"
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT    "SlopAudio Connect needs microphone access for instrument monitoring."
)

juce_generate_juce_header(SlopAudioConnect)

target_sources(SlopAudioConnect PRIVATE
    Source/Main.cpp
    Source/Audio/AudioEngine.cpp
    Source/Audio/PitchDetector.cpp
    Source/Audio/PluginChainProcessor.cpp
    Source/Network/BeaconScanner.cpp
    Source/Network/UdpSession.cpp
    Source/App/AppState.cpp
    Source/App/SessionController.cpp
    Source/App/ProfileManager.cpp
    Source/GUI/MainWindow.cpp
    Source/GUI/SlopLookAndFeel.cpp
    Source/GUI/SidebarNav.cpp
    Source/GUI/LucideIcons.cpp
    Source/GUI/LogConsoleWindow.cpp
    Source/GUI/ConnectionPanel.cpp
    Source/GUI/DetectionPanel.cpp
    Source/GUI/ProfilePanel.cpp
    Source/GUI/AudioSettingsPanel.cpp
    Source/GUI/PluginChainPanel.cpp
    Source/GUI/StatusBar.cpp
)

target_compile_definitions(SlopAudioConnect PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=1
    JUCE_PLUGINHOST_VST3=1
    JUCE_PLUGINHOST_AU=0
    JUCE_PLUGINHOST_LADSPA=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:SlopAudioConnect,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:SlopAudioConnect,JUCE_VERSION>"
)

target_link_libraries(SlopAudioConnect
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_cryptography
        nlohmann_json::nlohmann_json
        CURL::libcurl
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)
