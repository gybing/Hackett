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

//==============================================================================
/**
    A class that automatically sends analytics events to the Analytics singleton
    when a button is clicked.

    @see Analytics, AnalyticsDestination::AnalyticsEvent

    @tags{Analytics}
*/
class API ButtonTracker   : private Button::Listener
{
public:
    //==============================================================================
    /**
        Creating one of these automatically sends analytics events to the Analytics
        singeton when the corresponding button is clicked.

        The name and parameters of the analytics event will be populated from the
        variables supplied here. If clicking changes the button's state then the
        parameters will have a {"ButtonState", "On"/"Off"} entry added.

        @param buttonToTrack               the button to track
        @param triggeredEventName          the name of the generated event
        @param triggeredEventParameters    the parameters to add to the generated
                                           event
        @param triggeredEventType          (optional) an integer to indicate the event
                                           type, which will be set to 0 if not supplied.

        @see Analytics, AnalyticsDestination::AnalyticsEvent
    */
    ButtonTracker (Button& buttonToTrack,
                   const String& triggeredEventName,
                   const StringPairArray& triggeredEventParameters = {},
                   int triggeredEventType = 0);

    /** Destructor. */
    ~ButtonTracker() override;

private:
    /** @internal */
    void buttonClicked (Button*) override;

    Button& button;
    const String eventName;
    const StringPairArray eventParameters;
    const int eventType;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButtonTracker)
};


