/*
SliderRotary.cpp
Copyright (C) 2024 Butch Warns

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "SliderRotary.h"
#include "../colours.h"

SliderRotary::SliderRotary(PluginParameters &_p,
                           const String &param_id,
                           std::function<String(float value, int maximumStringLength)> _string_from_value,
                           std::function<std::optional<double>(const String &string)> _value_from_string)
    : p(_p),
      string_from_value(_string_from_value),
      value_from_string(_value_from_string),
      attachment(nullptr),
      shadow(),
      shadower(shadow)
{
    slider.addListener(this);

    addAndMakeVisible(&label);
    label.setJustificationType(Justification::centred);
    label.setEditable(true);
    label.addListener(this);
    label.setColour(TextEditor::highlightColourId, RED_TRANSPARENT);

    addAndMakeVisible(&slider);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    slider.setRange(0.0f, 1.0f, 0.0001f);

    attachment = std::make_unique<SliderAttachment>(p.apvts, param_id, slider);

    touch();

    shadow.colour = Colours::black.withAlpha(0.42f);
}

void SliderRotary::touch()
{
    sliderValueChanged(&slider);
}

void SliderRotary::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds().removeFromTop(KNOB_DIM);
    bounds.reduce((COLUMN2_WIDTH - KNOB_DIM) / 2, 0);
    bounds.reduce(PAD, PAD);

    Path knob_path;
    knob_path.addEllipse(bounds.toFloat().reduced(OUTLINE));

    shadow.offset = Point<int>(5, 5);
    shadow.radius = 12;
    shadow.drawForPath(g, knob_path);
}

void SliderRotary::resized()
{
    auto bounds = getLocalBounds();
    slider.setBounds(bounds.removeFromTop(KNOB_DIM));
    label.setBounds(bounds);
}

void SliderRotary::sliderValueChanged(Slider *s)
{
    const double val_norm = s->getValue();

    const String val_formatted = string_from_value((float)val_norm, 9);

    label.setText(val_formatted, dontSendNotification);
}

void SliderRotary::labelTextChanged(Label *labelThatHasChanged)
{
    const String text = labelThatHasChanged->getText();
    const std::optional<double> opt = value_from_string(text);

    if (opt.has_value())
    {
        set_value(opt.value());
    }
    else
    {
        touch();
    }
}

void SliderRotary::set_value(double val)
{
    const double val_old = slider.getValue();

    slider.setValue(val, sendNotification);

    // If value did not change, trigger update manually
    if (slider.getValue() - val_old < 0.0001)
    {
        sliderValueChanged(&slider);
    }
}