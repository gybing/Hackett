

//==============================================================================
/**
    Represents a MAC network card adapter address ID.

    @tags{Core}
*/
class API MACAddress  final
{
public:
    //==============================================================================
    /** Returns a list of the MAC addresses of all the available network cards. */
    static Array<MACAddress> getAllAddresses();

    /** Populates a list of the MAC addresses of all the available network cards. */
    static void findAllAddresses (Array<MACAddress>& results);

    //==============================================================================
    /** Creates a null address (00-00-00-00-00-00). */
    MACAddress() noexcept;

    /** Creates a copy of another address. */
    MACAddress (const MACAddress&) noexcept;

    /** Creates a copy of another address. */
    MACAddress& operator= (const MACAddress&) noexcept;

    /** Creates an address from 6 bytes. */
    explicit MACAddress (const uint8 bytes[6]) noexcept;

    /** Creates an address from a hex string.
        If the string isn't a 6-byte hex value, this will just default-initialise
        the object.
    */
    explicit MACAddress (const String& address);

    /** Returns a pointer to the 6 bytes that make up this address. */
    const uint8* getBytes() const noexcept        { return address; }

    /** Returns a dash-separated string in the form "11-22-33-44-55-66" */
    String toString() const;

    /** Returns a hex string of this address, using a custom separator between each byte. */
    String toString (const String& separator) const;

    /** Returns the address in the lower 6 bytes of an int64.

        This uses a little-endian arrangement, with the first byte of the address being
        stored in the least-significant byte of the result value.
    */
    int64 toInt64() const noexcept;

    /** Returns true if this address is null (00-00-00-00-00-00). */
    bool isNull() const noexcept;

    bool operator== (const MACAddress&) const noexcept;
    bool operator!= (const MACAddress&) const noexcept;

    //==============================================================================
private:
    uint8 address[6];
};


