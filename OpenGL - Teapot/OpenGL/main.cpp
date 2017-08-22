// Author: Jorge Ribeiro
// Graphic Computing - 2017.1
// UFMA - Computer Science
// Load a model and applies flat, gourard and phong shading
// Applies also decal, bump and spherical texturing

#include <vector>
#include <map>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "TriangleMesh.h"    // model loading
#include "Shader.h"          // compiling shaders
#include "camera.h"			 // camera setup
#include "utils.h"           // generic helper functions
#include "scene_constants.h" // material and light properties
#include "path_to_files.h"   // paths to textures and shaders

TriangleMesh trig;
Shader shader;

glm::mat4 projectionMatrix, viewMatrix, modelMatrix;
glm::mat3 normalMatrix;

GLuint vertex_position_buffer, vertex_normal_buffer, vertex_uv_buffer;
GLuint textureID;

int useTexture = 0;
char *model_path = model;
char *vertexshader_path = NULL;
char *fragmentshader_path = NULL;
char *texture_path = NULL;
bool use_smoothed_normals = false;

void display_handler(void) {
    // clear scene
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.Bind();

	// pass uniform variables to shader
	GLint projectionMatrix_location    = glGetUniformLocation(shader.ID(), "projectionMatrix");
	GLint viewMatrix_location          = glGetUniformLocation(shader.ID(), "viewMatrix");
	GLint modelMatrix_location         = glGetUniformLocation(shader.ID(), "modelMatrix");
	GLint normalMatrix_location        = glGetUniformLocation(shader.ID(), "normalMatrix");
	GLint materialAmbient_location     = glGetUniformLocation(shader.ID(), "materialAmbient");
	GLint materialDiffuse_location     = glGetUniformLocation(shader.ID(), "materialDiffuse");
	GLint materialSpecular_location    = glGetUniformLocation(shader.ID(), "materialSpecular");
	GLint lightPosition_location       = glGetUniformLocation(shader.ID(), "lightPosition");
	GLint lightAmbient_location        = glGetUniformLocation(shader.ID(), "lightAmbient");
	GLint lightDiffuse_location        = glGetUniformLocation(shader.ID(), "lightDiffuse");
	GLint lightSpecular_location       = glGetUniformLocation(shader.ID(), "lightSpecular");
	GLint lightGlobal_location         = glGetUniformLocation(shader.ID(), "lightGlobal");
	GLint materialShininess_location   = glGetUniformLocation(shader.ID(), "materialShininess");
	GLint constantAttenuation_location = glGetUniformLocation(shader.ID(), "constantAttenuation");
	GLint linearAttenuation_location   = glGetUniformLocation(shader.ID(), "linearAttenuation");
	GLint useTexture_location          = glGetUniformLocation(shader.ID(), "useTexture");
	glUniformMatrix4fv( projectionMatrix_location, 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniformMatrix4fv( viewMatrix_location,       1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv( modelMatrix_location,      1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix3fv( normalMatrix_location,     1, GL_FALSE, &normalMatrix[0][0]);
    glUniform3fv(       materialAmbient_location,  1, materialAmbient);
    glUniform3fv(       materialDiffuse_location,  1, materialDiffuse);
    glUniform3fv(       materialSpecular_location, 1, materialSpecular);
    glUniform3fv(       lightPosition_location,    1, lightPosition);
    glUniform3fv(       lightAmbient_location,     1, lightAmbient);
    glUniform3fv(       lightDiffuse_location,     1, lightDiffuse);
    glUniform3fv(       lightSpecular_location,    1, lightSpecular);
    glUniform3fv(       lightGlobal_location,      1, lightGlobal);
    glUniform1f(        materialShininess_location,   materialShininess);
    glUniform1f(        constantAttenuation_location, constantAttenuation);
    glUniform1f(        linearAttenuation_location,   linearAttenuation);
    glUniform1i(        useTexture_location,          useTexture);

    // bind texture to shader
    GLint texture0_location = glGetAttribLocation(shader.ID(), "texture0");
    if (texture0_location != -1) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(texture0_location, 0);
    }

    // bind vertex uv coordinates to shader
	GLint uv_location = glGetAttribLocation(shader.ID(), "vertex_uv");
	if (uv_location != -1) {
        glEnableVertexAttribArray(uv_location);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_uv_buffer);
        glVertexAttribPointer(uv_location, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    // bind vertex positions to shader
	GLint position_location = glGetAttribLocation(shader.ID(), "vertex_position");
	if (position_location != -1) {
        glEnableVertexAttribArray(position_location);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_position_buffer);
        glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    // bind vertex normals to shader
	GLint normal_location = glGetAttribLocation(shader.ID(), "vertex_normal");
	if (normal_location != -1) {
        glEnableVertexAttribArray(normal_location);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_normal_buffer);
        glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    // draw the scene
	glDrawArrays(GL_TRIANGLES, 0, trig.VertexCount());
	glDisableVertexAttribArray(position_location);
	glDisableVertexAttribArray(uv_location);
	glDisableVertexAttribArray(normal_location);
	shader.Unbind();
	glFlush();
}

glm::mat4 get_default_projectionMatrix(void) {
    return glm::ortho(-windowX * 0.5f,
                       windowX * 0.5f,
                      -windowY * 0.5f,
                       windowY * 0.5f,
                      -1.0f, 400.0f);
}

glm::mat4 get_default_viewMatrix(void) {
	
    return glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -60.0f, -300.0f));
}

glm::mat3 get_default_normalMatrix(void) {
    return glm::transpose(glm::inverse(glm::mat3(viewMatrix * modelMatrix)));
}

glm::mat4 get_default_modelMatrix(void) {
    return glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
}

void keyboard_handler(unsigned char key, int x, int y) {
    glm::vec3 translation = glm::vec3(0, 0, 0);
    glm::vec3 rotation = glm::vec3(0, 0, 0);
    // check which translation or rotation was requested
    switch (key) {
        case 'q': translation = glm::vec3( 1, 0, 0); break;
        case 'w': translation = glm::vec3(-1, 0, 0); break;
        case 'e': translation = glm::vec3( 0, 1, 0); break;
        case 'r': translation = glm::vec3( 0,-1, 0); break;
        case 't': translation = glm::vec3( 0, 0, 1); break;
        case 'y': translation = glm::vec3( 0, 0,-1); break;
        case 'a': rotation = glm::vec3( 1, 0, 0); break;
        case 's': rotation = glm::vec3(-1, 0, 0); break;
        case 'd': rotation = glm::vec3( 0, 1, 0); break;
        case 'f': rotation = glm::vec3( 0,-1, 0); break;
        case 'g': rotation = glm::vec3( 0, 0, 1); break;
        case 'h': rotation = glm::vec3( 0, 0,-1); break;
        case ' ': viewMatrix = get_default_viewMatrix(); break;
        case  27: exit(0);
    }
    // perform the translation or rotation
    if (translation.x != 0 || translation.y != 0 || translation.z != 0) {
        viewMatrix = glm::translate(viewMatrix, translation);
    } else if (rotation.x != 0 || rotation.y != 0 || rotation.z != 0) {
        viewMatrix = glm::rotate(viewMatrix, 1.0f, rotation);
    }
    normalMatrix = get_default_normalMatrix();
    // re-draw the scene
    display_handler();
}

void setup_texture(char *texture_path, GLuint *textureID) {
    if (texture_path == NULL) return;
    unsigned char header[54];
    unsigned int data_pos, width, height, texture_size;
    unsigned char *data;
    // open file
    FILE *file = fopen(texture_path, "rb");
    if (!file) {
        std::cerr << "couldn't open image" << std::endl;
        return;
    }
    // validate header
    if ((fread(header, 1, 54, file) != 54) || (header[0] != 'B' || header[1] != 'M')) {
        std::cerr << "not a valid bmp file" << std::endl;
        return;
    }
    // read integers
    data_pos     = *(int*)&(header[0x0A]);
    texture_size = *(int*)&(header[0x22]);
    width        = *(int*)&(header[0x12]);
    height       = *(int*)&(header[0x16]);
    // set defaults if bmp is misformatted
    if (texture_size == 0) texture_size = width * height * 3;
    if (data_pos == 0)     data_pos = 54;
    // read image data
    data = new unsigned char[texture_size];
    fread(data, 1, texture_size, file);
    fclose(file);
    // convert to OpenGL texture'
    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, *textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    glEnable(GL_TEXTURE_2D);
}

void setup_vertex_position_buffer_object(void) {
	glGenBuffers(1, &vertex_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * trig.VertexCount(),
		         &trig.Vertices()[0], GL_STATIC_DRAW);
}

void setup_vertex_uv_buffer_object(void) {
	glGenBuffers(1, &vertex_uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * trig.UVs().size(),
		         &trig.UVs()[0], GL_STATIC_DRAW);
}

void setup_vertex_normal_buffer_object(bool smoothed) {
    std::vector<glm::vec3> vertices = trig.Vertices();
    std::vector<glm::vec3> normals;
    if (smoothed) {
        // initialize map of normals to zero
        // note that we store readily hashable vector<double> types instead of
        // vec3s and convert between the two as required
        // ...avoids some of the pain using <map> without much C++ knowledge
        std::map< std::vector<double>, std::vector<double> > normal_map;
        for (int i = 0; i < vertices.size(); i++) {
            std::vector<double> zeros;
            zeros.push_back(0.0);
            zeros.push_back(0.0);
            zeros.push_back(0.0);
            normal_map[to_vector(vertices[i])] = zeros;
        }
        for (int i = 0; i < vertices.size(); i += 3) {
            // get vertices of the current triangle
            glm::vec3 v1 = vertices[i];
            glm::vec3 v2 = vertices[i + 1];
            glm::vec3 v3 = vertices[i + 2];
            std::vector<double> v1_key = to_vector(v1);
            std::vector<double> v2_key = to_vector(v2);
            std::vector<double> v3_key = to_vector(v3);
            // compute face normal
            glm::vec3 face_normal = glm::cross(v3 - v2, v1 - v2);
            // get the old vertex normal
            glm::vec3 v1_old = to_vec3(normal_map[v1_key]);
            glm::vec3 v2_old = to_vec3(normal_map[v2_key]);
            glm::vec3 v3_old = to_vec3(normal_map[v3_key]);
            // replace the old value with the new value
            normal_map.erase(v1_key);
            normal_map.erase(v2_key);
            normal_map.erase(v3_key);
            normal_map[v1_key] = to_vector(glm::normalize(v1_old + face_normal));
            normal_map[v2_key] = to_vector(glm::normalize(v2_old + face_normal));
            normal_map[v3_key] = to_vector(glm::normalize(v3_old + face_normal));
        }
        // convert the map of normals to a vector of normals
        for (int i = 0; i < vertices.size(); i++) {
            normals.push_back(to_vec3(normal_map[to_vector(vertices[i])]));
        }
    } else {
        for (int i = 0; i < vertices.size(); i += 3) {
            // get vertices of this triangle
            glm::vec3 v1 = vertices[i];
            glm::vec3 v2 = vertices[i + 1];
            glm::vec3 v3 = vertices[i + 2];
            // compute face normal
            glm::vec3 face_normal = glm::cross(v3 - v2, v1 - v2);
            normals.push_back(glm::normalize(face_normal));
            normals.push_back(glm::normalize(face_normal));
            normals.push_back(glm::normalize(face_normal));
        }
    }
    glGenBuffers(1, &vertex_normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(),
                 &normals[0], GL_STATIC_DRAW);
}

void setup_data() {
	// create shader, prepare data for OpenGL
	shader.Init(vertexshader_path, fragmentshader_path);
	setup_texture(texture_path, &textureID);
	setup_vertex_position_buffer_object();
	setup_vertex_uv_buffer_object();
	setup_vertex_normal_buffer_object(use_smoothed_normals);

	// set up camera and object transformation matrices
	projectionMatrix = get_default_projectionMatrix();
	viewMatrix = get_default_viewMatrix();
	modelMatrix = get_default_modelMatrix();
	normalMatrix = get_default_normalMatrix();
}

void menu1(int id) {
	if (id == 1) { //Flat
		vertexshader_path = simple_shader_v;
		fragmentshader_path = simple_shader_f;
		use_smoothed_normals = false;
		texture_path = NULL;
		useTexture = 0;
	}
	else if (id == 2) { //Gourard
		vertexshader_path = simple_shader_v;
		fragmentshader_path = simple_shader_f;
		use_smoothed_normals = true;
		texture_path = NULL;
		useTexture = 0;
	}
	else if (id == 3) { //Phong
		vertexshader_path = phong_shader_v;
		fragmentshader_path = phong_shader_f;
		use_smoothed_normals = true;
		texture_path = NULL;
		useTexture = 0;
	}
	setup_data();
	glutPostRedisplay();
}

void menu2(int id) {
	if (id == 1) { //Decal
		texture_path = decal;
		useTexture = 1;
	}
	else if (id == 2) { //Bump
		vertexshader_path = bump_map_v;
		fragmentshader_path = bump_map_f;
		use_smoothed_normals = true;
		texture_path = bump_map3;
		useTexture = 1;
	}
	else if (id == 3) { //Spherical
		vertexshader_path = spherical_map_v;
		fragmentshader_path = spherical_map_f;
		use_smoothed_normals = true;
		texture_path = sphere;
		useTexture = 1;
	}
	setup_data();
	glutPostRedisplay();
}

void mainmenu(int id) {
	//Do nothing, just show the menu
}


void setup_menu() {
	int submenu1, submenu2;
	submenu1 = glutCreateMenu(menu1);
	glutAddMenuEntry("Flat", 1);
	glutAddMenuEntry("Gourard", 2);
	glutAddMenuEntry("Phong", 3);
	submenu2 = glutCreateMenu(menu2);
	glutAddMenuEntry("Decal", 1);
	glutAddMenuEntry("Bump", 2);
	glutAddMenuEntry("Spherical", 3);
	glutCreateMenu(mainmenu);
	glutAddSubMenu("Shaders", submenu1);
	glutAddSubMenu("Textures", submenu2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char **argv) {
	// starts with flat shader and no textures
	vertexshader_path = simple_shader_v;
	fragmentshader_path = simple_shader_f;
	texture_path = NULL;
	use_smoothed_normals = false;

	// initialise OpenGL
	glutInit(&argc, argv);
	glutInitWindowSize(windowX, windowY);
	glutCreateWindow("Computação Gráfica - OpenGL");
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);

	// set display and keyboard callbacks and setup menu
	glutDisplayFunc(display_handler);
	glutKeyboardFunc(keyboard_handler);
	setup_menu();

	// initialise the OpenGL Extension Wrangler library for VBOs
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cerr << "Error!" << std::endl;
		exit(1);
	}
	if (!GLEW_VERSION_2_1) {
		std::cerr << "Error 2.1!" << std::endl;
		exit(1);
	}

	// create shader, prepare data for OpenGL
	trig.LoadFile(model_path);
	setup_data();

	glutMainLoop();
	return 0;
}
