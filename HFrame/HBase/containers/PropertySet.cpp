

PropertySet::PropertySet (bool ignoreCaseOfKeyNames)
    : properties (ignoreCaseOfKeyNames),
      fallbackProperties (nullptr),
      ignoreCaseOfKeys (ignoreCaseOfKeyNames)
{
}

PropertySet::PropertySet (const PropertySet& other)
    : properties (other.properties),
      fallbackProperties (other.fallbackProperties),
      ignoreCaseOfKeys (other.ignoreCaseOfKeys)
{
}

PropertySet& PropertySet::operator= (const PropertySet& other)
{
    properties = other.properties;
    fallbackProperties = other.fallbackProperties;
    ignoreCaseOfKeys = other.ignoreCaseOfKeys;

    propertyChanged();
    return *this;
}

PropertySet::~PropertySet()
{
}

void PropertySet::clear()
{
    const ScopedLock sl (lock);

    if (properties.size() > 0)
    {
        properties.clear();
        propertyChanged();
    }
}

String PropertySet::getValue (StringRef keyName, const String& defaultValue) const noexcept
{
    const ScopedLock sl (lock);
    auto index = properties.getAllKeys().indexOf (keyName, ignoreCaseOfKeys);

    if (index >= 0)
        return properties.getAllValues() [index];

    return fallbackProperties != nullptr ? fallbackProperties->getValue (keyName, defaultValue)
                                         : defaultValue;
}

int PropertySet::getIntValue (StringRef keyName, int defaultValue) const noexcept
{
    const ScopedLock sl (lock);
    auto index = properties.getAllKeys().indexOf (keyName, ignoreCaseOfKeys);

    if (index >= 0)
        return std::atoi(properties.getAllValues()[index].c_str());

    return fallbackProperties != nullptr ? fallbackProperties->getIntValue (keyName, defaultValue)
                                         : defaultValue;
}

double PropertySet::getDoubleValue (StringRef keyName, double defaultValue) const noexcept
{
    const ScopedLock sl (lock);
    auto index = properties.getAllKeys().indexOf (keyName, ignoreCaseOfKeys);

    if (index >= 0)
        return std::atof(properties.getAllValues()[index].c_str());

    return fallbackProperties != nullptr ? fallbackProperties->getDoubleValue (keyName, defaultValue)
                                         : defaultValue;
}

bool PropertySet::getBoolValue (StringRef keyName, bool defaultValue) const noexcept
{
    const ScopedLock sl (lock);
    auto index = properties.getAllKeys().indexOf (keyName, ignoreCaseOfKeys);

    if (index >= 0)
        return std::atoi(properties.getAllValues()[index].c_str()) != 0;

    return fallbackProperties != nullptr ? fallbackProperties->getBoolValue (keyName, defaultValue)
                                         : defaultValue;
}

std::unique_ptr<XmlElement> PropertySet::getXmlValue (StringRef keyName) const
{
    return parseXML (getValue (keyName));
}

void PropertySet::setValue (const String& keyName, const var& v)
{
    HAssert (!keyName.empty()); // shouldn't use an empty key name!

    if (!keyName.empty())
    {
        auto value = v.toString();
        const ScopedLock sl (lock);
        auto index = properties.getAllKeys().indexOf (keyName, ignoreCaseOfKeys);

        if (index < 0 || properties.getAllValues() [index] != value)
        {
            properties.set (keyName, value);
            propertyChanged();
        }
    }
}

void PropertySet::removeValue (StringRef keyName)
{
    if (!keyName.empty())
    {
        const ScopedLock sl (lock);
        auto index = properties.getAllKeys().indexOf (keyName, ignoreCaseOfKeys);

        if (index >= 0)
        {
            properties.remove (keyName);
            propertyChanged();
        }
    }
}

void PropertySet::setValue (const String& keyName, const XmlElement* xml)
{
    setValue (keyName, xml == nullptr ? var()
                                      : var (xml->toString (XmlElement::TextFormat().singleLine().withoutHeader())));
}

bool PropertySet::containsKey (StringRef keyName) const noexcept
{
    const ScopedLock sl (lock);
    return properties.getAllKeys().contains (keyName, ignoreCaseOfKeys);
}

void PropertySet::addAllPropertiesFrom (const PropertySet& source)
{
    const ScopedLock sl (source.getLock());

    for (int i = 0; i < source.properties.size(); ++i)
        setValue (source.properties.getAllKeys() [i],
                  source.properties.getAllValues() [i]);
}

void PropertySet::setFallbackPropertySet (PropertySet* fallbackProperties_) noexcept
{
    const ScopedLock sl (lock);
    fallbackProperties = fallbackProperties_;
}

std::unique_ptr<XmlElement> PropertySet::createXml (const String& nodeName) const
{
    auto xml = std::make_unique<XmlElement> (nodeName);

    const ScopedLock sl (lock);

    for (int i = 0; i < properties.getAllKeys().size(); ++i)
    {
        auto e = xml->createNewChildElement ("VALUE");
        e->setAttribute ("name", properties.getAllKeys()[i]);
        e->setAttribute ("val", properties.getAllValues()[i]);
    }

    return xml;
}

void PropertySet::restoreFromXml (const XmlElement& xml)
{
    const ScopedLock sl (lock);
    clear();

    forEachXmlChildElementWithTagName (xml, e, "VALUE")
    {
        if (e->hasAttribute ("name")
             && e->hasAttribute ("val"))
        {
            properties.set (e->getStringAttribute ("name"),
                            e->getStringAttribute ("val"));
        }
    }

    if (properties.size() > 0)
        propertyChanged();
}

void PropertySet::propertyChanged()
{
}


