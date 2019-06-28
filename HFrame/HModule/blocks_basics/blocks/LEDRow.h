

/**
    Represents an LED strip on a device.

    @tags{Blocks}
*/
class LEDRow
{
public:
    LEDRow (Block&);

    /** Destructor. */
    virtual ~LEDRow();

    //==============================================================================
    /** Return the number of LEDs in the row. */
    virtual int getNumLEDs() const = 0;

    /** Sets the colour of the corresponding LED. */
    virtual void setLEDColour (int index, LEDColour newColour) = 0;

    /** Overlays all LEDs with a single colour.

        Whilst the overlay is set subsequent calls to setLEDColour will happen
        *behind* the overlay, and will be invisible to the user until the
        overlay is removed.
    */
    virtual void setOverlayColour (LEDColour newColour) = 0;

    /* Removes an overlay colour. */
    virtual void resetOverlayColour() = 0;

    /** The device that these lights belong to. */
    Block& block;

private:
    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LEDRow)
};


