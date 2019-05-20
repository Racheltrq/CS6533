/************************************************************
 * Handout: rotate-cube-new.cpp (A Sample Code for Shader-Based OpenGL ---
								 for OpenGL version 3.1 and later)
 * Originally from Ed Angel's textbook "Interactive Computer Graphics" 6th Ed
			  sample code "example3.cpp" of Chapter 4.
 * Moodified by Yi-Jen Chiang to include the use of a general rotation function
   Rotate(angle, x, y, z), where the vector (x, y, z) can have length != 1.0,
   and also to include the use of the function NormalMatrix(mv) to return the
   normal matrix (mat3) of a given model-view matrix mv (mat4).

   (The functions Rotate() and NormalMatrix() are added to the file "mat-yjc-new.h"
   by Yi-Jen Chiang, where a new and correct transpose function "transpose1()" and
   other related functions such as inverse(m) for the inverse of 3x3 matrix m are
   also added; see the file "mat-yjc-new.h".)

 * Extensively modified by Yi-Jen Chiang for the program structure and user
   interactions. See the function keyboard() for the keyboard actions.
   Also extensively re-structured by Yi-Jen Chiang to create and use the new
   function drawObj() so that it is easier to draw multiple objects. Now a floor
   and a rotating cube are drawn.

** Perspective view of a color cube using LookAt() and Perspective()

** Colors are assigned to each vertex and then the rasterizer interpolates
   those colors across the triangles.
**************************************************************/
#include "Angel-yjc.h"
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#define sqrt3f(x,y,z) sqrt(x*x+y*y+z*z)
#define ImageWidth  64
#define ImageHeight 64
#define	stripeImageWidth 32

using namespace std;

typedef Angel::vec3  color3;
typedef Angel::vec3  point3;
typedef Angel::vec4  point4;
typedef Angel::vec4  color4;


GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);

GLuint program;       /* shader program object id */
GLuint program_lighting;
GLuint program_ex;
GLuint sphere_buffer;   /* vertex buffer object id for cube */
GLuint floor_buffer;  /* vertex buffer object id for floor */
GLuint axis_buffer;
GLuint cube_buffer;
GLuint shadow_buffer;
GLuint flat_shading_buffer;
GLuint smooth_shading_buffer;
GLuint texture_buffer;
GLuint firework_buffer;

GLubyte Image[ImageHeight][ImageWidth][4];
GLubyte stripeImage[4 * stripeImageWidth];
static GLuint textureName;
static GLuint line_textureName;
point3 *shadow_data;
color3 shadow_color[4500];
point4 *shading_vertices;
point3 *flat_shading_vertices;
point3 *smooth_shading_vertices;

GLuint  model_view;  // model-view matrix uniform shader variable location
GLuint  projection;  // projection matrix uniform shader variable location
GLuint  model_view_lighting;
GLuint  projection_lighting;
GLuint  model_view_ex;
GLuint  projection_ex;

// Projection transformation parameters
GLfloat  fovy = 45.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 0.5, zFar = 14.0;

GLfloat angle = 0.0; // rotation angle in degrees
vec4 init_eye(7.0, 3.0, -10.0, 1.0); // initial viewer position
vec4 eye = init_eye;               // current viewer position

int animationFlag = 0; // 1: animation; 0: non-animation. Toggled by key 'a' or 'A'

int cubeFlag = 1;   // 1: solid cube; 0: wireframe cube. Toggled by key 'c' or 'C'
int floorFlag = 1;  // 1: solid floor; 0: wireframe floor. Toggled by key 'f' or 'F'

int wireFlag = 1;
int pointFlag = 1;

int shadowFlag = 0;
int lightingFlag = 0;
int flatFlag = 1;
int smoothFlag = 0;
int blendFlag = 0;
int groundTextureFlag = 0;
int fireworkFlag = 0;

int upright_lat_flag = 0;
int text_sphere_flag = 0;
int text_sphere_line_flag = 1;
int text_sphere_check_flag = 0;
int vertical_flag = 1;
int slant_flag = 0;
int eye_space_flag = 0;
int object_space_flag = 1;
int enable_lat = 1;

int fog = 0;
float t_sub = 0.0;

const int cube_NumVertices = 36; //(6 faces)*(2 triangles/face)*(3 vertices/triangle)
#if 0
point3 cube_points[cube_NumVertices]; // positions for all vertices
color3 cube_colors[cube_NumVertices]; // colors for all vertices
#endif
#if 1
point3 cube_points[100];
color3 cube_colors[100];
point3 line_points[9];
color3 line_colors[9];
point3 *spherePoints;
color3 sphere_color[4500];
point3 firework_v[300];
point3 firework_c[300];
int sphere_Num_Triangle;
point3 route[] = { point3(3.0, 1.0, 5.0), point3(-2.0, 1.0, -2.5), point3(2.0, 1.0, -4.0), point3(3.0, 1.0, 5.0) };
int currentRoute = 1;
int totalRoute = 3;
float speed = 0.3;
point3 spherePos = route[currentRoute];
point3 *translateVector;
point3 *rotationAxis;
point3 crossProduct(point3 A, point3 B);
mat4 rotationMatrix = Angel::identity();
float radius = 0;
bool pressB = false;
#endif

point4 light_position(-14.0, 12.0, -3.0, 1.0);
vec3 floor_normal[6];
vec4 floor_light_vertices[6];
GLuint floor_light_buffer;

color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0);
float const_att = 1.0;
float linear_att = 0.01;
float quad_att = 0.01;

//point4 light_position(2.0, 2.0, 1.0, 1.0);


color4 material_ambient(1.0, 0.0, 1.0, 1.0);
color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
color4 material_specular(1.0, 0.8, 0.0, 1.0);
float  material_shininess = 125.0;

color4 ambient_product = light_ambient * material_ambient;
color4 diffuse_product = light_diffuse * material_diffuse;
color4 specular_product = light_specular * material_specular;


const int floor_NumVertices = 6; //(1 face)*(2 triangles/face)*(3 vertices/triangle)
point3 floor_points[floor_NumVertices]; // positions for all vertices
color3 floor_colors[floor_NumVertices]; // colors for all vertices

// Vertices of a unit cube centered at origin, sides aligned with axes
point3 vertices[8] = {
	point3(-0.5, -0.5,  0.5),
	point3(-0.5,  0.5,  0.5),
	point3(0.5,  0.5,  0.5),
	point3(0.5, -0.5,  0.5),
	point3(-0.5, -0.5, -0.5),
	point3(-0.5,  0.5, -0.5),
	point3(0.5,  0.5, -0.5),
	point3(0.5, -0.5, -0.5)
};
// RGBA colors
color3 vertex_colors[9] = {
	color3(0.0, 0.0, 0.0),  // black
	color3(1.0, 0.0, 0.0),  // red
	color3(1.0, 1.0, 0.0),  // yellow
	color3(0.0, 1.0, 0.0),  // green
	color3(0.0, 0.0, 1.0),  // blue
	color3(1.0, 0.0, 1.0),  // magenta
	color3(1.0, 1.0, 1.0),  // white
	color3(0.0, 1.0, 1.0),   // cyan
	color3(0.5, 1.0, 1.0)

};
//----------------------------------------------------------------------------



point3 vertices_floor[4] = {
	point3(5, 0,  8),
	point3(5, 0,  -4),
	point3(-5, 0,  -4),
	point3(-5, 0,  8),
};

point4 vertices_floor_f[4] = {
	point4(5, 0,  8, 1),
	point4(5, 0,  -4, 1),
	point4(-5,  0,  -4, 1),
	point4(-5,  0,  8, 1),
};

//-------------------------------
// generate 2 triangles: 6 vertices and 6 colors

void floor()
{
	floor_colors[0] = vertex_colors[3]; floor_points[0] = vertices_floor[0];
	floor_colors[1] = vertex_colors[3]; floor_points[1] = vertices_floor[1];
	floor_colors[2] = vertex_colors[3]; floor_points[2] = vertices_floor[2];

	floor_colors[3] = vertex_colors[3]; floor_points[3] = vertices_floor[2];
	floor_colors[4] = vertex_colors[3]; floor_points[4] = vertices_floor[3];
	floor_colors[5] = vertex_colors[3]; floor_points[5] = vertices_floor[0];
}
//----------------------------------------------------------------------------
// OpenGL initialization


void quad() {
	vec4 u = vertices_floor_f[1] - vertices_floor_f[0];
	vec4 v = vertices_floor_f[2] - vertices_floor_f[0];
	vec3 vec_normal = normalize(cross(u, v));


	floor_normal[0] = vec_normal; floor_light_vertices[0] = vertices_floor_f[0];
	floor_normal[1] = vec_normal; floor_light_vertices[1] = vertices_floor_f[1];
	floor_normal[2] = vec_normal; floor_light_vertices[2] = vertices_floor_f[2];
	floor_normal[3] = vec_normal; floor_light_vertices[3] = vertices_floor_f[2];
	floor_normal[4] = vec_normal; floor_light_vertices[4] = vertices_floor_f[3];
	floor_normal[5] = vec_normal; floor_light_vertices[5] = vertices_floor_f[0];

 }

void axis()
{
	line_colors[0] = vertex_colors[1]; line_points[0] = point3(0, 0, 0);
	line_colors[1] = vertex_colors[1]; line_points[1] = point3(5, 0, 0);
	line_colors[2] = vertex_colors[1]; line_points[2] = point3(10, 0, 0);


	line_colors[3] = vertex_colors[8]; line_points[3] = point3(0, 0, 0);
	line_colors[4] = vertex_colors[8]; line_points[4] = point3(0, 5, 0);
	line_colors[5] = vertex_colors[8]; line_points[5] = point3(0, 10, 0);

	line_colors[6] = vertex_colors[4]; line_points[6] = point3(0, 0, 0);
	line_colors[7] = vertex_colors[4]; line_points[7] = point3(0, 0, 5);
	line_colors[8] = vertex_colors[4]; line_points[8] = point3(0, 0, 10);
}


void sphere()
{
	for (int i = 0; i < sphere_Num_Triangle * 3; i++) {
		sphere_color[i] = color3(1.0, 0.84, 0);
	}
	
	for (int i = 0; i < sphere_Num_Triangle * 3; i++) {
		shadow_color[i] = color3(0.25, 0.25, 0.25);
		
	}
}

point4 texture_vertices[6] = {
	point4(-5,  0,  -4, 1),
	point4(-5,  0,  8, 1),
	point4(5, 0,  8, 1),

	point4(5, 0,  8, 1),
	point4(5, 0,  -4, 1),
	point4(-5,  0,  -4, 1),
};

point3 texture_normals[6] = {
	point3(0,  1,  0),
	point3(0,  1,  0),
	point3(0,  1,  0),
	point3(0,  1,  0),
	point3(0,  1,  0),
	point3(0,  1,  0),
};

vec2 texture_coord[6] = {
	vec2(0.0, 0.0),
	vec2(0.0, 6 / 4.0),
	vec2(5 / 4.0, 6 / 4.0),

	vec2(5 / 4.0, 6 / 4.0),
	vec2(5 / 4.0, 0.0),
	vec2(0.0, 0.0),
};


void SetUp_Lighting_Uniform_Vars(mat4 mv)
{
	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"),
		1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"),
		1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"),
		1, specular_product);

	// The Light Position in Eye Frame
	vec4 light_position_eyeFrame = mv * light_position;
	glUniform4fv(glGetUniformLocation(program, "LightPosition"),
		1, light_position_eyeFrame);

	glUniform1f(glGetUniformLocation(program, "ConstAtt"),
		const_att);
	glUniform1f(glGetUniformLocation(program, "LinearAtt"),
		linear_att);
	glUniform1f(glGetUniformLocation(program, "QuadAtt"),
		quad_att);

	glUniform1f(glGetUniformLocation(program, "Shininess"),
		material_shininess);
}

void floor_lighting_param()
{
	color4 material_ambient(0.2, 0.2, 0.2, 1.0);
	color4 material_diffuse(0, 1.0, 0, 1);
	color4 material_specular(0, 0, 0, 1);
	glUniform4fv(glGetUniformLocation(program_lighting, "ini_material_ambient"), 1, material_ambient);
	glUniform4fv(glGetUniformLocation(program_lighting, "ini_material_diffuse"), 1, material_diffuse);
	glUniform4fv(glGetUniformLocation(program_lighting, "ini_material_specular"), 1, material_specular);
	glUniform1f(glGetUniformLocation(program_lighting, "Shininess"), 1.0);
}

void sphere_lighting_param()
{
	color4 material_ambient(0.2, 0.2, 0.2, 1.0);
	color4 material_diffuse(1.0, 0.84, 0, 1);
	color4 material_specular(1.0, 0.84, 0, 1);
	glUniform4fv(glGetUniformLocation(program_lighting, "ini_material_ambient"), 1, material_ambient);
	glUniform4fv(glGetUniformLocation(program_lighting, "ini_material_diffuse"), 1, material_diffuse);
	glUniform4fv(glGetUniformLocation(program_lighting, "ini_material_specular"), 1, material_specular);
	glUniform1f(glGetUniformLocation(program_lighting, "Shininess"), 125);

}

void point_light_param(mat4 mv) {
	color4 light_ambient(0, 0, 0, 1.0);
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);
	float const_att = 2.0;
	float linear_att = 0.01;
	float quad_att = 0.001;
	vec4 light_position_eyeFrame =  mv * light_position;
	glUniform4fv(glGetUniformLocation(program_lighting, "LightPosition"),
		1, light_position_eyeFrame);

	glUniform1f(glGetUniformLocation(program_lighting, "ConstAtt"),
		const_att);
	glUniform1f(glGetUniformLocation(program_lighting, "LinearAtt"),
		linear_att);
	glUniform1f(glGetUniformLocation(program_lighting, "QuadAtt"),
		quad_att);
	glUniform4fv(glGetUniformLocation(program_lighting, "input_light_ambient"), 1,
		light_ambient);
	glUniform4fv(glGetUniformLocation(program_lighting, "input_light_diffuse"), 1,
		light_diffuse);
	glUniform4fv(glGetUniformLocation(program_lighting, "input_light_specular"), 1,
		light_specular);
	glUniform1f(glGetUniformLocation(program_lighting, "point_flag"), 1.0);
	glUniform1f(glGetUniformLocation(program_lighting, "spot_flag"), 0.0);
}

void spot_light_param(mat4 mv) {
	color4 light_ambient(0, 0, 0, 1.0);
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);
	float const_att = 2.0;
	float linear_att = 0.01;
	float quad_att = 0.001;

	float spot_exp = 15.0;
	float spot_ang = cos(20.0 * 3.14 / 180);

	vec4 light_position_eyeFrame = mv * light_position;
	glUniform4fv(glGetUniformLocation(program_lighting, "LightPosition"),
		1, light_position_eyeFrame);

	glUniform1f(glGetUniformLocation(program_lighting, "ConstAtt"),
		const_att);
	glUniform1f(glGetUniformLocation(program_lighting, "LinearAtt"),
		linear_att);
	glUniform1f(glGetUniformLocation(program_lighting, "QuadAtt"),
		quad_att);
	glUniform4fv(glGetUniformLocation(program_lighting, "input_light_ambient"), 1,
		light_ambient);
	glUniform4fv(glGetUniformLocation(program_lighting, "input_light_diffuse"), 1,
		light_diffuse);
	glUniform4fv(glGetUniformLocation(program_lighting, "input_light_specular"), 1,
		light_specular);
	glUniform1f(glGetUniformLocation(program_lighting, "point_flag"), 0.0);
	glUniform1f(glGetUniformLocation(program_lighting, "spot_flag"), 1.0);
	point4 spot_direction = mv * vec4(-6.0, 0.0, -4.5, 1.0);
	glUniform4fv(glGetUniformLocation(program_lighting, "spot_direction"), 1,
		spot_direction);
	glUniform1f(glGetUniformLocation(program_lighting, "spot_exp"), spot_exp);
	glUniform1f(glGetUniformLocation(program_lighting, "spot_ang"), spot_ang);
}

void init_firework()
{
	for (int i = 0; i < 300; i++)
	{
		vec3 tmp_v = vec3(2.0 * ((rand() % 256) / 256.0 - 0.5), 2.4 * (rand() % 256) / 256.0, 2.0 * ((rand() % 256) / 256.0 - 0.5));
		firework_v[i] = tmp_v;
		vec3 tmp_c = vec3((rand() % 256) / 256.0, (rand() % 256) / 256.0, (rand() % 256) / 256.0);
		firework_c[i] = tmp_c;
		cout << firework_v[i] << "  " << firework_c[i] << endl;
	}

}

point3 crossProduct(point3 u, point3 v) {
	point3 n;
	n.x = u.y * v.z - u.z * v.y;
	n.y = u.z * v.x - u.x * v.z;
	n.z = u.x * v.y - u.y * v.x;
	return n;
}

point3 directionVector(point3 A, point3 B) {
	point3 res;
	res.x = B.x - A.x;
	res.y = B.y - A.y;
	res.z = B.z - A.z;
	float normal = sqrt3f(res.x, res.y, res.z);
	res.x /= normal;
	res.y /= normal;
	res.z /= normal;
	return res;
}

point3 cross_Product(point3 A, point3 B) {
	point3 res;
	res.x = A.y * B.z - A.z * B.y;
	res.y = A.z * B.x - A.x * B.z;
	res.z = A.x * B.y - A.y * B.x;
	return res;
	
}

float distance(point3 p1, point3 p2) {
	float dx = p1.x - p2.x;
	float dy = p1.y - p2.y;
	float dz = p1.z - p2.z;
	return sqrt3f(dx, dy, dz);
}

void normal_vec_flat() {
	int i = 0;
	for (int j = 0; j < sphere_Num_Triangle; j++) {
		vec4 ver1 = shading_vertices[j * 3];
		vec4 ver2 = shading_vertices[j * 3 + 1];
		vec4 ver3 = shading_vertices[j * 3 + 2];
		vec4 u = ver2 - ver1;
		vec4 v = ver3 - ver1;

		vec3 res_normal = normalize(cross(u, v));
		flat_shading_vertices[i] = res_normal;
		//cout << flat_shading_vertices[i] << endl;
		i++;
		
		flat_shading_vertices[i] = res_normal;
		//cout << flat_shading_vertices[i] << endl;
		i++;
		flat_shading_vertices[i] = res_normal;
		//cout << flat_shading_vertices[i] << endl;
		i++;
		
	}
}

void normal_vec_smooth() {
	for (int i = 0; i < sphere_Num_Triangle * 3; i++) {
		vec3 ver = vec3(shading_vertices[i][0], shading_vertices[i][1], shading_vertices[i][2]);
		smooth_shading_vertices[i] = normalize(ver);
		//cout << smooth_shading_vertices[i] << endl;
	}
}

void image_set_up(void)
{
	int i, j, c;
	/* --- Generate checkerboard image to the image array ---*/
	for (i = 0; i < ImageHeight; i++)
		for (j = 0; j < ImageWidth; j++)
		{
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0));

			if (c == 1) /* white */
			{
				c = 255;
				Image[i][j][0] = (GLubyte)c;
				Image[i][j][1] = (GLubyte)c;
				Image[i][j][2] = (GLubyte)c;
			}
			else  /* green */
			{
				Image[i][j][0] = (GLubyte)0;
				Image[i][j][1] = (GLubyte)150;
				Image[i][j][2] = (GLubyte)0;
			}

			Image[i][j][3] = (GLubyte)255;
		}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	/*--- Generate 1D stripe image to array stripeImage[] ---*/
	for (j = 0; j < stripeImageWidth; j++) {
		//When j <= 4, the color is (255, 0, 0),   i.e., red stripe/line.
		   //When j > 4,  the color is (255, 255, 0), i.e., yellow remaining texture

		stripeImage[4 * j] = (GLubyte)255;
		stripeImage[4 * j + 1] = (GLubyte)((j > 4) ? 255 : 0);
		stripeImage[4 * j + 2] = (GLubyte)0;
		stripeImage[4 * j + 3] = (GLubyte)255;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

}

void init_tex_ground()
{
	image_set_up();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/*--- Create and Initialize a texture object ---*/

	glGenTextures(1, &textureName);      // Generate texture obj name(s)

	glActiveTexture(GL_TEXTURE0);  // Set the active texture unit to be 0
	glBindTexture(GL_TEXTURE_2D, textureName); // Bind the texture to this texture unit

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth, ImageHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, Image);

	glGenBuffers(1, &texture_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, texture_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texture_vertices) + sizeof(texture_normals) + sizeof(texture_coord),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texture_vertices), texture_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(texture_vertices), sizeof(texture_normals), texture_normals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(texture_vertices) + sizeof(texture_normals),
		sizeof(texture_coord), texture_coord);
	
	for (int i = 0; i < (sizeof(texture_vertices)); i ++) {
		cout << texture_coord[i] << endl;
	}
}



void init()
{
	translateVector = new point3[totalRoute];
	rotationAxis = new point3[totalRoute];
	for (int i = 1; i < totalRoute + 1; i++) {
		translateVector[i - 1] = directionVector(route[i-1], route[i]);
	}

	for (int i = 1; i < totalRoute + 1; i++) {
		rotationAxis[i - 1] = cross_Product(point3(0, 1, 0), translateVector[i - 1]);
	}

	init_tex_ground();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/*--- Create and Initialize a texture object ---*/
	glGenTextures(1, &line_textureName);      // Generate texture obj name(s)

	glActiveTexture(GL_TEXTURE1);  // Set the active texture unit to be 0
	glBindTexture(GL_TEXTURE_1D, line_textureName); // Bind the texture to this texture unit

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 32,
		0, GL_RGBA, GL_UNSIGNED_BYTE, stripeImage);

	floor();
	// Create and initialize a vertex buffer object for floor, to be used in display()
	glGenBuffers(1, &floor_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, floor_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_colors),
		floor_colors);

	quad();
	glGenBuffers(1, &floor_light_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, floor_light_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_normal),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points),
		sizeof(floor_normal), floor_normal);
	//cout << floor_normal[0] << endl;
	//cout << floor_light_vertices[0] << endl;

	normal_vec_flat();
	glGenBuffers(1, &flat_shading_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, flat_shading_buffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * sphere_Num_Triangle * sizeof(point4) + 3 * sphere_Num_Triangle * sizeof(color3),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sphere_Num_Triangle * sizeof(point4), shading_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sphere_Num_Triangle * sizeof(point4),
		3 * sphere_Num_Triangle * sizeof(color3), flat_shading_vertices);

	normal_vec_smooth();
	glGenBuffers(1, &smooth_shading_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, smooth_shading_buffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * sphere_Num_Triangle * sizeof(point4) + 3 * sphere_Num_Triangle * sizeof(color3),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sphere_Num_Triangle * sizeof(point4), shading_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sphere_Num_Triangle * sizeof(point4),
		3 * sphere_Num_Triangle * sizeof(color3), smooth_shading_vertices);

	axis();
	glGenBuffers(1, &axis_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, axis_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_points) + sizeof(line_colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(line_points), line_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(line_points), sizeof(line_colors),
		line_colors);

	//cout << "Num_triangle: " << sphere_Num_Triangle << endl;

	sphere();
	// create and initialize a vertex buffer object for sphere.
	glGenBuffers(1, &sphere_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * sphere_Num_Triangle * sizeof(point3) + 3 * sphere_Num_Triangle * sizeof(color3),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sphere_Num_Triangle * sizeof(point3), spherePoints);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sphere_Num_Triangle * sizeof(point3), 3 * sphere_Num_Triangle * sizeof(color3),
		sphere_color);

	glGenBuffers(1, &shadow_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, shadow_buffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * sphere_Num_Triangle * sizeof(point3) + 3 * sphere_Num_Triangle * sizeof(color3),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sphere_Num_Triangle * sizeof(point3), shadow_data);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sphere_Num_Triangle * sizeof(point3), 3 * sphere_Num_Triangle * sizeof(color3),
		shadow_color);

	init_firework();
	glGenBuffers(1, &firework_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, firework_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(firework_v) + sizeof(firework_c),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(firework_v), firework_v);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(firework_v), sizeof(firework_c),
		firework_c);


	// Load shaders and create a shader program (to be used in display())
	program = InitShader("vshader42.glsl", "fshader42.glsl");
	
	program_lighting = InitShader("vshader53.glsl", "fshader53.glsl");

	program_ex = InitShader("vshader_ex.glsl", "fshader_ex.glsl");

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glLineWidth(2.5);
}


char* filename = new char[100];

void file_in() {

	string line;
	ifstream myfile;
	printf("Please enter the name of the file(sphere.8 or sphere.128): ");
	cin.getline(filename, 100);
	myfile.open(filename);
	int line_num = 0;
	int vertex_num = 0;
	int count = 0;
	int x_max = 0;
	int x_min = 1;
	


	if (!myfile.is_open()) {
		perror("Error open file");
		exit(EXIT_FAILURE);
	}
	while (getline(myfile, line)) {
		if (line_num == 0) {
			sphere_Num_Triangle = std::stof(line);
			spherePoints = new point3[sphere_Num_Triangle * 3];
			shadow_data = new point3[sphere_Num_Triangle * 3];
			shading_vertices = new point4[sphere_Num_Triangle * 3];
			flat_shading_vertices = new point3[sphere_Num_Triangle * 3];
			smooth_shading_vertices = new point3[sphere_Num_Triangle * 3];
		}

		if ((line_num % 4 != 1) && (line_num > 1)) {
			int index1 = line.find(' ');
			int index2 = line.find(' ', index1 + 1);
			float x = std::stof(line.substr(0, index1));
			float y = std::stof(line.substr(index1 + 1, index2));
			float z = std::stof(line.substr(index2 + 1, line.length()));
			point3 pt(x, y, z);
			if (x > x_max) x_max = x;
			if (x < x_min) x_min = x;
			spherePoints[count] = {x, y, z};
			shadow_data[count] = { x, y, z };
			shading_vertices[count] = vec4(x, y, z, 1);
			count++;
			vertex_num += 1;
			

		}
		line_num += 1;
	}
	radius = (x_max - x_min) / 2;
	myfile.close();
}

void drawObj(GLuint buffer, int num_vertices)
{
	//--- Activate the vertex buffer object to be drawn ---//
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/*----- Set up vertex attribute arrays for each vertex attribute -----*/
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point3) * num_vertices));
	// the offset is the (total) size of the previous vertex attribute array(s)

  /* Draw a sequence of geometric objs (triangles) from the vertex buffer
	 (using the attributes specified in each enabled vertex attribute array) */
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);

	/*--- Disable each vertex attribute array being enabled ---*/
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
}

void drawObj_lighting(GLuint buffer, int num_vertices)
{
	//--- Activate the vertex buffer object to be drawn ---//
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/*----- Set up vertex attribute arrays for each vertex attribute -----*/
	GLuint vPosition = glGetAttribLocation(program_lighting, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program_lighting, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point4) * num_vertices));
	// the offset is the (total) size of the previous vertex attribute array(s)

  /* Draw a sequence of geometric objs (triangles) from the vertex buffer
	 (using the attributes specified in each enabled vertex attribute array) */
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);

	/*--- Disable each vertex attribute array being enabled ---*/
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vNormal);
}

void drawObj_texture(GLuint buffer, int num_vertices)
{
	//--- Activate the vertex buffer object to be drawn ---//
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/*----- Set up vertex attribute arrays for each vertex attribute -----*/

	glUniform1i(glGetUniformLocation(program_lighting, "ground_texture_flag"), groundTextureFlag);
	GLuint vPosition = glGetAttribLocation(program_lighting, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program_lighting, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(num_vertices * sizeof(point4)));

	GLuint vTexCoord = glGetAttribLocation(program_lighting, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(num_vertices * (sizeof(point4) + sizeof(point3))));
	// the offset is the (total) size of the previous vertex attribute array(s)

	/* Draw a sequence of geometric objs (triangles) from the vertex buffer
	   (using the attributes specified in each enabled vertex attribute array) */
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);
	glDisableVertexAttribArray(vTexCoord);
	/*--- Disable each vertex attribute array being enabled ---*/
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vNormal);
}

void drawObj_firework(GLuint buffer, int num_vertices)
{	
	cout << "firework" << endl;
	//--- Activate the vertex buffer object to be drawn ---//
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/*----- Set up vertex attribute arrays for each vertex attribute -----*/
	GLuint velocity = glGetAttribLocation(program_ex, "velocity");
	glEnableVertexAttribArray(velocity);
	glVertexAttribPointer(velocity, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program_ex, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point3) * num_vertices));
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);
	/*--- Disable each vertex attribute array being enabled ---*/
	glDisableVertexAttribArray(velocity);
	glDisableVertexAttribArray(vColor);
}

void draw_floor(mat4 mv, mat4 eye_frame, mat4 p) {
	if (lightingFlag == 0) {
		//mv = LookAt(eye, at, up) * Translate(0, 0, 0);
		glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
		glUniformMatrix4fv(projection, 1, GL_TRUE, p);
		if (floorFlag == 1) // Filled floor
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else              // Wireframe floor
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		drawObj(floor_buffer, floor_NumVertices);  // draw the floor
	}
	else
	{
		glUseProgram(program_lighting);
		model_view_lighting = glGetUniformLocation(program_lighting, "ModelView");
		projection_lighting = glGetUniformLocation(program_lighting, "Projection");
		glUniformMatrix4fv(projection_lighting, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major
		if (pointFlag)
		{
			point_light_param(eye_frame);
		}
		else
		{
			spot_light_param(eye_frame);
		}
		floor_lighting_param();
		mat4  mv2 = mv;
		glUniformMatrix4fv(model_view_lighting, 1, GL_TRUE, mv2);
		mat3 normal_matrix = NormalMatrix(mv2, 1);
		glUniformMatrix3fv(glGetUniformLocation(program_lighting, "Normal_Matrix"), 1, GL_TRUE, normal_matrix);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUniform1i(glGetUniformLocation(program_lighting, "text_ground_flag"), 0);
		if (groundTextureFlag == 1) {
			glUniform1i(glGetUniformLocation(program_lighting, "texture_1D"), 1);
			glUniform1i(glGetUniformLocation(program_lighting, "texture_2D"), 0);
			glUniform1i(glGetUniformLocation(program_lighting, "ground_texture_flag"), 1);
			drawObj(floor_buffer, 6);
			drawObj_texture(texture_buffer, 6);
		
		}
		else {

			drawObj(floor_light_buffer, 6);
			glUniform1i(glGetUniformLocation(program_lighting, "text_ground_flag"), 0);
		}
	}
	
}

//----------------------------------------------------------------------------
void display(void)
{
	shadowFlag = 0;
	if (wireFlag == 1 || lightingFlag == 0)
	{
		flatFlag = 0;
		smoothFlag = 0;
	}
	if (lightingFlag == 1)
	{
		shadowFlag = 1.0;
	}
	glUniform1f(glGetUniformLocation(program, "shade_flag"), shadowFlag);
	glUniform1i(glGetUniformLocation(program, "text_ground_flag"), 0);
	glUniform1i(glGetUniformLocation(program, "f_sphere_flag"), 0);
	glUniform1i(glGetUniformLocation(program, "vertical_flag"), 1);
	glUniform1i(glGetUniformLocation(program, "slant_flag"), 0);
	glUniform1i(glGetUniformLocation(program, "eye_space_flag"), 0);
	glUniform1i(glGetUniformLocation(program, "object_space_flag"), 1);



	glUseProgram(program_lighting);
	glUniform1i(glGetUniformLocation(program_lighting, "fogModeLocal"), fog);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program); // Use the shader program

	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");
	glClearColor(0.592, 0.807, 0.92, 0);
	glEnable(GL_DEPTH_TEST);

	/*---  Set up and pass on Projection matrix to the shader ---*/
	mat4  p = Perspective(fovy, aspect, zNear, zFar);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major


	vec4    at(0.0, 0.0, 0.0, 1.0);
	vec4    up(0.0, 1.0, 0.0, 0.0);
	mat4  mv = LookAt(eye, at, up);
	mat4  eye_frame = LookAt(eye, at, up);
	//cout << "mv: " << mv << endl;

	//mv = mv * Translate(0, 0, 0);
	//SetUp_Lighting_Uniform_Vars(mv);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawObj(axis_buffer, 9);

	glDepthMask(GL_FALSE);
	//draw floor
	draw_floor(mv, eye_frame, p);
	glDepthMask(GL_TRUE);

	//draw shadow
	if (shadowFlag == 1) {
		glUseProgram(program_lighting);
		glUniform1i(glGetUniformLocation(program_lighting, "draw_shadow_lat"), 1);
		glUniform1i(glGetUniformLocation(program_lighting, "f_draw_shadow_lat"), 1);
		glUniform1i(glGetUniformLocation(program_lighting, "upright_lat_flag"), upright_lat_flag);
		glUseProgram(program);
		mat4 shadow_transMatrix(12, 0, 0, 0, 14, 0, 3, -1, 0, 0, 12, 0, 0, 0, 0, 12);
		mv = LookAt(eye, at, up) * shadow_transMatrix * Translate(spherePos.x, spherePos.y, spherePos.z) * rotationMatrix;

		//cout << "blendingFlag " << blendFlag << endl;
		glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
		if (wireFlag == 0) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (blendFlag == 1)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUniform1f(glGetUniformLocation(program_lighting, "shade_flag"), 0);
			glUniform1f(glGetUniformLocation(program_lighting, "shadow_semi_flag"), 1.0);
			drawObj(shadow_buffer, sphere_Num_Triangle * 3);
			glDisable(GL_BLEND);
		}
		else
		{
			glUniform1f(glGetUniformLocation(program_lighting, "shade_flag"), 0);
			glUniform1f(glGetUniformLocation(program_lighting, "shadow_semi_flag"), 0);
			drawObj(shadow_buffer, sphere_Num_Triangle * 3);
		}
		glUniform1i(glGetUniformLocation(program_lighting, "draw_shadow_lat"), 0);
		glUniform1i(glGetUniformLocation(program_lighting, "f_draw_shadow_lat"), 0);

	}

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	//draw floor again
	draw_floor(mv, eye_frame, p);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);


	//calculate rotation
	//////////////////////////////////////////////
	if (animationFlag == 1) {
		spherePos.x = spherePos.x + translateVector[currentRoute].x * speed * 3.14 / 180 * radius;
		spherePos.y = spherePos.y + translateVector[currentRoute].y * speed * 3.14 / 180 * radius;
		spherePos.z = spherePos.z + translateVector[currentRoute].z * speed * 3.14 / 180 * radius;

		rotationMatrix = Rotate(speed, rotationAxis[currentRoute].x, rotationAxis[currentRoute].y, rotationAxis[currentRoute].z) * rotationMatrix;
		mv = LookAt(eye, at, up) * Translate(spherePos.x, spherePos.y, spherePos.z) * rotationMatrix;
	}
	else {
		rotationMatrix = Rotate(0, rotationAxis[currentRoute].x, rotationAxis[currentRoute].y, rotationAxis[currentRoute].z) * rotationMatrix;
		mv = LookAt(eye, at, up) * Translate(spherePos.x, spherePos.y, spherePos.z) * rotationMatrix;
	}
	//////////////////////////////////////////////
	glUseProgram(program_lighting);
	if (lightingFlag == 1) glUniform1f(glGetUniformLocation(program_lighting, "shade_flag"), 1.0);
	if (flatFlag == 1) {
		model_view_lighting = glGetUniformLocation(program_lighting, "ModelView");
		projection_lighting = glGetUniformLocation(program_lighting, "Projection");
		glUniformMatrix4fv(projection_lighting, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major

		if (pointFlag)
		{
			point_light_param(eye_frame);
		}
		else
		{
			spot_light_param(eye_frame);
		}
		sphere_lighting_param();
		mat4  mv2 = mv;
		//cout << mv2 << endl;
		glUniformMatrix4fv(model_view_lighting, 1, GL_TRUE, mv2);
		mat3 normal_matrix = NormalMatrix(mv2, 1);
		//cout << normal_matrix << endl;
		glUniformMatrix3fv(glGetUniformLocation(program_lighting, "Normal_Matrix"),
			1, GL_TRUE, normal_matrix);
		if (wireFlag == 0) { // Filled floor
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			if (text_sphere_flag == 1)
			{
				glUniform1i(glGetUniformLocation(program_lighting, "vertical_flag"), vertical_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "slant_flag"), slant_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "eye_space_flag"), eye_space_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "object_space_flag"), object_space_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "sphere_line_flag"), text_sphere_line_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "f_sphere_flag"), text_sphere_line_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "sphere_check_flag"), text_sphere_check_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "f_sphere_check_flag"), text_sphere_check_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "upright_lat_flag"), upright_lat_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "enable_lat"), enable_lat);
				glUniform1i(glGetUniformLocation(program_lighting, "texture_1D"), 1);
				glUniform1i(glGetUniformLocation(program_lighting, "texture_2D"), 0);
				drawObj_lighting(flat_shading_buffer, sphere_Num_Triangle * 3);
				glUniform1i(glGetUniformLocation(program_lighting, "f_sphere_flag"), 0);
				glUniform1i(glGetUniformLocation(program_lighting, "f_sphere_check_flag"), 0);
				glUniform1i(glGetUniformLocation(program_lighting, "eye_space_flag"), 0);
				glUniform1i(glGetUniformLocation(program_lighting, "object_space_flag"), 0);
				glUniform1i(glGetUniformLocation(program_lighting, "sphere_line_flag"), 0);
				glUniform1i(glGetUniformLocation(program_lighting, "sphere_check_flag"), 0);
			}
			else
			{
				drawObj_lighting(flat_shading_buffer, sphere_Num_Triangle * 3);
			}
		}
		else {           // Wireframe floor
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			drawObj_lighting(flat_shading_buffer, sphere_Num_Triangle * 3);
		}
		
	}
	else if (smoothFlag == 1) {
		glUseProgram(program_lighting);
		model_view_lighting = glGetUniformLocation(program_lighting, "ModelView");
		projection_lighting = glGetUniformLocation(program_lighting, "Projection");
		glUniformMatrix4fv(projection_lighting, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major

		if (pointFlag)
		{
			point_light_param(eye_frame);
		}
		else
		{
			spot_light_param(eye_frame);
		}
		sphere_lighting_param();
		mat4  mv2 = mv;
		//cout << mv2 << endl;
		glUniformMatrix4fv(model_view_lighting, 1, GL_TRUE, mv2);
		mat3 normal_matrix = NormalMatrix(mv2, 1);
		//cout << normal_matrix << endl;
		glUniformMatrix3fv(glGetUniformLocation(program_lighting, "Normal_Matrix"),
			1, GL_TRUE, normal_matrix);
		if (wireFlag == 0) { // Filled floor
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			if (text_sphere_flag == 1)
			{
				glUniform1i(glGetUniformLocation(program_lighting, "vertical_flag"), vertical_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "slant_flag"), slant_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "eye_space_flag"), eye_space_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "object_space_flag"), object_space_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "sphere_line_flag"), text_sphere_line_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "f_sphere_flag"), text_sphere_line_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "sphere_check_flag"), text_sphere_check_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "f_sphere_check_flag"), text_sphere_check_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "upright_lat_flag"), upright_lat_flag);
				glUniform1i(glGetUniformLocation(program_lighting, "enable_lat"), enable_lat);
				glUniform1i(glGetUniformLocation(program_lighting, "texture_1D"), 1);
				glUniform1i(glGetUniformLocation(program_lighting, "texture_2D"), 0);
				glUniform1i(glGetUniformLocation(program, "texture_1D"), 1);
				glUniform1i(glGetUniformLocation(program, "texture_2D"), 0);
				glUniform1i(glGetUniformLocation(program, "my_texture_2D"), 2);
				drawObj_lighting(smooth_shading_buffer, sphere_Num_Triangle * 3);
				glUniform1i(glGetUniformLocation(program_lighting, "f_sphere_flag"), 0);
				glUniform1i(glGetUniformLocation(program_lighting, "f_sphere_check_flag"), 0);
				glUniform1i(glGetUniformLocation(program_lighting, "eye_space_flag"), 0);
				glUniform1i(glGetUniformLocation(program_lighting, "object_space_flag"), 0);
				glUniform1i(glGetUniformLocation(program_lighting, "sphere_line_flag"), 0);
				glUniform1i(glGetUniformLocation(program_lighting, "sphere_check_flag"), 0);
			}
			else
			{
				drawObj_lighting(smooth_shading_buffer, sphere_Num_Triangle * 3);
			}
		}

		else              // Wireframe floor
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		drawObj_lighting(smooth_shading_buffer, sphere_Num_Triangle * 3);
	}
	else
	{
		glUseProgram(program);
		model_view = glGetUniformLocation(program, "model_view");
		projection = glGetUniformLocation(program, "projection");
		glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major
		glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
		if (wireFlag == 1)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		drawObj(sphere_buffer, sphere_Num_Triangle * 3);
	}
	
	glUseProgram(program_ex);
	model_view_ex = glGetUniformLocation(program_ex, "model_view");
	projection_ex = glGetUniformLocation(program_ex, "projection");
	glUniformMatrix4fv(model_view_ex, 1, GL_TRUE, p);
	glUniformMatrix4fv(projection_ex, 1, GL_TRUE, eye_frame);
	glPointSize(3.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	float t = glutGet(GLUT_ELAPSED_TIME);
	int time = int((t - t_sub)) % 5000;
	if (pressB == true)
		glUniform1f(glGetUniformLocation(program_ex, "time"), time);
	else
		glUniform1f(glGetUniformLocation(program_ex, "time"), 0);

	if (fireworkFlag == 1) drawObj_lighting(firework_buffer, 300);
	glutSwapBuffers();
}
//---------------------------------------------------------------------------
void idle(void)
{
	angle += speed;
	if (angle > 360) {
		angle -= 360;
	}
	
	
	float traveled = distance(spherePos, route[currentRoute]);
	float travelRoute = distance(route[currentRoute], route[currentRoute + 1]);
	if (traveled > travelRoute) {
		currentRoute += 1;
		spherePos = route[currentRoute];
	}
	if (currentRoute == 3) currentRoute = 0;
	glutPostRedisplay();
}

void main_menu(int index)

{
	switch (index)
	{
		case (0):{
			eye = init_eye;

			break;
		}
		case (1):{
			exit(1);
			break;
		}
	}

	display();
}

void wire_menu(int index) {
	wireFlag = index - 1;
	display();
}

void shadow_menu(int index) {
	shadowFlag = index - 1;
	display();
}

void lighting_menu(int index) {
	lightingFlag = index - 1;
	smoothFlag = 1;
	display();
}

void shade_menu(int index) {
	if (index == 1) {
		if (flatFlag == 0)
			flatFlag = 1;
			smoothFlag = 0;
	}
	else {
		flatFlag = 0;
		smoothFlag = 1;
	}
	display();
};

void spotlight_menu(int index) {
	if (index) {
		pointFlag = 0;
	}
	else
	{
		pointFlag = 1;
	}
	display();
};

void fog_menu(int index) {
	fog = index;
	display();
}

void blending_menu(int index) {
	blendFlag = index;
	display();
}

void groundTexture_menu(int index) {
	groundTextureFlag = index;
	display();
}

void firework_menu(int index)
{
	fireworkFlag = index;
	display();
}

void tex_sphere_menu(int index)
{
	if (index == 1)
	{
		text_sphere_flag = 0;
	}
	if (index == 2)
	{
		text_sphere_flag = 1;
		text_sphere_line_flag = 1;
		text_sphere_check_flag = 0;
	}
	if (index == 3)
	{
		text_sphere_flag = 1;
		text_sphere_line_flag = 0;
		text_sphere_check_flag = 1;
	}
	display();
}

void addMenu() {
	int wire = glutCreateMenu(wire_menu);
	glutAddMenuEntry("Solid", 1);
	glutAddMenuEntry("Wire frame", 2);

	int shadow = glutCreateMenu(shadow_menu);
	glutAddMenuEntry("Yes", 2);
	glutAddMenuEntry("No", 1);

	int enable_lighting = glutCreateMenu(lighting_menu);
	glutAddMenuEntry("Yes", 2);
	glutAddMenuEntry("No", 1);

	int shade = glutCreateMenu(shade_menu);
	glutAddMenuEntry("flat shading", 1);
	glutAddMenuEntry("smooth shading", 2);

	int spotlight = glutCreateMenu(spotlight_menu);
	glutAddMenuEntry("Spot light", 1);
	glutAddMenuEntry("Point light", 2);

	int fogSetting = glutCreateMenu(fog_menu);
	glutAddMenuEntry("No fog", 0);
	glutAddMenuEntry("linear fog", 1);
	glutAddMenuEntry("exponential fog", 2);
	glutAddMenuEntry("exponential square fog", 3);

	int BlendShadow = glutCreateMenu(blending_menu);
	glutAddMenuEntry("No", 0);
	glutAddMenuEntry("Yes", 1);

	int groundTexture = glutCreateMenu(groundTexture_menu);
	glutAddMenuEntry("No", 0);
	glutAddMenuEntry("Yes", 1);

	int fireworks = glutCreateMenu(firework_menu);
	glutAddMenuEntry("No", 0);
	glutAddMenuEntry("Yes", 1);

	int text_sphere = glutCreateMenu(tex_sphere_menu);
	glutAddMenuEntry("No", 1);
	glutAddMenuEntry("Yes-Contour Lines", 2);
	glutAddMenuEntry("Yes-Checkerboard", 3);

	glutCreateMenu(main_menu);
	glutAddMenuEntry("Default View Point", 0);
	glutAddSubMenu("Wire Frame", wire);
	glutAddSubMenu("Shadow", shadow);
	glutAddSubMenu("Enable Lighting", enable_lighting);
	glutAddSubMenu("Shading", shade);
	glutAddSubMenu("light source", spotlight);
	glutAddSubMenu("fog", fogSetting);
	glutAddSubMenu("Blending Shadow", BlendShadow);
	glutAddSubMenu("ground texture", groundTexture);
	glutAddSubMenu("Fireworks", fireworks);
	glutAddSubMenu("Texture Mapped Sphere", text_sphere);
	glutAddMenuEntry("Quit", 1);
	
	glutAttachMenu(GLUT_LEFT_BUTTON);
}





//----------------------------------------------------------------------------
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;

	case 'b': case 'B':
		t_sub = glutGet(GLUT_ELAPSED_TIME);
		pressB = true;
		animationFlag = 1 - animationFlag;
		if (animationFlag == 1) glutIdleFunc(idle);
		else                    glutIdleFunc(NULL);
		break;

	case 'X': eye[0] += 1.0; break;
	case 'x': eye[0] -= 1.0; break;
	case 'Y': eye[1] += 1.0; break;
	case 'y': eye[1] -= 1.0; break;
	case 'Z': eye[2] += 1.0; break;
	case 'z': eye[2] -= 1.0; break;

	case 'a': case 'A': // Toggle between animation and non-animation
		animationFlag = 1 - animationFlag;
		if (animationFlag == 1) glutIdleFunc(idle);
		else                    glutIdleFunc(NULL);
		break;

	case 'c': case 'C': // Toggle between filled and wireframe cube
		cubeFlag = 1 - cubeFlag;
		break;

	case 'f': case 'F': // Toggle between filled and wireframe floor
		floorFlag = 1 - floorFlag;
		break;

	case ' ':  // reset to initial viewer/eye position
		eye = init_eye;
		break;

	case 'v': case'V':
		vertical_flag = 1;
		slant_flag = 0;
		break;
	case 's': case'S':
		vertical_flag = 0;
		slant_flag = 1;
		break;
	case 'o': case'O':
		object_space_flag = 1;
		eye_space_flag = 0;
		break;
	case 'e': case'E':
		object_space_flag = 0;
		eye_space_flag = 1;
		break;
	case 'u': case'U':
		upright_lat_flag = 0;
		break;
	case 't': case'T':
		upright_lat_flag = 1;
		break;
	case 'l': case'L':
		enable_lat = 1 - enable_lat;
		break;
	}


	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP && pressB) {
		animationFlag = 1 - animationFlag;
		if (animationFlag == 1) {
			
			glutIdleFunc(idle);
		}
		else {

			glutIdleFunc(NULL);
		}
	}
	
}

void menuPause(int status, int x, int y) {
	if (pressB) {
		if (status == GLUT_MENU_IN_USE) {
			animationFlag = 0;
		}
		else {
			animationFlag = 1;
		}
		if (animationFlag == 1) {

			glutIdleFunc(idle);
		}
		else {

			glutIdleFunc(NULL);
		}
	}
}

//----------------------------------------------------------------------------
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	aspect = (GLfloat)width / (GLfloat)height;
	glutPostRedisplay();
}
//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	file_in();
	glutInit(&argc, argv);
#ifdef __APPLE__ // Enable core profile of OpenGL 3.2 on macOS.
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
#else
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	glutInitWindowSize(512, 512);
	glutCreateWindow("rotate sphere");
	addMenu();
#ifdef __APPLE__ // on macOS
	// Core profile requires to create a Vertex Array Object (VAO).
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
#else           // on Linux or Windows, we still need glew
	/* Call glewInit() and error checking */
	int err = glewInit();
	if (GLEW_OK != err)
	{
		printf("Error: glewInit failed: %s\n", (char*)glewGetErrorString(err));
		exit(1);
	}
#endif

	// Get info of GPU and supported OpenGL version
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMenuStatusFunc(menuPause);

	init();
	glutMainLoop();
	return 0;
}
