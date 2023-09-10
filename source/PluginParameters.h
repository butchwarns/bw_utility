#ifndef __PLUGIN_PARAMETERS_H__
#define __PLUGIN_PARAMETERS_H__

#include <JuceHeader.h>
#include "../BDSP/source/decibel.h"
#include "dsp/SmoothLinear.h"
#include "../BDSP/source/maps.h"
#include "../BDSP/source/VoltPerOct.h"

typedef juce::AudioProcessorValueTreeState Apvts;

const juce::StringArray CHANNELS_CHOICES{"STEREO", "LEFT", "RIGHT"};
enum ChannelsChoice
{
    STEREO = 0,
    LEFT = 1,
    RIGTH = 2
};

// Tune control voltage conversion to 20Hz lowest freq (at -5V)
// With 10 octaves above (+-5V), the highest freq will be 20Hz * 2^10 = 20480Hz (at +5V)
const float ZERO_VOLT_FREQ_BASS_MONO = 20.0f * std::powf(2.0f, 5.0f);

class PluginParameters
{
public:
    PluginParameters(juce::AudioProcessor &processor);
    ~PluginParameters();

    void reset(double _sample_rate);

    juce::ValueTree copy_state();
    juce::Identifier state_type();
    void replace_state(juce::ValueTree const &new_state);

    ChannelsChoice channels();

    float volume();
    static float normalise_volume(float gain);
    float width();
    bool mono();
    bool bass_mono();
    float bass_mono_freq();
    static float normalise_bass_mono_freq(float freq);

private:
    Apvts apvts;
    Apvts::ParameterLayout parameter_layout();

    std::atomic<float> *channels_norm;
    std::atomic<float> *volume_norm;
    std::atomic<float> *width_norm;
    std::atomic<float> *mono_norm;
    std::atomic<float> *bass_mono_norm;
    std::atomic<float> *bass_mono_freq_norm;

    double sample_rate;
    SmoothLinear smooth_volume;
    SmoothLinear smooth_width;
    SmoothLinear smooth_bass_mono_freq;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginParameters)
};

#endif // __PLUGIN_PARAMETERS_H__