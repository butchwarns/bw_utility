#include "PluginEditor.h"
#include "PluginProcessor.h"

PluginEditor::PluginEditor(PluginProcessor &p, PluginParameters &params)
    : AudioProcessorEditor(&p), processorRef(p), sliders(params), channels(params), bass_mono(params), phase_flip(params), dc_block(params)
{
    juce::ignoreUnused(processorRef);

    // Get (usable) screen size
    // const juce::Displays::Display *display = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay();
    // juce::Rectangle<int> user_area = display->userArea;

    setSize(WIN_WIDTH, WIN_HEIGHT);

    setLookAndFeel(&look);

    addAndMakeVisible(&spacer_vertical);

    addAndMakeVisible(&header);
    addAndMakeVisible(&phase_flip);
    addAndMakeVisible(&spacer1);
    addAndMakeVisible(&channels);
    addAndMakeVisible(&spacer2);
    addAndMakeVisible(&bass_mono);
    addAndMakeVisible(&spacer3);
    addAndMakeVisible(&dc_block);
    addAndMakeVisible(&footer);
    addAndMakeVisible(&sliders);
}

void PluginEditor::paint(juce::Graphics &g)
{
    g.fillAll(BLUE);
}

void PluginEditor::resized()
{
    auto bounds = getLocalBounds();

    header.setBounds(bounds.removeFromTop(HEADER_HEIGHT));
    footer.setBounds(bounds.removeFromBottom(FOOTER_HEIGHT));

    bounds.reduce(PAD, PAD);

    auto col1_bounds = bounds.removeFromLeft(COLUMN1_WIDTH);
    col1_bounds.reduce(0, (col1_bounds.getHeight() - COLUMN1_HEIGHT) / 2);

    spacer_vertical.setBounds(bounds.removeFromLeft(SPACER_DIM));

    auto col2_bounds = bounds;

    phase_flip.setBounds(col1_bounds.removeFromTop(PHASE_FLIP_HEIGHT));
    spacer1.setBounds(col1_bounds.removeFromTop(SPACER_DIM));

    channels.setBounds(col1_bounds.removeFromTop(CHANNELS_HEIGHT));
    spacer2.setBounds(col1_bounds.removeFromTop(SPACER_DIM));

    bass_mono.setBounds(col1_bounds.removeFromTop(BASS_MONO_HEIGHT));
    spacer3.setBounds(col1_bounds.removeFromTop(SPACER_DIM));

    dc_block.setBounds(col1_bounds.removeFromTop(DC_BLOCK_HEIGHT));

    sliders.setBounds(col2_bounds);
}
