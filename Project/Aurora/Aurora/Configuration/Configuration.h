#pragma once

class Configuration
{
public:
    Configuration(void);
    ~Configuration(void);

    static const Colour
        colorLigntWhite, // 0xfff3f3f0
        colorLightBlue, // 0xfff3f3f0
        colorLightRed,
        colorLightBlack;
public:
    juce_DeclareSingleton (Configuration, false)
};

juce_ImplementSingleton(Configuration)