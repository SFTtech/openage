#version 330 core
out vec4 FragColor;

in vec4 ourColor;
in vec2 pos;
in vec2 uv;
in float a_id;
uniform vec2 mouse_pos = vec2(0.0,0.0);

uniform sampler2D texture_0;
uniform sampler2D texture_1;
uniform sampler2D texture_2;
uniform sampler2D texture_3;
uniform sampler2D texture_4;
uniform sampler2D texture_5;
uniform sampler2D texture_6;
uniform sampler2D texture_7;
uniform sampler2D texture_8;
uniform sampler2D texture_9;
uniform sampler2D texture_10;
uniform sampler2D texture_11;
uniform sampler2D texture_12;
uniform sampler2D texture_13;
uniform sampler2D texture_14;
uniform sampler2D texture_15;
uniform sampler2D texture_16;
uniform sampler2D texture_17;
uniform sampler2D texture_18;
uniform sampler2D texture_19;
uniform sampler2D texture_20;
uniform sampler2D texture_21;
uniform sampler2D texture_22;
uniform sampler2D texture_23;
uniform sampler2D texture_24;
uniform sampler2D texture_25;
uniform sampler2D texture_26;
uniform sampler2D texture_27;
uniform sampler2D texture_28;
uniform sampler2D texture_29;
uniform sampler2D texture_30;
uniform sampler2D texture_31;

void switcher(in int index,out vec4 color){

switch(index){
case 0:
        color = texture(texture_0, uv);
        break;
case 1:
        color = texture(texture_1, uv);
        break;
case 2:
        color = texture(texture_2, uv);
        break;
case 3:
        color = texture(texture_3, uv);
        break;
case 4:
        color = texture(texture_4, uv);
        break;
case 5:
        color = texture(texture_5, uv);
        break;
case 6:
        color = texture(texture_6, uv);
        break;
case 7:
        color = texture(texture_7, uv);
        break;
case 8:
        color = texture(texture_8, uv);
        break;
case 9:
        color = texture(texture_9, uv);
        break;
case 10:
        color = texture(texture_10, uv);
        break;
case 11:
        color = texture(texture_11, uv);
        break;
case 12:
        color = texture(texture_12, uv);
        break;
case 13:
        color = texture(texture_13, uv);
        break;
case 14:
        color = texture(texture_14, uv);
        break;
case 15:
        color = texture(texture_15, uv);
        break;
case 16:
        color = texture(texture_16, uv);
        break;
case 17:
        color = texture(texture_17, uv);
        break;
case 18:
        color = texture(texture_18, uv);
        break;
case 19:
        color = texture(texture_19, uv);
        break;
case 20:
        color = texture(texture_20, uv);
        break;
case 21:
        color = texture(texture_21, uv);
        break;
case 22:
        color = texture(texture_22, uv);
        break;
case 23:
        color = texture(texture_23, uv);
        break;
case 24:
        color = texture(texture_24, uv);
        break;
case 25:
        color = texture(texture_25, uv);
        break;
case 26:
        color = texture(texture_26, uv);
        break;
case 27:
        color = texture(texture_27, uv);
        break;
case 28:
        color = texture(texture_28, uv);
        break;

case 29:
        color = texture(texture_29, uv);
        break;
case 30:
        color = texture(texture_30, uv);
        break;

case 31:
        color = texture(texture_31, uv);
        break;
}
}
void main()
{
	float intensity = 100.0/(length(pos - mouse_pos));
	FragColor = ourColor;//*intensity;	
	if(a_id != -1.0){
	int index = int(a_id + 0.5);
	vec4 tex_color;
 	switcher(index,tex_color);	
	FragColor = tex_color;
	}
	
}
