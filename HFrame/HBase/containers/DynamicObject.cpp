

DynamicObject::DynamicObject()
{
}

DynamicObject::DynamicObject (const DynamicObject& other)
   : ReferenceCountedObject(), properties (other.properties)
{
}

DynamicObject::~DynamicObject()
{
}

bool DynamicObject::hasProperty (const Identifier& propertyName) const
{
    const var* const v = properties.getVarPointer (propertyName);
    return v != nullptr && ! v->isMethod();
}

const var& DynamicObject::getProperty (const Identifier& propertyName) const
{
    return properties [propertyName];
}

void DynamicObject::setProperty (const Identifier& propertyName, const var& newValue)
{
    properties.set (propertyName, newValue);
}

void DynamicObject::removeProperty (const Identifier& propertyName)
{
    properties.remove (propertyName);
}

bool DynamicObject::hasMethod (const Identifier& methodName) const
{
    return getProperty (methodName).isMethod();
}

var DynamicObject::invokeMethod (Identifier method, const var::NativeFunctionArgs& args)
{
    if (auto function = properties [method].getNativeFunction())
        return function (args);

    return {};
}

void DynamicObject::setMethod (Identifier name, var::NativeFunction function)
{
    properties.set (name, var (function));
}

void DynamicObject::clear()
{
    properties.clear();
}

void DynamicObject::cloneAllProperties()
{
    for (int i = properties.size(); --i >= 0;)
        if (auto* v = properties.getVarPointerAt (i))
            *v = v->clone();
}

DynamicObject::Ptr DynamicObject::clone()
{
    Ptr d (new DynamicObject (*this));
    d->cloneAllProperties();
    return d;
}

void DynamicObject::writeAsJSON (OutputStream& out, const int indentLevel, const bool allOnOneLine, int maximumDecimalPlaces)
{
    out << '{';
    if (! allOnOneLine)
        out << newLine;

    const int numValues = properties.size();

    for (int i = 0; i < numValues; ++i)
    {
        if (! allOnOneLine)
            JSONFormatter::writeSpaces (out, indentLevel + JSONFormatter::indentSize);

        out << '"';
        JSONFormatter::writeString (out, properties.getName (i));
        out << "\": ";
        JSONFormatter::write (out, properties.getValueAt (i), indentLevel + JSONFormatter::indentSize, allOnOneLine, maximumDecimalPlaces);

        if (i < numValues - 1)
        {
            if (allOnOneLine)
                out << ", ";
            else
                out << ',' << newLine;
        }
        else if (! allOnOneLine)
            out << newLine;
    }

    if (! allOnOneLine)
        JSONFormatter::writeSpaces (out, indentLevel);

    out << '}';
}


