

/**
    A simple ARGB colour class for setting LEDs.

    @tags{Blocks}
*/
struct LEDColour
{
    LEDColour() = default;
    LEDColour (const LEDColour&) = default;
    LEDColour& operator= (const LEDColour&) = default;

    LEDColour (uint32 argbColour) noexcept : argb (argbColour) {}

    template <typename ColourType>
    LEDColour (const ColourType& colour)  : LEDColour (colour.getARGB()) {}

    uint8 getAlpha() const noexcept         { return (uint8) (argb >> 24); }
    uint8 getRed() const noexcept           { return (uint8) (argb >> 16); }
    uint8 getGreen() const noexcept         { return (uint8) (argb >> 8); }
    uint8 getBlue() const noexcept          { return (uint8) argb; }

    uint32 getARGB() const noexcept         { return argb; }

    uint32 argb = 0;
};


//==============================================================================
/**
    Represents a 2D grid of LEDs on a block device.

    @tags{Blocks}
*/
class LEDGrid
{
public:
    LEDGrid (Block&);

    /** Destructor. */
    virtual ~LEDGrid();

    //==============================================================================
    /** Returns the number of columns in the LED grid. */
    virtual int getNumColumns() const = 0;

    /** Returns the number of rows in the LED grid. */
    virtual int getNumRows() const = 0;

    //==============================================================================
    /** An interface to use for LEDGrid rendering. */
    struct Renderer     : public ReferenceCountedObject
    {
        ~Renderer() override;
        virtual void renderLEDGrid (LEDGrid&) = 0;

        /** The Renderer class is reference-counted, so always use a Renderer::Ptr when
            you are keeping references to them.
         */
        using Ptr = ReferenceCountedObjectPtr<Renderer>;
    };

    /** Set the visualiser that will create visuals for this block (nullptr for none).
        Note that the LEDGrid will NOT take ownership of this object, so the caller
        must ensure that it doesn't get deleted while in use here.
    */
    void setRenderer (Renderer::Ptr newRenderer) noexcept   { renderer = newRenderer; }

    /** Returns the visualiser currently attached to this block (nullptr for none). */
    Renderer::Ptr getRenderer() const noexcept              { return renderer; }

    /** The device that this LEDGrid belongs to. */
    Block& block;

private:
    Renderer::Ptr renderer;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LEDGrid)
};


