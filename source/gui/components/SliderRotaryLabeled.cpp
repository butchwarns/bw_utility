#include "SliderRotaryLabeled.h"

SliderRotaryLabeled::SliderRotaryLabeled(PluginParameters &p, ParameterID param_id)
    : slider(p, param_id)
{
    addAndMakeVisible(&label);

    addAndMakeVisible(&slider);
}

void SliderRotaryLabeled::paint(Graphics &g)
{
    ignoreUnused(g);
}

void SliderRotaryLabeled::resized()
{
    auto bounds = getLocalBounds();
    label.setBounds(bounds.removeFromTop(LABEL_HEIGHT));
    slider.setBounds(bounds);
}