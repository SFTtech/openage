#include <eigen3/Eigen/Dense>
#include "../resources/texture_data.h"
#include "../../util/path.h"

namespace openage{
namespace renderer{
namespace opengl{

class Sprite_2{
    public:
        //Sprite_2(float x, float y,float w, float h, float r,float g, float b, float a,float left,float right,float top,float bottom);
        Sprite_2(util::Path &path,float x, float y,float w, float h, float r,float g, float b, float a);
        void set_texture(int id,bool use_metafile);
        void set_subtex(int tex);
        float x,y,w,h,r,g,b,a;
        float left = 0.0;
        float bottom = 1.0;
        float top = 0.0;
        float right = 1.0;  
        bool is_tex = false;
        int tex_id;
        int active_id = -1;
        bool meta;
        int subtex = 0;
        resources::TextureData* texture_data;

    private:
        util::Path root;

        

};

}
}
}