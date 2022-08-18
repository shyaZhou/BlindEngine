//
// Created by ByteDance on 2022/8/10.
//
#include <texture2d.h>
#include <stb/stb_image.h>
#include <stopwatch.h>
#include <iostream>
#include <fstream>

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

    int img_data_format = GL_RGB;
    if(data != nullptr) {
        // get color channels
        switch (channels_in_file) {
            case 1: {
                img_data_format = GL_ALPHA;
                texture2d->_gl_texture_format = GL_COMPRESSED_ALPHA;
                break;
            }
            case 3: {
                img_data_format = GL_RGB;
                texture2d->_gl_texture_format = GL_COMPRESSED_RGB;
                break;
            }
            case 4: {
                img_data_format = GL_RGBA;
                texture2d->_gl_texture_format = GL_COMPRESSED_RGBA;
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
    glTexImage2D(GL_TEXTURE_2D, 0, texture2d->_gl_texture_format, texture2d->_width, texture2d->_height, 0, img_data_format, GL_UNSIGNED_BYTE, data);
    stopWatch.stop();
    std::int64_t upload_and_compress_cost = stopWatch.milliseconds();

    // set texture setting
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // release texture memory
    stbi_image_free(data);

    return texture2d;
}

void Texture2D::CompressImageFile(std::string &img_file_path, std::string &save_img_file_path) {
    Texture2D *texture2d = LoadFromFile(img_file_path);

    //1. compress success?
    GLint compress_success = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compress_success);

    //2. get compressed img size
    GLint  compress_size = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compress_size);

    //3. get concrete img compress format
    GLint compress_format = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &compress_format);

    //3. download compressed img data from GPU to memory
    void *img = malloc(compress_size);
    glGetCompressedTexImage(GL_TEXTURE_2D, 0, img);

    //4. save file
    std::ofstream output_file_stream(save_img_file_path, std::ios::out | std::ios::binary);

    CptFileHead cpt_file_head;
    cpt_file_head._type[0]='c';
    cpt_file_head._type[0]='p';
    cpt_file_head._type[0]='t';
    cpt_file_head._mipmap_level = texture2d->_mipmap_level;
    cpt_file_head._width = texture2d->_width;
    cpt_file_head._height = texture2d->_height;
    cpt_file_head._gl_texture_format = compress_format;
    cpt_file_head._compress_size = compress_size;

    output_file_stream.write((char *)&cpt_file_head, sizeof(CptFileHead));
    output_file_stream.write((char *)img, compress_size);
    output_file_stream.close();
}
