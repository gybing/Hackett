

MPESynthesiserVoice::MPESynthesiserVoice()
{
}

MPESynthesiserVoice::~MPESynthesiserVoice()
{
}

//==============================================================================
bool MPESynthesiserVoice::isCurrentlyPlayingNote (MPENote note) const noexcept
{
    return isActive() && currentlyPlayingNote.noteID == note.noteID;
}

bool MPESynthesiserVoice::isPlayingButReleased() const noexcept
{
    return isActive() && currentlyPlayingNote.keyState == MPENote::off;
}

void MPESynthesiserVoice::clearCurrentNote() noexcept
{
    currentlyPlayingNote = MPENote();
}


