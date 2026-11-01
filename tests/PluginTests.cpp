// SPDX-License-Identifier: AGPL-3.0-only
#include "Plugin.h"
#include "Editor.h"
#include <iostream>
#include <vector>
#include <stdexcept>
#include <set>
void require(bool b,const char* message){if(!b)throw std::runtime_error(message);std::cout<<"PASS "<<message<<'\n';}
void set(GardenProcessor& p,const char* id,float value){auto* v=p.state.getParameter(id);v->setValueNotifyingHost(v->convertTo0to1(value));}
struct Playhead:juce::AudioPlayHead {
 double beat=0;bool playing=true;
 juce::Optional<PositionInfo> getPosition()const override{PositionInfo p;p.setBpm(120);p.setPpqPosition(beat);p.setIsPlaying(playing);p.setTimeSignature(TimeSignature{4,4});return p;}
};
std::vector<float> render(int block){GardenProcessor p;Playhead head;p.setPlayHead(&head);p.prepareToPlay(48000,block);set(p,"mode",2);std::vector<float> out;
 for(int n=0;n<48000;n+=block){const int count=std::min(block,48000-n);head.beat=n/24000.;juce::AudioBuffer<float> b(2,count);juce::MidiBuffer m;p.processBlock(b,m);out.insert(out.end(),b.getReadPointer(0),b.getReadPointer(0)+count);}return out;}
int main(int argc,char** argv){
 try{
 juce::ScopedJuceInitialiser_GUI init;
 GardenProcessor p;p.prepareToPlay(48000,128);juce::AudioBuffer<float> b(2,128);juce::MidiBuffer m;p.processBlock(b,m);require(b.getMagnitude(0,128)==0,"plugin MIDI idle silent");
 m.addEvent(juce::MidiMessage::noteOn(1,69,juce::uint8(120)),32);p.processBlock(b,m);require(b.getMagnitude(0,32)==0,"MIDI event offset respected");require(b.getMagnitude(32,96)>0,"MIDI note produces stereo audio");
 auto graph=p.patch();require(graph.add({garden::Port::mod,garden::Port::fmIn,.5f}),"plugin route accepted");p.updatePatch(graph);set(p,"fold",.7f);set(p,"seed",12345);
 juce::MemoryBlock data;p.getStateInformation(data);GardenProcessor q;q.setStateInformation(data.getData(),int(data.getSize()));require(q.patch().size==graph.size,"routing state roundtrip");require(q.settings().seed==12345&&std::abs(q.settings().fold-.7f)<1.e-6,"parameter state roundtrip");
 auto invalidTree=juce::ValueTree::fromXml(*juce::AudioProcessor::getXmlFromBinary(data.getData(),int(data.getSize())));
 invalidTree.getChild(0).setProperty("value",10000000.f,nullptr);juce::MemoryBlock invalidRange;juce::AudioProcessor::copyXmlToBinary(*invalidTree.createXml(),invalidRange);
 q.setStateInformation(invalidRange.getData(),int(invalidRange.getSize()));require(q.settings().seed==12345&&q.settings().mode==garden::Mode::midi,"out of range state rejected atomically");
 const char invalid[]="bad state";q.setStateInformation(invalid,sizeof invalid);require(q.settings().seed==12345,"malformed state leaves existing patch intact");
 require(render(32)==render(1024),"plugin offline render invariant to block size");
 if(argc>1&&juce::String(argv[1])=="--audio"){
  const auto samples=render(64);juce::AudioBuffer<float> audio(2,int(samples.size()));for(int ch=0;ch<2;++ch)audio.copyFrom(ch,0,samples.data(),int(samples.size()));
  auto file=juce::File::getCurrentWorkingDirectory().getChildFile("out/audio/generative.wav");file.getParentDirectory().createDirectory();file.deleteFile();
  std::unique_ptr<juce::OutputStream> stream=file.createOutputStream();juce::WavAudioFormat format;
  auto writer=format.createWriterFor(stream,juce::AudioFormatWriterOptions{}.withSampleRate(48000).withNumChannels(2).withBitsPerSample(24));
  require(writer&&writer->writeFromAudioSampleBuffer(audio,0,audio.getNumSamples()),"24 bit stereo WAV offline export written");writer.reset();
  auto reader=std::unique_ptr<juce::AudioFormatReader>(format.createReaderFor(file.createInputStream().release(),true));
  require(reader&&reader->lengthInSamples==48000&&reader->sampleRate==48000&&reader->numChannels==2,"WAV file readback format and duration");
  juce::AudioBuffer<float> decoded(2,48000);require(reader->read(&decoded,0,48000,0,true,true),"WAV decoded");
  float error=0;for(int i=0;i<48000;++i)error=std::max(error,std::abs(decoded.getSample(0,i)-samples[size_t(i)]));require(error<.000001f,"WAV readback matches processor within 24 bit quantisation");
 }
 if(argc>1&&juce::String(argv[1])=="--presets"){
  auto folder=juce::File::getCurrentWorkingDirectory().getChildFile("presets");folder.createDirectory();
  for(int i=0;i<3;++i){GardenProcessor factory;factory.factory(i);juce::MemoryBlock bytes;factory.getStateInformation(bytes);require(folder.getChildFile(juce::String(i)+".vgarden").replaceWithData(bytes.getData(),bytes.getSize()),"factory preset written");}
 }
 if(argc>1&&juce::String(argv[1])=="--screenshots"){
  const auto output=juce::File::getCurrentWorkingDirectory().getChildFile("out/screenshots");output.createDirectory();
  auto editor=std::unique_ptr<GardenEditor>(static_cast<GardenEditor*>(p.createEditor()));editor->addToDesktop(juce::ComponentPeer::windowIsTemporary);editor->setVisible(true);
  auto shot=[&](const char* name){juce::MessageManager::getInstance()->runDispatchLoopUntil(60);auto image=editor->createComponentSnapshot(editor->getLocalBounds());auto file=output.getChildFile(juce::String(name)+".png");juce::FileOutputStream stream(file);stream.setPosition(0);stream.truncate();juce::PNGImageFormat png;require(png.writeImageToStream(image,stream),name);};
  p.updatePatch({});shot("panel-clean");p.updatePatch(garden::Graph::normal());shot("panel-patched");set(p,"mode",2);shot("generative");editor->setExpert(true);shot("expert");
  juce::MemoryBlock genImage,expertImage;output.getChildFile("generative.png").loadFileAsData(genImage);output.getChildFile("expert.png").loadFileAsData(expertImage);require(genImage!=expertImage,"expert screenshot contains distinct diagnostics view");
  const auto mouse=juce::Desktop::getInstance().getMainMouseSource();auto event=[&](juce::Point<float> pos){return juce::MouseEvent(mouse,pos,juce::ModifierKeys(),1.f,0.f,0.f,0.f,0.f,editor.get(),editor.get(),juce::Time::getCurrentTime(),pos,juce::Time::getCurrentTime(),1,false);};
  const auto before=p.patch().size;editor->mouseDown(event({38,250}));editor->mouseDrag(event({680,274}));editor->mouseUp(event({680,274}));require(p.patch().size==before+1,"GUI drag connects real graph cable");
  // Exercise the touch keyboard against the actual processor, including release outside it.
  p.factory(0);p.processBlock(b,m);p.prepareToPlay(48000,128);
  editor->mouseDown(event({42,680}));p.processBlock(b,m);require(b.getMagnitude(0,128)>0,"GUI touch key produces audio");
  editor->mouseDrag(event({94,680}));p.processBlock(b,m);
  juce::AudioBuffer<float> expected(2,128);GardenProcessor reference;reference.factory(0);reference.processBlock(expected,m);reference.prepareToPlay(48000,128);reference.uiNote(48,110);reference.processBlock(expected,m);reference.uiNote(48,0);reference.uiNote(49,110);
  reference.processBlock(expected,m);
  require(std::equal(b.getReadPointer(0),b.getReadPointer(0)+128,expected.getReadPointer(0)),"GUI slide releases old key and plays new pitch");
  editor->mouseDrag(event({10,630}));p.processBlock(b,m);editor->mouseUp(event({10,630}));
  for(int n=0;n<4000;++n)p.processBlock(b,m);
  require(b.getMagnitude(0,128)<.00001f,"GUI drag outside leaves no held note");
  editor->removeFromDesktop();
 }
 if(argc>2&&juce::String(argv[1])=="--validate"){
  juce::VST3PluginFormat format;juce::OwnedArray<juce::PluginDescription> descriptions;
  format.findAllTypesForFile(descriptions,argv[2]);require(descriptions.size()==1,"actual VST3 bundle scanned");
  juce::String error;auto instance=format.createInstanceFromDescription(*descriptions[0],48000,128,error);
  require(instance!=nullptr,("actual VST3 instantiated: "+error).toRawUTF8());
  require(instance->getName().startsWith("Jerzy"),"loaded VST3 product starts with Jerzy");
  instance->setPlayConfigDetails(0,2,48000,128);instance->prepareToPlay(48000,128);
  juce::AudioBuffer<float> block(2,128);juce::MidiBuffer events;
  instance->processBlock(block,events);require(block.getMagnitude(0,128)==0,"loaded VST3 MIDI idle silent");
  events.addEvent(juce::MidiMessage::noteOn(1,60,juce::uint8(110)),0);instance->processBlock(block,events);
  require(block.getMagnitude(0,128)>0,"loaded VST3 renders MIDI audio");
  juce::MemoryBlock saved;instance->getStateInformation(saved);require(saved.getSize()>0,"loaded VST3 saves state");instance->setStateInformation(saved.getData(),int(saved.getSize()));
  auto window=std::unique_ptr<juce::AudioProcessorEditor>(instance->createEditor());require(window!=nullptr,"loaded VST3 creates editor");
  window->setVisible(false);window->addToDesktop(juce::ComponentPeer::windowIsTemporary);window->getPeer()->setCurrentRenderingEngine(0);window->setVisible(true);juce::MessageManager::getInstance()->runDispatchLoopUntil(200);
  auto image=juce::createSnapshotOfNativeWindow(window->getPeer()->getNativeHandle());
  require(image.isValid(),"loaded native VST3 window snapshot is valid");
  std::set<juce::uint32> colours;for(int y=0;y<image.getHeight();y+=16)for(int x=0;x<image.getWidth();x+=16)colours.insert(image.getPixelAt(x,y).getARGB());
  require(colours.size()>20,"loaded VST3 screenshot contains rendered controls");
  auto file=juce::File::getCurrentWorkingDirectory().getChildFile("out/screenshots/Jerzy-loaded-vst3.png");file.getParentDirectory().createDirectory();
  juce::FileOutputStream stream(file);stream.setPosition(0);stream.truncate();juce::PNGImageFormat png;require(png.writeImageToStream(image,stream),"loaded VST3 GUI captured");
  stream.flush();window.reset();instance->releaseResources();instance.reset();juce::MessageManager::getInstance()->runDispatchLoopUntil(60);
 }
 std::cout<<"RESULT plugin integration complete\n";return 0;
 }catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}
}
