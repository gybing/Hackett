

namespace
{
    uint16 generateNoteID (int midiChannel, int midiNoteNumber) noexcept
    {
        HAssert (midiChannel > 0 && midiChannel <= 16);
        HAssert (midiNoteNumber >= 0 && midiNoteNumber < 128);

        return uint16 ((midiChannel << 7) + midiNoteNumber);
    }
}

//==============================================================================
MPENote::MPENote (int midiChannel_,
                  int initialNote_,
                  MPEValue noteOnVelocity_,
                  MPEValue pitchbend_,
                  MPEValue pressure_,
                  MPEValue timbre_,
                  KeyState keyState_) noexcept
    : noteID (generateNoteID (midiChannel_, initialNote_)),
      midiChannel (uint8 (midiChannel_)),
      initialNote (uint8 (initialNote_)),
      noteOnVelocity (noteOnVelocity_),
      pitchbend (pitchbend_),
      pressure (pressure_),
      initialTimbre (timbre_),
      timbre (timbre_),
      keyState (keyState_)
{
    HAssert (keyState != MPENote::off);
    HAssert (isValid());
}

MPENote::MPENote() noexcept {}

//==============================================================================
bool MPENote::isValid() const noexcept
{
    return midiChannel > 0 && midiChannel <= 16 && initialNote < 128;
}

//==============================================================================
double MPENote::getFrequencyInHertz (double frequencyOfA) const noexcept
{
    auto pitchInSemitones = double (initialNote) + totalPitchbendInSemitones;
    return frequencyOfA * std::pow (2.0, (pitchInSemitones - 69.0) / 12.0);
}

//==============================================================================
bool MPENote::operator== (const MPENote& other) const noexcept
{
    HAssert (isValid() && other.isValid());
    return noteID == other.noteID;
}

bool MPENote::operator!= (const MPENote& other) const noexcept
{
    HAssert (isValid() && other.isValid());
    return noteID != other.noteID;
}


//==============================================================================
//==============================================================================
#if HUNIT_TESTS

class MPENoteTests : public UnitTest
{
public:
    MPENoteTests()
        : UnitTest ("MPENote class", UnitTestCategories::midi)
    {}

    //==============================================================================
    void runTest() override
    {
        beginTest ("getFrequencyInHertz");
        {
            MPENote note;
            note.initialNote = 60;
            note.totalPitchbendInSemitones = -0.5;
            expectEqualsWithinOneCent (note.getFrequencyInHertz(), 254.178);
        }
    }

private:
    //==============================================================================
    void expectEqualsWithinOneCent (double frequencyInHertzActual,
                                    double frequencyInHertzExpected)
    {
        double ratio = frequencyInHertzActual / frequencyInHertzExpected;
        double oneCent = 1.0005946;
        expect (ratio < oneCent);
        expect (ratio > 1.0 / oneCent);
    }
};

static MPENoteTests MPENoteUnitTests;

#endif


