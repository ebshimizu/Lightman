#include "lmWorkspace.hpp"
#include "lmConsole.hpp"
#include "lmArnold.hpp"

// System
#include <sys/stat.h>
#include <libgen.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>

// OpenEXR
#include "ImfTestFile.h"    // Header checks
#include "ImfInputFile.h"   // Imf file IO
#include "ImfOutputFile.h"  // Imf file IO
#include "ImfRgbaFile.h"    // Imf RGBA scaneline
#include "ImfChannelList.h" // Imf channels

// Arnold
#ifdef LIGHTMAN_ARNOLD_SUPPORT
#include "ai.h"
#endif

// FFmpeg - maybe later
//#include <libavutil/imgutils.h>
//#include <libavcodec/avcodec.h>
//#include <libswscale/swscale.h>

// Jassson
#include "jansson.h"

namespace LightmanCore {

using namespace Imf;
using namespace Imath;

Workspace::Workspace(const char *title) {

  this->title = title;

  // Has no size till something is loaded
  w = 0;
  h = 0;

  // Create a new empty compositor.
  compositor = new Compositor();

  // Use the default tone mapper for new workspaces.
  tonemapper = new ToneMapper();

  // New workspaces contains no animation.
  animation = NULL;

  // Compositor output will be set when compositor is initialized.
  compositor_output = NULL;

  // No application space output buffers until set.
  hdr_output_buffer = NULL;
  bmp_output_buffer = NULL;
}

Workspace::~Workspace() {}

void Workspace::close() {

  delete compositor;
  delete tonemapper;

  if (animation)
    delete animation;
}

string Workspace::get_title() { return title; }

void Workspace::set_title(const char *title) { this->title = title; }

size_t Workspace::get_width() { return w; }
size_t Workspace::get_height() { return h; }

Compositor *Workspace::get_compositor() { return compositor; }

ToneMapper *Workspace::get_tonemapper() { return tonemapper; }

Animation *Workspace::get_animation() { return animation; }

void Workspace::set_compositor(Compositor *compositor) {

  this->compositor = compositor;
}

void Workspace::set_tonemapper(ToneMapper *tonemapper) {

  this->tonemapper = tonemapper;
}

void Workspace::set_animation(Animation *animation) {

  this->animation = animation;
}

void Workspace::set_bmp_output_buffer(char *buffer) {

  if (buffer)
    this->bmp_output_buffer = buffer;
  tonemapper->set_output_bmp(bmp_output_buffer);
}

void Workspace::dump_bmp_to_buffer() {

  if (!bmp_output_buffer) {
    lmerr("No bitmap output buffer is set");
    return;
  }

  compositor->render();
  tonemapper->apply_bmp();
}

void Workspace::set_hdr_output_buffer(Pixel3 *buffer) {

  if (buffer)
    this->hdr_output_buffer = buffer;
  tonemapper->set_output_hdr(hdr_output_buffer);
}

void Workspace::dump_hdr_to_buffer() {

  if (!hdr_output_buffer) {
    lmerr("No HDR output buffer is set");
    return;
  }

  compositor->render();
  tonemapper->apply_hdr();
}

int Workspace::dump_exr(const char *path) {

  lmmsg("Writing OpenEXR file: " << path);

  // if application did not set an output buffer
  if (!hdr_output_buffer) {

    // create temporary output buffer
    hdr_output_buffer = new Pixel3[w * h];
    set_hdr_output_buffer(hdr_output_buffer);

    // render and save as exr
    compositor->render();
    tonemapper->apply_hdr();
    save_exr(path);

    // free temporary output buffer
    delete[] hdr_output_buffer;
    hdr_output_buffer = NULL;

    lmmsg("Done");
    return 0;
  }

  compositor->render();
  save_exr(path);

  lmmsg("Done");
  return 0;
}

int Workspace::dump_png(const char *path) {

  lmmsg("Writing png file: " << path);

  // if application did not set an output buffer
  if (!bmp_output_buffer) {

    // create temporary output buffer
    bmp_output_buffer = new char[3 * w * h];
    set_bmp_output_buffer(bmp_output_buffer);

    // render and save as exr
    compositor->render();
    tonemapper->apply_bmp();
    save_png(path);

    // free temporary output buffer
    delete[] bmp_output_buffer;
    bmp_output_buffer = NULL;

    lmmsg("Done");
    return 0;
  }

  compositor->render();
  save_exr(path);

  lmmsg("Done");
  return 0;
}

int Workspace::dump_mpg(const char *path) { return 0; }

int Workspace::load(const char *path) {

  // file exists?
  struct stat st;
  if (stat(path, &st) < 0) {
    lmerr("File does not exit: " << path);
    return -1;
  }

  bool error;
  string dir = dirname((char *)path);
  string base = basename((char *)path);
  string prefix = base.substr(0, base.find_last_of("."));
  string suffix = base.substr(base.find_last_of(".") + 1);

  // load workspace components
  if (suffix == "lmw") {

    // lmw
    lmmsg("Loading Lightman workspace: " << path);
    error = load_lmw(path);

  } else if (suffix == "exr") {

    // exr
    lmmsg("Loading OpenEXR image: " << path);
    error = load_exr(path);

  } else if (suffix == "ass") {

    // ass
    lmmsg("Loading Arnold Scene Source: " << path);

#ifdef LIGHTMAN_ARNOLD_SUPPORT
    error = load_ass(path);
#else
    lmmsg("Lightman not built with Arnold support");
#endif

  } else {

    // unsupported
    lmerr("Unsupported file format: " << suffix);
    return -1;
  }

  if (error) {
    return -1;
  }

  // create default pipeline
  compositor_output = new Pixel3[w * h];
  compositor->set_output_buffer(compositor_output);
  tonemapper->set_input(compositor_output, w, h);

  lmmsg("Done");
  return 0;
}

int Workspace::save(const char *path) {

  const char *name = title.c_str();
  lmmsg("Saving workspace: " << name);
  save_lmw(name);
  lmmsg("Done");
  return 0;
}

int Workspace::load_lmw(const char *file_path) {

  // load json root
  json_error_t error;
  json_t *root = json_load_file(file_path, 0, &error);
  if (!root)
    return -1;

  // load name
  string title = json_string_value(json_object_get(root, "title"));

  // load layer data
  string dir = dirname((char *)file_path);
  json_t *file_obj;
  const char *file_name;
  json_t *file_arr = json_object_get(root, "files");
  for (size_t i = 0; i < json_array_size(file_arr); ++i) {

    file_obj = json_array_get(file_arr, i);
    file_name = json_string_value(file_obj);

    load_exr((dir + "/" + file_name).c_str());
  }

  // load compositor settings
  json_t *layer_obj;
  Layer *layer;
  const char *layer_name;
  json_t *layer_arr = json_object_get(root, "layers");
  for (size_t i = 0; i < json_array_size(layer_arr); ++i) {

    layer_obj = json_array_get(layer_arr, i);
    layer_name = json_string_value(json_object_get(layer_obj, "name"));
    layer = compositor->get_layer_by_name(layer_name);

    if (layer) {

      // active
      bool active = json_boolean_value(json_object_get(layer_obj, "active"));
      if (active) {
        layer->enable();
      } else {
        layer->disable();
      }

      // modulator
      json_t *mod_obj = json_object_get(layer_obj, "modulator");
      float mod_r = json_real_value(json_array_get(mod_obj, 0));
      float mod_g = json_real_value(json_array_get(mod_obj, 1));
      float mod_b = json_real_value(json_array_get(mod_obj, 2));
      layer->set_modulator(vec3(mod_r, mod_g, mod_b));

    } else {
      lmwrn("Modifiers ignored: no layer named " << layer_name);
    }
  }
  json_array_clear(layer_arr);

  // load ambient layer
  json_t *ambient_layer = json_object_get(root, "ambient");
  if (ambient_layer) {
    const char *layer_name = json_string_value(ambient_layer);
    compositor->set_ambient(compositor->get_layer_by_name(layer_name));
  }

  // load tone map
  json_t *tonemapper_obj = json_object_get(root, "tonemapper");
  string type = json_string_value(json_object_get(tonemapper_obj, "type"));
  if (type == "REINHARD") {

    tonemapper = new TMReinhard();

    float level = json_real_value(json_object_get(tonemapper_obj, "level"));
    float gamma = json_real_value(json_object_get(tonemapper_obj, "gamma"));
    tonemapper->set_level(level);
    tonemapper->set_gamma(gamma);

    float key = json_real_value(json_object_get(tonemapper_obj, "key"));
    float wht = json_real_value(json_object_get(tonemapper_obj, "wht"));
    static_cast<TMReinhard *>(tonemapper)->set_key(key);
    static_cast<TMReinhard *>(tonemapper)->set_wht(wht);

  } else {

    tonemapper = new ToneMapper();

    float level = json_real_value(json_object_get(tonemapper_obj, "level"));
    float gamma = json_real_value(json_object_get(tonemapper_obj, "gamma"));
    tonemapper->set_level(level);
    tonemapper->set_gamma(gamma);
  }

  // load animation
  json_t *animation_obj = json_object_get(root, "animation");
  if (animation_obj) {

    // load animation settings
    animation = new Animation();
    animation->set_len(json_real_value(json_object_get(animation_obj, "len")));
    animation->set_fps(json_real_value(json_object_get(animation_obj, "fps")));

    // load timelines
    json_t *timeline_obj;
    json_t *timeline_arr = json_object_get(animation_obj, "timelines");
    Layer *layer;
    const char *layer_name;
    for (size_t i = 0; i < json_array_size(timeline_arr); ++i) {

      timeline_obj = json_array_get(timeline_arr, i);
      layer_name = json_string_value(json_object_get(timeline_obj, "layer"));
      layer = compositor->get_layer_by_name(layer_name);

      if (layer) {

        Timeline *tl = new Timeline(layer);

        json_t *keyframe_obj;
        json_t *keyframe_arr = json_object_get(timeline_obj, "keyframes");
        for (size_t j = 0; j < json_array_size(keyframe_arr); ++j) {

          keyframe_obj = json_array_get(keyframe_arr, j);

          float t = json_real_value(json_object_get(keyframe_obj, "time"));

          json_t *mod_obj = json_object_get(keyframe_obj, "modulator");
          float mod_r = json_real_value(json_array_get(mod_obj, 0));
          float mod_g = json_real_value(json_array_get(mod_obj, 1));
          float mod_b = json_real_value(json_array_get(mod_obj, 2));

          tl->add_keyframe(Keyframe(t, vec3(mod_r, mod_g, mod_b)));
        }
        json_array_clear(keyframe_arr);

        animation->add_timeline(tl);

      } else {
        lmwrn("Timeline ignored: no layer named " << layer_name);
      }
    }
    json_array_clear(timeline_arr);
  }

  json_decref(root);

  return 0;
}

int Workspace::load_exr(const char *file_path) {

  // check header
  bool isTiled;
  bool isValid = isOpenExrFile(file_path, isTiled);
  if (isValid) {

    // check for tiled exr
    if (isTiled) {
      lmerr("Only scanline images are supported");
      return -1;
    }

  } else {

    // file not valid
    lmerr("Invalid input OpenEXR file: " << file_path);
    return -1;
  }

  // read dimensions
  InputFile file(file_path);
  Box2i dw = file.header().dataWindow();
  w = dw.max.x - dw.min.x + 1;
  h = dw.max.y - dw.min.y + 1;

  // read channel information
  const ChannelList &channels = file.header().channels();
  set<string> names;
  channels.layers(names);

  // read layers
  string layer_name;
  Pixel3 *pixels;
  if (names.size()) {

    // read pixels
    FrameBuffer frame_buffer;
    for (set<string>::iterator i = names.begin(); i != names.end(); ++i) {
      // load layers
      layer_name = *i;

      // allocate memory
      pixels = new Pixel3[w * h]();
      if (!pixels)
        lmerr("Failed to allocate memory for new layer");

      // layer.R
      frame_buffer.insert((layer_name + ".R").c_str(), // name
                          Slice(PixelType::FLOAT,                 // type
                                (char *)&pixels[0].r,  // base
                                sizeof(Pixel3) * 1,    // xstride
                                sizeof(Pixel3) * w,    // ystride
                                1, 1,                  // sampling
                                0.0));                 // fill value
      // layer.R
      frame_buffer.insert((layer_name + ".G").c_str(), // name
                          Slice(PixelType::FLOAT,                 // type
                                (char *)&pixels[0].g,  // base
                                sizeof(Pixel3) * 1,    // xstride
                                sizeof(Pixel3) * w,    // ystride
                                1, 1,                  // sampling
                                0.0));                 // fill value
      // layer.R
      frame_buffer.insert((layer_name + ".B").c_str(), // name
                          Slice(PixelType::FLOAT,                 // type
                                (char *)&pixels[0].b,  // base
                                sizeof(Pixel3) * 1,    // xstride
                                sizeof(Pixel3) * w,    // ystride
                                1, 1,                  // sampling
                                0.0));                 // fill value

      // add layer
      Layer *layer = new Layer(pixels, w, h, layer_name.c_str());
      compositor->add_layer(layer);
      lmout(" - Found layer: " << layer_name);
    }

    // read pixels
    lmout(" - Loading layers from multi-layered EXR file");
    file.setFrameBuffer(frame_buffer);
    file.readPixels(dw.min.y, dw.max.y);

    return 0;
  }

  // otherwise, read pixels from the base layer
  lmout(" - Loading base layer from single-layered EXR file");

  // allocate memory
  pixels = new Pixel3[w * h]();
  if (!pixels)
    lmerr("Failed to allocate memory for new layer");

  // read pixels
  FrameBuffer frame_buffer;
  frame_buffer.insert("R",                        // name
                      Slice(PixelType::FLOAT,     // type
                            (char *)&pixels[0].r, // base
                            sizeof(Pixel3) * 1,   // xstride
                            sizeof(Pixel3) * w,   // ystride
                            1, 1,                 // sampling
                            0.0));                // fill value
  frame_buffer.insert("G",                        // name
                      Slice(PixelType::FLOAT,                // type
                            (char *)&pixels[0].g, // base
                            sizeof(Pixel3) * 1,   // xstride
                            sizeof(Pixel3) * w,   // ystride
                            1, 1,                 // sampling
                            0.0));                // fill value
  frame_buffer.insert("B",                        // name
                      Slice(PixelType::FLOAT,                // type
                            (char *)&pixels[0].b, // base
                            sizeof(Pixel3) * 1,   // xstride
                            sizeof(Pixel3) * w,   // ystride
                            1, 1,                 // sampling
                            0.0));                // fill value

  file.setFrameBuffer(frame_buffer);
  file.readPixels(dw.min.y, dw.max.y);

  // use file name as layer name
  layer_name = basename((char *)file_path);
  layer_name = layer_name.substr(0, layer_name.find_last_of("."));

  // craete new layer for base layer
  Layer *layer = new Layer(pixels, w, h, layer_name.c_str());
  compositor->add_layer(layer);
  lmout(" - Added layer: " << layer_name);

  return 0;
}

#ifdef LIGHTMAN_ARNOLD_SUPPORT
int Workspace::load_ass(const char *file_path) {

  AiBegin();

  // set log path
  AiMsgSetLogFileName("log/arnold.log");
  AiMsgSetLogFileFlags(AI_LOG_ALL);

  // load plugins
  AiLoadPlugins("external/arnold/plugin");
  AiLoadPlugins("external/arnold/shaders");

  // licence server - enter license here
  // AiLicenseSetServer("IP_ADDRESS", PORT);

  // load ass file
  if (AiASSLoad(file_path) < 0) {
    lmerr("Cannot load ass file");
    return -1;
  }
  // get size information
  AtNode *options = AiUniverseGetOptions();
  w = AiNodeGetInt(options, "xres");
  h = AiNodeGetInt(options, "yres");

  // setup buffer driver
  AtNode *driver = AiNode("driver_buffer");
  AiNodeSetStr(driver, "name", "driver_buffer");
  AiNodeSetInt(driver, "width", w);
  AiNodeSetInt(driver, "height", h);
  AiNodeSetFlt(driver, "gamma", 1);
  AiNodeSetBool(driver, "predictive", true);

  // setup buckets
  size_t num_buckets = std::thread::hardware_concurrency();
  BucketPosInfo *bucketpos_info = new BucketPosInfo[num_buckets];
  ProgressInfo *progress_info = new ProgressInfo;
  AiNodeSetPtr(driver, "bucket_pos_pointer", bucketpos_info);
  AiNodeSetPtr(driver, "progress_pointer", progress_info);

  // create a filter - override filter in ass file
  AtNode *filter = AiNode("gaussian_filter");
  AiNodeSetStr(filter, "name", "filter");
  AiNodeSetFlt(filter, "width", 2);

  // use buffer driver for output
  AtArray *outputs = AiNodeGetArray(options, "outputs");
  AiArraySetStr(outputs, 0, "RGBA RGBA filter driver_buffer");

  // add layers
  // this first records each of the light's color and intensity
  // as the layer multiplier information and then disables all lights.
  // Then enable one light at a time and generates per-light renderings
  // using arnold.
  lmout(" - Generating per-light render layers...");

  // find all mesh lights
  size_t num_lights = 0;
  AtNodeIterator *it = AiUniverseGetNodeIterator(AI_NODE_LIGHT);
  while (!AiNodeIteratorFinished(it)) {

    AtNode *light = AiNodeIteratorGetNext(it);

    // create new layer
    string name = AiNodeGetStr(light, "name");
    Pixel3 *pixels = new Pixel3[w * h]();
    Layer *layer = new Layer(pixels, w, h, name.c_str());

    // get light information
    AtRGB rgb = AiNodeGetRGB(light, "color");
    float intensity = AiNodeGetFlt(light, "intensity");
    layer->set_modulator(intensity * vec3(rgb.r, rgb.g, rgb.b));

    // add layer to compositor (render later)
    compositor->add_layer(layer);
    lmout(" - Created layer: " << name);

    // disable light
    // note that this does not disable the mesh
    // and the light shape will be rendered as white
    // will need to override the mesh light metarial
    // color to completely take the light out of
    // the scene
    AiNodeSetDisabled(light, true);

    // increment count
    num_lights++;
  }
  AiNodeIteratorDestroy(it);

  // temp buffer to hold arnold output
  AtRGBA *buffer = new AtRGBA[w * h]();

  // render each per-light layer
  lmout(" - Rendering layers");
  it = AiUniverseGetNodeIterator(AI_NODE_LIGHT);
  while (!AiNodeIteratorFinished(it)) {

    AtNode *light = AiNodeIteratorGetNext(it);

    // enable light
    AiNodeSetDisabled(light, false);

    // render to layer buffer
    // since RGB is not supported, we nned to use rgba here
    AiNodeSetPtr(driver, "buffer_pointer", buffer);

    // render image
    AiRender(AI_RENDER_MODE_CAMERA);

    // copy to layer buffer
    string name = AiNodeGetStr(light, "name");
    Layer *layer = compositor->get_layer_by_name(name.c_str());
    Pixel3 *layer_buffer = layer->get_pixels();
    for (size_t idx = 0; idx < w * h; ++idx) {
      layer_buffer[idx].r = buffer[idx].r;
      layer_buffer[idx].g = buffer[idx].g;
      layer_buffer[idx].b = buffer[idx].b;
    }

    // disable light
    AiNodeSetDisabled(light, true);
  }
  AiNodeIteratorDestroy(it);

  // free temp buffer
  delete[] buffer;

  AiEnd();
  return 0;
}
#endif

int Workspace::save_lmw(const char *file_name) {

  // write root
  json_t *root = json_object();
  json_object_set(root, "title", json_string(title.c_str()));

  // write layers
  // TODO: write out the whole compositor unit instead,
  // include the ambient layer pointer.
  json_t *layer_arr = json_array();
  set<Layer *> layers = compositor->get_layers();
  for (set<Layer *>::iterator it = layers.begin(); it != layers.end(); ++it) {

    Layer *layer = *it;

    json_t *layer_obj = json_object();
    json_t *name = json_string(layer->get_name().c_str());
    json_t *active = json_boolean(layer->is_active());
    json_t *modulator = json_array();
    json_array_append(modulator, json_real(layer->get_modulator().r));
    json_array_append(modulator, json_real(layer->get_modulator().g));
    json_array_append(modulator, json_real(layer->get_modulator().b));

    json_object_set(layer_obj, "name", name);
    json_object_set(layer_obj, "active", active);
    json_object_set(layer_obj, "modulator", modulator);

    json_array_append(layer_arr, layer_obj);
  }
  json_object_set(root, "layers", layer_arr);

  // write tonemapper - common
  string type = tonemapper->get_type();
  float gamma = tonemapper->get_gamma();
  float level = tonemapper->get_level();

  json_t *tonemapper_obj = json_object();
  json_object_set(tonemapper_obj, "type", json_string(type.c_str()));
  json_object_set(tonemapper_obj, "gamma", json_real(gamma));
  json_object_set(tonemapper_obj, "level", json_real(level));

  // write tonemapper - specific
  if (type == "REINHARD") {
    TMReinhard *rtm = static_cast<TMReinhard *>(tonemapper);
    json_object_set(tonemapper_obj, "key", json_real(rtm->get_key()));
    json_object_set(tonemapper_obj, "wht", json_real(rtm->get_wht()));
  }
  json_object_set(root, "tonemap", tonemapper_obj);

  // write animation
  json_t *animation_obj = json_object();
  if (animation) {

    // timelines
    json_t *timeline_arr = json_array();
    set<Timeline *> timelines = animation->get_timelines();
    for (set<Timeline *>::iterator it = timelines.begin();
         it != timelines.end(); ++it) {

      Timeline *timeline = *it;

      json_t *timeline_obj = json_object();

      json_t *layer = json_string(timeline->get_layer()->get_name().c_str());

      json_t *frame_arr = json_array();
      vector<Keyframe> frames = timeline->get_keyframes();
      for (vector<Keyframe>::iterator f = frames.begin(); f != frames.end();
           ++f) {

        Keyframe key = *f;

        json_t *key_obj = json_object();
        json_t *time = json_real(key.get_time());
        json_t *modulator = json_array();
        json_array_append(modulator, json_real(key.get_modulator().r));
        json_array_append(modulator, json_real(key.get_modulator().g));
        json_array_append(modulator, json_real(key.get_modulator().b));

        json_object_set(key_obj, "time", time);
        json_object_set(key_obj, "modulator", modulator);

        json_array_append(frame_arr, key_obj);
      }

      json_object_set(timeline_obj, "layer", layer);
      json_object_set(timeline_obj, "keyframes", frame_arr);
    }

    json_object_set(animation_obj, "len", json_real(animation->get_len()));
    json_object_set(animation_obj, "fps", json_real(animation->get_fps()));
    json_object_set(animation_obj, "timelines", timeline_arr);

    // TODO: maybe there should be some freeing here?
  }

  // dump to file
  string file = ((string)file_name) + ".lmw";
  size_t flag = JSON_PRESERVE_ORDER | JSON_INDENT(4) | JSON_REAL_PRECISION(16);
  return json_dump_file(root, file.c_str(), flag);

  // free memory
  json_decref(layer_arr);
  json_decref(tonemapper_obj);
  json_decref(animation_obj);
  json_decref(root);

  return 0;
}

int Workspace::save_exr(const char *file_name) {

  // get layers
  set<Layer *> layers = compositor->get_layers();

  // create header
  Header header(w, h);

  // composition as base layer for compatability in other application
  header.channels().insert("R", Channel(PixelType::FLOAT));
  header.channels().insert("G", Channel(PixelType::FLOAT));
  header.channels().insert("B", Channel(PixelType::FLOAT));

  // layer
  for (set<Layer *>::iterator it = layers.begin(); it != layers.end(); ++it) {

    Layer *layer = *it;
    string name = layer->get_name();

    header.channels().insert((name + ".R").c_str(), Channel(PixelType::FLOAT));
    header.channels().insert((name + ".G").c_str(), Channel(PixelType::FLOAT));
    header.channels().insert((name + ".B").c_str(), Channel(PixelType::FLOAT));
  }

  // create file
  string name = file_name;
  OutputFile file((name + ".exr").c_str(), header);
  FrameBuffer frame_buffer;

  // composition
  frame_buffer.insert("R",                                   // channel
                      Slice(PixelType::FLOAT,                           // type
                            (char *)&hdr_output_buffer[0].r, // base
                            sizeof(Pixel3) * 1,              // xstride
                            sizeof(Pixel3) * w));            // ystride
  frame_buffer.insert("G",                                   // channel
                      Slice(PixelType::FLOAT,                           // type
                            (char *)&hdr_output_buffer[0].g, // base
                            sizeof(Pixel3) * 1,              // xstride
                            sizeof(Pixel3) * w));            // ystride
  frame_buffer.insert("B",                                   // channel
                      Slice(PixelType::FLOAT,                           // type
                            (char *)&hdr_output_buffer[0].b, // base
                            sizeof(Pixel3) * 1,              // xstride
                            sizeof(Pixel3) * w));            // ystride

  // layers
  for (set<Layer *>::iterator it = layers.begin(); it != layers.end(); ++it) {

    Layer *layer = *it;
    string name = layer->get_name();
    Pixel3 *pixels = layer->get_pixels();

    frame_buffer.insert((name + ".R").c_str(),      // channel
                        Slice(PixelType::FLOAT,                // type
                              (char *)&pixels[0].r, // base
                              sizeof(Pixel3) * 1,   // xstride
                              sizeof(Pixel3) * w)); // ystride
    frame_buffer.insert((name + ".G").c_str(),      // channel
                        Slice(PixelType::FLOAT,                // type
                              (char *)&pixels[0].g, // base
                              sizeof(Pixel3) * 1,   // xstride
                              sizeof(Pixel3) * w)); // ystride
    frame_buffer.insert((name + ".B").c_str(),      // channel
                        Slice(PixelType::FLOAT,                // type
                              (char *)&pixels[0].b, // base
                              sizeof(Pixel3) * 1,   // xstride
                              sizeof(Pixel3) * w)); // ystride
  }

  // write to file
  lmout(" - Writing exr file");
  file.setFrameBuffer(frame_buffer);
  file.writePixels(h);

  return 0;
}

int Workspace::save_png(const char *file_name) {

  // this should be using bitmap_output_buffer
  return 0;
}

int Workspace::save_mpg(const char *file_name) {

  //    AVCodec *codec;
  //    codec = avcodec_find_encoder(CODEC_ID_MPEG1VIDEO);
  //    if (!codec) {
  //        lmerr("Cannot find codec");
  //        return -1;
  //    }
  //
  //    AVCodecContext *c = avcodec_alloc_context3(codec);
  //    AVFrame *frame = avcodec_alloc_frame();
  //
  //    c->bit_rate = 400000;
  //    c->width  = w;
  //    c->height = h;
  //    c->time_base = (AVRational){1, animation->get_fps()};
  //    c->gop_size  = 10;
  //    c->max_b_frames = 1;
  //    c->pix_fmt = PIX_FMT_YUV420P;
  //
  //    if(avcodec_open2(c, codec, NULL) < 0) {
  //        lmerr("Cannot open codec");
  //        return -1;
  //    }
  //
  //    FILE *f = fopen(file_name, "wb");
  //    if(!f) {
  //        lmerr("Cannot open file");
  //        return -1;
  //    }
  //
  //    size_t outbuf_size = 100000;
  //    uint8_t *outbuf = (uint8_t*) malloc(outbuf_size);
  //
  //    size_t frame_size = w * h;
  //    uint8_t *frame_buf = (uint8_t*) malloc(3 * frame_size / 2); /* YUV 420
  //    */
  //
  //    frame->data[0] = frame_buf;
  //    frame->data[1] = frame->data[0] + frame_size;
  //    frame->data[2] = frame->data[1] + frame_size / 4;
  //    frame->linesize[0] = c->width;
  //    frame->linesize[1] = c->width / 2;
  //    frame->linesize[2] = c->width / 2;
  //
  //    size_t i; size_t out_size;
  //    size_t num_frames = animation->get_fps() * animation->get_len();
  //    float step_time = 1.0 / animation->get_fps(); animation->start();
  //    for (i = 0; i < num_frames; ++i) {
  //
  //        fflush(stdout);
  //        for(size_t y = 0; y < c->height; ++y) {
  //            for(size_t x = 0; x < c->width; ++x) {
  //
  //                //rgb 2 yuv
  //                uint8_t in_r = bmp_output_buffer[x + w * y + 0];
  //                uint8_t in_g = bmp_output_buffer[x + w * y + 1];
  //                uint8_t in_b = bmp_output_buffer[x + w * y + 2];
  //                uint8_t out_y = 0.299 * in_r + 0.587 * in_g + 0.114 * in_b;
  //                uint8_t out_u = 0.492 * (in_b - out_y);
  //                uint8_t out_v = 0.877 * (in_r - out_y);
  //
  //                frame->data[0][y * frame->linesize[0] + x] = out_y;
  //                frame->data[0][y * frame->linesize[1] + x] = out_u;
  //                frame->data[0][y * frame->linesize[2] + x] = out_v;
  //            }
  //        }
  //
  //        out_size = avcodec_encode_video(c, outbuf, outbuf_size, frame);
  //        fwrite(outbuf, 1, out_size, f);
  //
  //        animation->step(step_time);
  //    }
  //
  //    for(; out_size; ++i) {
  //
  //        fflush(stdout);
  //        out_size = avcodec_encode_video(c, outbuf, outbuf_size, NULL);
  //        fwrite(outbuf, 1, out_size, f);
  //    }
  //
  //    outbuf[0] = 0x00;
  //    outbuf[1] = 0x00;
  //    outbuf[2] = 0x01;
  //    outbuf[3] = 0xb7;
  //
  //    fwrite(outbuf, 1, 4, f);
  //    fclose(f);
  //
  //    avcodec_close(c);
  //    av_free(c);
  //    av_free(frame);

  return 0;
}

}; // namespace LightmanCore
