// SPDX-License-Identifier: AGPL-3.0-only
#pragma once
#include "Patch.h"
#include <numbers>
namespace garden {
constexpr double pi=std::numbers::pi;
enum class Mode { manual, midi, generative, hybrid };
struct Settings {
 Mode mode=Mode::midi; bool hostSync=true, run=false;
 float tempo=110, tune=0, timbre=.2f, fold=0, fm=0, modRate=3, decay=.25f,
       cutoff=5000, level=.65f, wet=.18f, density=.75f, swing=.12f, motion=.5f;
 int root=0, scale=0, seed=42, division=4, lpgMode=0;
 std::array<float,5> steps{0,3,7,5,2};
};
struct Transport { bool playing=false; double beat=0, bpm=120; bool valid=false; };
inline uint32_t hash(uint32_t x) noexcept {x^=x>>16;x*=0x7feb352du;x^=x>>15;x*=0x846ca68bu;return x^(x>>16);}
inline float unit(uint32_t x)noexcept{return static_cast<float>(hash(x)>>8)/16777216.f;}
inline float blep(double t,double dt)noexcept {
 if(t<dt){t/=dt;return static_cast<float>(t+t-t*t-1);}
 if(t>1-dt){t=(t-1)/dt;return static_cast<float>(t*t+t+t+1);}return 0;
}
struct Oscillator {
 double phase=0; float lp=0;
 float tick(double hz,double rate,float shape,float foldAmount,float phaseMod=0)noexcept {
  // Four substeps plus polyBLEP at discontinuities; not a claim of alias-free nonlinear FM.
  float sum=0; const auto dt=std::clamp(hz/(rate*4),0.,.2);
  for(int i=0;i<4;++i){phase+=dt;phase-=std::floor(phase);auto p=phase+phaseMod;p-=std::floor(p);
   float sine=static_cast<float>(std::sin(2*pi*p));float saw=static_cast<float>(2*p-1)-blep(p,dt);
   float pulse=(p<.5?1.f:-1.f)+blep(p,dt)-blep(std::fmod(p+.5,1.),dt);
   float tri=static_cast<float>(1-4*std::abs(p-.5));
   float x=shape<.333f?std::lerp(sine,tri,shape*3):shape<.666f?std::lerp(tri,saw,(shape-.333f)*3):std::lerp(saw,pulse,(shape-.666f)*3);
   const float drive=1+foldAmount*5;
   x=std::lerp(x,static_cast<float>(2/pi*std::asin(std::sin(x*drive*pi*.5))),foldAmount);
   lp+=.45f*(x-lp);sum+=lp;
  } return sum*.25f;
 }
};
struct LPG {
 float env=0, filter=0;
 float tick(float x,float gate,double rate,float decay,float cutoff,int mode)noexcept {
  const float target=std::clamp(gate,0.f,1.f);
  const float coeff=static_cast<float>(1-std::exp(-(target>env?500.:6.907755/std::max(.015f,decay))/rate));
  env+=(target-env)*coeff;
  const float fc=mode==1?cutoff:std::clamp(cutoff*(.03f+.97f*env),20.f,20000.f);
  const float a=static_cast<float>(1-std::exp(-2*pi*std::min(double(fc),rate*.4)/rate));
  filter+=a*(x-filter);if(std::abs(filter)<1.e-20f)filter=0;
  return (mode==1?x:filter)*(mode==2?1.f:env);
 }
};
class Engine {
 double rate=48000, freeBeat=0, expectedBeat=0; bool wasPlaying=false, expectedValid=false;
 Oscillator osc, mod; LPG lpgA,lpgB;
 std::array<float,portCount> v{};
 std::array<std::array<float,8192>,3> delays{}; size_t write=0;
 std::array<bool,128> held{}, sustained{}; std::array<float,128> velocities{}; std::array<uint64_t,128> ages{};uint64_t age=0;
 bool pedal=false; int note=60;float velocity=0,bend=0,pressure=0,wheel=0;
 float smoothLevel=0,smoothFold=0; uint64_t frame=0;
 Settings s{}; Transport transport{}; Graph graph=Graph::normal();
 int selectedNote()const noexcept {uint64_t newest=0;int n=-1;for(int i=0;i<128;++i)if((held[i]||sustained[i])&&ages[i]>=newest){newest=ages[i];n=i;}return n;}
public:
 void prepare(double sampleRate)noexcept {rate=std::isfinite(sampleRate)?std::clamp(sampleRate,8000.,192000.):48000.; reset();}
 void reset()noexcept {
  osc={};mod={};lpgA={};lpgB={};v.fill(0);for(auto& d:delays)d.fill(0);write=0;
  allNotesOff();bend=pressure=wheel=0;age=0;freeBeat=0;expectedBeat=0;expectedValid=false;wasPlaying=false;frame=0;smoothLevel=0;smoothFold=0;
 }
 void allNotesOff()noexcept {held.fill(false);sustained.fill(false);ages.fill(0);velocities.fill(0);pedal=false;velocity=0;}
 void noteOn(int n,float vel)noexcept {if(n<0||n>127||!std::isfinite(vel))return;if(vel<=0){noteOff(n);return;}held[n]=true;sustained[n]=false;ages[n]=++age;note=n;velocity=std::clamp(vel,0.f,1.f);velocities[n]=velocity;}
 void noteOff(int n)noexcept {if(n>=0&&n<128&&held[n]){held[n]=false;sustained[n]=pedal;}}
 void sustain(bool on)noexcept{pedal=on;if(!on)sustained.fill(false);}
 void pitchBend(float normalized)noexcept{bend=std::clamp(normalized,-1.f,1.f)*2;}
 void aftertouch(float p)noexcept{pressure=std::clamp(p,0.f,1.f);}
 void polyAftertouch(int n,float p)noexcept{if(n==selectedNote())aftertouch(p);}
 void modWheel(float p)noexcept{wheel=std::clamp(p,0.f,1.f);}
 void setGraph(const Graph& g)noexcept{graph=g;}
 const Graph& getGraph()const noexcept{return graph;}
 void begin(const Settings& settings,Transport t)noexcept {
  s=settings; transport=t;
  if(!std::isfinite(t.bpm)||t.bpm<20||t.bpm>400)transport.bpm=120;
  if(!std::isfinite(t.beat)||std::abs(t.beat)>1.e9){transport.beat=0;transport.valid=false;t.valid=false;}
  if(wasPlaying&&!t.playing){allNotesOff();lpgA.env=lpgB.env=0;}
  if(t.valid&&t.playing&&(!expectedValid||std::abs(t.beat-expectedBeat)>1.e-5)){
   if(expectedValid)allNotesOff();
   // Explicit seek/loop resets all time-dependent DSP; same timeline restart gives same render.
   osc={};mod={};lpgA={};lpgB={};for(auto& d:delays)d.fill(0);write=0;frame=0;smoothLevel=0;smoothFold=0;
  }
  wasPlaying=t.playing;expectedValid=t.valid&&t.playing;
 }
 std::array<float,2> sample(int offset)noexcept {
  const auto activeNote=selectedNote();if(activeNote>=0){note=activeNote;velocity=velocities[activeNote];}
  const bool midi=s.mode==Mode::midi||s.mode==Mode::hybrid||s.mode==Mode::manual;
  const bool gen=(s.mode==Mode::generative||s.mode==Mode::hybrid)&&(s.hostSync?transport.valid&&transport.playing:s.run);
  const double bpm=s.hostSync?transport.bpm:std::clamp(double(s.tempo),20.,300.);
  const double beat=s.hostSync?transport.beat+offset*bpm/(60*rate):freeBeat;
  const int division=std::clamp(s.division,1,128);
  const double grid=beat*division;const auto step=static_cast<int64_t>(std::floor(grid));
  const double frac=grid-std::floor(grid);const uint32_t key=static_cast<uint32_t>(step)^static_cast<uint32_t>(s.seed);
  const int phrase=static_cast<int>(std::floor(beat/16));const int chord=std::array<int,4>{0,3,5,4}[static_cast<unsigned>(phrase)%4];
  constexpr int scales[4][7]={{0,2,3,5,7,8,10},{0,2,4,5,7,9,11},{0,2,3,5,7,9,10},{0,2,4,7,9,12,14}};
  // Repeating eight-step motif, phrase-controlled mutation and bounded voice movement.
  int degree=static_cast<int>(hash(static_cast<uint32_t>(step%8)^static_cast<uint32_t>(s.seed))%7);
  if(unit(key+12)<s.motion*.25f)degree=(degree+1)%7;
  int genNote=48+std::clamp(s.root,0,11)+scales[std::clamp(s.scale,0,3)][(degree+chord)%7]+12*((degree+chord)/7);
  if(s.mode==Mode::hybrid&&activeNote>=0)genNote+=note%12-s.root;
  const double swing=(step&1)?std::clamp(s.swing,0.f,.45f):0;
  float gg=gen&&frac>=swing&&frac<swing+.40+unit(key+2)*.13&&unit(key)<s.density?(.55f+.4f*unit(key+4)):0;
  v[idx(Port::midiPitch)]=midi&&activeNote>=0?static_cast<float>((note-48+bend)/60.):0;
  v[idx(Port::generativePitch)]=gen?static_cast<float>((genNote-48)/60.):0;
  // HYBRID uses generator pitch with MIDI as harmonic context, rather than summing two pitches.
  if(s.mode==Mode::hybrid)v[idx(Port::midiPitch)]=0;
  v[idx(Port::midiGate)]=midi&&activeNote>=0&&s.mode!=Mode::hybrid?velocity:0;
  v[idx(Port::generativeGate)]=gg;
  v[idx(Port::random)]=unit(key+31)*2-1;
  v[idx(Port::sequence)]=s.steps[static_cast<uint64_t>(step)%5]/60.f;
  v[idx(Port::pulse)]=frac<.1?1.f:0.f;
  v[idx(Port::envelope)]=static_cast<float>(std::exp(-frac*8));
  // Noise is indexed by audio sample, independent of callback block sizes.
  v[idx(Port::noise)]=unit(static_cast<uint32_t>(frame)^static_cast<uint32_t>(s.seed))*2-1;
  const auto input=[&](Port p){return graph.input(p,v);};
  smoothFold+=.002f*(std::clamp(s.fold+input(Port::foldIn)*.5f,0.f,1.f)-smoothFold);
  smoothLevel+=.002f*(std::clamp(s.level,0.f,1.f)-smoothLevel);
  for(int n:graph.order)switch(n){
   case 1:{double pitch=48+s.tune+60*input(Port::pitchIn);double hz=440*std::exp2((std::clamp(pitch,0.,120.)-69)/12.);
    hz*=std::exp2(std::clamp(double(input(Port::fmIn)*(s.fm+wheel)), -4.,4.));
    v[idx(Port::osc)]=osc.tick(hz,rate,std::clamp(s.timbre+pressure*.15f,0.f,1.f),smoothFold);break;}
   case 2:v[idx(Port::mod)]=mod.tick(std::max(.01,double(s.modRate)*std::exp2(input(Port::modRateIn)*4)),rate,0,0);break;
   case 3:v[idx(Port::lpg1)]=lpgA.tick(input(Port::lpg1Audio),input(Port::lpg1Gate),rate,s.decay,s.cutoff,s.lpgMode);break;
   case 4:v[idx(Port::lpg2)]=lpgB.tick(input(Port::lpg2Audio),input(Port::lpg2Gate),rate,s.decay*1.4f,s.cutoff,s.lpgMode);break;
   case 5:v[idx(Port::mix)]=std::tanh((input(Port::mixA)+input(Port::mixB))*.6f);break;
   case 6:{const float dry=input(Port::reverbIn);float tail=0;const size_t lengths[3]={1499,2137,3163};
    for(size_t k=0;k<3;++k){const size_t length=std::clamp(static_cast<size_t>(lengths[k]*rate/48000),size_t(1),size_t(8191));
     const float y=delays[k][(write+8192-length)%8192];delays[k][write]=std::tanh(dry*.3f+y*.73f);tail+=y/3;}
    write=(write+1)%8192;v[idx(Port::reverb)]=std::lerp(dry,tail,std::clamp(s.wet,0.f,1.f));break;}
   case 7:v[idx(Port::output)]=(input(Port::outL)+input(Port::outR))*.5f;break;
   default:break;
  }
  ++frame;if(gen&&!s.hostSync)freeBeat+=bpm/(60*rate);
  expectedBeat=transport.beat+(offset+1)*transport.bpm/(60*rate);
  const auto safe=[&](float x){return std::isfinite(x)?std::clamp(x*smoothLevel,-.98f,.98f):0.f;};
  return {safe(input(Port::outL)),safe(input(Port::outR))};
 }
};
}
