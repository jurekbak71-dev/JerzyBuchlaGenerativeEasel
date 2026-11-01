// SPDX-License-Identifier: AGPL-3.0-only
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class GardenHardwareLook final : public juce::LookAndFeel_V4 {
public:
 juce::Font getLabelFont(juce::Label& label) override {
  if(auto* slider=dynamic_cast<juce::Slider*>(label.getParentComponent()))
   if(slider->getName()=="SEED")return juce::Font(juce::FontOptions(9));
  return LookAndFeel_V4::getLabelFont(label);
 }
 void drawRotarySlider(juce::Graphics& g,int x,int y,int width,int height,float position,float start,float end,juce::Slider& slider) override {
  auto body=juce::Rectangle<float>(float(x),float(y),float(width),float(height)).reduced(5);
  const float diameter=std::min(body.getWidth(),body.getHeight());
  body=body.withSizeKeepingCentre(diameter,diameter);auto centre=body.getCentre();
  g.setColour(juce::Colours::black.withAlpha(.25f));g.fillEllipse(body.translated(1,2));
  const bool blueCap=slider.findColour(juce::Slider::thumbColourId)==juce::Colour(0xff334f91);
  g.setColour(blueCap?juce::Colour(0xff183966):juce::Colour(0xff202421));g.fillEllipse(body);
  g.setGradientFill(juce::ColourGradient(blueCap?juce::Colour(0xff5c8fcd):juce::Colour(0xffe2e2d8),body.getX(),body.getY(),blueCap?juce::Colour(0xff254b85):juce::Colour(0xff92968d),body.getRight(),body.getBottom(),false));g.fillEllipse(body.reduced(3));
  g.setColour(juce::Colour(0xff55594f));g.drawEllipse(body.reduced(3),.7f);
  const float angle=start+position*(end-start)-juce::MathConstants<float>::halfPi;
  const auto tip=centre+juce::Point<float>(std::cos(angle),std::sin(angle))*(diameter*.31f);
  g.setColour(blueCap?juce::Colour(0xffe4eadf):juce::Colour(0xff252925));g.drawLine(centre.x,centre.y,tip.x,tip.y,2.2f);
  g.setColour(slider.findColour(juce::Slider::thumbColourId));g.fillEllipse(tip.x-2.3f,tip.y-2.3f,4.6f,4.6f);
 }
 void drawLinearSlider(juce::Graphics& g,int x,int y,int width,int height,float position,float minimum,float maximum,juce::Slider::SliderStyle style,juce::Slider& slider) override {
  if(style!=juce::Slider::LinearVertical){LookAndFeel_V4::drawLinearSlider(g,x,y,width,height,position,minimum,maximum,style,slider);return;}
  const float centre=float(x)+float(width)*.5f;
  g.setColour(juce::Colour(0xff999c92));g.fillRect(centre-2.5f,float(y)-2,5.f,float(height)+4);
  g.setColour(juce::Colour(0xff232925));g.fillRect(centre-1.2f,float(y),2.4f,float(height));
  for(int tick=0;tick<=10;++tick){const float ty=float(y)+float(height)*tick/10.f;g.setColour(juce::Colour(0xff73776b));g.drawLine(centre-10,ty,centre-5,ty,.6f);g.drawLine(centre+5,ty,centre+10,ty,.6f);}
  auto cap=juce::Rectangle<float>(centre-11,position-4,22,8);
  g.setColour(juce::Colours::black.withAlpha(.35f));g.fillRoundedRectangle(cap.translated(0,2),1);
  g.setColour(slider.findColour(juce::Slider::thumbColourId));g.fillRoundedRectangle(cap,1);
  g.setColour(juce::Colours::white.withAlpha(.55f));g.drawLine(cap.getX()+1,cap.getY()+1,cap.getRight()-1,cap.getY()+1,1);
  g.setColour(juce::Colour(0xff363d36));g.drawLine(centre-9,position,centre+9,position,1);
 }
};
