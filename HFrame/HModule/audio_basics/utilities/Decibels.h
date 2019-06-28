

//==============================================================================
/**
    This class contains some helpful static methods for dealing with decibel values.

    @tags{Audio}
*/
class Decibels
{
public:
    //==============================================================================
    /** Converts a dBFS value to its equivalent gain level.

        A gain of 1.0 = 0 dB, and lower gains map onto negative decibel values. Any
        decibel value lower than minusInfinityDb will return a gain of 0.
    */
    template <typename Type>
    static Type decibelsToGain (Type decibels,
                                Type minusInfinityDb = Type (defaultMinusInfinitydB))
    {
        return decibels > minusInfinityDb ? std::pow (Type (10.0), decibels * Type (0.05))
                                          : Type();
    }

    /** Converts a gain level into a dBFS value.

        A gain of 1.0 = 0 dB, and lower gains map onto negative decibel values.
        If the gain is 0 (or negative), then the method will return the value
        provided as minusInfinityDb.
    */
    template <typename Type>
    static Type gainToDecibels (Type gain,
                                Type minusInfinityDb = Type (defaultMinusInfinitydB))
    {
        return gain > Type() ? jmax (minusInfinityDb, static_cast<Type> (std::log10 (gain)) * Type (20.0))
                             : minusInfinityDb;
    }

    //==============================================================================
    /** Converts a decibel reading to a string.

        By default the returned string will have the 'dB' suffix added, but this can be removed by
        setting the shouldIncludeSuffix argument to false. If a customMinusInfinityString argument
        is provided this will be returned if the value is lower than minusInfinityDb, otherwise
        the return value will be "-INF".
    */
    template <typename Type>
    static String toString (Type decibels,
                            int decimalPlaces = 2,
                            Type minusInfinityDb = Type (defaultMinusInfinitydB),
                            bool shouldIncludeSuffix = true,
                            StringRef customMinusInfinityString = {})
    {
        String s;
        s.preallocateBytes (20);

        if (decibels <= minusInfinityDb)
        {
            if (customMinusInfinityString.empty())
                s << "-INF";
            else
                s << customMinusInfinityString;
        }
        else
        {
            if (decibels >= Type())
                s << '+';

            if (decimalPlaces <= 0)
                s << roundToInt (decibels);
            else
                s << String (decibels, decimalPlaces);
        }

        if (shouldIncludeSuffix)
            s << " dB";

        return s;
    }

private:
    //==============================================================================
    enum { defaultMinusInfinitydB = -100 };

    Decibels() = delete; // This class can't be instantiated, it's just a holder for static methods..
};


