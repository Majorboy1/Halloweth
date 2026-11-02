cmake_minimum_required(VERSION 3.22)

project(AFRONOVA VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Link the MSVC runtime statically so the plugin has no VC++ redistributable
# dependency - hosts load it on machines that never installed the redist.
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# ------------------------------------------------------------------------------
# JUCE
# ------------------------------------------------------------------------------
# Prefer a local JUCE checkout (git submodule / manual clone) so builds work
# offline and are reproducible. Falls back to a pinned download if it is absent.
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/JUCE/CMakeLists.txt")
    message(STATUS "AFRONOVA: using local JUCE at ThirdParty/JUCE")
    add_subdirectory(ThirdParty/JUCE EXCLUDE_FROM_ALL)
else()
    message(STATUS "AFRONOVA: ThirdParty/JUCE not found - fetching JUCE 8.0.8")
    include(FetchContent)
    FetchContent_Declare(JUCE
        GIT_REPOSITORY https://github.com/juce-framework/juce.git
        GIT_TAG        8.0.8
        GIT_SHALLOW    TRUE)
    FetchContent_MakeAvailable(JUCE)
endif()

# ------------------------------------------------------------------------------
# Plugin target
# ------------------------------------------------------------------------------
juce_add_plugin(AFRONOVA
    COMPANY_NAME                "Halloweth Audio"
    BUNDLE_ID                   "com.halloweth.audio.halloweth"
    PLUGIN_MANUFACTURER_CODE    Hwth
    PLUGIN_CODE                 Hwl1
    PRODUCT_NAME                "HALLOWETH"
    DESCRIPTION                 "Cinematic hybrid synth and premium world-instrument platform"
    IS_SYNTH                    TRUE
    NEEDS_MIDI_INPUT            TRUE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD     FALSE
    VST3_CATEGORIES             Instrument Synth
    FORMATS                     VST3 Standalone)

target_sources(AFRONOVA PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/AudioEngine/AudioEngine.cpp
    Source/AudioEngine/SynthVoice.cpp
    Source/AudioEngine/SampleVoice.cpp
    Source/AudioEngine/SampleLoader.cpp
    Source/Instruments/SampleMap.cpp
    Source/Instruments/DrumKit.cpp
    Source/Instruments/InstrumentLoader.cpp
    Source/Effects/EffectChain.cpp
    Source/Presets/PresetManager.cpp
    Source/Presets/FactoryPresets.cpp
    Source/UI/AfronovaLookAndFeel.cpp
    Source/UI/NavigationBar.cpp
    Source/UI/MidiActivityLight.cpp
    Source/UI/PresetBar.cpp
    Source/UI/Knobs/LabeledKnob.cpp
    Source/UI/Pages/HomePage.cpp
    Source/UI/Pages/InstrumentsPage.cpp
    Source/UI/Pages/DrumsPage.cpp
    Source/UI/Pages/EffectsPage.cpp
    Source/UI/Pages/LibraryPage.cpp
    Source/UI/Pages/PlaceholderPage.cpp)

target_include_directories(AFRONOVA PRIVATE Source)

target_compile_definitions(AFRONOVA PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_STRICT_REFCOUNTEDPOINTER=1)

target_link_libraries(AFRONOVA
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_formats
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

# JUCE 8 refuses to compile under MinGW (see juce_core/juce_TargetPlatform.h).
# Fail with an actionable message rather than a wall of compiler errors.
if (MINGW)
    message(FATAL_ERROR
        "AFRONOVA requires MSVC on Windows. JUCE 8 does not support MinGW.\n"
        "Install Visual Studio 2022 Build Tools with the 'Desktop development with C++' "
        "workload, then delete the build directory and reconfigure.")
endif()

if (MSVC)
    # /utf-8 keeps the non-ASCII separators used in UI strings intact.
    target_compile_options(AFRONOVA PRIVATE /MP /permissive- /utf-8)
endif()

# ------------------------------------------------------------------------------
# Automated tests
# ------------------------------------------------------------------------------
option(AFRONOVA_BUILD_TESTS "Build the AFRONOVA engine test suite" ON)

if (AFRONOVA_BUILD_TESTS)
    enable_testing()

    juce_add_console_app(AFRONOVA_Tests PRODUCT_NAME "AFRONOVA Tests")

    target_sources(AFRONOVA_Tests PRIVATE Tests/AudioEngineTests.cpp)
    target_include_directories(AFRONOVA_Tests PRIVATE Source)

    target_link_libraries(AFRONOVA_Tests
        PRIVATE
            AFRONOVA
            juce::juce_audio_utils
            juce::juce_dsp
        PUBLIC
            juce::juce_recommended_config_flags)

    add_test(NAME AudioEngine COMMAND AFRONOVA_Tests)
endif()

