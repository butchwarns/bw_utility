#include "PluginParameters.h"
#include "../BDSP/source/utility.h"

constexpr double VOLUME_MIN = -66.1;
constexpr double VOLUME_MAX = 35.0;
constexpr double VOLUME_OFF_THRESHOLD = VOLUME_MIN + 0.1;

PluginParameters::PluginParameters(juce::AudioProcessor &processor)
    : apvts(processor, nullptr, "parameters", parameter_layout())
{
    channels_norm = apvts.getRawParameterValue("channels");
    volume_norm = apvts.getRawParameterValue("volume");
    width_norm = apvts.getRawParameterValue("width");
    mono_norm = apvts.getRawParameterValue("mono");
    bass_mono_norm = apvts.getRawParameterValue("bass_mono");
    bass_mono_freq_norm = apvts.getRawParameterValue("bass_mono_freq");
    phase_flip_l_norm = apvts.getRawParameterValue("phase_flip_l");
    phase_flip_r_norm = apvts.getRawParameterValue("phase_flip_r");
    pan_norm = apvts.getRawParameterValue("pan");
    dc_block_norm = apvts.getRawParameterValue("dc_block");
}

ChannelsChoice PluginParameters::channels()
{
    const auto choice = static_cast<ChannelsChoice>((int)*channels_norm);
    return choice;
}

String PluginParameters::channels_string_from_index(int value, int max_string_len)
{
    String choice = CHANNELS_CHOICES[value];

    constrain_string_length(choice, max_string_len);

    return choice;
}

double PluginParameters::volume()
{
    const double gain = denormalise_volume((double)*volume_norm);
    return gain;
}

double PluginParameters::normalise_volume(double gain)
{
    const double db = bdsp::decibel::raw_gain_to_db(gain);

    const auto val_norm = normalise_volume_db(db);

    return val_norm;
}
double PluginParameters::normalise_volume_db(double db)
{
    const auto val_norm = bdsp::mappings::normalise<double>(db, VOLUME_MIN, VOLUME_MAX);

    return val_norm;
}

double PluginParameters::denormalise_volume(double val_norm)
{
    const auto db = denormalise_volume_db(val_norm);

    const double gain = bdsp::decibel::db_to_raw_gain_off(db, VOLUME_MIN + VOLUME_OFF_THRESHOLD);

    return gain;
}

double PluginParameters::denormalise_volume_db(double val_norm)
{
    const auto db = bdsp::mappings::map_linear_norm<double>(val_norm, VOLUME_MIN, VOLUME_MAX);

    return db;
}

String PluginParameters::volume_string_from_value(double value, int max_string_len)
{
    const double val_denorm = denormalise_volume_db(value);

    std::stringstream val_formatted;
    if (val_denorm <= VOLUME_OFF_THRESHOLD)
    {
        // Below threshold, turn off
        val_formatted << "OFF";
    }
    else // Default
    {
        // Format value string to the correct number of decimal places
        const int num_decimal_places = 1;
        const String value_suffix = "dB";
        val_formatted << std::fixed << std::setprecision(num_decimal_places);
        val_formatted << val_denorm << value_suffix;
    }

    String val_formatted_str = val_formatted.str();

    constrain_string_length(val_formatted_str, max_string_len);

    return val_formatted_str;
}

std::optional<double> PluginParameters::volume_value_from_string(const String &string)
{
    if (string.toLowerCase() == "off")
    {
        return 0.0;
    }
    else
    {
        if (string.endsWithIgnoreCase("dB"))
        {
            string.dropLastCharacters(2);
        }

        try
        {
            double value = 0.0;
            value = std::stod(string.toStdString());
            value = normalise_volume_db(value);
            return value;
        }
        catch (const std::invalid_argument &e)
        {
        }
        catch (const std::out_of_range &e)
        {
        }
    }

    return std::nullopt;
}

double PluginParameters::width()
{
    return denormalise_width((double)*width_norm);
}

double PluginParameters::normalise_width(double width)
{
    return width / 4.0;
}

double PluginParameters::normalise_width_percent(double width_percent)
{
    return width_percent / 400.0;
}

double PluginParameters::denormalise_width(double val_norm)
{
    return 4.0 * val_norm;
}

double PluginParameters::denormalise_width_percent(double val_norm)
{
    return 400.0 * val_norm;
}

String PluginParameters::width_string_from_value(double value, int max_string_len)
{
    // Convert to percent (0 to 400)
    const double val_denorm = denormalise_width_percent(value);

    std::stringstream val_formatted;
    const int num_decimal_places = 0;
    const String value_suffix = "%";
    val_formatted << std::fixed << std::setprecision(num_decimal_places);
    val_formatted << val_denorm << value_suffix;

    String val_formatted_str = val_formatted.str();

    constrain_string_length(val_formatted_str, max_string_len);

    return val_formatted_str;
}

std::optional<double> PluginParameters::width_value_from_string(const String &string)
{
    if (string.endsWithIgnoreCase("%"))
    {
        string.dropLastCharacters(1);
    }

    try
    {
        double value = 0.0;
        value = std::stod(string.toStdString());
        value = normalise_width_percent(value);
        return value;
    }
    catch (const std::invalid_argument &e)
    {
    }
    catch (const std::out_of_range &e)
    {
    }

    return std::nullopt;
}

bool PluginParameters::mono()
{
    return static_cast<bool>(*mono_norm);
}

String PluginParameters::mono_string_from_bool(bool value, int max_string_len)
{
    String s;
    if (value)
    {
        s = "MONO";
    }
    else
    {
        s = "STEREO";
    }

    constrain_string_length(s, max_string_len);

    return s;
}

bool PluginParameters::bass_mono()
{
    return static_cast<bool>(*bass_mono_norm);
}

String PluginParameters::bass_mono_string_from_bool(bool value, int max_string_len)
{
    String s;
    if (value)
    {
        s = "BASS MONO";
    }
    else
    {
        s = "BASS STEREO";
    }

    constrain_string_length(s, max_string_len);

    return s;
}

double PluginParameters::bass_mono_freq()
{
    const double freq = denormalise_bass_mono_freq((double)*bass_mono_freq_norm);

    return freq;
}

double PluginParameters::normalise_bass_mono_freq(double freq)
{
    const double cv = bdsp::cv::VoltPerOctave<double>::freq_to_volt(freq, ZERO_VOLT_FREQ_BASS_MONO);
    const double cv_norm = bdsp::mappings::normalise(cv, -5.0, 5.0);

    return cv_norm;
}

double PluginParameters::denormalise_bass_mono_freq(double val_norm)
{
    const double cv = bdsp::mappings::map_linear_norm(val_norm, -5.0, 5.0);
    const double freq = bdsp::cv::VoltPerOctave<double>::volt_to_freq(cv, ZERO_VOLT_FREQ_BASS_MONO);

    return freq;
}

String PluginParameters::bass_mono_freq_string_from_value(double value, int max_string_len)
{
    const double val_denorm = denormalise_bass_mono_freq(value);

    std::stringstream val_formatted;
    const int num_decimal_places = 0;
    const String value_suffix = "Hz";
    val_formatted << std::fixed << std::setprecision(num_decimal_places);
    val_formatted << val_denorm << value_suffix.toStdString();

    String val_formatted_str = val_formatted.str();

    constrain_string_length(val_formatted_str, max_string_len);

    return val_formatted_str;
}

double PluginParameters::phase_flip_l()
{
    return denormalise_phase_flip((double)*phase_flip_l_norm);
}

double PluginParameters::phase_flip_r()
{
    return denormalise_phase_flip((double)*phase_flip_r_norm);
}

double PluginParameters::denormalise_phase_flip(double val_norm)
{
    return -1.0 * bdsp::mappings::unipolar_to_bipolar((double)val_norm);
}

String PluginParameters::phase_flip_l_string_from_bool(bool value, int max_string_len)
{
    String s;
    if (value)
    {
        s = "L FLIPPED";
    }
    else
    {
        s = "L NOT FLIPPED";
    }

    constrain_string_length(s, max_string_len);

    return s;
}

String PluginParameters::phase_flip_r_string_from_bool(bool value, int max_string_len)
{
    String s;
    if (value)
    {
        s = "R FLIPPED";
    }
    else
    {
        s = "R NOT FLIPPED";
    }

    constrain_string_length(s, max_string_len);

    return s;
}

double PluginParameters::pan()
{
    return denormalise_pan((double)*pan_norm);
}

double PluginParameters::denormalise_pan(double val_norm)
{
    return bdsp::mappings::map_linear_norm(val_norm, 0.0, bdsp::constants::PI / 2.0);
}

String PluginParameters::pan_string_from_value(double value, int max_string_len)
{
    std::stringstream val_formatted;
    const double centre_range = 0.005;
    val_formatted << std::fixed << std::setprecision(0);
    if (value < 0.5 - centre_range)
    {
        val_formatted << bdsp::mappings::map_linear(value, 0.0, 0.5, 50.0, 0.0) << "L";
    }
    else if (value > 0.5 + centre_range)
    {
        val_formatted << bdsp::mappings::map_linear(value, 0.5, 1.0, 0.0, 50.0) << "R";
    }
    else
    {
        val_formatted << "C";
    }

    String val_formatted_str = val_formatted.str();

    constrain_string_length(val_formatted_str, max_string_len);

    return val_formatted_str;
}

std::optional<double> PluginParameters::pan_value_from_string(const String &string)
{
    if (string.toLowerCase() == "c")
    {
        return 0.5;
    }
    else if (string.endsWithIgnoreCase("L"))
    {
        try
        {
            string.dropLastCharacters(1);
            double value = 0.0;
            value = std::stod(string.toStdString());
            value = bdsp::mappings::map_linear<double>(value, 0, 50, 0.5, 0.0);
            return value;
        }
        catch (const std::invalid_argument &e)
        {
        }
        catch (const std::out_of_range &e)
        {
        }
    }
    else if (string.endsWithIgnoreCase("R"))
    {
        try
        {
            string.dropLastCharacters(1);
            double value = 0.0;
            value = std::stod(string.toStdString());
            value = bdsp::mappings::map_linear<double>(value, 0, 50, 0.5, 1.0);
            return value;
        }
        catch (const std::invalid_argument &e)
        {
        }
        catch (const std::out_of_range &e)
        {
        }
    }
    else
    {
        try
        {
            double value = 0.0;
            value = std::stod(string.toStdString());
            if (value == 0.0)
            {
                return 0.5;
            }
        }
        catch (const std::invalid_argument &e)
        {
        }
        catch (const std::out_of_range &e)
        {
        }
    }

    return std::nullopt;
}

bool PluginParameters::dc_block()
{
    return (bool)*dc_block_norm;
}

String PluginParameters::dc_block_string_from_bool(bool value, int max_string_len)
{
    String s;
    if (value)
    {
        s = "DC BLOCKED";
    }
    else
    {
        s = "DC NOT BLOCKED";
    }

    constrain_string_length(s, max_string_len);

    return s;
}

Apvts::ParameterLayout PluginParameters::parameter_layout()
{
    Apvts::ParameterLayout layout;

    typedef juce::AudioProcessorParameterGroup ParameterGroup;

    std::unique_ptr<ParameterGroup> phase_flip_grp = std::make_unique<ParameterGroup>("phase_flip", "PHASE_FLIP", "|");
    phase_flip_grp->addChild(std::make_unique<juce::AudioParameterBool>("phase_flip_l", "PHASE_FLIP_L", false, "", phase_flip_l_string_from_bool));
    phase_flip_grp->addChild(std::make_unique<juce::AudioParameterBool>("phase_flip_r", "PHASE_FLIP_R", false, "", phase_flip_r_string_from_bool));

    std::unique_ptr<ParameterGroup> channels_grp = std::make_unique<ParameterGroup>("channels", "CHANNELS", "|");
    channels_grp->addChild(std::make_unique<juce::AudioParameterChoice>("channels", "CHANNELS", CHANNELS_CHOICES, 0, "", channels_string_from_index));
    channels_grp->addChild(std::make_unique<juce::AudioParameterBool>("mono", "MONO", false, "", mono_string_from_bool));

    std::unique_ptr<ParameterGroup> bass_mono_grp = std::make_unique<ParameterGroup>("bass_mono", "BASS_MONO", "|");
    bass_mono_grp->addChild(std::make_unique<juce::AudioParameterBool>("bass_mono", "BASS_MONO", false, "", bass_mono_string_from_bool));
    const auto bass_mono_freq_default = (float)normalise_bass_mono_freq(120.0);
    bass_mono_grp->addChild(std::make_unique<juce::AudioParameterFloat>("bass_mono_freq", "BASS_MONO_FREQ", NormalisableRange<float>(0.0f, 1.0f, 0.0001f), bass_mono_freq_default, "", AudioProcessorParameter::genericParameter, bass_mono_freq_string_from_value));

    std::unique_ptr<ParameterGroup> sliders_grp = std::make_unique<ParameterGroup>("sliders", "SLIDERS", "|");
    const auto width_default = (float)normalise_width_percent(100);
    sliders_grp->addChild(std::make_unique<juce::AudioParameterFloat>("width", "WIDTH", NormalisableRange<float>(0.0f, 1.0f, 0.0025f), width_default, "", AudioProcessorParameter::genericParameter, width_string_from_value));
    const auto volume_default = (float)normalise_volume_db(0.0);
    sliders_grp->addChild(std::make_unique<juce::AudioParameterFloat>("volume", "VOLUME", NormalisableRange<float>(0.0f, 1.0f, 0.0000001f), volume_default, "", AudioProcessorParameter::genericParameter, volume_string_from_value));
    sliders_grp->addChild(std::make_unique<juce::AudioParameterFloat>("pan", "PAN", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f, "", AudioProcessorParameter::genericParameter, pan_string_from_value));

    std::unique_ptr<ParameterGroup> dc_block_group = std::make_unique<ParameterGroup>("dc_block", "DC_BLOCK", "|");
    dc_block_group->addChild(std::make_unique<juce::AudioParameterBool>("dc_block", "DC_BLOCK", false, "", dc_block_string_from_bool));

    layout.add(std::move(phase_flip_grp), std::move(channels_grp), std::move(bass_mono_grp), std::move(sliders_grp), std::move(dc_block_group));

    return layout;
}

void PluginParameters::constrain_string_length(String &s, int max_len)
{
    const int len = s.length();
    if (len > max_len)
    {
        s = s.dropLastCharacters(len - max_len);
    }
}