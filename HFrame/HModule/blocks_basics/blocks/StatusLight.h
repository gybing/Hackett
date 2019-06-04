

/**
    Represents a status LED on a device.

    @tags{Blocks}
*/
class StatusLight
{
public:
    StatusLight (Block&);

    /** Destructor. */
    virtual ~StatusLight();

    //==============================================================================
    /** Returns a name to describe this light. */
    virtual String getName() const = 0;

    /** Changes the light's colour. */
    virtual bool setColour (LEDColour newColour) = 0;

    /** The device that this LED belongs to. */
    Block& block;

private:
    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StatusLight)
};


