

/** This topology source holds and applies a set of rules for transforming
    one device topology into another one that may involve virtual and/or
    aggregate devices.

    Given an input PhysicalTopologySource and a set of Rule objects, this class
     will apply the rules and present the resulting topology to clients.

    @tags{Blocks}
*/
class RuleBasedTopologySource  : public TopologySource
{
public:
    /** Creates a RuleBasedTopologySource which wraps another TopologySource
        passed in here.
    */
    RuleBasedTopologySource (TopologySource&);

    /** Destructor. */
    ~RuleBasedTopologySource() override;

    //==========================================================================
    /** Returns the currently active topology. */
    BlockTopology getCurrentTopology() const override;

    /** A rule that can transform parts of a topology. */
    struct Rule
    {
        virtual ~Rule() = default;

        /** Subclasses should implement this method and use it as their opportunity to
            examine the given topology and modify it. For example they may want to substitute
            one or more blocks for more specialised, aggregated Block objects.
        */
        virtual void transformTopology (BlockTopology&) = 0;
    };

    /** Clears the list of active rules.
        Calling this method will cause an asynchronous topology update if the new rule-set
        results in a change to the topology.
    */
    void clearRules();

    /** Adds a rule to the list that will be applied.
        The object passed-in will be owned by this object, so don't keep any references
        to it.
        Calling this method will cause an asynchronous topology update if the new rule-set
        results in a change to the topology.
    */
    void addRule (Rule*);

    /** Sets the TopologySource as active, occupying the midi port and trying to connect to the block devices */
    void setActive (bool shouldBeActive) override;

    /** Returns true, if the TopologySource is currently trying to connect the block devices */
    bool isActive() const override;

private:
    //==========================================================================
    struct Internal;
    std::unique_ptr<Internal> internal;
};


