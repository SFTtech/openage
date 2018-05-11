#include "../../log/log.h"
#include "../../error/error.h"
#include "sprite_2.h"

namespace openage{
namespace renderer{
namespace opengl{

   Sprite_2::Sprite_2(util::Path& path,float x, float y,float w, float h, float r,float g, float b, float a) 
   :x(x),y(y),w(w),h(h),r(r),g(g),b(b),a(a)
   {
        root = path;       
   }

   /*Sprite_2::Sprite_2(float x, float y,float w, float h, float r,float g, float b, float a,float left,float right,float top,float bottom) 
   :x(x),y(y),w(w),h(h),r(r),g(g),b(b),a(a),left(left),right(right),top(top),bottom(bottom)
   {
         
   }*/

   void Sprite_2::set_texture(int id,bool use_metafile){
       this->meta = use_metafile;
       std::string path = "/assets/converted/graphics/"+ std::to_string(id) +".slp.png";
       this->texture_data = new resources::TextureData(root / path,use_metafile);
       this->is_tex = true;
       this->tex_id = id;
   }

   void Sprite_2::set_subtex(int subtex){
        
        std::tuple<float, float, float, float> sprite_coord = texture_data->get_info().get_subtexture_coordinates(subtex);
        left = std::get<0>(sprite_coord);
  	    right = std::get<1>(sprite_coord);
  	    bottom = std::get<2>(sprite_coord);
  	    top = std::get<3>(sprite_coord);
   }

    

}
}
}