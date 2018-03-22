#pragma once
#include "../renderer.h"
#include <memory>
#include <eigen3/Eigen/Dense>
#include "../texture.h"
#include "../geometry.h"
#include "shader_program.h"

class Sprite{
    private:
    std::shared_ptr<Texture> sprite_texture;
    resources::TextureData texture_data;
    std::shared_ptr<ShaderProgram> shader;
    std::shared_ptr<Renderer> renderer;
    float aspect;
    float Y_screen;
    
    public:
    int load_texture();
}