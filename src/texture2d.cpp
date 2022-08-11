//
// Created by ByteDance on 2022/8/10.
//
#include <texture2d.h>
#include <stb/stb_image.h>
#include <stopwatch.h>

Texture2D *Texture2D::LoadFromFile(std::string &img_file_path) {
    Texture2D *texture2d = new Texture2D();

    stbi_set_flip_vertically_on_load(true); // flip texture because OpenGL texture start with left-bottom
    int channels_in_file; // img channels

    StopWatch stopWatch;
    stopWatch.start();
    unsigned char *data = stbi_load(img_file_path.c_str(), &(texture2d->_width), &(texture2d->_height), &channels_in_file, 0);
    assert(data != nullptr);
    stopWatch.stop();
    std::int64_t decompress_jpg_cost = stopWatch.milliseconds();

    if(data != nullptr) {
        // get color channels
        switch (channels_in_file) {
            case 1: {
                texture2d->_gl_texture_format = GL_ALPHA;
                break;
            }
            case 3: {
                texture2d->_gl_texture_format = GL_RGB;
                break;
            }
            case 4: {
                texture2d->_gl_texture_format = GL_RGBA;
                break;
            }
        }
    }

    // note GPU to create texture object
    glGenTextures(1, &(texture2d->_gl_texture_id));

    // bind texture with target GL_TEXTURE_2D
    glBindTexture(GL_TEXTURE_2D, texture2d->_gl_texture_id);

    stopWatch.restart();

    // upload texture data to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, texture2d->_width, texture2d->_height, 0, texture2d->_gl_texture_format, GL_UNSIGNED_BYTE, data);
    stopWatch.stop();
    std::int64_t upload_and_compress_cost = stopWatch.milliseconds();

    // set texture setting
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // release texture memory
    stbi_image_free(data);

    return texture2d;
}