/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LaunchpadLightsAudioProcessor::LaunchpadLightsAudioProcessor()
:	fftEngine(10),  // FFT Size: 2 ^ 10 samples
    tempBlock(fftEngine.getFFTSize()),
	circularBuffer	(fftEngine.getMagnitudesBuffer().getSize() * 4)
{
	for (int row = 0; row < 8; ++row)
	{
		for (int col = 0; col < 8; ++col)
		{
			colorGrid[row][col] = Light::OFF;
			lastColorGrid[row][col] = Light::OFF;

			rowColToMidiNote[row][col] = (uint8) row * 16 + col;
		}
	}

	updateLaunchpad = false;

#if JUCE_DEBUG
	samplesSinceLastMessageCount = 0;
	messagesSinceLastMessageCount = 0;
	lastSecondMessageCount = 0;
#endif

	fftEngine.setWindowType(drow::Window::Hann);

	circularBuffer.reset();

	startTimer(41);  // Launchpad update timer
}

LaunchpadLightsAudioProcessor::~LaunchpadLightsAudioProcessor()
{
}

//==============================================================================
const String LaunchpadLightsAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int LaunchpadLightsAudioProcessor::getNumParameters()
{
    return 0;
}

float LaunchpadLightsAudioProcessor::getParameter (int index)
{
    return 0.0f;
}

void LaunchpadLightsAudioProcessor::setParameter (int index, float newValue)
{
}

const String LaunchpadLightsAudioProcessor::getParameterName (int index)
{
    return String::empty;
}

const String LaunchpadLightsAudioProcessor::getParameterText (int index)
{
    return String::empty;
}

const String LaunchpadLightsAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String LaunchpadLightsAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool LaunchpadLightsAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool LaunchpadLightsAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool LaunchpadLightsAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool LaunchpadLightsAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool LaunchpadLightsAudioProcessor::silenceInProducesSilenceOut() const
{
    return true;
}

double LaunchpadLightsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LaunchpadLightsAudioProcessor::getNumPrograms()
{
    return 0;
}

int LaunchpadLightsAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LaunchpadLightsAudioProcessor::setCurrentProgram (int index)
{
}

const String LaunchpadLightsAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void LaunchpadLightsAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void LaunchpadLightsAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void LaunchpadLightsAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void LaunchpadLightsAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	// Clear MIDI input
	midiMessages.clear();

	// In case we have more outputs than inputs, we'll clear any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
	{
		buffer.clear(i, 0, buffer.getNumSamples());
	}

	// Convert stereo  input to mono
	// set up array of pointers to samples
	const int numInputChannels = getNumInputChannels();
	const float oneOverNumInputChannels = 1.0f / numInputChannels;

	if (numInputChannels == 2)  // Stereo input
	{
		float* pfSample[2];
		pfSample[0] = buffer.getSampleData(0);
		pfSample[1] = buffer.getSampleData(1);

		// set-up mixed mono buffer
		AudioSampleBuffer mixedBuffer(1, buffer.getNumSamples());
		float* pfMixedSample = mixedBuffer.getSampleData(0);

		// fill mono mixed buffer
		for (int i = 0; i < mixedBuffer.getNumSamples(); i++)
		{
			*pfMixedSample = 0.0;
			pfMixedSample++;
		}
		pfMixedSample = mixedBuffer.getSampleData(0);

		for (int i = 0; i < mixedBuffer.getNumSamples(); i++)
		{
			*pfMixedSample += oneOverNumInputChannels * (*pfSample[0]);
			pfSample[0]++;
			*pfMixedSample += oneOverNumInputChannels * (*pfSample[1]);
			pfSample[1]++;
			pfMixedSample++;
		}

		// Write mono samples into circular buffer
		circularBuffer.writeSamples(mixedBuffer.getSampleData(0), mixedBuffer.getNumSamples());
	}
	else
	{
		// Mono input, or more than 2 channels?
		// We'll just grab the samples in the first channel
		circularBuffer.writeSamples(buffer.getSampleData(0), buffer.getNumSamples());
	}

	// Process samples in FFT
	jassert (circularBuffer.getNumFree() != 0);

	while (circularBuffer.getNumAvailable() > fftEngine.getFFTSize())
	{
		circularBuffer.readSamples(tempBlock.getData(), fftEngine.getFFTSize());
		fftEngine.performFFT(tempBlock);
		fftEngine.updateMagnitudesIfBigger();
	}

	// Only do some processing if we need to display to launchpad
	if (updateLaunchpad)
	{
		const float* fftData = fftEngine.getMagnitudesBuffer().getData();

		// Each bin is 43 Hz
		const int startBinPerColumn[8] = { 1, 2, 4, 7,  13, 27,  55, 111 };
		const int endBinPerColumn[8] =	 { 1, 3, 6, 12, 26, 54, 110, 222 };
		float colMagnitudes[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
		float dbMagnitudes[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

		// Average the FFT down to 8 col magnitudes
		for (int col = 0; col < 8; ++col)
		{
			for (int bin = startBinPerColumn[col]; bin <= endBinPerColumn[col]; ++bin)
			{
				colMagnitudes[col] += jlimit(0.0f, 1.0f, float (1 + (drow::toDecibels(fftData[bin]) / 100.0f)));
				dbMagnitudes[col] += drow::toDecibels(fftData[bin]);
			}

			colMagnitudes[col] = colMagnitudes[col] / float (endBinPerColumn[col] - startBinPerColumn[col] + 1);
			dbMagnitudes[col] = dbMagnitudes[col] / float(endBinPerColumn[col] - startBinPerColumn[col] + 1);
		}

		// Display the range from -60 to -10 db, in increments of 5db
		// Normalized value range: 0.4 to 0.9
		// Map the column magnitudes to colorGrid (0 - 8) for Launchpad to display
		int colMagnitudeInt[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

		for (int col = 0; col < 8; ++col)
		{
			// Cheat and show a different Y-axis scale for higher frequencies
			// to make it more visually interesting
			const float oldMin = 0.4f - (0.25f * (col / 7.0f));
			const float oldMax = 0.9f - (0.25f * (col / 7.0f));
			const float oldRange = oldMax - oldMin;;
			const float newRange = 8.0f;

			if (colMagnitudes[col] < oldMin) 
				colMagnitudes[col] = oldMin;

			else if (colMagnitudes[col] > oldMax) 
				colMagnitudes[col] = oldMax;

			float newValue = (((colMagnitudes[col] - oldMin) * newRange) / oldRange);

			colMagnitudeInt[col] = roundToInt(newValue);
		}

		for (int col = 0; col < 8; ++col)
		{
			for (int row = 7; row >= 8 - colMagnitudeInt[col]; --row)
			{
				switch (row)
				{
					case 0:
						colorGrid[row][col] = Light::RED;
						break;

					case 1:
					case 2:
						colorGrid[row][col] = Light::YELLOW;
						break;

					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
						colorGrid[row][col] = Light::GREEN;
				}
			}

			for (int row = 7 - colMagnitudeInt[col]; row >= 0; --row)
			{
				colorGrid[row][col] = Light::OFF;
			}
		}

		fftEngine.findMagnitudes();  // Reset the FFT magnitudes

		this->dumpGridStateToMidiOutputQueue(UpdateStyle::Diff);

		updateLaunchpad = false;  // Possible race condition?
	}

	this->popRateLimitedMidiMessagesToOutput(midiMessages, buffer.getNumSamples(), getSampleRate());
#if JUCE_DEBUG
	this->logMidiMessagesPerSecond(midiMessages.getNumEvents(), buffer.getNumSamples(), getSampleRate());
#endif
}

//==============================================================================
bool LaunchpadLightsAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* LaunchpadLightsAudioProcessor::createEditor()
{
    return new LaunchpadLightsAudioProcessorEditor (this);
}

//==============================================================================
void LaunchpadLightsAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void LaunchpadLightsAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void LaunchpadLightsAudioProcessor::triggerLaunchpadUpdateOnNextBlock()
{
	updateLaunchpad = true;
}

void LaunchpadLightsAudioProcessor::timerCallback()
{
	this->triggerLaunchpadUpdateOnNextBlock();
}

void LaunchpadLightsAudioProcessor::dumpGridStateToMidiOutputQueue(UpdateStyle update)
{
	switch (update)
	{
		case Diff:
		{
			for (int row = 0; row < 8; ++row)
			{
				for (int col = 0; col < 8; ++col)
				{
					if (colorGrid[row][col] != lastColorGrid[row][col])
					{
						midiOutQueue.push_back(144);
						midiOutQueue.push_back(rowColToMidiNote[row][col]);
						midiOutQueue.push_back(colorGrid[row][col]);
						
						lastColorGrid[row][col] = colorGrid[row][col];
					}
				}
			}
		}
		break;

		case FullUpdate:
		{
			for (int row = 0; row < 8; ++row)
			{
				for (int col = 0; col < 8; col += 2)
				{
					// The Rapid LED update function
					// Updates 2 LEDs per 3 byte message
					// 146, Velocity1, Velocity2
					midiOutQueue.push_back(146);
					midiOutQueue.push_back(colorGrid[row][col]);
					midiOutQueue.push_back(colorGrid[row][col + 1]);

					lastColorGrid[row][col] = colorGrid[row][col];
					lastColorGrid[row][col + 1] = colorGrid[row][col + 1];
				}
			}
		}
		break;
	}
}

void LaunchpadLightsAudioProcessor::popRateLimitedMidiMessagesToOutput(MidiBuffer& midiMessages, int samplesInBlock, int sampleRate)
{
	// Keep the rate below 400 messages per second, due to hardware limitations
	int samplesBetweenMessages = sampleRate / 400;
	//int samplesBetweenMessages = 1025;
	int messageSample = 0;

	while ((midiOutQueue.size() >= 3) && (messageSample < samplesInBlock))
	{
		uint8 firstByte = midiOutQueue.front();
		midiOutQueue.pop_front();
		uint8 secondByte = midiOutQueue.front();
		midiOutQueue.pop_front();
		uint8 thirdByte = midiOutQueue.front();
		midiOutQueue.pop_front();

		MidiMessage outputMessage(firstByte, secondByte, thirdByte);

		// This is hacky - message 146 is a rapid LED update, should be on channel 3
		outputMessage.setChannel(firstByte == 146 ? 3 : 1);  

		midiMessages.addEvent(outputMessage, messageSample);

		messageSample += samplesBetweenMessages;
	}
}

#if JUCE_DEBUG
void LaunchpadLightsAudioProcessor::logMidiMessagesPerSecond(int messageCount, int samples, double sampleRate)
{
	messagesSinceLastMessageCount += messageCount;
	samplesSinceLastMessageCount += samples;

	if (samplesSinceLastMessageCount > sampleRate)
	{
		lastSecondMessageCount = messagesSinceLastMessageCount;
		messagesSinceLastMessageCount = 0;
		samplesSinceLastMessageCount = 0;
	}
}
#endif

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LaunchpadLightsAudioProcessor();
}