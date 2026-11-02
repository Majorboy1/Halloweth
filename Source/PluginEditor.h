#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include <array>

#include "UI/AfronovaLookAndFeel.h"
#include "UI/NavigationBar.h"
#include "UI/Pages/DrumsPage.h"
#include "UI/Pages/EffectsPage.h"
#include "UI/Pages/HomePage.h"
#include "UI/Pages/InstrumentsPage.h"
#include "UI/Pages/LibraryPage.h"
#include "UI/Pages/PlaceholderPage.h"
#include "UI/PresetBar.h"

namespace afronova
{
class AfronovaAudioProcessor;

class AfronovaAudioProcessorEditor : public juce::AudioProcessorEditor,
                                    private juce::Timer
{
public:
    explicit AfronovaAudioProcessorEditor (AfronovaAudioProcessor&);
    ~AfronovaAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void showPage (NavigationBar::Page);

    AfronovaAudioProcessor& processor;
    AfronovaLookAndFeel lookAndFeel;

    NavigationBar navBar;
    PresetBar presetBar;

    HomePage homePage;
    InstrumentsPage instrumentsPage;
    DrumsPage drumsPage;
    LibraryPage libraryPage;
    EffectsPage effectsPage;
    PlaceholderPage vocalsPage, settingsPage;

    std::array<juce::Component*, (size_t) NavigationBar::Page::numPages> pages {};

    juce::MidiKeyboardComponent keyboard;

    float displayPeak = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AfronovaAudioProcessorEditor)
};
}
