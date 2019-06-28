/*
  ==============================================================================

    %%filename%%
    Created: %%date%%
    Author:  %%author%%

  ==============================================================================
*/

#pragma once

%%include_H%%

//==============================================================================
/*
*/
class %%component_class%%    : public Component
{
public:
    %%component_class%%();
    ~%%component_class%%();

    void paint (Graphics&) override;
    void resized() override;

private:
    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (%%component_class%%)
};
