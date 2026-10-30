// SPDX-License-Identifier: AGPL-3.0-only
#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "Engine.h"
#include <mutex>
class GardenProcessor final : public juce::AudioProcessor {
public:
 GardenProcessor();
 juce::AudioProcessorValueTreeState state;
 void prepareToPlay(double,int)override;
 void releaseResources()override{}
 bool isBusesLayoutSupported(const BusesLayout& b)const override{return b.getMainOutputChannelSet()==juce::AudioChannelSet::stereo()&&b.getMainInputChannelSet().isDisabled();}
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&)override;
 juce::AudioProcessorEditor* createEditor()override;
 bool hasEditor()const override{return true;}
 const juce::String getName()const override{return "Voltage Garden";}
 bool acceptsMidi()const override{return true;} bool producesMidi()const override{return false;}
 double getTailLengthSeconds()const override{return 8;}
 int getNumPrograms()override{return 1;} int getCurrentProgram()override{return 0;}
 void setCurrentProgram(int)override{} const juce::String getProgramName(int)override{return "Garden";}
 void changeProgramName(int,const juce::String&)override{}
 void getStateInformation(juce::MemoryBlock&)override;
 void setStateInformation(const void*,int)override;
 garden::Graph patch()const;
 bool updatePatch(garden::Graph);
 void uiNote(int note,int velocity)noexcept;
 std::atomic<bool> panic{false};
 std::atomic<float> peak{0};
 void factory(int index);
 garden::Settings settings()const noexcept;
private:
 static juce::AudioProcessorValueTreeState::ParameterLayout layout();
 garden::Engine engine;
 garden::Graph audioGraph=garden::Graph::normal();
 garden::GraphMailbox mailbox;
 mutable std::mutex patchMutex;
 garden::Graph editableGraph=garden::Graph::normal();
 std::array<std::atomic<int>,128> uiEvents{};
 std::array<float,2> lastOutput{}, transitionFrom{};
 int transition=0,transitionLength=240;
 static constexpr int parameterCount=27;
 std::array<std::atomic<float>*,parameterCount> values{};
};
