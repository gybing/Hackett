

/** Base class for an entity that provides access to a blocks topology.

    @tags{Blocks}
*/
class TopologySource
{
public:
    //==========================================================================
    /** Destructor. */
    virtual ~TopologySource() = default;

    /** Returns the current topology that this object manages. */
    virtual BlockTopology getCurrentTopology() const = 0;

    /** Sets the TopologySource as active, occupying the midi port and trying to connect to the block devices */
    virtual void setActive (bool shouldBeActive) = 0;

    /** Returns true, if the TopologySource is currently trying to connect the block devices */
    virtual bool isActive() const = 0;

    //==========================================================================
    /** Used to receive callbacks for topology changes */
    struct Listener
    {
        virtual ~Listener() = default;

        /** Called for any change in topology - devices changed, connections changed, etc. */
        virtual void topologyChanged() {}

        /** Called when a new block is added to the topology. */
        virtual void blockAdded (const Block::Ptr) {}

        /** Called when a block is removed from the topology. */
        virtual void blockRemoved (const Block::Ptr) {}

        /** Called when a known block is updated.
            This could be becasue details have been reveived asyncroniously. E.g. Block name.
         */
        virtual void blockUpdated (const Block::Ptr) {}
    };

    void addListener (Listener* l)       { listeners.add (l); }
    void removeListener (Listener* l)    { listeners.remove (l); }

    /** Invoke this to force touches-off on all physical devices. */
    virtual void cancelAllActiveTouches() noexcept {}

protected:
    //==========================================================================
    ListenerList<Listener> listeners;
};


