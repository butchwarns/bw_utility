/*
Channels.h
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

#ifndef CHANNELS_H
#define CHANNELS_H

#include <JuceHeader.h>
#include "sizes.h"
#include "../typedefs.h"
#include "../PluginParameters.h"
#include "components/CheckboxLabeled.h"
#include "components/LabelTitle.h"

class Channels : public juce::Component
{
public:
    Channels(PluginParameters &p);

private:
    void paint(juce::Graphics &g) override;
    void resized() override;

    LabelTitle channels;
    ComboBox selector;
    CheckboxLabeled checkbox;

    std::unique_ptr<ComboBoxAttachment> attachment_selector;
    std::unique_ptr<ButtonAttachment> attachment_checkbox;
};

#endif // CHANNELS_H