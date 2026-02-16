#version 450

//layout( location = 1 ) in vec4 vert_position;

//layout( set = 0, binding = 1 ) uniform sampler2D ImageSampler;

layout( location = 0 ) out vec4 frag_color;

void main() {
  
  //float normal = texture( ImageSampler, vec2(vert_position.x, vert_position.y) + vec2(vert_position.z / 32.0f, vert_position.w / 32.0f) ).r;
  
  //if(normal == 0.0f) discard;
  
  frag_color = vec4(1.0f, 0.0f,0.0f, 1.0f);
}