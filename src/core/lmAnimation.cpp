#include "lmConsole.hpp"
#include "lmAnimation.hpp"

namespace LightmanCore {

/////////////////////////////
// Keyframe implementation //
/////////////////////////////

Keyframe::Keyframe(float t, vec3 mod) {

  time = t;
  modulator = mod;
}

float Keyframe::get_time() { return time; }

vec3 Keyframe::get_modulator() { return modulator; }

/////////////////////////////
// Timeline implementation //
/////////////////////////////

Timeline::Timeline(Layer *layer) {

  this->layer = layer;

  len = 0;

  keyframes = vector<Keyframe>();
}

float Timeline::get_len() {

  if (keyframes.size()) {
    return keyframes.back().get_time();
  }

  return 0;
}

Layer *Timeline::get_layer() { return layer; }

vector<Keyframe> Timeline::get_keyframes() { return keyframes; }

void Timeline::add_keyframe(Keyframe key) {

  keyframes.push_back(key);

  // TODO: sort by time again?
}

void Timeline::del_keyframe(size_t index) {

  if (index < keyframes.size()) {
    keyframes.erase(keyframes.begin() + index);
  }

  return;
}

Keyframe Timeline::get_frame(float t) {

  if (keyframes.size() == 0) {
    return Keyframe(0, vec3(1, 1, 1));
  }
  if (keyframes.size() == 1) {
    return keyframes.front();
  }

  float t_min = keyframes.front().get_time();
  float t_max = keyframes.back().get_time();

  // if time is outside timeline, return the start/end frame
  if (t < t_min) {
    return keyframes.front();
  }
  if (t > t_max) {
    return keyframes.back();
  }

  Keyframe *prev_key, *next_key;

  // find prev & next
  for (size_t i = 0; i < keyframes.size(); ++i) {

    Keyframe *key = &keyframes[i];

    if (key->get_time() < t) {
      prev_key = key;
      continue;
    } else {
      next_key = key;
      break;
    }
  }

  float dist = next_key->get_time() - prev_key->get_time();
  float prev_r = 1.0 - (t - prev_key->get_time()) / dist;
  float next_r = 1.0 - (next_key->get_time() - t) / dist;

  vec3 mod =
      prev_r * prev_key->get_modulator() + next_r * next_key->get_modulator();

  return Keyframe(t, mod);
}

void Timeline::clear() {

  len = 0;
  keyframes.clear();
}

//////////////////////////////
// Animation implementation //
//////////////////////////////

Animation::Animation() {

  playing = false;

  len = 0.0f;

  loc_last = 0.0f;
  loc_curr = 0.0f;

  fps = 0.0f;

  timelines = set<Timeline *>();
}

Animation::~Animation() { timelines.clear(); }

void Animation::start() {

  playing = true;

  loc_last = 0.0f;
  loc_curr = 0.0f;

  sys_last = system_clock::now();
  sys_curr = system_clock::now();
}

void Animation::end() {

  playing = false;

  loc_last = 0.0f;
  loc_curr = 0.0f;
}

void Animation::update() {

  // update system timers
  sys_curr = system_clock::now();
  double elapsed_seconds = ((duration<double>)(sys_curr - sys_last)).count();
  sys_last = sys_curr;

  // step animation
  step(elapsed_seconds);
}

void Animation::step(float t) {

  // update local time
  loc_curr += t;
  while (loc_curr > len) {
    loc_curr -= len;
  }

  // step each timeline
  Timeline *timeline;
  Layer *layer;

  set<Timeline *>::iterator it = timelines.begin();
  while (it != timelines.end()) {

    timeline = *it;
    layer = timeline->get_layer();

    Keyframe frame = timeline->get_frame(loc_curr);
    layer->set_modulator(frame.get_modulator());

    ++it;
  }

  // update local time
  loc_last = loc_curr;

  return;
}

float Animation::get_len() { return len; }

void Animation::set_len(size_t len) { this->len = len; }

size_t Animation::get_fps() { return fps; }

void Animation::set_fps(size_t fps) { this->fps = fps; }

void Animation::add_timeline(Timeline *timeline) { timelines.insert(timeline); }

void Animation::del_timeline(Timeline *timeline) { timelines.erase(timeline); }

set<Timeline *> Animation::get_timelines() { return timelines; }

}; // namespace LightmanCore
