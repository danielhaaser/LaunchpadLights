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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "PluginEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
LaunchpadLightsAudioProcessorEditor::LaunchpadLightsAudioProcessorEditor (LaunchpadLightsAudioProcessor* ownerFilter)
    : AudioProcessorEditor(ownerFilter)
{

    //[UserPreSize]
#if JUCE_DEBUG
	addAndMakeVisible(lblDebug = new Label("new label",
		TRANS("debug label")));
	lblDebug->setFont(Font(15.00f, Font::plain));
	lblDebug->setJustificationType(Justification::centred);
	lblDebug->setEditable(false, false, false);
	lblDebug->setColour(Label::textColourId, Colours::white);
	lblDebug->setColour(TextEditor::textColourId, Colours::black);
	lblDebug->setColour(TextEditor::backgroundColourId, Colour(0x00000000));
#endif
    //[/UserPreSize]

    setSize (484, 484);


    //[Constructor] You can add your own custom stuff here..
	for (int row = 0; row < 8; ++row)
	{
		for (int col = 0; col < 8; ++col)
		{
			this->colorGrid[row][col] = Light::OFF;
		}
	}

	startTimer(16);
    //[/Constructor]
}

LaunchpadLightsAudioProcessorEditor::~LaunchpadLightsAudioProcessorEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
#if JUCE_DEBUG
	lblDebug = nullptr;
#endif
    //[/Destructor]
}

//==============================================================================
void LaunchpadLightsAudioProcessorEditor::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff151515));

    //[UserPaint] Add your own custom painting code here..

	for (int row = 0; row < 8; ++row)
	{
		for (int col = 0; col < 8; ++col)
		{
			this->drawLight(g, row, col, static_cast<Light>(this->colorGrid[row][col]));
		}
	}

    //[/UserPaint]
}

void LaunchpadLightsAudioProcessorEditor::resized()
{
    //[UserResized] Add your own custom resize handling here..
#if JUCE_DEBUG
	lblDebug->setBounds(40, 446, 368, 40);
#endif
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void LaunchpadLightsAudioProcessorEditor::timerCallback()
{
	LaunchpadLightsAudioProcessor* processor = getProcessor();

	for (int row = 0; row < 8; ++row)
	{
		for (int col = 0; col < 8; ++col)
		{
			colorGrid[row][col] = processor->colorGrid[row][col];
		}
	}

	// Debug Stuff
#if JUCE_DEBUG
	int messagesInLastSecond = processor->lastSecondMessageCount;
	lblDebug->setText(String("MIDI Messages per Second: " + String(messagesInLastSecond)), NotificationType::sendNotification);
#endif

	repaint();
}

void LaunchpadLightsAudioProcessorEditor::drawLight(Graphics& g, int row, int col, Light color)
{
	const float width = 40.0f;
	const float height = 40.0f;
	const float spacing = 52.0f;
	const float startX = 40.0f;
	const float startY = 42.0f;
	const float cornerRounding = 2.0f;

	const Colour greenColorOne = Colour(0xffc0ff01);
	const Colour greenColorTwo = Colour(0xffd4ff4f);
	const Colour yellowColorOne = Colour(0xfff6ff01);
	const Colour yellowColorTwo = Colour(0xfff9ff4f);
	const Colour redColorOne = Colour(0xffff4201);
	const Colour redColorTwo = Colour(0xffff7d4f);
	const Colour greyColorOne = Colour(0xff1d1d1d);
	const Colour greyColorTwo = Colour(0xff262626);

	Colour colorOne = greyColorOne;
	Colour colorTwo = greyColorTwo;

	switch (color)
	{
		case GREEN:	colorOne = greenColorOne; colorTwo = greenColorTwo; break;
		case YELLOW:colorOne = yellowColorOne; colorTwo = yellowColorTwo; break;
		case RED:	colorOne = redColorOne; colorTwo = redColorTwo; break;
		case OFF:	colorOne = greyColorOne; colorTwo = greyColorTwo; break;
	}

	g.setGradientFill(ColourGradient(colorOne,
		50.0f, 50.0f,
		colorTwo,
		static_cast<float> (proportionOfWidth(0.2162f)), static_cast<float> (proportionOfHeight(0.3182f)),
		false));
	g.fillRoundedRectangle(startX + spacing * col, startY + spacing * row, width, height, cornerRounding);
}

//Return String of multiple float values separated by commas
String LaunchpadLightsAudioProcessorEditor::FloatArrayToString(float* fData, int numFloat)
{
	String result = "";

	if (numFloat < 1)
	{
		return result;
	}

	for (int i = 0; i < (numFloat - 1); i++)
	{
		result << String(fData[i], 2) << ",";//Use juce::String initializer for each value
	}

	result << String(fData[numFloat - 1], 2);

	return result;
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="LaunchpadLightsAudioProcessorEditor"
                 componentName="" parentClasses="public AudioProcessorEditor, public Timer"
                 constructorParams="LaunchpadLightsAudioProcessor* ownerFilter"
                 variableInitialisers="AudioProcessorEditor(ownerFilter)" snapPixels="4"
                 snapActive="1" snapShown="1" overlayOpacity="0.330" fixedSize="1"
                 initialWidth="484" initialHeight="484">
  <BACKGROUND backgroundColour="ff151515"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
