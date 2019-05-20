/* 
File Name: "fshader53.glsl":
           Fragment Shader
*/

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
                 //      due to different settings of the default GLSL version

in vec4 color;
flat in int fogMode;
in float fog_d;

out vec4 fColor;

void main() 
{ 
    fColor = color;
    vec4 tex_color = color;

	vec4 fogColor = vec4(0.7, 0.7, 0.7, 0.5); //fog color

	if (fogMode == 0){
		fColor =tex_color;
	}
	else if(fogMode == 1){
		float temp = clamp(fog_d, 0.0, 1.0);
		fColor=mix(tex_color,fogColor,temp);
	}
	else if (fogMode == 2){
		float temp=1/exp(0.09*fog_d);
		fColor=mix(tex_color,fogColor,1-temp);
	}
	else if (fogMode == 3){
		float temp =1/exp(0.09*0.09*fog_d*fog_d);
		fColor=mix(tex_color,fogColor,1-temp);
	}


	
	else{
	fColor = vec4(0, 0, 0, 1);
	}
	
	
} 

