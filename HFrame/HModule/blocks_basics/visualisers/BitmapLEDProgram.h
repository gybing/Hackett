

/**
    A simple Program to set the colours of individual LEDs.

    @tags{Blocks}
*/
struct BitmapLEDProgram   : public Block::Program
{
    BitmapLEDProgram (Block&);

    /** Set the colour of the LED at coordinates {x, y}. */
    void setLED (uint32 x, uint32 y, LEDColour);

private:
    String getLittleFootProgram() override;
};


