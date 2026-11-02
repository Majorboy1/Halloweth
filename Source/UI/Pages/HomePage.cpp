#include "HomePage.h"

#include "../../PluginProcessor.h"
#include "../Knobs/LabeledKnob.h"
#include "../Theme.h"

namespace afronova
{
HomePage::HomePage (AfronovaAudioProcessor& p) : processor (p)
{
    knobs.add (new LabeledKnob ("MASTER", processor.apvts, param::masterGain));
    knobs.add (new LabeledKnob ("TONE", processor.apvts, param::tone));

    for (auto* knob : knobs)
        addAndMakeVisible (knob);
}

HomePage::~HomePage() = default;

void HomePage::resized()
{
    auto area = getLocalBounds();

    heroBounds = area.removeFromTop (juce::jlimit (78, 96, area.getHeight() / 7));
    area.removeFromTop (12);

    auto leftColumn = area.removeFromLeft (juce::jmax (270, area.getWidth() * 38 / 100));
    area.removeFromLeft (12);

    outputBounds = leftColumn;
    statusBounds = area;

    meterBounds = outputBounds.reduced (18, 0)
                              .withTrimmedTop (48)
                              .withHeight (12);

    auto knobArea = outputBounds.reduced (18, 0)
                                .withTrimmedTop (96)
                                .withTrimmedBottom (14);

    const auto knobWidth = juce::jmin (118, knobArea.getWidth() / juce::jmax (1, knobs.size()));
    const auto knobHeight = juce::jmax (86, juce::jmin (140, knobArea.getHeight()));

    auto x = knobArea.getX();

    for (auto* knob : knobs)
    {
        knob->setBounds (x, knobArea.getY(), knobWidth, knobHeight);
        x += knobWidth;
    }
}

void HomePage::updateMeters (float peak)
{
    meterDb = juce::Decibels::gainToDecibels (peak, -60.0f);
    const auto normalised = juce::jlimit (0.0f, 1.0f, juce::jmap (meterDb, -60.0f, 0.0f, 0.0f, 1.0f));

    if (std::abs (normalised - meterLevel) > 0.004f)
    {
        meterLevel = normalised;
        repaint (meterBounds.getUnion (meterBounds.translated (0, 22)).expanded (2));
    }
}

void HomePage::paint (juce::Graphics& g)
{
    // ---- Hero ----------------------------------------------------------------
    theme::drawCard (g, heroBounds);
    {
        auto area = heroBounds.reduced (22, 14);

        auto chipArea = area.removeFromRight (104);
        auto chip = chipArea.withSizeKeepingCentre (104, 24).toFloat();
        g.setColour (theme::accent.withAlpha (0.16f));
        g.fillRoundedRectangle (chip, 12.0f);
        g.setColour (theme::accentBright);
        g.setFont (theme::boldFont (9.5f));
        g.drawText ("VST3  ·  64-BIT", chip.toNearestInt(), juce::Justification::centred, false);

        g.setColour (theme::text);
        g.setFont (theme::boldFont (22.0f));
        g.drawText ("AFRICAN MUSIC PRODUCTION INSTRUMENT",
                    area.removeFromTop (28), juce::Justification::centredLeft, false);

        g.setColour (theme::textDim);
        g.setFont (theme::font (11.0f));
        g.drawFittedText ("Phase 3: multisample maps and the two-oscillator synth engine are live. "
                          "Switch engines and load instruments on the INSTRUMENTS page.",
                          area, juce::Justification::topLeft, 2);
    }

    // ---- Output card ---------------------------------------------------------
    theme::drawCard (g, outputBounds, "OUTPUT");

    {
        auto track = meterBounds.toFloat();
        g.setColour (theme::backgroundAlt);
        g.fillRoundedRectangle (track, 4.0f);

        if (meterLevel > 0.002f)
        {
            auto filled = track.withWidth (juce::jmax (4.0f, track.getWidth() * meterLevel));
            g.setGradientFill (juce::ColourGradient (theme::accentDeep, track.getX(), track.getY(),
                                                     theme::accentBright, track.getRight(),
                                                     track.getY(), false));
            g.fillRoundedRectangle (filled, 4.0f);
        }

        g.setColour (theme::outline);
        g.drawRoundedRectangle (track, 4.0f, 1.0f);

        g.setColour (meterLevel > 0.001f ? theme::text : theme::textFaint);
        g.setFont (theme::boldFont (10.5f));
        g.drawText (meterLevel > 0.001f ? juce::String (meterDb, 1) + " dB" : juce::String ("-inf dB"),
                    meterBounds.translated (0, 20).withHeight (14),
                    juce::Justification::centredLeft, false);
    }

    // ---- Build status --------------------------------------------------------
    theme::drawCard (g, statusBounds, "BUILD STATUS");

    {
        struct Row { const char* label; const char* note; juce::Colour colour; };

        const Row rows[]
        {
            { "Plugin shell & UI",            "done",    theme::success },
            { "MIDI, polyphony, pitch bend",  "done",    theme::success },
            { "Sample engine (WAV/AIFF)",     "done",    theme::success },
            { "Multisample maps + synth",     "done",    theme::success },
            { "Drum engine & pads",           "next",    theme::pending },
            { "Effects engine",              "phase 4", theme::textFaint },
            { "Presets & sample library",    "phase 5", theme::textFaint },
            { "Polish & release build",      "phase 6", theme::textFaint }
        };

        auto area = statusBounds.reduced (18, 0).withTrimmedTop (36).withTrimmedBottom (14);
        const auto rowHeight = juce::jlimit (15, 24, area.getHeight() / (int) juce::numElementsInArray (rows));

        for (const auto& row : rows)
        {
            auto line = area.removeFromTop (rowHeight);

            g.setColour (row.colour);
            g.fillEllipse (juce::Rectangle<float> (5.0f, 5.0f)
                               .withCentre ({ (float) line.getX() + 2.5f,
                                              (float) line.getCentreY() }));

            g.setColour (row.colour == theme::textFaint ? theme::textFaint : theme::textDim);
            g.setFont (theme::font (10.5f));
            g.drawText (row.label, line.withTrimmedLeft (14), juce::Justification::centredLeft, false);

            g.setColour (row.colour);
            g.setFont (theme::boldFont (9.5f));
            g.drawText (row.note, line, juce::Justification::centredRight, false);
        }
    }
}
}
