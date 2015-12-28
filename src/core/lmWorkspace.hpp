#ifndef LIGHTMAN_WORKSPACE_H
#define LIGHTMAN_WORKSPACE_H

#include "lmCompositor.hpp"
#include "lmToneMapper.hpp"
#include "lmAnimation.hpp"

namespace LightmanCore {

using namespace std;

/**
 * A workspace contains all of Lightman's core components and provides
 * a complete image generation pipepline. This consists of a sequence
 * of HDR layers managed and composited by the compositor, a tone mapper
 * that converts illuminance-based HDR layers to color space images,
 * and an optional animation for the scene. Working inside a workspace
 * is easier for simple tasks. For more complicated applications, it is
 * possible to define different pipelines on top of the core components.
 */
class Workspace {
public:

    /**
     * Constructor.
     * Creates an empty workspace under the given title.
     */
    Workspace(const char* title);

    /**
     * Destructor.
     * Destroys the workspace. However, since it is possible to share assets
     * across multiple workspaces, workspace assets are not freed. However,
     * if there is no shared asset, and freeing is desired, call the close
     * function before deleting the workspace.
     */
    ~Workspace();

    /**
     * Load a workspace from file.
     * Lightman supports the following inputs:
     *
     *  .lmw: Lightman workspace (json) \n
     *  .exr: OpenExr Images (scanline only) \n
     *  .ass: Arnold Scene Source \n
     *
     * @param path path to input file.
     * @return 0 if successful
     *        -1 if failed
     */
    int load(const char* path);

    /**
     * Save the workspace to file.
     * Note that there are times when there will be multiple output files.
     * As such, workspaces will be saved into a folder under the title.
     * @param path path to save the workspace to
     * @return 0 if successful
     *        -1 if failed
     */
    int save(const char* path);

    /**
     * Close the workspace.
     * Note that there are times when there will be multiple output files.
     * As such, workspaces will be saved into a folder under the title.
     * @param path path to save the workspace to
     */
    void close();

    /**
     * Get the title of the workspace.
     * @return the title of the workspace.
     */
    string get_title();

    /**
     * set the title of the workspace.
     * @param title the title to set.
     */
    void set_title(const char* title);

    /**
     * Get the width of the workspace.
     * @return the width of the workspace (0 when workspace is empty)
     */
    size_t get_width();

    /**
     * Get the height of the workspace.
     * @return the height of the workspace (0 when workspace is empty)
     */
    size_t get_height();

    /**
     * Get a poniter to the compositor of the workspace.
     * @return pointer to the compositor used in the workspace
     */
    Compositor* get_compositor();

    /**
     * Get a pointer to the tonemapper of the workspace.
     * @return pointer to the tone mapper used in the workspace
     */
    ToneMapper* get_tonemapper();

    /**
     * Get a pointer to the animation of the workspace.
     * @return pointer to the animation used in the workspace
     */
    Animation* get_animation();

    /**
     * Set a new compositor for the workspace.
     * Note that this allows sharing the compositor (hence layers),
     * which is useful when working with multiple workspaces that
     * shares layers (for example, same layers but different tone mappers
     * and animations). However, as the compositor is really the core of
     * a workspace, instead of changing the compositor, creating a new
     * workspace might be a better option for cases where the composition
     * is not exactly the same.
     */
    void set_compositor(Compositor* compositor);

    /**
     * Set a new tonemapper for the workspace.
     * Note that this allows sharing the same tone mapper
     * across multiple workspaces for sake of consistency, etc.
     * @param tonemapper pointer to the new tone mapper to use
     */
    void set_tonemapper(ToneMapper* tonemapper);

    /**
     * Set a new animation for the workspace.
     * Note that this can be used to devide complicated animations
     * into smaller pieces and preview / render them piece-by-piece.
     * @param animation pointer to the new animation to use
     */
    void set_animation(Animation* animation);

    /**
     * Set an output buffer for bitmap outputs.
     * This will set the address to write the bitmap output to.
     * The given buffer should be managed by he application.
     * When this is set, calls to dump_bmp_to_buffer will write a 8-bit RGB
     * bitmap image to the target buffer using Lightman's default pipeline.
     * @param buffer application managed buffer to write the result to
     */
    void set_bmp_output_buffer(char* buffer);

    /**
     * Write bitmap output into application space buffer.
     * Application need to define a application space target buffer
     * using set_bitmap_buffer, or calls to dump_bmp_to_buffer will be ignored.
     */
    void dump_bmp_to_buffer();

    /**
     * Set an output buffer for HDR outputs.
     * This will set the address to write the HDR output to.
     * The given buffer should be managed by he application.
     * When this is set, calls to dump_bitmap will write a single percision
     * floating point RGB (see Pixel class for details) image to the target
     * buffer using Lightman's default pipeline.
     * @param buffer application managed buffer to write the result to.
     */
    void set_hdr_output_buffer(Pixel3* buffer);

    /**
     * Write HDR output into application space buffer.
     * Application need to define a application space target buffer
     * using set_hdr_output_buffer, or calls to dump_hdr_to_buffer will
     * be ignored.
     */
    void dump_hdr_to_buffer();

    /**
     * Save result to exr file (HDR, no tonemapping).
     * @param path path where the file should be saved.
     */
    int dump_exr(const char* path);

    /**
     * Save result to png file (tonemapped 8-bit RGB bitmap).
     * @param path path where the file should be saved.
     */
    int dump_png(const char* path);

    /**
     * Save result to a mpeg video (requires animation).
     * @param path path where the file should be saved.
     */
    int dump_mpg(const char* path);

private:

    /**
     * Title of workspace.
     */
    string title;

    /**
     * Width of workspace.
     */
    size_t w;

    /**
     * Height of workspace.
     */
    size_t h;

    /**
     * Compositor.
     */
    Compositor* compositor;

    /**
     * Pointer to the tone mapper used in the workspace.
     */
    ToneMapper* tonemapper;

    /**
     * Pointer to the animation used in the workspace (optional).
     */
    Animation* animation;

    /**
     * Compositor output. (internal)
     */
    Pixel3 *compositor_output;

    /**
     * Buffer to write illuminance pixel output of the compositor.
     */
    Pixel3* hdr_output_buffer;

    /**
     * Buffer to write 8 bit RGB color bitmap output of the the workspace.
     */
    char* bmp_output_buffer;

    /* internal format specif loading routines */
    int load_lmw(const char* file_path);
    int load_exr(const char* file_path);
    int load_ass(const char* file_path);
    int save_lmw(const char* file_name);
    int save_exr(const char* file_name);
    int save_png(const char* file_name);
    int save_mpg(const char* file_name);

};

}; // namespace LightmanCore

#endif // LIGHTMAN_WORKSPACE_H
