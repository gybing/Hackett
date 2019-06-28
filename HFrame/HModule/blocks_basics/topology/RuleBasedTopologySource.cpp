

struct RuleBasedTopologySource::Internal  : public TopologySource::Listener,
                                            private AsyncUpdater
{
    Internal (RuleBasedTopologySource& da, TopologySource& bd)  : owner (da), detector (bd)
    {
        detector.addListener (this);
    }

    ~Internal()
    {
        detector.removeListener (this);
    }

    void clearRules()
    {
        if (! rules.empty())
        {
            rules.clear();
            triggerAsyncUpdate();
        }
    }

    void addRule (Rule* r)
    {
        if (r != nullptr)
        {
            rules.add (r);
            triggerAsyncUpdate();
        }
    }

    void topologyChanged() override
    {
        cancelPendingUpdate();
        regenerateTopology();
    }

    void handleAsyncUpdate() override
    {
        topologyChanged();
    }

    void regenerateTopology()
    {
        auto newTopology = detector.getCurrentTopology();

        for (auto rule : rules)
            rule->transformTopology (newTopology);

        if (topology != newTopology)
        {
            topology = newTopology;
            owner.listeners.call ([] (TopologySource::Listener& l) { l.topologyChanged(); });
        }
    }

    void setActive (bool shouldBeActive)
    {
        detector.setActive (shouldBeActive);
    }

    bool isActive() const
    {
        return detector.isActive();
    }

    RuleBasedTopologySource& owner;
    TopologySource& detector;

    BlockTopology topology;
    OwnedArray<Rule> rules;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Internal)
};

RuleBasedTopologySource::RuleBasedTopologySource (TopologySource& d)
{
    internal.reset (new Internal (*this, d));
}

RuleBasedTopologySource::~RuleBasedTopologySource()
{
    internal = nullptr;
}

BlockTopology RuleBasedTopologySource::getCurrentTopology() const             { return internal->topology; }

void RuleBasedTopologySource::clearRules()                                    { internal->clearRules(); }
void RuleBasedTopologySource::addRule (Rule* r)                               { internal->addRule (r); }

void RuleBasedTopologySource::setActive (bool shouldBeActive)
{
    internal->setActive (shouldBeActive);
}

bool RuleBasedTopologySource::isActive() const
{
    return internal->isActive();
}


