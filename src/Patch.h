// SPDX-License-Identifier: AGPL-3.0-only
#pragma once
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <string_view>
namespace garden {
enum class Kind { audio, cv, gate, trigger, clock };
enum class Port : uint8_t {
 osc, mod, noise, random, sequence, pulse, envelope, midiPitch, midiGate, generativePitch,
 generativeGate, lpg1, lpg2, mix, reverb, output,
 pitchIn, fmIn, foldIn, modRateIn, lpg1Audio, lpg1Gate, lpg2Audio, lpg2Gate,
 mixA, mixB, reverbIn, outL, outR, count
};
constexpr int portCount = static_cast<int>(Port::count);
struct PortInfo { std::string_view name; Kind kind; bool input; int node; };
inline constexpr std::array<PortInfo, portCount> ports {{
 {"OSC",Kind::audio,false,1},{"MOD",Kind::audio,false,2},{"NOISE",Kind::audio,false,0},
 {"RANDOM",Kind::cv,false,0},{"SEQ 5",Kind::cv,false,0},{"PULSER",Kind::clock,false,0},
 {"ENVELOPE",Kind::cv,false,0},{"MIDI PITCH",Kind::cv,false,0},{"MIDI GATE",Kind::gate,false,0},
 {"GEN PITCH",Kind::cv,false,0},{"GEN GATE",Kind::gate,false,0},
 {"LPG I",Kind::audio,false,3},{"LPG II",Kind::audio,false,4},{"MIX",Kind::audio,false,5},
 {"SPRING",Kind::audio,false,6},{"OUT",Kind::audio,false,7},
 {"PITCH CV",Kind::cv,true,1},{"FM",Kind::audio,true,1},{"FOLD CV",Kind::cv,true,1},
 {"RATE CV",Kind::cv,true,2},{"LPG I AUDIO",Kind::audio,true,3},{"LPG I GATE",Kind::gate,true,3},
 {"LPG II AUDIO",Kind::audio,true,4},{"LPG II GATE",Kind::gate,true,4},
 {"MIX A",Kind::audio,true,5},{"MIX B",Kind::audio,true,5},{"SPRING IN",Kind::audio,true,6},
 {"LEFT",Kind::audio,true,7},{"RIGHT",Kind::audio,true,7}
}};
constexpr int idx(Port p) { return static_cast<int>(p); }
struct Cable { Port from{}, to{}; float gain=1; bool muted=false; };
struct Graph {
 static constexpr size_t capacity=64;
 std::array<Cable,capacity> cables{}; size_t size=0;
 std::array<int,8> order{0,1,2,3,4,5,6,7};
 bool compile() noexcept {
  bool edges[8][8]{}; int indegree[8]{};
  if(size>capacity)return false;
  for(size_t i=0;i<size;++i) {
   const auto& c=cables[i];
   if(idx(c.from)>=portCount||idx(c.to)>=portCount||!std::isfinite(c.gain)||std::abs(c.gain)>2)return false;
   const auto a=ports[idx(c.from)], b=ports[idx(c.to)];
   if(a.input||!b.input)return false;
   // Explicit safe conversion: clock/trigger/gate may drive each other; CV may drive gate.
   const bool compatible=a.kind==b.kind || (b.kind==Kind::gate && a.kind!=Kind::audio);
   if(!compatible)return false;
   if(!c.muted&&!edges[a.node][b.node]){edges[a.node][b.node]=true; ++indegree[b.node];}
  }
  int used=0; bool done[8]{};
  for(int pass=0;pass<8;++pass){int n=-1;for(int j=0;j<8;++j)if(!done[j]&&indegree[j]==0){n=j;break;}
   if(n<0)return false;
   done[n]=true;order[used++]=n;for(int j=0;j<8;++j)if(edges[n][j])--indegree[j];
  } return true;
 }
 bool add(Cable c) noexcept {
  if(size==capacity)return false;
  for(size_t i=0;i<size;++i)if(cables[i].from==c.from&&cables[i].to==c.to)return false;
  cables[size++]=c;if(compile())return true;--size;compile();return false;
 }
 void remove(size_t n) noexcept {if(n<size){for(size_t i=n+1;i<size;++i)cables[i-1]=cables[i];--size;compile();}}
 float input(Port p,const std::array<float,portCount>& v)const noexcept {
  float s=0;for(size_t i=0;i<size;++i)if(cables[i].to==p&&!cables[i].muted)s+=v[idx(cables[i].from)]*cables[i].gain;
  return std::isfinite(s)?std::clamp(s,-2.f,2.f):0.f;
 }
 static Graph normal() {
  Graph g;
  g.add({Port::midiPitch,Port::pitchIn});g.add({Port::generativePitch,Port::pitchIn});
  g.add({Port::osc,Port::lpg1Audio});g.add({Port::midiGate,Port::lpg1Gate});
  g.add({Port::generativeGate,Port::lpg1Gate});g.add({Port::lpg1,Port::mixA});
  g.add({Port::mix,Port::reverbIn});g.add({Port::reverb,Port::outL});g.add({Port::reverb,Port::outR});return g;
 }
};
// SPSC mailbox: producer owns pending storage until publication; consumer copies at block boundary.
// No shared_ptr destruction, allocation, spin or lock in the audio callback.
class GraphMailbox {
 std::array<Graph,3> slots{};
 std::atomic<int> middle{1}; int front=0, back=2;
 static constexpr int dirty=4;
public:
 void publish(const Graph& g) noexcept {slots[back]=g;back=middle.exchange(back|dirty,std::memory_order_acq_rel)&3;}
 bool consume(Graph& g) noexcept {
  if(!(middle.load(std::memory_order_acquire)&dirty))return false;
  front=middle.exchange(front,std::memory_order_acq_rel)&3;g=slots[front];return true;
 }
};
static_assert(std::atomic<int>::is_always_lock_free);
}
