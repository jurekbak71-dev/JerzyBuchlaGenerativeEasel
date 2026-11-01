// SPDX-License-Identifier: AGPL-3.0-only
#include "Editor.h"
namespace {
const juce::Colour ink(0xff293426),red(0xff9d303b),blue(0xff334f91);
juce::Colour jackColour(garden::Kind kind){switch(kind){case garden::Kind::audio:return juce::Colour(0xffd29629);case garden::Kind::cv:return juce::Colour(0xff3981a5);case garden::Kind::gate:return juce::Colour(0xffc34742);case garden::Kind::trigger:case garden::Kind::clock:return juce::Colour(0xff6472b0);}return ink;}
struct Lane {const char* id;const char* label;};
constexpr Lane lanes[]={{"tune","PITCH"},{"timbre","TIMBRE"},{"fold","FOLD"},{"modRate","MOD Hz"},{"fm","FM"},{"decay","DECAY"},{"cutoff","CUTOFF"},{"tempo","BPM"},{"swing","SWING"},{"division","DIVISION"},{"density","DENSITY"},{"motion","MOTION"},{"seed","SEED"},{"wet","SPRING"},{"level","LEVEL"},{"step1","STEP 1"},{"step2","STEP 2"},{"step3","STEP 3"},{"step4","STEP 4"},{"step5","STEP 5"}};
constexpr garden::Port upperOutputs[]={garden::Port::osc,garden::Port::mod,garden::Port::noise,garden::Port::lpg1,garden::Port::lpg2,garden::Port::mix,garden::Port::reverb,garden::Port::output};
constexpr garden::Port controllerOutputs[]={garden::Port::random,garden::Port::sequence,garden::Port::pulse,garden::Port::envelope,garden::Port::midiPitch,garden::Port::midiGate,garden::Port::generativePitch,garden::Port::generativeGate};
constexpr float keyLeft=294,keyPitch=40,keyTop=612,keyBottom=866;
bool accidental(int note){return std::array<bool,12>{false,true,false,true,false,false,true,false,true,false,true,false}[note%12];}
void screw(juce::Graphics& g,float x,float y){g.setColour(juce::Colour(0xff555b56));g.fillEllipse(x-4,y-4,8,8);g.setColour(juce::Colour(0xffdbddd4));g.fillEllipse(x-3,y-3,6,6);g.setColour(juce::Colour(0xff68706a));g.drawLine(x-2,y+1,x+2,y-1,1);}
}
float GardenEditor::panelScale()const noexcept{return std::min(getWidth()/float(designWidth),getHeight()/float(designHeight));}
void GardenEditor::control(const char* id,const char* text,juce::Rectangle<int> bounds,bool fader,juce::Colour colour){
 auto slider=std::make_unique<juce::Slider>();slider->setSliderStyle(fader?juce::Slider::LinearVertical:juce::Slider::RotaryHorizontalVerticalDrag);
 slider->setTextBoxStyle(juce::Slider::TextBoxBelow,false,fader?44:56,17);slider->setColour(juce::Slider::thumbColourId,colour);
 slider->setColour(juce::Slider::textBoxTextColourId,ink);slider->setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::transparentBlack);
 slider->setName(text);slider->setTitle(text);slider->setTooltip(juce::String(text)+" — "+processor.state.getParameter(id)->getName(80));slider->setBounds(bounds);addAndMakeVisible(*slider);
 auto label=std::make_unique<juce::Label>();label->setText(text,juce::dontSendNotification);label->setFont(juce::FontOptions(fader?9.f:9.5f));label->setJustificationType(juce::Justification::centred);label->setColour(juce::Label::textColourId,ink);label->setBounds(bounds.getX()-3,bounds.getY()-17,bounds.getWidth()+6,16);addAndMakeVisible(*label);
 attachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.state,id,*slider));
 const auto name=juce::String(id);slider->textFromValueFunction=[integer=name=="seed"||name=="division"||name=="cutoff"||name.startsWith("step")](double value){return juce::String(value,integer?0:2);};slider->updateText();knobs.push_back(std::move(slider));labels.push_back(std::move(label));
}
GardenEditor::GardenEditor(GardenProcessor& p):AudioProcessorEditor(p),processor(p){
 setLookAndFeel(&look);look.setColour(juce::ComboBox::backgroundColourId,juce::Colour(0xffe8e6d8));look.setColour(juce::ComboBox::textColourId,ink);look.setColour(juce::ComboBox::arrowColourId,ink);
 look.setColour(juce::TextButton::buttonColourId,juce::Colour(0xffd9d8ca));look.setColour(juce::TextButton::buttonOnColourId,blue);look.setColour(juce::TextButton::textColourOffId,ink);look.setColour(juce::TextButton::textColourOnId,juce::Colours::white);
 setSize(designWidth,designHeight);setResizable(true,true);setResizeLimits(924,644,designWidth*2,designHeight*2);getConstrainer()->setFixedAspectRatio(double(designWidth)/designHeight);
 const char* choiceIds[]={"mode",nullptr,"lpgMode","root","scale"};
 choices[0].addItemList({"MANUAL","MIDI","GENERATIVE","HYBRID"},1);choices[1].addItemList({"MIDI | Init","Ambient | Field","Berlin | Motion"},1);choices[2].addItemList({"LPG","VCA","VCF"},1);choices[3].addItemList({"C","C#","D","Eb","E","F","F#","G","Ab","A","Bb","B"},1);choices[4].addItemList({"Minor","Major","Dorian","Pentatonic"},1);
 for(int i=0;i<5;++i){addAndMakeVisible(choices[i]);if(choiceIds[i])choiceAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.state,choiceIds[i],choices[i]));}
 choices[1].setSelectedId(1,juce::dontSendNotification);choices[1].onChange=[this]{processor.factory(choices[1].getSelectedItemIndex());};
 for(auto* button:{&start,&sync,&panic,&expert,&save,&load,&normal,&clear,&cables,&octaveDown,&octaveUp})addAndMakeVisible(button);
 for(auto* button:{&start,&sync,&expert})button->setClickingTogglesState(true);
 buttonAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.state,"run",start));buttonAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.state,"sync",sync));buttonAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.state,"expert",expert));
 panic.setColour(juce::TextButton::buttonColourId,juce::Colour(0xffbd443e));panic.setColour(juce::TextButton::textColourOffId,juce::Colours::white);
 panic.onClick=[this]{for(const auto* id:{"run","mode"}){auto* parameter=processor.state.getParameter(id);parameter->beginChangeGesture();parameter->setValueNotifyingHost(parameter->convertTo0to1(juce::String(id)=="mode"?1.f:0.f));parameter->endChangeGesture();}processor.panic.store(true);};normal.onClick=[this]{processor.updatePatch(garden::Graph::normal());};clear.onClick=[this]{processor.updatePatch({});};
 save.onClick=[this]{chooser=std::make_unique<juce::FileChooser>("Save patch",juce::File(),"*.vgarden");chooser->launchAsync(juce::FileBrowserComponent::saveMode|juce::FileBrowserComponent::canSelectFiles|juce::FileBrowserComponent::warnAboutOverwriting,[safe=juce::Component::SafePointer<GardenEditor>(this)](const juce::FileChooser& fc){if(!safe)return;auto file=fc.getResult();if(file==juce::File())return;juce::MemoryBlock data;safe->processor.getStateInformation(data);safe->status=file.withFileExtension("vgarden").replaceWithData(data.getData(),data.getSize())?"Patch saved":"Save failed";});};
 load.onClick=[this]{chooser=std::make_unique<juce::FileChooser>("Load patch",juce::File(),"*.vgarden");chooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safe=juce::Component::SafePointer<GardenEditor>(this)](const juce::FileChooser& fc){if(!safe)return;auto file=fc.getResult();if(!file.existsAsFile()||file.getSize()>1024*1024)return;juce::MemoryBlock data;if(file.loadFileAsData(data))safe->processor.setStateInformation(data.getData(),static_cast<int>(data.getSize()));});};
 for(int i=0;i<20;++i){const auto colour=i<5?juce::Colour(0xffddad54):i<7?juce::Colour(0xffb95148):i<10?blue:i<13?juce::Colour(0xff5b8e5c):i<15?juce::Colour(0xff777c70):juce::Colour(0xffd7a549);control(lanes[i].id,lanes[i].label,{274+i*46,249,44,185},true,colour);}
 const Lane fine[]={{"tune","FINE PITCH"},{"modRate","MOD RATE"},{"fm","FM DEPTH"},{"timbre","SHAPE"},{"fold","FOLD"},{"decay","DECAY"},{"cutoff","CUTOFF"},{"tempo","TEMPO"},{"swing","SWING"},{"density","DENSITY"},{"motion","MOTION"},{"wet","SPRING"}};
 for(int i=0;i<12;++i)control(fine[i].id,fine[i].label,{37+(i%3)*70,200+(i/3)*63,60,48},false,juce::Colour(0xff515746));
 control("wet","SPRING",{1210,151,66,65},false,blue);control("level","OUTPUT",{1210,251,66,65},false,blue);control("cutoff","LPG FREQ",{1210,351,66,65},false,blue);
 for(int i=0;i<8;++i){jacks[garden::idx(upperOutputs[i])]={310.f+i*122,142};jacks[garden::idx(controllerOutputs[i])]={320.f+i*120,524};}
 for(int i=16;i<garden::portCount;++i)jacks[i]={294.f+(i-16)*72,194};
 cables.setClickingTogglesState(true);cables.setToggleState(true,juce::dontSendNotification);cables.onClick=[this]{showCables=cables.getToggleState();repaint();};
 octaveDown.onClick=[this]{releaseKey();baseNote=std::max(0,baseNote-12);repaint();};octaveUp.onClick=[this]{releaseKey();baseNote=std::min(96,baseNote+12);repaint();};
 resized();startTimerHz(24);
}
GardenEditor::~GardenEditor(){releaseKey();setLookAndFeel(nullptr);}
void GardenEditor::resized(){
 for(auto* child:getChildren())child->setTransform(dynamic_cast<juce::ResizableCornerComponent*>(child)?juce::AffineTransform():juce::AffineTransform::scale(panelScale()));
 choices[1].setBounds(296,16,160,28);choices[0].setBounds(466,16,125,28);sync.setBounds(601,16,62,28);start.setBounds(673,16,104,28);expert.setBounds(787,16,76,28);save.setBounds(873,16,62,28);load.setBounds(945,16,62,28);
 choices[2].setBounds(52,143,150,24);choices[3].setBounds(468,562,62,24);choices[4].setBounds(540,562,138,24);
 normal.setBounds(40,510,94,26);clear.setBounds(142,510,94,26);cables.setBounds(40,546,196,26);panic.setBounds(1210,438,66,26);
 octaveDown.setBounds(290,562,78,24);octaveUp.setBounds(378,562,78,24);
}
void GardenEditor::setExpert(bool b){processor.state.getParameter("expert")->setValueNotifyingHost(b?1.f:0.f);expertView=b;repaint();}
void GardenEditor::timerCallback(){expertView=processor.state.getRawParameterValue("expert")->load()>.5f;repaint();}
juce::Path GardenEditor::cablePath(const garden::Cable& cable)const {
 const auto a=jacks[garden::idx(cable.from)],b=jacks[garden::idx(cable.to)];const float sag=std::min(145.f,25.f+a.getDistanceFrom(b)*.17f);juce::Path path;path.startNewSubPath(a);path.cubicTo(a.x,a.y+sag,b.x,b.y+sag,b.x,b.y);return path;
}
void GardenEditor::paint(juce::Graphics& g){
 g.fillAll(juce::Colour(0xff14191c));g.addTransform(juce::AffineTransform::scale(panelScale()));
 g.setColour(juce::Colour(0xffdedfd5));g.setFont(juce::FontOptions(19).withStyle("Bold"));g.drawText("JERZY VOLTAGE GARDEN",20,14,266,30,juce::Justification::centredLeft);
 g.setFont(juce::FontOptions(10));g.setColour(juce::Colour(0xff98a69e));g.drawText("GENERATIVE EASEL  /  PATCHABLE INSTRUMENT",1020,16,282,26,juce::Justification::centredRight);
 g.setGradientFill(juce::ColourGradient(juce::Colour(0xffe2e4df),10,60,juce::Colour(0xff62696a),1310,896,false));g.fillRoundedRectangle(10,60,1300,836,9);
 g.setColour(juce::Colour(0xff242b2e));g.fillRoundedRectangle(20,70,1280,816,4);
 g.setGradientFill(juce::ColourGradient(juce::Colour(0xffe7e7d7),0,78,juce::Colour(0xffc7c9b6),0,478,false));g.fillRect(26,78,1268,400);
 g.setColour(juce::Colour(0xff969d8d));g.drawRect(26,78,1268,400,1);g.drawLine(260,88,260,466,1);g.drawLine(1202,88,1202,466,1);
 for(int line=0;line<190;++line){g.setColour(juce::Colours::white.withAlpha(line%3==0?.04f:.015f));g.drawHorizontalLine(80+line*2,27,1293);}
 g.setColour(ink);g.setFont(juce::FontOptions(11).withStyle("Bold"));
 const struct {const char* name;int lane,count;} groups[]={{"COMPLEX / MODULATION",0,5},{"DUAL LPG",5,2},{"PULSER / CLOCK",7,3},{"GENERATIVE",10,3},{"SPRING / OUT",13,2},{"FIVE STAGE SEQUENCER",15,5}};
 for(const auto& group:groups){const int x=274+group.lane*46,w=group.count*46;g.drawFittedText(group.name,x,88,w-3,20,juce::Justification::centred,2);g.setColour(juce::Colour(0xff727c68));g.drawLine(float(x+3),113,float(x+w-6),113,1);g.setColour(ink);if(group.lane>0){g.setColour(juce::Colour(0xffa1a58f));g.drawLine(float(x-2),223,float(x-2),448,.6f);g.setColour(ink);}}
 g.setColour(juce::Colour(0xff343e35));g.fillRoundedRectangle(44,94,156,40,2);g.setColour(juce::Colour(0xffccd775));g.setFont(juce::FontOptions(12).withStyle("Bold"));const auto settings=processor.settings();g.drawText(settings.mode==garden::Mode::generative?"GENERATIVE":settings.mode==garden::Mode::hybrid?"HYBRID":settings.mode==garden::Mode::midi?"MIDI CONTROL":"MANUAL",50,97,144,16,juce::Justification::centred);
 g.setFont(juce::FontOptions(10));g.drawText("SEED "+juce::String(settings.seed)+"  /  "+juce::String(settings.tempo,1)+" BPM",50,115,144,14,juce::Justification::centred);
 g.setColour(ink);g.setFont(juce::FontOptions(9));g.drawText("PRECISION CONTROLS",38,169,212,11,juce::Justification::centred);g.drawText("DUAL LOW PASS GATE",36,448,216,17,juce::Justification::centred);
 for(int lane=0;lane<20;++lane){g.setColour(ink.withAlpha(.55f));g.setFont(juce::FontOptions(8));g.drawText(juce::String(lane+1),274+lane*46,441,44,15,juce::Justification::centred);}
 g.setColour(juce::Colour(0xff1b2427));g.fillRect(26,490,232,394);g.setColour(juce::Colour(0xff0e1416));g.drawRect(32,496,220,382,1);
 g.setColour(blue);g.fillRect(270,490,1024,108);g.setColour(juce::Colour(0xffd6a9ab));g.drawLine(272,552,1292,552,1);
 g.setColour(red);g.fillRect(270,598,1024,286);g.setColour(juce::Colour(0xffdfc8b8));g.setFont(juce::FontOptions(11).withStyle("Bold"));g.drawText("JERZY TOUCH CONTROLLER",696,562,570,24,juce::Justification::centred);
 for(int i=0;i<24;++i){const float x=keyLeft+i*keyPitch;const bool sharp=accidental(i+baseNote),active=i+baseNote==heldNote;
  g.setColour(juce::Colour(0xff76232e));g.drawRect(x-2,608.f,39.f,263.f,1.f);
  g.setGradientFill(juce::ColourGradient(active?juce::Colour(0xffffd591):juce::Colour(0xffe4e1d5),x,670,active?juce::Colour(0xffd39b58):juce::Colour(0xffa7adac),x+34,860,false));g.fillRect(x+4,670.f,27.f,190.f);
  g.setColour(active?juce::Colour(0xffffe6ad):juce::Colour(0xffd7dddc));g.fillRect(x+4,sharp?615.f:643.f,27.f,sharp?38.f:15.f);
  g.setColour(juce::Colour(0xff6f7774));g.drawRect(x+4,670.f,27.f,190.f,1.f);g.setColour(juce::Colour(0xfff0d9c9));g.setFont(juce::FontOptions(9));g.drawText(juce::MidiMessage::getMidiNoteName(i+baseNote,true,true,4),int(x)-2,sharp?655:615,39,20,juce::Justification::centred);
 }
 const auto graph=processor.patch();
 for(int i=0;i<garden::portCount;++i){const auto point=jacks[i];const auto& info=garden::ports[i];g.setColour(juce::Colour(0xff666e65));g.fillEllipse(point.x-8,point.y-8,16,16);g.setColour(juce::Colour(0xffe6e5d7));g.drawEllipse(point.x-8,point.y-8,16,16,1);g.setColour(jackColour(info.kind));g.fillEllipse(point.x-6,point.y-6,12,12);g.setColour(juce::Colour(0xff26322c));g.fillEllipse(point.x-2,point.y-2,4,4);
  const bool lower=point.y>480;g.setColour(lower?juce::Colour(0xfff1e3d4):ink);g.setFont(juce::FontOptions(info.input?8.f:9.f));g.drawFittedText(juce::String(info.input?"IN ":"OUT ")+juce::String(info.name.data()),int(point.x)-35,int(point.y)+11,70,24,juce::Justification::centredTop,2);
 }
 if(showCables)for(size_t i=0;i<graph.size;++i){const auto& cable=graph.cables[i];const auto path=cablePath(cable);g.setColour(juce::Colours::black.withAlpha(cable.muted?.08f:.3f));g.strokePath(path,juce::PathStrokeType(5.5f));g.setColour(jackColour(garden::ports[garden::idx(cable.from)].kind).withAlpha(cable.muted?.22f:.9f));g.strokePath(path,juce::PathStrokeType(2.8f));}
 if(dragging>=0){const auto a=jacks[dragging];juce::Path path;path.startNewSubPath(a);path.cubicTo(a.x,a.y+65,cursor.x,cursor.y+65,cursor.x,cursor.y);g.setColour(jackColour(garden::ports[dragging].kind));g.strokePath(path,juce::PathStrokeType(3));}
 g.setColour(juce::Colour(0xffaab6a6));g.setFont(juce::FontOptions(11));g.drawText(juce::String(int(graph.size))+" PATCH CORDS  /  LIVE DSP",40,590,196,20,juce::Justification::centred);
 if(expertView){g.setColour(juce::Colour(0xffd4dcbd));g.setFont(juce::FontOptions(11));g.drawFittedText("PATCH INSPECTOR\n\nDAG validated\nCV: 1 unit = 60 semitones\nSUM limited to +/-2\nPeak: "+juce::String(processor.peak.load(),3)+"\n\nRight-click a cable:\nmute / gain / polarity / delete",43,626,194,174,juce::Justification::topLeft,12);}
 else{g.setColour(juce::Colour(0xff303b3c));g.drawLine(66,726,213,726,1);g.drawLine(66,731,213,731,1);g.setColour(juce::Colour(0xffe2e5d8));g.setFont(juce::FontOptions(29).withStyle("Bold"));g.drawText("Jerzy",45,782,190,42,juce::Justification::centred);g.setFont(juce::FontOptions(9));g.drawText("VOLTAGE GARDEN",45,825,190,18,juce::Justification::centred);}
 for(auto point:{juce::Point<float>{31,83},{1289,83},{31,473},{1289,473},{31,879},{1289,879}})screw(g,point.x,point.y);
 g.setColour(juce::Colour(0xffb0b9ae));g.setFont(juce::FontOptions(10));g.drawText(status,22,899,1270,17,juce::Justification::centredLeft);
}
int GardenEditor::jackAt(juce::Point<float> p)const {for(int i=0;i<garden::portCount;++i)if(jacks[i].getDistanceFrom(p)<10)return i;return -1;}
void GardenEditor::mouseDown(const juce::MouseEvent& e){
 const auto pos=e.position/panelScale();int port=jackAt(pos);
 if(e.mods.isPopupMenu()){
  auto graph=processor.patch();for(size_t i=showCables?graph.size:0;i>0;--i)if(cablePath(graph.cables[i-1]).getNearestPoint(pos,cursor)>=0&&cursor.getDistanceFrom(pos)<8){
   const size_t index=i-1;auto c=graph.cables[index];juce::PopupMenu menu;menu.addSectionHeader(juce::String(garden::ports[garden::idx(c.from)].name.data())+" to "+juce::String(garden::ports[garden::idx(c.to)].name.data()));menu.addItem(1,c.muted?"Unmute":"Mute");menu.addItem(2,"Remove");menu.addItem(3,"Invert polarity");menu.addItem(4,"Gain 0.5");menu.showMenuAsync(juce::PopupMenu::Options(),[safe=juce::Component::SafePointer<GardenEditor>(this),graph,index](int result)mutable{if(!safe||result==0)return;if(result==1)graph.cables[index].muted=!graph.cables[index].muted;if(result==2)graph.remove(index);if(result==3)graph.cables[index].gain*=-1;if(result==4)graph.cables[index].gain=.5f;safe->processor.updatePatch(graph);});return;
  }return;
 }
 if(port>=0&&!garden::ports[port].input){dragging=port;cursor=pos;return;}
 if(const int note=keyAt(pos);note>=0){releaseKey();heldNote=note;processor.uiNote(heldNote,110);repaint();}
}
int GardenEditor::keyAt(juce::Point<float> p)const {
 if(p.y<keyTop||p.y>=keyBottom||p.x<keyLeft||p.x>=keyLeft+24*keyPitch)return -1;
 const int key=int((p.x-keyLeft)/keyPitch);
 return p.x-(keyLeft+key*keyPitch)<35?baseNote+key:-1;
}
void GardenEditor::releaseKey(){if(heldNote>=0){processor.uiNote(heldNote,0);heldNote=-1;}}
void GardenEditor::mouseDrag(const juce::MouseEvent& e){
 cursor=e.position/panelScale();
 if(heldNote>=0){const int next=keyAt(cursor);if(next!=heldNote){releaseKey();if(next>=0){heldNote=next;processor.uiNote(next,110);}}}
 repaint();
}
void GardenEditor::mouseUp(const juce::MouseEvent& e){
 releaseKey();repaint();
 if(dragging>=0){const auto pos=e.position/panelScale();int dest=jackAt(pos);if(dest>=0){auto g=processor.patch();if(g.add({static_cast<garden::Port>(dragging),static_cast<garden::Port>(dest)})&&processor.updatePatch(g))status="Cable connected: live DSP graph updated";else status="Rejected: incompatible port, duplicate, capacity or feedback cycle";}dragging=-1;repaint();}
}
