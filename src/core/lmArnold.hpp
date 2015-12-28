/*
 *  lmArnold.hpp
 *  Lightman
 *
 *  Lightman supports image generation using Solid Angel's Arnold renderer
 *  This file defines some data structures needed to use the buffer driver
 *  plugin so that Arnold directly passes the rendering results to Lightman
 *
 */

namespace LightmanCore {

struct BucketPosInfo {
    int bucket_xo;
    int bucket_yo;
    int bucket_size_x;
    int bucket_size_y;
    BucketPosInfo() :
        bucket_xo(-1), bucket_yo(-1), bucket_size_x(-1), bucket_size_y(-1) { }
};

struct ProgressInfo {
    int bucket_sum;
    int bucket_cur;
    ProgressInfo() : bucket_sum(-1), bucket_cur(-1) { }
};

}; // namespace LightmanCore
