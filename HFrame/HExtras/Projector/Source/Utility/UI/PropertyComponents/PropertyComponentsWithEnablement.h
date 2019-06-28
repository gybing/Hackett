/*
  ==============================================================================

   This file is part of the H library.
   Copyright (c) 2017 - ROLI Ltd.

   H is an open source library subject to commercial or open-source
   licensing.

   By using H, you agree to the terms of both the H 5 End-User License
   Agreement and H 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.H.com/H-5-licence
   Privacy Policy: www.H.com/H-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   H IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#pragma once


//==============================================================================
class TextPropertyComponentWithEnablement    : public TextPropertyComponent,
                                               private Value::Listener
{
public:
    TextPropertyComponentWithEnablement (ValueWithDefault& valueToControl,
                                         ValueWithDefault valueToListenTo,
                                         const String& propertyName,
                                         int maxNumChars,
                                         bool isMultiLine)
        : TextPropertyComponent (valueToControl, propertyName, maxNumChars, isMultiLine),
          valueWithDefault (valueToListenTo),
          value (valueWithDefault.getPropertyAsValue())
    {
        value.addListener (this);
        setEnabled (valueWithDefault.get());
    }

    ~TextPropertyComponentWithEnablement() override    { value.removeListener (this); }

private:
    ValueWithDefault valueWithDefault;
    Value value;

    void valueChanged (Value&) override       { setEnabled (valueWithDefault.get()); }
};

//==============================================================================
class ChoicePropertyComponentWithEnablement    : public ChoicePropertyComponent,
                                                 private Value::Listener
{
public:
    ChoicePropertyComponentWithEnablement (ValueWithDefault& valueToControl,
                                           ValueWithDefault valueToListenTo,
                                           const String& propertyName,
                                           const StringArray& choices,
                                           const Array<var>& correspondingValues)
        : ChoicePropertyComponent (valueToControl, propertyName, choices, correspondingValues),
          valueWithDefault (valueToListenTo),
          value (valueToListenTo.getPropertyAsValue())
    {
        value.addListener (this);
        valueChanged (value);
    }

    ChoicePropertyComponentWithEnablement (ValueWithDefault& valueToControl,
                                           ValueWithDefault valueToListenTo,
                                           const Identifier& multiChoiceID,
                                           const String& propertyName,
                                           const StringArray& choices,
                                           const Array<var>& correspondingValues)
        : ChoicePropertyComponentWithEnablement (valueToControl, valueToListenTo, propertyName, choices, correspondingValues)
    {
        HAssert (valueToListenTo.get().getArray() != nullptr);

        isMultiChoice = true;
        idToCheck = multiChoiceID;

        valueChanged (value);
    }

    ~ChoicePropertyComponentWithEnablement() override    { value.removeListener (this); }

private:
    ValueWithDefault valueWithDefault;
    Value value;

    bool isMultiChoice = false;
    Identifier idToCheck;

    bool checkMultiChoiceVar() const
    {
        HAssert (isMultiChoice);

        auto v = valueWithDefault.get();

        if (auto* varArray = v.getArray())
            return varArray->contains (idToCheck.toString());

        HAssertfalse;
        return false;
    }

    void valueChanged (Value&) override
    {
        if (isMultiChoice)
            setEnabled (checkMultiChoiceVar());
        else
            setEnabled (valueWithDefault.get());
    }
};

//==============================================================================
class MultiChoicePropertyComponentWithEnablement    : public MultiChoicePropertyComponent,
                                                      private Value::Listener
{
public:
    MultiChoicePropertyComponentWithEnablement (ValueWithDefault& valueToControl,
                                                ValueWithDefault valueToListenTo,
                                                const String& propertyName,
                                                const StringArray& choices,
                                                const Array<var>& correspondingValues)
        : MultiChoicePropertyComponent (valueToControl,
                                        propertyName,
                                        choices,
                                        correspondingValues),
          valueWithDefault (valueToListenTo),
          value (valueToListenTo.getPropertyAsValue())
    {
        value.addListener (this);
        valueChanged (value);
    }

    ~MultiChoicePropertyComponentWithEnablement() override    { value.removeListener (this); }

private:
    void valueChanged (Value&) override       { setEnabled (valueWithDefault.get()); }

    ValueWithDefault valueWithDefault;
    Value value;
};
