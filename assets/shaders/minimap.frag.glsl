#version 120

uniform ivec2 minimap_orig;
uniform ivec2 minimap_size;

varying vec4 out_vertex_position;
varying vec3 out_color;
 
void main(void) {
  ivec2 center = ivec2(minimap_size.x/2, minimap_size.y/2);
  ivec2 vpos = minimap_orig + center - ivec2(out_vertex_position.x, out_vertex_position.y);
  vec3 color;

  if (!(abs(vpos.x) * center.y + abs(vpos.y) * center.x <= center.x * center.y))
    discard;
  else
    color = out_color;

  gl_FragColor = vec4(color, 1.0);
}
