// SPDX-License-Identifier: AGPL-3.0-only
#include "Plugin.h"
#include "Editor.h"
#include <iostream>
int main(){
 juce::ScopedJuceInitialiser_GUI init;
 GardenProcessor processor;processor.prepareToPlay(48000,128);
 auto editor=std::unique_ptr<GardenEditor>(static_cast<GardenEditor*>(processor.createEditor()));
 editor->addToDesktop(juce::ComponentPeer::windowIsTemporary);editor->setVisible(true);editor->setSize(2640,1840);
 auto source=juce::Desktop::getInstance().getMainMouseSource();
 auto event=[&](juce::Point<float> p){return juce::MouseEvent(source,p,juce::ModifierKeys(),1.f,0,0,0,0,editor.get(),editor.get(),juce::Time::getCurrentTime(),p,juce::Time::getCurrentTime(),1,false);};
 auto before=processor.patch().size;
 editor->mouseDown(event({620,284}));editor->mouseDrag(event({1452,388}));editor->mouseUp(event({1452,388}));
 if(processor.patch().size!=before+1){std::cerr<<"FAIL 200 percent cable routing\n";return 1;}
 editor->mouseDown(event({622,1500}));juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;processor.processBlock(b,midi);
 if(b.getMagnitude(0,128)<=0){std::cerr<<"FAIL 200 percent keyboard sound\n";return 1;}
 auto image=editor->createComponentSnapshot(editor->getLocalBounds());
 if(image.getWidth()!=2640||image.getHeight()!=1840)return 1;
 auto file=juce::File::getCurrentWorkingDirectory().getChildFile("out/screenshots/panel-200.png");juce::FileOutputStream stream(file);stream.setPosition(0);stream.truncate();juce::PNGImageFormat png;
 if(!png.writeImageToStream(image,stream))return 1;stream.flush();editor->mouseUp(event({622,1500}));editor->removeFromDesktop();
 std::cout<<"PASS real editor 2640x1840, 200 percent cable routing, keyboard-generated audio and PNG snapshot\n";
}
