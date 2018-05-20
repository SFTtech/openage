// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "texturearray.h"

#include <epoxy/gl.h>

#include <tuple>

#include "../../error/error.h"
#include "../../datastructure/constexpr_map.h"
#include "../../log/log.h"

#include "../resources/texture_data.h"
#include "render_target.h"


namespace openage {
namespace renderer {
namespace opengl {

/// The input and output formats for GL.
static constexpr auto gl_format = datastructure::create_const_map<resources::pixel_format, std::tuple<GLint, GLenum, GLenum>>(
	// TODO check correctness of formats here
	std::make_pair(resources::pixel_format::r16ui, std::make_tuple(GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_INT)),
	std::make_pair(resources::pixel_format::r32ui, std::make_tuple(GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT)),
	std::make_pair(resources::pixel_format::rgb8, std::make_tuple(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE)),
	std::make_pair(resources::pixel_format::bgr8, std::make_tuple(GL_RGB8, GL_BGR, GL_UNSIGNED_BYTE)),
	std::make_pair(resources::pixel_format::rgba8, std::make_tuple(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE)),
	std::make_pair(resources::pixel_format::rgba8ui, std::make_tuple(GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE)),
	std::make_pair(resources::pixel_format::depth24, std::make_tuple(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE))
);

GlTextureArray::GlTextureArray(const resources::TextureData& data)
	: GlSimpleObject([] (GLuint handle) { glDeleteTextures(1, &handle); } )
{
	// set info
    this->info = data.get_info();
    // generate opengl texture handle
	glGenTextures(1, &*this->handle);
	glBindTexture(GL_TEXTURE_2D_ARRAY, *this->handle);
    // select pixel format
	auto fmt_in_out = gl_format.get(this->info.get_format());
    //log::log(INFO << "format  : "<<(int)this->info.get_format());
    log::log(INFO << "format  : "<< (int)this->info.get_format() << " " << std::get<0>(fmt_in_out) << " " << std::get<1>(fmt_in_out) << " " << std::get<2>(fmt_in_out));
    
    //log::log(INFO << std::get<0>(fmt_in_out) << " " << std::get<1>(fmt_in_out) << " " << std::get<2>(fmt_in_out) << " " );
    // store raw pixels to gpu
	auto size = this->info.get_size();
    log::log(INFO << size.first << "hey " << size.second);
    uint len_z = 40;

    glTexImage3D(GL_TEXTURE_2D_ARRAY,
             0,                 // mipmap level
             std::get<0>(fmt_in_out),          // gpu texel format
             size.first,             // width
             size.second,             // height
             len_z,             // depth
             0,                 // border
             std::get<1>(fmt_in_out),      // cpu pixel format
             std::get<2>(fmt_in_out),  // cpu pixel coord type
             0);   

    log::log(INFO<<"what up 100");
	for(uint i = 0; i < len_z;++i)
    {
    //Choose a random color for the i-essim image
    GLubyte color[4] = {rand()%255,rand()%255,rand()%255,1};
    log::log(INFO<<"what up "<<i);
    //Specify i-essim image
    glTexSubImage3D( GL_TEXTURE_2D_ARRAY,
                     0,                     //Mipmap number
                     0,0,i,                 //xoffset, yoffset, zoffset
                     size.first,size.second,1,                 //width, height, depth
                     std::get<1>(fmt_in_out),                //format
                     std::get<2>(fmt_in_out),      //type
                     data.get_data());                //pointer to data
    }
    log::log(INFO<<"what up 100");

	//glPixelStorei(GL_UNPACK_ALIGNMENT, this->info.get_row_alignment());

	// drawing settings
	// TODO these are outdated, use sampler settings
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	log::log(MSG(dbg) << "Created OpenGL texture array for the terrain");
}


GlTextureArray::GlTextureArray(int layers,int height, int width,resources::pixel_format format)
: GlSimpleObject([] (GLuint handle) { glDeleteTextures(1, &handle); } ),
  depth(layers),height(height),width(width),format(format)  
{
	//auto fmt_in_out = gl_format.get(this->info.get_format());
    auto fmt_in_out = gl_format.get(format);
    glGenTextures(1, &*this->handle);
	glBindTexture(GL_TEXTURE_2D_ARRAY, *this->handle);
    int len_z = layers;
    glTexImage3D(GL_TEXTURE_2D_ARRAY,
             0,                 // mipmap level
             std::get<0>(fmt_in_out),          // gpu texel format
             height,             // width
             width,             // height
             len_z,             // depth
             0,                 // border
             std::get<1>(fmt_in_out),      // cpu pixel format
             std::get<2>(fmt_in_out),  // cpu pixel coord type
             0);
    //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

int GlTextureArray::submit_texture(resources::TextureData& data){
    this->bind();
	auto fmt_in_out = gl_format.get(this->format);
    glTexSubImage3D( GL_TEXTURE_2D_ARRAY,
                     0,                     //Mipmap number
                     0,0,current_layers,                 //xoffset, yoffset, zoffset
                     width,height,1,                 //width, height, depth
                     std::get<1>(fmt_in_out),                //format
                     std::get<2>(fmt_in_out),      //type
                     data.get_data());                //pointer to data
    
    log::log(INFO<<"what up -----" << current_layers);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    current_layers++;
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    return current_layers;
}

void GlTextureArray::bind(){
	glBindTexture(GL_TEXTURE_2D_ARRAY, *this->handle);
}

}}} // openage::renderer::opengl
