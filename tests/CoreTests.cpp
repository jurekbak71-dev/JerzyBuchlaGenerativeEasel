// SPDX-License-Identifier: AGPL-3.0-only
#include "Engine.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <new>
#include <atomic>
#include <thread>
#include <vector>
thread_local bool realtime=false;
thread_local int realtimeAllocations=0;
void* operator new(std::size_t bytes){if(realtime)++realtimeAllocations;if(void* p=std::malloc(bytes?bytes:1))return p;throw std::bad_alloc();}
void operator delete(void* p)noexcept{std::free(p);}
void operator delete(void* p,std::size_t)noexcept{std::free(p);}
void* operator new[](std::size_t bytes){return ::operator new(bytes);}
void operator delete[](void* p)noexcept{std::free(p);}
void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
using namespace garden;
int checks=0;
void check(bool b,const char* what){++checks;if(!b){std::cerr<<"FAIL "<<what<<'\n';std::exit(1);}std::cout<<"PASS "<<what<<'\n';}
double energy(const std::vector<float>& x){double s=0;for(auto f:x)s+=f*f;return s/x.size();}
std::vector<float> render(int block,Settings settings,double rate=48000,Graph graph=Graph::normal(),int samples=96000){
 Engine e;e.prepare(rate);e.setGraph(graph);std::vector<float> result;result.reserve(samples);
 for(int n=0;n<samples;n+=block){e.begin(settings,{true,n*120./(60*rate),120,true});
  for(int i=0;i<std::min(block,samples-n);++i)result.push_back(e.sample(i)[0]);}
 return result;
}
int main(){
 Graph g;check(g.add({Port::osc,Port::lpg1Audio}),"audio cable accepted");check(!g.add({Port::midiGate,Port::fmIn}),"unsafe gate/audio conversion rejected");
 check(!g.add({Port::lpg1,Port::fmIn}),"zero delay feedback rejected");check(g.size==1,"failed edit leaves graph intact");
 check(g.add({Port::osc,Port::lpg2Audio}),"fan out accepted");check(g.add({Port::noise,Port::lpg1Audio}),"multiple input sources accepted");
 check(!g.add({Port::osc,Port::lpg1Audio}),"duplicate rejected");check(!g.add({Port::noise,Port::mixA,NAN}),"NaN gain rejected");
 GraphMailbox mailbox;std::atomic<bool> finished{false};std::atomic<bool> valid{true};
 std::thread producer([&]{for(int i=0;i<100000;++i){auto p=Graph::normal();p.cables[0].gain=(i%3)*.5f;mailbox.publish(p);}finished=true;});
 Graph received;while(!finished){if(mailbox.consume(received)&&!received.compile())valid=false;}producer.join();check(valid,"100000 concurrent mailbox publications remain valid");
 Settings s;s.mode=Mode::midi;auto silence=render(256,s);check(energy(silence)==0,"MIDI mode does not self start");
 s.mode=Mode::generative;auto a=render(32,s),b=render(1024,s);check(energy(a)>.0001,"host synced generator produces audio without MIDI");check(a==b,"offline/block size deterministic render bit exact");
 s.seed=99;auto c=render(64,s);check(a!=c,"seed changes musical/audio output");
 auto empty=render(256,s,48000,Graph{});check(energy(empty)==0,"empty graph is silent: graph controls output");
 auto patched=Graph::normal();check(patched.add({Port::mod,Port::fmIn}),"FM cable added");s.fm=.8f;auto fm=render(256,s,48000,patched);auto dry=render(256,s);check(fm!=dry,"adding modulation cable changes rendered audio");
 auto muted=Graph::normal();muted.cables[2].muted=true;muted.compile();auto disconnected=render(64,s,48000,muted);check(energy(disconnected)==0,"muting oscillator cable silences audio path");
 Engine midi;midi.prepare(48000);s={};s.wet=0;midi.begin(s,{});midi.noteOn(69,1);std::vector<float> tone(48000);
 for(int i=0;i<48000;++i){tone[i]=midi.sample(i)[0];}
 check(energy(tone)>.001,"MIDI note creates sound");
 int crossings=0;for(int i=24001;i<48000;++i)if(tone[i-1]<=0&&tone[i]>0)++crossings;check(std::abs(crossings*2-440)<=2,"A4 oscillator frequency 440 Hz");
 midi.sustain(true);midi.noteOff(69);float sustained=0;for(int i=0;i<10000;++i)sustained+=std::abs(midi.sample(i)[0]);check(sustained>1,"sustain holds released note");
 midi.sustain(false);float tail=0;for(int i=0;i<300000;++i){auto out=midi.sample(i);if(i>290000)tail+=std::abs(out[0]);}check(tail<.001f,"sustain release decays without stuck note");
 s.mode=Mode::generative;s.hostSync=false;s.run=false;auto stopped=render(64,s);check(energy(stopped)==0,"FREE clock requires explicit start");s.run=true;check(energy(render(64,s))>.0001,"FREE start produces audio");
 for(double rate:{44100.,48000.,96000.})for(int block:{32,64,256,1024}){
  s={};s.mode=Mode::generative;s.fm=1;s.fold=1;s.wet=1;s.cutoff=20000;s.decay=4;
  auto stress=render(block,s,rate,patched,static_cast<int>(rate/3));bool finite=true;for(float f:stress)finite&=std::isfinite(f)&&std::abs(f)<=.98f;
  check(finite,("finite bounded stress "+std::to_string(int(rate))+" / "+std::to_string(block)).c_str());
 }
 Engine realtimeEngine;realtimeEngine.prepare(48000);auto rtGraph=Graph::normal();
 realtime=true;
 realtimeEngine.begin(s,{true,0,120,true});realtimeEngine.setGraph(rtGraph);
 for(int i=0;i<48000;++i)realtimeEngine.sample(i);
 realtime=false;check(realtimeAllocations==0,"DSP begin, graph swap and 48000 samples allocate no heap");
 const auto lpgEnergy=[](float hz,int mode){LPG l;double total=0;for(int i=0;i<48000;++i){float y=l.tick(float(std::sin(2*pi*hz*i/48000)),1,48000,.2f,2000,mode);if(i>24000)total+=y*y;}return total;};
 check(lpgEnergy(10000,0)<lpgEnergy(10000,1)*.1,"LPG attenuates high frequency relative to VCA");
 check(lpgEnergy(100,0)>lpgEnergy(10000,0)*5,"LPG preserves bass relative to treble");
 Engine loop;loop.prepare(48000);Settings midiSettings;loop.begin(midiSettings,{true,0,120,true});loop.noteOn(60,1);for(int i=0;i<256;++i)loop.sample(i);
 loop.begin(midiSettings,{true,0,120,true});float afterLoop=0;for(int i=0;i<256;++i)afterLoop+=std::abs(loop.sample(i)[0]);check(afterLoop==0,"timeline loop clears stale MIDI gate");
 Engine malformed;malformed.prepare(NAN);malformed.begin(s,{true,1.e300,INFINITY,true});bool guarded=true;for(int i=0;i<100;++i){auto x=malformed.sample(i);guarded &= std::isfinite(x[0])&&std::isfinite(x[1]);}check(guarded,"invalid sample rate and extreme host timeline guarded");
 Engine priority,reference;priority.prepare(48000);reference.prepare(48000);Settings plain;plain.wet=0;
 priority.begin(plain,{});reference.begin(plain,{});priority.noteOn(60,.2f);reference.noteOn(60,.2f);priority.noteOn(64,1.f);
 for(int i=0;i<100;++i){priority.sample(i);}
 priority.noteOff(64);
 double fallbackEnergy=0,referenceEnergy=0;
 for(int i=0;i<48000;++i){auto x=priority.sample(i)[0],y=reference.sample(i)[0];if(i>24000){fallbackEnergy+=x*x;referenceEnergy+=y*y;}}
 check(std::abs(fallbackEnergy/referenceEnergy-1)<.05,"last-note fallback restores original velocity");
 Engine duplicate;duplicate.prepare(48000);duplicate.begin(plain,{});duplicate.noteOn(60,1);for(int i=0;i<1000;++i)duplicate.sample(i);
 duplicate.noteOff(60);duplicate.sustain(true);duplicate.noteOff(60);double duplicateTail=0;
 for(int i=0;i<96000;++i){auto x=duplicate.sample(i)[0];if(i>90000)duplicateTail+=std::abs(x);}
 check(duplicateTail<.001,"duplicate NoteOff cannot resurrect a released note under sustain");
 auto start=std::chrono::steady_clock::now();render(256,s,48000,patched,48000*5);double sec=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
 std::cout<<"BENCHMARK 5 seconds audio / "<<sec<<" seconds compute (not host CPU measurement)\n";
 std::cout<<"RESULT "<<checks<<" checks passed\n";
}
