#define no_init_all deprecated

// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.


// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Project includes
#include "maths_funcs.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/*----------------------------------------------------------------------------
MESH TO LOAD
----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here
#define BOX "teapot.dae"
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/



vec3 light_pos = vec3(10.0, 10.0, 4.0);
vec3 blue = vec3(0.0f, 0.0f, 1.0f);
vec3 red = vec3(1.0f, 0.0f, 0.0f);
vec3 green = vec3(0.0f, 1.0f, 0.0f);
vec3 black = vec3(0.0f, 0.0f, 0.0f);
vec3 white = vec3(1.0f, 1.0f, 1.0f);



vec3 lightPositions[] = {
	vec3(0.0f,  20.0f, 10.0f),
	vec3(20.0f,  20.0f, 10.0f),
	vec3(0.0f, 5.0f, 10.0f),
	vec3(20.0f, 0.0f, 10.0f),
};

vec3 lightColours[] = {
	vec3(300.0f, 300.0f, 300.0f),
	vec3(300.0f, 300.0f, 300.0f),
	vec3(300.0f, 300.0f, 300.0f),
	vec3(300.0f, 300.0f, 300.0f),
};





vec3 cameraPos = vec3(0.0f, 0.0f, 30.0f);
vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
vec3 cameraDirec = normalise(cameraPos - cameraTarget);
vec3 up = vec3(0.0f, 1.0f, 0.0f);
vec3 camRight = normalise(cross(up, cameraDirec));
//vec3 cameraUp = cross(cameraDirec, camRight);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);

float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;


// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


// mouse state
bool firstMouse = true;
float lastX = 800.0f / 2.0;
float lastY = 600.0f / 2.0;


float camX = 0.0f;

#pragma region SimpleTypes
typedef struct ModelData
{
	size_t mPointCount = 0;
	std::vector<vec3> mVertices;
	std::vector<vec3> mNormals;
	std::vector<vec2> mTextureCoords;
} ModelData;
#pragma endregion SimpleTypes

using namespace std;
GLuint shaderProgramID;
GLuint shaderProgramRED;
GLuint shaderProgramBLUE;
GLuint shaderProgramP;
GLuint shaderProgramBP;
GLuint shaderProgramG;
GLuint shaderProgramT;
GLuint shaderProgramCT;
GLuint shaderProgramGREEN;

ModelData box, cone;
GLuint box_vao, cone_vao;

//ModelData mesh_data;
unsigned int mesh_vao = 0;
int width = 800;
int height = 600;

GLuint loc1, loc2, loc3;
GLfloat rotate_x = 0.0f;
GLfloat update_x = 0.0f;
GLfloat rotate_y = 0.0f;
GLfloat update_y = 0.0f;
GLfloat rotate_z = 0.0f;
GLfloat update_z = 0.0f;

GLfloat view_x = 0;
GLfloat view_y = 2;
GLfloat view_z = -15;



#pragma region MESH LOADING
/*----------------------------------------------------------------------------
MESH LOADING FUNCTION
----------------------------------------------------------------------------*/

ModelData load_mesh(const char* file_name) {

	ModelData modelData;

	/* Use assimp to read the model file, forcing it to be read as    */
	/* triangles. The second flag (aiProcess_PreTransformVertices) is */
	/* relevant if there are multiple meshes in the model file that   */
	/* are offset from the origin. This is pre-transform them so      */
	/* they're in the right position.                                 */
	const aiScene* scene = aiImportFile(
		file_name,
		aiProcess_Triangulate | aiProcess_PreTransformVertices
	);

	if (!scene) {
		fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
		return modelData;
	}

	printf("  %i materials\n", scene->mNumMaterials);
	printf("  %i meshes\n", scene->mNumMeshes);
	printf("  %i textures\n", scene->mNumTextures);

	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		const aiMesh* mesh = scene->mMeshes[m_i];
		printf("    %i vertices in mesh\n", mesh->mNumVertices);
		modelData.mPointCount += mesh->mNumVertices;
		for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
			if (mesh->HasPositions()) {
				const aiVector3D* vp = &(mesh->mVertices[v_i]);
				modelData.mVertices.push_back(vec3(vp->x, vp->y, vp->z));
			}
			if (mesh->HasNormals()) {
				const aiVector3D* vn = &(mesh->mNormals[v_i]);
				modelData.mNormals.push_back(vec3(vn->x, vn->y, vn->z));
			}
			if (mesh->HasTextureCoords(0)) {
				const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
				modelData.mTextureCoords.push_back(vec2(vt->x, vt->y));
			}
			if (mesh->HasTangentsAndBitangents()) {
				/* You can extract tangents and bitangents here              */
				/* Note that you might need to make Assimp generate this     */
				/* data for you. Take a look at the flags that aiImportFile  */
				/* can take.                                                 */
			}
		}
	}

	aiReleaseImport(scene);
	return modelData;
}

#pragma endregion MESH LOADING

// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS
char* readShaderSource(const char* shaderFile) {
	FILE* fp;
	fopen_s(&fp, shaderFile, "rb");

	if (fp == NULL) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';

	fclose(fp);

	return buf;
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		std::cerr << "Error creating shader..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	const char* pShaderSource = readShaderSource(pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
	glCompileShader(ShaderObj);
	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024] = { '\0' };
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		std::cerr << "Error compiling "
			<< (ShaderType == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader program: " << InfoLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	// Attach the compiled shader object to the program object
	glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders(const char* vertex, const char* fragment)
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
	GLuint shaderProgram = glCreateProgram();
	if (shaderProgram == 0) {
		std::cerr << "Error creating shader program..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}


	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(shaderProgram, vertex, GL_VERTEX_SHADER);
	AddShader(shaderProgram, fragment, GL_FRAGMENT_SHADER);


	GLint Success = 0;
	GLchar ErrorLog[1024] = { '\0' };
	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(shaderProgram);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Error linking shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(shaderProgram);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(shaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Invalid shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
	glUseProgram(shaderProgram);
	return shaderProgram;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS
GLuint generateObjectBufferMesh(ModelData mesh_data, GLuint shader) {
	/*----------------------------------------------------------------------------
	LOAD MESH HERE AND COPY INTO BUFFERS
	----------------------------------------------------------------------------*/

	//Note: you may get an error "vector subscript out of range" if you are using this code for a mesh that doesnt have positions and normals
	//Might be an idea to do a check for that before generating and binding the buffer.



	unsigned int vp_vbo = 0;
	loc1 = glGetAttribLocation(shader, "vertex_position");
	loc2 = glGetAttribLocation(shader, "vertex_normal");
	loc3 = glGetAttribLocation(shader, "vertex_texture");

	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec3), &mesh_data.mVertices[0], GL_STATIC_DRAW);
	unsigned int vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec3), &mesh_data.mNormals[0], GL_STATIC_DRAW);



	// This is for texture coordinates which you don't currently need, so I have commented it out
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(loc1);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// This is for texture coordinates which you don't currently need, so I have commented it out

	return vao;

}
#pragma endregion VBO_FUNCTIONS


void display() {

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	glUseProgram(shaderProgramBLUE);

	
	vec3 lightposition = vec3(10.0, 10.0, 4.0);

	//Declare your uniform variables that will be used in your shader
	// Vertex Shader Uniforms
	int matrix_location = glGetUniformLocation(shaderProgramBLUE, "model");
	int view_mat_location = glGetUniformLocation(shaderProgramBLUE, "view");
	int proj_mat_location = glGetUniformLocation(shaderProgramBLUE, "proj");

	int matrix_locationP = glGetUniformLocation(shaderProgramP, "model");
	int view_mat_locationP = glGetUniformLocation(shaderProgramP, "view");
	int proj_mat_locationP = glGetUniformLocation(shaderProgramP, "proj");

	
	int matrix_location1 = glGetUniformLocation(shaderProgramT, "model");
	int view_mat_location1 = glGetUniformLocation(shaderProgramT, "view");
	int proj_mat_location1 = glGetUniformLocation(shaderProgramT, "proj");

	int matrix_locationBP = glGetUniformLocation(shaderProgramBP, "model");
	int view_mat_locationBP = glGetUniformLocation(shaderProgramBP, "view");
	int proj_mat_locationBP = glGetUniformLocation(shaderProgramBP, "proj");

	int matrix_locationG = glGetUniformLocation(shaderProgramG, "model");
	int view_mat_locationG = glGetUniformLocation(shaderProgramG, "view");
	int proj_mat_locationG = glGetUniformLocation(shaderProgramG, "proj");

	int light_pos_vec3_P = glGetUniformLocation(shaderProgramP, "LightPosition");
	int light_pos_vec3_BP = glGetUniformLocation(shaderProgramBP, "LightPosition");
	int light_pos_vec3_G = glGetUniformLocation(shaderProgramG, "LightPosition");
	int light_pos_vec3_T = glGetUniformLocation(shaderProgramT, "LightPosition");

	vec3 light_position_attempt = vec3(10, 10, 5);
	glUniform3fv(light_pos_vec3_P, 1, light_position_attempt.v);
	glUniform3fv(light_pos_vec3_BP, 1, light_position_attempt.v);
	glUniform3fv(light_pos_vec3_G, 1, light_position_attempt.v);
	glUniform3fv(light_pos_vec3_T, 1, light_position_attempt.v);


	// Toon ----------------------------------------------------------------------------------

	glUseProgram(shaderProgramT);

	// Root of the Hierarchy
	mat4 view = identity_mat4();
	mat4 persp_proj = perspective(fov, (float)width / (float)height, 0.1f, 1000.0f);
	mat4 model = identity_mat4();
	model = translate(model, vec3(0.0f, -2.0f, 0.0f));
	model = rotate_y_deg(model, rotate_y);
	view = look_at(cameraPos, cameraPos + cameraFront, cameraUp);

	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location1, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location1, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, model.m);


	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);

	// Blinn Phong -------------------
	glUseProgram(shaderProgramBP);
	
	
	mat4 thirdModel = identity_mat4();
	thirdModel = rotate_y_deg(thirdModel, rotate_y);
	thirdModel = translate(thirdModel, vec3(-8.0f, -2.0f, 0.0f));

	int object_colour_vec3_BP = glGetUniformLocation(shaderProgramBP, "objectColor");
	float shine_BP = glGetUniformLocation(shaderProgramBP, "shininess");

	glUniform3fv(object_colour_vec3_BP, 1, red.v);
	glUniform1f(shine_BP, 200);

	glUniformMatrix4fv(proj_mat_locationBP, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_locationBP, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, thirdModel.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);



	// Cook Torrence Physically based rendering

	glUseProgram(shaderProgramCT);


	mat4 seventhModel = identity_mat4();
	seventhModel = rotate_y_deg(seventhModel, rotate_y);
	seventhModel = translate(seventhModel, vec3(8.0f, -2.0f, 0.0f));

	int matrix_location_CookTor = glGetUniformLocation(shaderProgramCT, "model");
	int view_mat_location_CookTor = glGetUniformLocation(shaderProgramCT, "view");
	int proj_mat_location_CookTor = glGetUniformLocation(shaderProgramCT, "proj");

	int albedo_Cooktor = glGetUniformLocation(shaderProgramCT, "albedo");
	int light_CookTor = glGetUniformLocation(shaderProgramCT, "lightPositions[0]");
	int light_COLOR_CookTor = glGetUniformLocation(shaderProgramCT, "lightColors[0]");
	int light_CookTor1 = glGetUniformLocation(shaderProgramCT, "lightPositions[1]");
	int light_COLOR_CookTor1 = glGetUniformLocation(shaderProgramCT, "lightColors[1]");
	int light_CookTor2 = glGetUniformLocation(shaderProgramCT, "lightPositions[2]");
	int light_COLOR_CookTor2 = glGetUniformLocation(shaderProgramCT, "lightColors[2]");
	int light_CookTor3 = glGetUniformLocation(shaderProgramCT, "lightPositions[3]");
	int light_COLOR_CookTor3 = glGetUniformLocation(shaderProgramCT, "lightColors[3]");
	int camPos_CookTor = glGetUniformLocation(shaderProgramCT, "camPos");
	float ao_CookTor = glGetUniformLocation(shaderProgramCT, "ao");
	float metal_CookTor = glGetUniformLocation(shaderProgramCT, "metallic");
	float rough_CookTor = glGetUniformLocation(shaderProgramCT, "roughness");

	glUniformMatrix4fv(proj_mat_location_CookTor, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location_CookTor, 1, GL_FALSE, view.m);
	
	vec3 albedo = vec3(0.0f, 0.0f, 1.0f);

	// Add lights to shader
	glUniform3fv(albedo_Cooktor, 1,  albedo.v);
//	glUniform3fv(light_CookTor, 1, lightPositions[0].v);
//	glUniform3fv(light_COLOR_CookTor, 1,  lightColours[0].v);
//	glUniform3fv(light_CookTor1, 1, lightPositions[1].v);
//	glUniform3fv(light_COLOR_CookTor1, 1, lightColours[1].v);
	glUniform3fv(light_CookTor2, 1, lightPositions[2].v);
	glUniform3fv(light_COLOR_CookTor2, 1, lightColours[2].v);
//	glUniform3fv(light_CookTor3, 1, lightPositions[3].v);
//	glUniform3fv(light_COLOR_CookTor3, 1, lightColours[3].v);
	glUniform3fv(camPos_CookTor, 1, cameraPos.v);
	glUniform1f(ao_CookTor, 1.0f);
	glUniform1f(metal_CookTor, 0.6f);
	glUniform1f(rough_CookTor, 0.4f);

	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, seventhModel.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);







	glutSwapBuffers();
}





void display2() {

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	// Cook Torrence Physically based rendering

	glUseProgram(shaderProgramCT);


	mat4 view = identity_mat4();
	mat4 persp_proj = perspective(fov, (float)width / (float)height, 0.1f, 1000.0f);
	view = look_at(cameraPos, cameraPos + cameraFront, cameraUp);

	mat4 modelArray[25];



	//mat4 model1 = identity_mat4();
//	model1 = rotate_y_deg(model1, rotate_y);
//	model1 = translate(model1, vec3(0.0f, 0.0f, 0.0f));

	int matrix_location_CookTor = glGetUniformLocation(shaderProgramCT, "model");
	int view_mat_location_CookTor = glGetUniformLocation(shaderProgramCT, "view");
	int proj_mat_location_CookTor = glGetUniformLocation(shaderProgramCT, "proj");

	int albedo_Cooktor = glGetUniformLocation(shaderProgramCT, "albedo");
	int light_CookTor = glGetUniformLocation(shaderProgramCT, "lightPositions[0]");
	int light_COLOR_CookTor = glGetUniformLocation(shaderProgramCT, "lightColors[0]");
	int light_CookTor1 = glGetUniformLocation(shaderProgramCT, "lightPositions[1]");
	int light_COLOR_CookTor1 = glGetUniformLocation(shaderProgramCT, "lightColors[1]");
	int light_CookTor2 = glGetUniformLocation(shaderProgramCT, "lightPositions[2]");
	int light_COLOR_CookTor2 = glGetUniformLocation(shaderProgramCT, "lightColors[2]");
	int light_CookTor3 = glGetUniformLocation(shaderProgramCT, "lightPositions[3]");
	int light_COLOR_CookTor3 = glGetUniformLocation(shaderProgramCT, "lightColors[3]");
	int camPos_CookTor = glGetUniformLocation(shaderProgramCT, "camPos");
	float ao_CookTor = glGetUniformLocation(shaderProgramCT, "ao");
	float metal_CookTor = glGetUniformLocation(shaderProgramCT, "metallic");
	float rough_CookTor = glGetUniformLocation(shaderProgramCT, "roughness");

	glUniformMatrix4fv(proj_mat_location_CookTor, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location_CookTor, 1, GL_FALSE, view.m);

	vec3 albedo = vec3(1.0f, 1.0f, 0.0f);

	// Add lights to shader
	glUniform3fv(albedo_Cooktor, 1, albedo.v);
	glUniform3fv(light_CookTor, 1, lightPositions[0].v);
	glUniform3fv(light_COLOR_CookTor, 1, lightColours[0].v);
	glUniform3fv(light_CookTor1, 1, lightPositions[1].v);
	glUniform3fv(light_COLOR_CookTor1, 1, lightColours[1].v);
	glUniform3fv(light_CookTor2, 1, lightPositions[2].v);
	glUniform3fv(light_COLOR_CookTor2, 1, lightColours[2].v);
	glUniform3fv(light_CookTor3, 1, lightPositions[3].v);
	glUniform3fv(light_COLOR_CookTor3, 1, lightColours[3].v);
	glUniform3fv(camPos_CookTor, 1, cameraPos.v);
	
	
	// Set variables 
	glUniform1f(ao_CookTor, 1.0f);
//	glUniform1f(metal_CookTor, 0.9f);
	//glUniform1f(rough_CookTor, 0.1f);
//	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model1.m);
//	glBindVertexArray(box_vao);
//	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);

/*	for (int row = 0; row < 5; row++) {


		float metallic = (row / 5);
		glUniform1f(metal_CookTor, metallic);

		for (int column = 0; column < 5; column++) {

			float roughness = (column / 5) + 0.1;

			mat4 model = identity_mat4();
			model = rotate_y_deg(model, rotate_y);
			model = translate(model, vec3((column * 6), (row * 5), 0.0f));
			glUniform1f(rough_CookTor, roughness);
			glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model.m);
			glBindVertexArray(box_vao);
	//		glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);

		}



	}

*/
	int row = 0;
	int column = 0;
	int index = 0;

	float metallic = 0.9;
	float roughness = 0.1;

//	1
	mat4 model1 = identity_mat4();
	model1 = rotate_y_deg(model1, rotate_y);
	model1 = translate(model1, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model1.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.3;
	column = 1;



//2
	mat4 model2 = identity_mat4();
	model2 = rotate_y_deg(model2, rotate_y);
	model2 = translate(model2, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model2.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.5;
	column = 2;




	//3
	mat4 model3 = identity_mat4();
	model3 = rotate_y_deg(model3, rotate_y);
	model3 = translate(model3, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model3.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.7;
	column = 3;
	


	//4
	mat4 model4 = identity_mat4();
	model4 = rotate_y_deg(model4, rotate_y);
	model4 = translate(model4, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model4.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.9;
	column = 4;


	//5
	mat4 model5 = identity_mat4();
	model5 = rotate_y_deg(model5, rotate_y);
	model5 = translate(model5, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model5.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	
	
	column = 0;
	row = 1;
	roughness = 0.1;
	metallic = 0.7;

	//	6
	mat4 model6 = identity_mat4();
	model6 = rotate_y_deg(model6, rotate_y);
	model6 = translate(model6, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model6.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.3;
	column = 1;


	//7
	mat4 model7 = identity_mat4();
	model7 = rotate_y_deg(model7, rotate_y);
	model7 = translate(model7, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model7.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.5;
	column = 2;



	//8
	mat4 model8 = identity_mat4();
	model8 = rotate_y_deg(model8, rotate_y);
	model8 = translate(model8, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model8.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.7;
	column = 3;


	//9
	mat4 model9 = identity_mat4();
	model9 = rotate_y_deg(model9, rotate_y);
	model9 = translate(model9, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model9.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.9;
	column = 4;


	//10
	mat4 model10 = identity_mat4();
	model10 = rotate_y_deg(model10, rotate_y);
	model10 = translate(model10, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model10.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);

	column = 0;
	row = 2;
	roughness = 0.1;
	metallic = 0.5;

	//	11
	mat4 model11 = identity_mat4();
	model11 = rotate_y_deg(model11, rotate_y);
	model11 = translate(model11, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model11.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.3;
	column = 1;


	//12
	mat4 model12 = identity_mat4();
	model12 = rotate_y_deg(model12, rotate_y);
	model12 = translate(model12, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model12.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.5;
	column = 2;



	//13
	mat4 model13 = identity_mat4();
	model13 = rotate_y_deg(model13, rotate_y);
	model13 = translate(model13, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model13.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.7;
	column = 3;


	//14
	mat4 model14 = identity_mat4();
	model14 = rotate_y_deg(model14, rotate_y);
	model14 = translate(model14, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model14.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.9;
	column = 4;


	//15
	mat4 model15 = identity_mat4();
	model15 = rotate_y_deg(model15, rotate_y);
	model15 = translate(model15, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model15.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);

	column = 0;
	row = 3;
	roughness = 0.1;
	metallic = 0.3;


	//	16
	mat4 model16 = identity_mat4();
	model16 = rotate_y_deg(model16, rotate_y);
	model16 = translate(model16, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model16.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.3;
	column = 1;


	//17
	mat4 model17 = identity_mat4();
	model17 = rotate_y_deg(model17, rotate_y);
	model17 = translate(model17, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model17.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.5;
	column = 2;



	//18
	mat4 model18 = identity_mat4();
	model18 = rotate_y_deg(model18, rotate_y);
	model18 = translate(model18, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model18.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.7;
	column = 3;


	//19
	mat4 model19 = identity_mat4();
	model19 = rotate_y_deg(model19, rotate_y);
	model19 = translate(model19, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model19.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.9;
	column = 4;


	//20
	mat4 model20 = identity_mat4();
	model20 = rotate_y_deg(model20, rotate_y);
	model20 = translate(model20, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model20.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);

	column = 0;
	row = 4;
	roughness = 0.1;
	metallic = 0.1;

	//21
	mat4 model21 = identity_mat4();
	model21 = rotate_y_deg(model21, rotate_y);
	model21 = translate(model21, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model21.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.3;
	column = 1;

	//	22
	mat4 model22 = identity_mat4();
	model22 = rotate_y_deg(model22, rotate_y);
	model22 = translate(model22, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model22.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.5;
	column = 2;


	//23
	mat4 model23 = identity_mat4();
	model23 = rotate_y_deg(model23, rotate_y);
	model23 = translate(model23, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model23.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.7;
	column = 3;


	//24
	mat4 model24 = identity_mat4();
	model24 = rotate_y_deg(model24, rotate_y);
	model24 = translate(model24, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model24.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);
	roughness = 0.9;
	column = 4;


	//25
	mat4 model25 = identity_mat4();
	model25 = rotate_y_deg(model25, rotate_y);
	model25 = translate(model25, vec3((column * 6), (row * 5), 0.0f));
	glUniform1f(metal_CookTor, metallic);
	glUniform1f(rough_CookTor, roughness);
	glUniformMatrix4fv(matrix_location_CookTor, 1, GL_FALSE, model25.m);
	glBindVertexArray(box_vao);
	glDrawArrays(GL_TRIANGLES, 0, box.mPointCount);







	glutSwapBuffers();
}



void updateScene() {

	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	float delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;


	deltaTime = delta;
	lastFrame = last_time;

	// Rotate the model slowly around the y axis at 20 degrees per second
	rotate_x += 20.0f * delta;
	rotate_x = fmodf(update_x, 360.0f);

	rotate_y += 20.0f * delta;
	rotate_y = fmodf(rotate_y, 360.0f);

	rotate_z += 20.0f * delta;
	rotate_z = fmodf(update_z, 360.0f);

	// Draw the next frame
	glutPostRedisplay();
}


void init()
{
	// Set up the shaders
	shaderProgramBLUE = CompileShaders("Vertex_Shader_Blue.txt", "Fragment_Shader_Blue.txt");
//	shaderProgramRED = CompileShaders("Vertex_Shader_Red.txt", "Fragment_Shader_Red.txt");
	shaderProgramBP = CompileShaders("Blinn_Phong_Vertex.txt", "Blinn_Phong_Fragment.txt");
	shaderProgramP = CompileShaders("Phong_Vertex.txt", "Phong_Fragment.txt");
	shaderProgramT = CompileShaders("Toon_Vertex.txt", "Toon_Fragment.txt");
	shaderProgramG = CompileShaders("Gooch_Vertex.txt", "Gooch_Fragment.txt");
	shaderProgramCT = CompileShaders("Cook_Torr_Vertex.txt", "Cook_Torr_Fragment.txt");
//	shaderProgramGREEN = CompileShaders("Vertex_GREEN.txt", "Fragment_GREEN.txt");
	box = load_mesh(BOX);
	box_vao = generateObjectBufferMesh(box, shaderProgramBLUE);

	// load mesh into a vertex buffer array
	// cone = load_mesh(CONE);
	// cone_vao = generateObjectBufferMesh(cone, shaderProgramRED);

}


vec3 vecXfloat(float f, vec3 v1) {

	vec3 result = vec3((v1.v[0] * f), (v1.v[1] * f), (v1.v[2] * f));
	return result;


}

// Placeholder code for the keypress
void keypress(unsigned char key, int x, int y) {
	
	float cameraSpeed = 20 * deltaTime;


	if (key == 'w') {
		cameraPos += vecXfloat(cameraSpeed, cameraFront);
	}
	if (key == 's') {
		cameraPos -= vecXfloat(cameraSpeed, cameraFront);
	}
	if (key == 'a') {
		cameraPos -= normalise(cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (key == 'd') {
		cameraPos += normalise(cross(cameraFront, cameraUp)) * cameraSpeed;
	}

}

double radians(double degree) {
	double pi = 3.14159265359;
	return (degree * (pi / 180));
}

void mouseCallback(int x, int y) {


	if (firstMouse) {
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	float xoffset = x - lastX;
	float yoffset = lastY - y;
	lastX = x;
	lastY = y;

	float sensitivity = 0.5f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	vec3 front;
	front.v[0] = cos(radians(yaw)) * cos(radians(pitch));
	front.v[1] = sin(radians(pitch));
	front.v[2] = sin(radians(yaw)) * cos(radians(pitch));
	cameraFront = normalise(front);


}


int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Assignment 1");





	// Tell glut where the display function is
	glutDisplayFunc(display2);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);
	glutPassiveMotionFunc(mouseCallback);
	glutSetCursor(GLUT_CURSOR_NONE);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}