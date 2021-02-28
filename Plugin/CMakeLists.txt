project(DeepboxPlugin VERSION 0.0.1)

set(BaseTargetName DeepboxPlugin)

juce_add_plugin("${BaseTargetName}"
        # VERSION ...                               # Set this if the plugin version is different to the project version
        # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
        # ICON_SMALL ...
        COMPANY_NAME "Eugene Upston"
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE
        PLUGIN_MANUFACTURER_CODE Juce
        PLUGIN_CODE Dem0
        FORMATS AU VST3 Standalone
        PRODUCT_NAME "Deepbox")

juce_generate_juce_header(${BaseTargetName})

target_sources(${BaseTargetName} PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/AudioFeatureExtractor.cpp
        Source/DragMidiFile.cpp
)

target_compile_definitions(${BaseTargetName}
        PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(${BaseTargetName} PRIVATE
        juce::juce_audio_utils
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
#        frugally-deep::fdeep
        OnsetClassification
        essentia
)

get_target_property(active_formats ${BaseTargetName} JUCE_FORMATS)
foreach(kind IN LISTS active_formats)
    juce_add_bundle_resources_directory(${BaseTargetName}_${kind} ../Resources/wavs)
    juce_add_bundle_resources_directory(${BaseTargetName}_${kind} ../Resources/imgs)
    juce_add_bundle_resources_directory(${BaseTargetName}_${kind} ../Resources/models)
endforeach()
