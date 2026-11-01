// SPDX-License-Identifier: AGPL-3.0-only
#pragma once
#include "Plugin.h"
#include "HardwareLook.h"
class GardenEditor final:public juce::AudioProcessorEditor,private juce::Timer {
public:
 explicit GardenEditor(GardenProcessor&);
 ~GardenEditor()override;
 void paint(juce::Graphics&)override;
 void resized()override;
 void mouseDown(const juce::MouseEvent&)override;
 void mouseDrag(const juce::MouseEvent&)override;
 void mouseUp(const juce::MouseEvent&)override;
 void setExpert(bool b);
 static constexpr int designWidth=1320,designHeight=920;
private:
 GardenProcessor& processor;
 GardenHardwareLook look;
 std::vector<std::unique_ptr<juce::Slider>> knobs;
 std::vector<std::unique_ptr<juce::Label>> labels;
 std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
 std::array<juce::ComboBox,5> choices;
 std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>> choiceAttachments;
 juce::TextButton start{"FREE START"},sync{"HOST"},panic{"PANIC"},expert{"EXPERT"},save{"SAVE"},load{"LOAD"},normal{"NORMAL"},clear{"CLEAR"},cables{"CABLES"},octaveDown{"OCT -"},octaveUp{"OCT +"};
 std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> buttonAttachments;
 std::unique_ptr<juce::FileChooser> chooser;
 std::array<juce::Point<float>,garden::portCount> jacks{};
 int dragging=-1,heldNote=-1,baseNote=48;
 bool showCables=true;juce::Point<float> cursor;
 juce::String status="Drag OUT to IN. Right click a cable to inspect, mute or remove.";
 void timerCallback()override;
 int jackAt(juce::Point<float>)const;
 int keyAt(juce::Point<float>)const;
 void releaseKey();
 juce::Path cablePath(const garden::Cable&)const;
 void control(const char*,const char*,juce::Rectangle<int>,bool fader,juce::Colour);
 float panelScale()const noexcept;
 bool expertView=false;
};
