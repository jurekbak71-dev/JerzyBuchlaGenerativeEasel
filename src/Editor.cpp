// SPDX-License-Identifier: AGPL-3.0-only
#include "Editor.h"
namespace {
juce::Colour jackColour(garden::Kind kind){switch(kind){case garden::Kind::audio:return juce::Colour(0xffdd6c25);case garden::Kind::cv:return juce::Colour(0xff237fa4);case garden::Kind::gate:return juce::Colour(0xffb93637);case garden::Kind::trigger:return juce::Colour(0xff775193);case garden::Kind::clock:return juce::Colour(0xff775193);}return juce::Colours::black;}
juce::Rectangle<int> panel(int m){return {20+(m%6)*214,100+(m/6)*260,204,250};}
constexpr int moduleForPort[]={0,1,4,4,5,4,7,7,7,6,6,2,3,8,9,10,0,0,0,1,2,2,3,3,8,8,9,10,10};
const char* names[]={"COMPLEX OSC","MODULATION","LOW PASS GATE I","LOW PASS GATE II","PULSER / RANDOM","FIVE STAGE CV","GENERATIVE BRAIN","MIDI / ENVELOPE","MIXER / DRIVE","SPRING FIELD","STEREO OUTPUT","PATCH INSPECTOR"};
}
void GardenEditor::knob(const char* id,const char* text,int m,int slot){
 auto s=std::make_unique<juce::Slider>();s->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);s->setTextBoxStyle(juce::Slider::TextBoxBelow,false,60,18);
 s->setColour(juce::Slider::rotarySliderFillColourId,juce::Colour(0xffd27332));s->setColour(juce::Slider::textBoxTextColourId,juce::Colour(0xff20323c));
 s->setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::transparentBlack);s->setName(text);s->setTitle(text);s->setTooltip(text);
 auto p=panel(m);s->setBounds(p.getX()+8+slot*63,p.getY()+55,62,72);addAndMakeVisible(*s);
 auto label=std::make_unique<juce::Label>();label->setText(text,juce::dontSendNotification);label->setFont(juce::FontOptions(11));label->setJustificationType(juce::Justification::centred);label->setColour(juce::Label::textColourId,juce::Colour(0xff33434a));label->setBounds(s->getX()-2,s->getY()-20,66,20);addAndMakeVisible(*label);
 attachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.state,id,*s));
 s->textFromValueFunction=[integer=juce::String(id)=="seed"||juce::String(id)=="division"||juce::String(id)=="cutoff"](double v){return juce::String(v,integer?0:2);};
 s->updateText();knobs.push_back(std::move(s));labels.push_back(std::move(label));
}
GardenEditor::GardenEditor(GardenProcessor& p):AudioProcessorEditor(p),processor(p){
 setLookAndFeel(&look);look.setColour(juce::ComboBox::backgroundColourId,juce::Colour(0xffe5e5dd));look.setColour(juce::ComboBox::textColourId,juce::Colour(0xff24313b));
 look.setColour(juce::TextButton::buttonColourId,juce::Colour(0xffd0d5d2));look.setColour(juce::TextButton::textColourOffId,juce::Colour(0xff273841));
 setSize(1320,800);setResizable(true,true);setResizeLimits(1320,800,2640,1600);
 const char* choiceIds[]={"mode",nullptr,"lpgMode","root","scale"};
 choices[0].addItemList({"MANUAL","MIDI","GENERATIVE","HYBRID"},1);choices[1].addItemList({"MIDI | Init","Ambient | Field","Berlin | Motion"},1);
 choices[2].addItemList({"LPG","VCA","VCF"},1);choices[3].addItemList({"C","C#","D","Eb","E","F","F#","G","Ab","A","Bb","B"},1);choices[4].addItemList({"Minor","Major","Dorian","Pentatonic"},1);
 for(int i=0;i<5;++i){addAndMakeVisible(choices[i]);if(choiceIds[i])choiceAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.state,choiceIds[i],choices[i]));}
 choices[1].setSelectedId(1,juce::dontSendNotification);choices[1].onChange=[this]{processor.factory(choices[1].getSelectedItemIndex());};
 for(auto* b:{&start,&sync,&panic,&expert,&save,&load,&normal,&clear})addAndMakeVisible(b);
 for(auto* b:{&start,&sync,&expert})b->setClickingTogglesState(true);
 buttonAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.state,"run",start));
 buttonAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.state,"sync",sync));
 buttonAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.state,"expert",expert));
 panic.onClick=[this]{for(const auto* id:{"run","mode"}){auto* parameter=processor.state.getParameter(id);parameter->beginChangeGesture();parameter->setValueNotifyingHost(parameter->convertTo0to1(juce::String(id)=="mode"?1.f:0.f));parameter->endChangeGesture();}processor.panic.store(true);};normal.onClick=[this]{processor.updatePatch(garden::Graph::normal());};clear.onClick=[this]{processor.updatePatch({});};
 save.onClick=[this]{chooser=std::make_unique<juce::FileChooser>("Save patch",juce::File(),"*.vgarden");chooser->launchAsync(juce::FileBrowserComponent::saveMode|juce::FileBrowserComponent::canSelectFiles|juce::FileBrowserComponent::warnAboutOverwriting,[safe=juce::Component::SafePointer<GardenEditor>(this)](const juce::FileChooser& fc){if(!safe)return;auto file=fc.getResult();if(file==juce::File())return;juce::MemoryBlock data;safe->processor.getStateInformation(data);safe->status=file.withFileExtension("vgarden").replaceWithData(data.getData(),data.getSize())?"Patch saved":"Save failed";});};
 load.onClick=[this]{chooser=std::make_unique<juce::FileChooser>("Load patch",juce::File(),"*.vgarden");chooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safe=juce::Component::SafePointer<GardenEditor>(this)](const juce::FileChooser& fc){if(!safe)return;auto file=fc.getResult();if(!file.existsAsFile()||file.getSize()>1024*1024)return;juce::MemoryBlock data;if(file.loadFileAsData(data))safe->processor.setStateInformation(data.getData(),static_cast<int>(data.getSize()));});};
 knob("tune","TUNE st",0,0);knob("timbre","SHAPE",0,1);knob("fold","FOLD",0,2);
 knob("modRate","RATE Hz",1,0);knob("fm","FM DEPTH",1,1);knob("decay","DECAY s",2,0);knob("cutoff","CUTOFF Hz",2,1);
 knob("tempo","BPM",4,0);knob("swing","SWING",4,1);knob("division","STEPS/BEAT",5,0);
 knob("density","DENSITY",6,0);knob("motion","MOTION",6,1);knob("seed","SEED",6,2);
 knob("wet","WET/DRY",9,0);knob("level","LEVEL",10,0);
 int counts[12]{};for(int i=0;i<garden::portCount;++i){auto box=panel(moduleForPort[i]);jacks[i]={float(box.getX()+18),float(box.getY()+(moduleForPort[i]==6?180:150)+counts[moduleForPort[i]]++*24)};}
 for(int i=0;i<5;++i){
  auto slider=std::make_unique<juce::Slider>();slider->setSliderStyle(juce::Slider::LinearVertical);slider->setTextBoxStyle(juce::Slider::TextBoxBelow,false,34,18);
  slider->setBounds(1100+i*38,270,36,66);slider->setName("Sequence step "+juce::String(i+1));slider->setTitle(slider->getName());slider->setColour(juce::Slider::textBoxTextColourId,juce::Colour(0xff20323c));
  addAndMakeVisible(*slider);attachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.state,"step"+juce::String(i+1),*slider));
  slider->textFromValueFunction=[](double v){return juce::String(v,0);};slider->updateText();knobs.push_back(std::move(slider));
 }
 resized();startTimerHz(24);
}
GardenEditor::~GardenEditor(){if(heldNote>=0)processor.uiNote(heldNote,0);setLookAndFeel(nullptr);}
void GardenEditor::resized(){
 // Fixed logical hardware panel, uniformly scaled for HiDPI and host resize.
 const float scale=std::min(getWidth()/1320.f,getHeight()/800.f);
 for(auto* c:getChildren())c->setTransform(juce::AffineTransform::scale(scale));
 choices[0].setBounds(350,24,126,30);choices[1].setBounds(484,24,160,30);choices[2].setBounds(676,140,150,28);
 choices[3].setBounds(26,495,60,26);choices[4].setBounds(92,495,124,26);
 sync.setBounds(656,24,65,30);start.setBounds(729,24,105,30);panic.setBounds(842,24,65,30);expert.setBounds(915,24,80,30);save.setBounds(1003,24,65,30);load.setBounds(1076,24,65,30);
 normal.setBounds(1100,400,90,28);clear.setBounds(1200,400,90,28);
}
void GardenEditor::setExpert(bool b){auto* p=processor.state.getParameter("expert");p->setValueNotifyingHost(b?1.f:0.f);expertView=b;repaint();}
void GardenEditor::timerCallback(){expertView=processor.state.getRawParameterValue("expert")->load()>.5f;repaint();}
juce::Path GardenEditor::cablePath(const garden::Cable& c)const {
 auto a=jacks[garden::idx(c.from)],b=jacks[garden::idx(c.to)];juce::Path p;p.startNewSubPath(a);const float sag=std::min(160.f,40.f+a.getDistanceFrom(b)*.22f);
 p.cubicTo(a.x,a.y+sag,b.x,b.y+sag,b.x,b.y);return p;
}
void GardenEditor::paint(juce::Graphics& g){
 g.fillAll(juce::Colour(0xffbbc1c2));g.addTransform(juce::AffineTransform::scale(std::min(getWidth()/1320.f,getHeight()/800.f)));
 juce::ColourGradient metal(juce::Colour(0xfff1f0e6),0,0,juce::Colour(0xffb8c0c0),0,800,false);g.setGradientFill(metal);g.fillRect(0,0,1320,800);
 g.setColour(juce::Colour(0xff233d48));g.setFont(juce::FontOptions(25).withStyle("Bold"));g.drawText("VOLTAGE GARDEN",22,20,320,34,juce::Justification::centredLeft);
 g.setFont(juce::FontOptions(11));g.drawText("JERZY AUDIO  /  WEST COAST GENERATIVE INSTRUMENT",24,59,540,18,juce::Justification::centredLeft);
 g.drawText("AUDIO  |    CV  |    GATE  |    CLOCK  |      OUT to IN",700,66,590,18,juce::Justification::centredRight);
 for(int m=0;m<12;++m){auto box=panel(m);g.setColour(juce::Colour(0xff586d75));g.drawRoundedRectangle(box.toFloat(),3,1);
  g.setColour(juce::Colour(0xff344e59));g.fillRoundedRectangle(float(box.getX()),float(box.getY()),204,28,3);g.setColour(juce::Colour(0xfff3f0de));g.setFont(juce::FontOptions(12).withStyle("Bold"));g.drawText(names[m],box.reduced(8).withHeight(14),juce::Justification::centredLeft);
  for(auto corner:{juce::Point<int>(box.getX()+6,box.getBottom()-7),juce::Point<int>(box.getRight()-6,box.getBottom()-7)}){g.setColour(juce::Colour(0xff74828a));g.fillEllipse(float(corner.x-2),float(corner.y-2),4,4);}
 }
 g.setColour(juce::Colour(0xff50626a));g.setFont(juce::FontOptions(12));
 g.drawFittedText("LPG / VCA / VCF\nShared decay & cutoff\nTwo independent paths",674,180,180,54,juce::Justification::centredLeft,3);
 g.drawFittedText("Last-note priority\nVelocity / bend +/-2 st\nCC1 / sustain / pressure",245,400,180,75,juce::Justification::centredLeft,4);
 g.drawFittedText("SUM bounded audio sum\nSoft saturation\nPatch both channels",459,400,180,75,juce::Justification::centredLeft,4);
 g.drawFittedText("Algorithmic resonators\nInspired spring texture\nNo sampled hardware",740,410,115,75,juce::Justification::centredLeft,4);
 const auto graph=processor.patch();
 for(int i=0;i<garden::portCount;++i){auto point=jacks[i];const auto& info=garden::ports[i];g.setColour(juce::Colour(0xff6b7477));g.fillEllipse(point.x-8,point.y-8,16,16);g.setColour(jackColour(info.kind));g.fillEllipse(point.x-6,point.y-6,12,12);g.setColour(juce::Colour(0xff26363d));g.fillEllipse(point.x-2.5f,point.y-2.5f,5,5);
  g.setFont(juce::FontOptions(10));g.drawText(juce::String(info.input?"IN  ":"OUT ")+juce::String(info.name.data()),int(point.x)+12,int(point.y)-8,170,16,juce::Justification::centredLeft);
 }
 for(size_t i=0;i<graph.size;++i){const auto& c=graph.cables[i];auto path=cablePath(c);g.setColour(juce::Colours::black.withAlpha(c.muted?.08f:.22f));g.strokePath(path,juce::PathStrokeType(6));g.setColour(jackColour(garden::ports[garden::idx(c.from)].kind).withAlpha(c.muted?.2f:.82f));g.strokePath(path,juce::PathStrokeType(3));}
 if(dragging>=0){juce::Path p;auto a=jacks[dragging];p.startNewSubPath(a);p.cubicTo(a.x,a.y+80,cursor.x,cursor.y+80,cursor.x,cursor.y);g.setColour(jackColour(garden::ports[dragging].kind));g.strokePath(p,juce::PathStrokeType(3));}
 g.setColour(juce::Colour(0xff263e4a));g.fillRoundedRectangle(20,646,1274,98,4);
 for(int i=0;i<24;++i){auto x=30+i*52;const bool black=std::array<bool,12>{false,true,false,true,false,false,true,false,true,false,true,false}[i%12];g.setColour(i+48==heldNote?juce::Colour(0xffdb873f):black?juce::Colour(0xff7f969f):juce::Colour(0xffe6e1c7));g.fillRoundedRectangle(float(x),656,48,72,3);g.setColour(juce::Colour(0xff35464e));g.setFont(juce::FontOptions(11));g.drawText(juce::MidiMessage::getMidiNoteName(i+48,true,true,4),x,700,48,20,juce::Justification::centred);}
 g.setColour(juce::Colour(0xff2b454f));g.drawText(status,24,753,1270,22,juce::Justification::centredLeft);
 if(expertView){g.setColour(juce::Colour(0xff273f49));g.fillRoundedRectangle(1098,444,190,135,3);g.setColour(juce::Colour(0xffe6e5d4));g.setFont(juce::FontOptions(12));g.drawFittedText("EXPERT\n"+juce::String(int(graph.size))+" / 64 cables\nDAG validated\nCV: 1 unit = 60 st\nSUM clamped to +/-2\nPeak: "+juce::String(processor.peak.load(),3),1108,450,170,123,juce::Justification::topLeft,7);}
}
int GardenEditor::jackAt(juce::Point<float> p)const {for(int i=0;i<garden::portCount;++i)if(jacks[i].getDistanceFrom(p)<10)return i;return -1;}
void GardenEditor::mouseDown(const juce::MouseEvent& e){
 const auto pos=e.position/std::min(getWidth()/1320.f,getHeight()/800.f);int port=jackAt(pos);
 if(e.mods.isPopupMenu()){
  auto graph=processor.patch();for(size_t i=graph.size;i>0;--i)if(cablePath(graph.cables[i-1]).getNearestPoint(pos,cursor)>=0&&cursor.getDistanceFrom(pos)<8){
   const size_t index=i-1;auto c=graph.cables[index];juce::PopupMenu menu;menu.addSectionHeader(juce::String(garden::ports[garden::idx(c.from)].name.data())+" to "+juce::String(garden::ports[garden::idx(c.to)].name.data()));menu.addItem(1,c.muted?"Unmute":"Mute");menu.addItem(2,"Remove");menu.addItem(3,"Invert polarity");menu.addItem(4,"Gain 0.5");menu.showMenuAsync(juce::PopupMenu::Options(),[safe=juce::Component::SafePointer<GardenEditor>(this),graph,index](int result)mutable{if(!safe||result==0)return;if(result==1)graph.cables[index].muted=!graph.cables[index].muted;if(result==2)graph.remove(index);if(result==3)graph.cables[index].gain*=-1;if(result==4)graph.cables[index].gain=.5f;safe->processor.updatePatch(graph);});return;
  }return;
 }
 if(port>=0&&!garden::ports[port].input){dragging=port;cursor=pos;return;}
 if(pos.y>=656&&pos.y<730&&pos.x>=30&&pos.x<1278){heldNote=48+int((pos.x-30)/52);processor.uiNote(heldNote,110);}
}
void GardenEditor::mouseDrag(const juce::MouseEvent& e){cursor=e.position/std::min(getWidth()/1320.f,getHeight()/800.f);repaint();}
void GardenEditor::mouseUp(const juce::MouseEvent& e){
 if(heldNote>=0){processor.uiNote(heldNote,0);heldNote=-1;}
 if(dragging>=0){const auto pos=e.position/std::min(getWidth()/1320.f,getHeight()/800.f);int dest=jackAt(pos);if(dest>=0){auto g=processor.patch();if(g.add({static_cast<garden::Port>(dragging),static_cast<garden::Port>(dest)})&&processor.updatePatch(g))status="Cable connected: live DSP graph updated";else status="Rejected: incompatible port, duplicate, capacity or feedback cycle";}dragging=-1;repaint();}
}
