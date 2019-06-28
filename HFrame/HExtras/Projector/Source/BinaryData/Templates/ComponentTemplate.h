/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projector!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projector version: %%version%%

  ------------------------------------------------------------------------------

  The Projector is part of the H library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
%%include_header%%
//[/Headers]

%%include_files_h%%

//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projector.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
%%class_declaration%%
{
public:
    //==============================================================================
    %%class_name%% (%%constructor_params%%);
    ~%%class_name%%();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    %%public_member_declarations%%

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    %%private_member_declarations%%

    //==============================================================================
    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (%%class_name%%)
};

//[EndFile] You can add extra defines here...
//[/EndFile]
