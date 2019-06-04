

BitmapLEDProgram::BitmapLEDProgram (Block& b)  : Program (b) {}

/*
    The heap format for this program is just an array of 15x15 5:6:5 colours,
    and the program just copies them onto the screen each frame.
*/

void BitmapLEDProgram::setLED (uint32 x, uint32 y, LEDColour colour)
{
    if (auto ledGrid = block.getLEDGrid())
    {
        auto w = (uint32) ledGrid->getNumColumns();
        auto h = (uint32) ledGrid->getNumRows();

        if (x < w && y < h)
        {
            auto bit = (x + y * w) * 16;

            block.setDataBits (bit,      5, colour.getRed()   >> 3);
            block.setDataBits (bit + 5,  6, colour.getGreen() >> 2);
            block.setDataBits (bit + 11, 5, colour.getBlue()  >> 3);
        }
    }
    else
    {
        HAssertfalse;
    }
}

String BitmapLEDProgram::getLittleFootProgram()
{
    String program (R"littlefoot(

    #heapsize: 15 * 15 * 2

    void repaint()
    {
        for (int y = 0; y < NUM_ROWS; ++y)
        {
            for (int x = 0; x < NUM_COLUMNS; ++x)
            {
                int bit = (x + y * NUM_COLUMNS) * 16;

                fillPixel (makeARGB (255,
                                     getHeapBits (bit,      5) << 3,
                                     getHeapBits (bit + 5,  6) << 2,
                                     getHeapBits (bit + 11, 5) << 3), x, y);
            }
        }
    }

    )littlefoot");

    if (auto ledGrid = block.getLEDGrid())
        return program.replace ("NUM_COLUMNS", String (ledGrid->getNumColumns()))
                      .replace ("NUM_ROWS",    String (ledGrid->getNumRows()));

    HAssertfalse;
    return {};
}


