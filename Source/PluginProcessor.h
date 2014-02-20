/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <queue>

enum UpdateStyle
{
	Diff,
	FullUpdate
};

enum Light
{
	OFF = 12,
	RED = 15,
	YELLOW = 62,
	GREEN = 60
};

//==============================================================================
/**
*/
class LaunchpadLightsAudioProcessor  :	public AudioProcessor,
										public Timer
{
public:
    //==============================================================================
    LaunchpadLightsAudioProcessor();
    ~LaunchpadLightsAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

    //==============================================================================
    AudioProcessorEditor* createEditor();
    bool hasEditor() const;

    //==============================================================================
    const String getName() const;

    int getNumParameters();

    float getParameter (int index);
    void setParameter (int index, float newValue);

    const String getParameterName (int index);
    const String getParameterText (int index);

    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;

    bool acceptsMidi() const;
    bool producesMidi() const;
    bool silenceInProducesSilenceOut() const;
    double getTailLengthSeconds() const;

    //==============================================================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);

    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);

	//==============================================================================
	void triggerLaunchpadUpdateOnNextBlock();
	void timerCallback();

#if JUCE_DEBUG
	int lastSecondMessageCount;
	int samplesSinceLastMessageCount;
	int messagesSinceLastMessageCount;
    
	void logMidiMessagesPerSecond(int messageCount, int samples, double sampleRate);
#endif

	uint8 colorGrid[8][8];

private:
    //==============================================================================

	uint8 lastColorGrid[8][8];
	uint8 rowColToMidiNote[8][8];
	bool updateLaunchpad;

	drow::FFTEngine fftEngine;
	HeapBlock<float> tempBlock;
	drow::FifoBuffer<float> circularBuffer;
	std::deque<uint8> midiOutQueue;

	void dumpGridStateToMidiOutputQueue(UpdateStyle update);
	void popRateLimitedMidiMessagesToOutput(MidiBuffer& midiMessages, int samplesInBlock, int sampleRate);
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LaunchpadLightsAudioProcessor)
};

#endif  // PLUGINPROCESSOR_H_INCLUDED
