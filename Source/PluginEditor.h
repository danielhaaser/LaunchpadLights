/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-13 by Raw Material Software Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_B403DB9E48F0E52__
#define __JUCE_HEADER_B403DB9E48F0E52__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
#include "PluginProcessor.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class LaunchpadLightsAudioProcessorEditor  : public AudioProcessorEditor,
                                             public Timer
{
public:
    //==============================================================================
    LaunchpadLightsAudioProcessorEditor (LaunchpadLightsAudioProcessor* ownerFilter);
    ~LaunchpadLightsAudioProcessorEditor();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void timerCallback();

	LaunchpadLightsAudioProcessor* getProcessor() const
	{
		return static_cast<LaunchpadLightsAudioProcessor*>(getAudioProcessor());
	}

	void drawLight(Graphics& g, int row, int col, Light color);

	String FloatArrayToString(float* fData, int numFloat);
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	uint8 colorGrid[8][8];

#if JUCE_DEBUG
	ScopedPointer<Label> lblDebug;
#endif
    //[/UserVariables]

    //==============================================================================


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LaunchpadLightsAudioProcessorEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_B403DB9E48F0E52__
