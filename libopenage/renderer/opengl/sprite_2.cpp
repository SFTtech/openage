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
       //std::string path = "/assets/converted/graphics/"+ std::to_string(id) +".slp.png";
       //this->texture_data = new resources::TextureData(root / path,use_metafile);
       this->is_tex = true;
       this->tex_id = id;
   }

   void Sprite_2::set_subtex(int tex){
        
       this->subtex = tex;
   }

   void Sprite_2::set_terrain(int id){
       this->tex_id = id;
       this->is_tex = true;
       this->meta = true;
       is_terrain = true;
   }

    

}
}
}