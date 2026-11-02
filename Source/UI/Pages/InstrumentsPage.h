#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace afronova
{
class AfronovaAudioProcessor;
class LabeledKnob;

/** Instruments page.

    Two engines are available and both are real:
    - SAMPLE MAP: loads a WAV/AIFF (single zone) or a JSON multisample map.
    - SYNTH ENGINE: two-oscillator subtractive synth with filter.

    Every control acts on the running engine. */
class InstrumentsPage : public juce::Component
{
public:
    explicit InstrumentsPage (AfronovaAudioProcessor&);
    ~InstrumentsPage() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    /** Called from the editor timer; refreshes labels when a load completes. */
    void pollProcessor();

private:
    void chooseSampleFile();
    void chooseMapFile();
    void refresh();
    void setEngineMode (int mode);

    AfronovaAudioProcessor& processor;

    juce::OwnedArray<LabeledKnob> envelopeKnobs;
    juce::OwnedArray<LabeledKnob> synthKnobs;

    juce::TextButton engineSampleButton, engineSynthButton;
    juce::TextButton loadFileButton, loadMapButton, clearButton, loopButton;
    juce::ComboBox waveCombo;

    juce::Label nameLabel, detailsLabel, statusLabel;

    std::unique_ptr<juce::FileChooser> fileChooser;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> loopAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveAttachment;

    juce::Rectangle<int> headerBounds, sampleBounds, synthBounds, envelopeBounds;
    juce::String engineStatusText;
    int lastRevision = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentsPage)
};
}
