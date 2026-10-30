// SPDX-License-Identifier: AGPL-3.0-only
#include "Plugin.h"
#include "Editor.h"
namespace {
constexpr const char* ids[]={"mode","sync","run","tempo","tune","timbre","fold","fm","modRate","decay","cutoff","level","wet","density","swing","motion","root","scale","seed","division","lpgMode","expert","step1","step2","step3","step4","step5"};
}
juce::AudioProcessorValueTreeState::ParameterLayout GardenProcessor::layout(){
 juce::AudioProcessorValueTreeState::ParameterLayout p;
 auto choice=[&](const char* id,const char* name,juce::StringArray opts,int def){p.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{id,1},name,opts,def));};
 auto range=[&](const char* id,const char* name,float lo,float hi,float def,float skew=1.f){p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{id,1},name,juce::NormalisableRange<float>{lo,hi,0,skew},def));};
 choice("mode","Mode",{"MANUAL","MIDI","GENERATIVE","HYBRID"},1);
 p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"sync",1},"Host sync",true));
 p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"run",1},"Free start",false));
 range("tempo","Tempo",20,300,110);range("tune","Tune",-24,24,0);range("timbre","Wave shape",0,1,.2f);
 range("fold","Wavefold",0,1,0);range("fm","FM depth",0,1,0);range("modRate","Mod rate Hz",.01f,2000,3,.3f);
 range("decay","LPG decay seconds",.015f,4,.25f,.4f);range("cutoff","LPG cutoff Hz",20,20000,5000,.3f);
 range("level","Master",0,1,.65f);range("wet","Spring mix",0,1,.18f);range("density","Density",0,1,.75f);
 range("swing","Swing",0,.45f,.12f);range("motion","Motion",0,1,.5f);
 choice("root","Root",{"C","C#","D","Eb","E","F","F#","G","Ab","A","Bb","B"},0);
 choice("scale","Scale",{"Minor","Major","Dorian","Pentatonic"},0);
 p.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"seed",1},"Seed",0,1000000,42));
 p.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"division",1},"Steps per beat",1,128,4));
 choice("lpgMode","LPG model",{"LPG","VCA","VCF"},0);
 p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"expert",1},"Expert view",false));
 range("step1","Step 1 semitones",-24,24,0);range("step2","Step 2 semitones",-24,24,3);
 range("step3","Step 3 semitones",-24,24,7);range("step4","Step 4 semitones",-24,24,5);range("step5","Step 5 semitones",-24,24,2);return p;
}
GardenProcessor::GardenProcessor():AudioProcessor(BusesProperties().withOutput("Stereo",juce::AudioChannelSet::stereo(),true)),state(*this,nullptr,"GARDEN",layout()){
 for(int i=0;i<parameterCount;++i)values[i]=state.getRawParameterValue(ids[i]);
 for(auto& e:uiEvents)e.store(-1);
}
garden::Settings GardenProcessor::settings()const noexcept {
 const auto v=[&](int i){return values[i]->load(std::memory_order_relaxed);};garden::Settings s;
 s.mode=static_cast<garden::Mode>(static_cast<int>(v(0)));s.hostSync=v(1)>.5f;s.run=v(2)>.5f;s.tempo=v(3);s.tune=v(4);
 s.timbre=v(5);s.fold=v(6);s.fm=v(7);s.modRate=v(8);s.decay=v(9);s.cutoff=v(10);s.level=v(11);s.wet=v(12);
 s.density=v(13);s.swing=v(14);s.motion=v(15);s.root=static_cast<int>(v(16));s.scale=static_cast<int>(v(17));
 s.seed=static_cast<int>(v(18));s.division=static_cast<int>(v(19));s.lpgMode=static_cast<int>(v(20));for(int i=0;i<5;++i)s.steps[i]=v(22+i);return s;
}
void GardenProcessor::prepareToPlay(double r,int){engine.prepare(r);engine.setGraph(audioGraph);lastOutput={};transition=0;transitionLength=std::max(1,int(r*.005));}
void GardenProcessor::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer& midi){
 juce::ScopedNoDenormals noDenormals; b.clear();
 if(mailbox.consume(audioGraph)){transitionFrom=lastOutput;transition=transitionLength;engine.setGraph(audioGraph);}
 garden::Transport t;
 if(auto* ph=getPlayHead())if(auto pos=ph->getPosition()){
  t.playing=pos->getIsPlaying();if(auto bpm=pos->getBpm())t.bpm=*bpm;
  if(auto beat=pos->getPpqPosition()){t.beat=*beat;t.valid=true;}
 }
 engine.begin(settings(),t);
 if(panic.exchange(false))engine.allNotesOff();
 for(int n=0;n<128;++n){int vel=uiEvents[n].exchange(-1,std::memory_order_acq_rel);if(vel==0)engine.noteOff(n);else if(vel>0)engine.noteOn(n,vel/127.f);}
 auto it=midi.cbegin();float maximum=0;
 for(int i=0;i<b.getNumSamples();++i){
  while(it!=midi.cend()&&(*it).samplePosition<=i){if((*it).numBytes>3){++it;continue;}auto msg=(*it).getMessage();
   if(msg.isNoteOn())engine.noteOn(msg.getNoteNumber(),msg.getFloatVelocity());
   else if(msg.isNoteOff())engine.noteOff(msg.getNoteNumber());
   else if(msg.isAllNotesOff()||msg.isAllSoundOff())engine.allNotesOff();
   else if(msg.isPitchWheel())engine.pitchBend((msg.getPitchWheelValue()-8192)/8192.f);
   else if(msg.isController()){if(msg.getControllerNumber()==64)engine.sustain(msg.getControllerValue()>=64);if(msg.getControllerNumber()==1)engine.modWheel(msg.getControllerValue()/127.f);}
   else if(msg.isChannelPressure())engine.aftertouch(msg.getChannelPressureValue()/127.f);
   else if(msg.isAftertouch())engine.polyAftertouch(msg.getNoteNumber(),msg.getAfterTouchValue()/127.f);
   ++it;
  }
  auto x=engine.sample(i);if(transition>0){const float t=1.f-float(transition)/float(transitionLength);for(int ch=0;ch<2;++ch)x[ch]=std::lerp(transitionFrom[ch],x[ch],t);--transition;}lastOutput=x;if(b.getNumChannels()>=2){b.setSample(0,i,x[0]);b.setSample(1,i,x[1]);}maximum=std::max(maximum,std::abs(x[0]));
 }
 peak.store(maximum,std::memory_order_relaxed);midi.clear();
}
garden::Graph GardenProcessor::patch()const {std::lock_guard lock(patchMutex);return editableGraph;}
bool GardenProcessor::updatePatch(garden::Graph g){if(!g.compile())return false;std::lock_guard lock(patchMutex);editableGraph=g;mailbox.publish(g);return true;}
void GardenProcessor::uiNote(int n,int velocity)noexcept{if(n>=0&&n<128)uiEvents[n].store(std::clamp(velocity,0,127),std::memory_order_release);}
void GardenProcessor::getStateInformation(juce::MemoryBlock& dest){
 auto tree=state.copyState();tree.setProperty("version",1,nullptr);juce::ValueTree routes("PATCH");
 auto g=patch();for(size_t i=0;i<g.size;++i){const auto& c=g.cables[i];juce::ValueTree wire("CABLE");
  wire.setProperty("from",garden::idx(c.from),nullptr);wire.setProperty("to",garden::idx(c.to),nullptr);wire.setProperty("gain",c.gain,nullptr);wire.setProperty("mute",c.muted,nullptr);routes.addChild(wire,-1,nullptr);}
 tree.addChild(routes,-1,nullptr);if(auto xml=tree.createXml())copyXmlToBinary(*xml,dest);
}
void GardenProcessor::setStateInformation(const void* data,int size){
 if(size<=0||size>1024*1024)return;
 auto xml=getXmlFromBinary(data,size);if(!xml||!xml->hasTagName("GARDEN"))return;
 auto tree=juce::ValueTree::fromXml(*xml);if(static_cast<int>(tree.getProperty("version",0))!=1)return;
 auto routes=tree.getChildWithName("PATCH");if(!routes.isValid()||routes.getNumChildren()>64)return;
 garden::Graph g;for(auto wire:routes){const int from=wire["from"],to=wire["to"];
  if(from<0||from>=garden::portCount||to<0||to>=garden::portCount)return;
  if(!g.add({static_cast<garden::Port>(from),static_cast<garden::Port>(to),static_cast<float>(wire["gain"]),static_cast<bool>(wire["mute"])}))return;
 }
 tree.removeChild(routes,nullptr);
 if(tree.getNumChildren()!=parameterCount)return;
 bool seen[parameterCount]{};
 for(auto child:tree){const juce::String id=child["id"].toString();int n=-1;for(int i=0;i<parameterCount;++i)if(id==ids[i]){n=i;break;}
  if(n<0||seen[n])return;seen[n]=true;const float value=child["value"];const auto& range=state.getParameter(id)->getNormalisableRange();
  if(!std::isfinite(value)||value<range.start||value>range.end)return;
 }
 state.replaceState(tree);updatePatch(g);panic.store(true);
}
void GardenProcessor::factory(int index){
 for(const auto* id:ids){auto* parameter=state.getParameter(id);parameter->beginChangeGesture();parameter->setValueNotifyingHost(parameter->getDefaultValue());parameter->endChangeGesture();}
 const auto set=[&](const char* id,float value){auto* p=state.getParameter(id);p->beginChangeGesture();p->setValueNotifyingHost(p->convertTo0to1(value));p->endChangeGesture();};
 set("mode",index==0?1.f:2.f);set("fold",index==2?.55f:.08f);set("timbre",index==2?.5f:.18f);set("decay",index==1?1.2f:.25f);
 set("wet",index==1?.45f:.18f);set("density",index==1?.45f:.8f);set("seed",42);set("run",0);updatePatch(garden::Graph::normal());panic.store(true);
}
juce::AudioProcessorEditor* GardenProcessor::createEditor(){return new GardenEditor(*this);}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new GardenProcessor();}
