

NamedValueSet::NamedValue::NamedValue() noexcept {}
NamedValueSet::NamedValue::~NamedValue() noexcept {}

NamedValueSet::NamedValue::NamedValue (const Identifier& n, const var& v)  : name (n), value (v) {}
NamedValueSet::NamedValue::NamedValue (const NamedValue& other) : NamedValue (other.name, other.value) {}

NamedValueSet::NamedValue::NamedValue (NamedValue&& other) noexcept
   : NamedValue (std::move (other.name),
                 std::move (other.value))
{}

NamedValueSet::NamedValue::NamedValue (const Identifier& n, var&& v) noexcept
   : name (n), value (std::move (v))
{
}

NamedValueSet::NamedValue::NamedValue (Identifier&& n, var&& v) noexcept
   : name (std::move (n)),
     value (std::move (v))
{}

NamedValueSet::NamedValue& NamedValueSet::NamedValue::operator= (NamedValue&& other) noexcept
{
    name = std::move (other.name);
    value = std::move (other.value);
    return *this;
}

bool NamedValueSet::NamedValue::operator== (const NamedValue& other) const noexcept   { return name == other.name && value == other.value; }
bool NamedValueSet::NamedValue::operator!= (const NamedValue& other) const noexcept   { return ! operator== (other); }

//==============================================================================
NamedValueSet::NamedValueSet() noexcept {}
NamedValueSet::~NamedValueSet() noexcept {}

NamedValueSet::NamedValueSet (const NamedValueSet& other)  : values (other.values) {}

NamedValueSet::NamedValueSet (NamedValueSet&& other) noexcept
   : values (std::move (other.values)) {}

NamedValueSet::NamedValueSet (std::initializer_list<NamedValue> list)
   : values (std::move (list))
{
}

NamedValueSet& NamedValueSet::operator= (const NamedValueSet& other)
{
    clear();
    values = other.values;
    return *this;
}

NamedValueSet& NamedValueSet::operator= (NamedValueSet&& other) noexcept
{
    other.values.swap (values);
    return *this;
}

void NamedValueSet::clear()
{
    values.clear();
}

bool NamedValueSet::operator== (const NamedValueSet& other) const noexcept
{
    auto num = values.size();

    if (num != other.values.size())
        return false;

    for (int i = 0; i < num; ++i)
    {
        // optimise for the case where the keys are in the same order
        if (values[i].name == other.values[i].name)
        {
            if (values[i].value != other.values[i].value)
                return false;
        }
        else
        {
            // if we encounter keys that are in a different order, search remaining items by brute force..
            for (int j = i; j < num; ++j)
            {
                if (auto* otherVal = other.getVarPointer (values[j].name))
                    if (values[j].value == *otherVal)
                        continue;

                return false;
            }

            return true;
        }
    }

    return true;
}

bool NamedValueSet::operator!= (const NamedValueSet& other) const noexcept   { return ! operator== (other); }

size_t NamedValueSet::size() const noexcept        { return values.size(); }
bool NamedValueSet::empty() const noexcept    { return values.empty(); }

static const var& getNullVarRef() noexcept
{
    static var nullVar;
    return nullVar;
}

const var& NamedValueSet::operator[] (const Identifier& name) const noexcept
{
    if (auto* v = getVarPointer (name))
        return *v;

    return getNullVarRef();
}

var NamedValueSet::getWithDefault (const Identifier& name, const var& defaultReturnValue) const
{
    if (auto* v = getVarPointer (name))
        return *v;

    return defaultReturnValue;
}

var* NamedValueSet::getVarPointer (const Identifier& name) noexcept
{
    for (auto& i : values)
        if (i.name == name)
            return &(i.value);

    return {};
}

const var* NamedValueSet::getVarPointer (const Identifier& name) const noexcept
{
    for (auto& i : values)
        if (i.name == name)
            return &(i.value);

    return {};
}

bool NamedValueSet::set (const Identifier& name, var&& newValue)
{
    if (auto* v = getVarPointer (name))
    {
        if (v->equalsWithSameType (newValue))
            return false;

        *v = std::move (newValue);
        return true;
    }

    values.push_back ({ name, std::move (newValue) });
    return true;
}

bool NamedValueSet::set (const Identifier& name, const var& newValue)
{
    if (auto* v = getVarPointer (name))
    {
        if (v->equalsWithSameType (newValue))
            return false;

        *v = newValue;
        return true;
    }

    values.push_back ({ name, newValue });
    return true;
}

bool NamedValueSet::contains (const Identifier& name) const noexcept
{
    return getVarPointer (name) != nullptr;
}

int NamedValueSet::indexOf (const Identifier& name) const noexcept
{
    auto numValues = values.size();

    for (int i = 0; i < numValues; ++i)
        if (values[i].name == name)
            return i;

    return -1;
}

bool NamedValueSet::remove (const Identifier& name)
{
    auto numValues = values.size();

    for (int i = 0; i < numValues; ++i)
    {
        if (values[i].name == name)
        {
            values.erase (values.begin() + i);
            return true;
        }
    }

    return false;
}

Identifier NamedValueSet::getName (const int index) const noexcept
{
    if (isPositiveAndBelow (index, values.size()))
        return values[index].name;

    HAssertfalse;
    return {};
}

const var& NamedValueSet::getValueAt (const int index) const noexcept
{
    if (isPositiveAndBelow (index, values.size()))
        return values[index].value;

    HAssertfalse;
    return getNullVarRef();
}

var* NamedValueSet::getVarPointerAt (int index) noexcept
{
    if (isPositiveAndBelow (index, values.size()))
        return &(values[index].value);

    return {};
}

const var* NamedValueSet::getVarPointerAt (int index) const noexcept
{
    if (isPositiveAndBelow (index, values.size()))
        return &(values[index].value);

    return {};
}

void NamedValueSet::setFromXmlAttributes (const XmlElement& xml)
{
    values.clear();

    for (auto* att = xml.attributes.get(); att != nullptr; att = att->nextListItem)
    {
        if (att->name.toString().compare (0, 7, "base64:") == 0)
        {
            MemoryBlock mb;

            if (mb.fromBase64Encoding (att->value))
            {
                values.push_back ({ att->name.toString().substr (7), var (mb) });
                continue;
            }
        }

        values.push_back ({ att->name, var (att->value) });
    }
}

void NamedValueSet::copyToXmlAttributes (XmlElement& xml) const
{
    for (auto& i : values)
    {
        if (auto* mb = i.value.getBinaryData())
        {
            xml.setAttribute ("base64:" + i.name.toString(), mb->toBase64Encoding());
        }
        else
        {
            // These types can't be stored as XML!
            HAssert (! i.value.isObject());
            HAssert (! i.value.isMethod());
            HAssert (! i.value.isArray());

            xml.setAttribute (i.name.toString(),
                              i.value.toString());
        }
    }
}


