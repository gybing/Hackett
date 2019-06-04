

struct CatmullRomAlgorithm
{
    static forcedinline float valueAtOffset (const float* const inputs, const float offset) noexcept
    {
        auto y0 = inputs[3];
        auto y1 = inputs[2];
        auto y2 = inputs[1];
        auto y3 = inputs[0];

        auto halfY0 = 0.5f * y0;
        auto halfY3 = 0.5f * y3;

        return y1 + offset * ((0.5f * y2 - halfY0)
                                + (offset * (((y0 + 2.0f * y2) - (halfY3 + 2.5f * y1))
                                              + (offset * ((halfY3 + 1.5f * y1) - (halfY0 + 1.5f * y2))))));
    }
};

CatmullRomInterpolator::CatmullRomInterpolator() noexcept  { reset(); }
CatmullRomInterpolator::~CatmullRomInterpolator() noexcept {}

void CatmullRomInterpolator::reset() noexcept
{
    subSamplePos = 1.0;

    for (auto& s : lastInputSamples)
        s = 0;
}

int CatmullRomInterpolator::process (double actualRatio, const float* in, float* out, int numOut, int available, int wrap) noexcept
{
    return interpolate<CatmullRomAlgorithm> (lastInputSamples, subSamplePos, actualRatio, in, out, numOut, available, wrap);
}

int CatmullRomInterpolator::process (double actualRatio, const float* in, float* out, int numOut) noexcept
{
    return interpolate<CatmullRomAlgorithm> (lastInputSamples, subSamplePos, actualRatio, in, out, numOut);
}

int CatmullRomInterpolator::processAdding (double actualRatio, const float* in, float* out, int numOut, int available, int wrap, float gain) noexcept
{
    return interpolateAdding<CatmullRomAlgorithm> (lastInputSamples, subSamplePos, actualRatio, in, out, numOut, available, wrap, gain);
}

int CatmullRomInterpolator::processAdding (double actualRatio, const float* in, float* out, int numOut, float gain) noexcept
{
    return interpolateAdding<CatmullRomAlgorithm> (lastInputSamples, subSamplePos, actualRatio, in, out, numOut, gain);
}


