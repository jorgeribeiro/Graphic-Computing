//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.

#include "Angel.h"

void init();
void idle(void);
void menu1(int id);
void menu2(int id);
void mainmenu(int id);
void setupMenu();
void display(void);
void keyboard(unsigned char key, int x, int y);
void reshape(int width, int height);

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.5, -0.5,  0.5, 1.0),
	point4(-0.5,  0.5,  0.5, 1.0),
	point4(0.5,  0.5,  0.5, 1.0),
	point4(0.5, -0.5,  0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5,  0.5, -0.5, 1.0),
	point4(0.5,  0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

// RGBA olors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 1.0, 1.0),  // white
	color4(0.0, 1.0, 1.0, 1.0)   // cyan
};

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };

// Viewing transformation parameters

GLfloat radius = 1.0;
GLfloat theta = 0.0;
GLfloat phi = 0.0;;

const GLfloat  dr = 5.0 * DegreesToRadians;

GLuint  model_view;  // model-view matrix uniform shader variable location

// Projection transformation parameters

// Parallel
GLfloat  left = -1.0, right = 1.0;
GLfloat  bottom = -1.0, top = 1.0;

// Perspective
GLfloat  fovy = 30.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio

// Both
GLfloat  zNear = 0.5, zFar = 3.0;

GLuint  projection; // projection matrix uniform shader variable location


//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void quad(int a, int b, int c, int d) {
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void colorcube() {
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------

// OpenGL initialization
void init() {
	colorcube();

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader42.glsl", "fshader42.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------

void idle(void) {
	Theta[Axis] += 0.05;

	if (Theta[Axis] > 360.0) {
		Theta[Axis] -= 360.0;
	}

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void menu1(int id) {
	if (id == 1) { //Perspective
		radius = 4.0;
		theta = 16.5;
		phi = 5.5;
		zNear = 3.05;
		zFar = 18.5;

		point4  eye(radius*sin(theta)*cos(phi),
			radius*sin(theta)*sin(phi),
			radius*cos(theta),
			1.0);
		point4  at(0.0, 0.0, 0.0, 1.0);
		vec4    up(0.0, 1.0, 0.0, 0.0);

		mat4  mv = LookAt(eye, at, up);
		glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

		mat4  p = Perspective(fovy, aspect, zNear, zFar);
		glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	}
	else if (id == 2) { //Parallel Ortographic XY
		radius = 1.0;
		theta = 0.0;
		phi = 0.0;
		zNear = 0.5;
		zFar = 3.0;

		point4  eye(radius*sin(theta)*cos(phi),
			radius*sin(theta)*sin(phi),
			radius*cos(theta),
			1.0);
		point4  at(0.0, 0.0, 0.0, 1.0);
		vec4    up(0.0, 1.0, 0.0, 0.0);

		mat4  mv = LookAt(eye, at, up);
		glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

		mat4  p = Ortho(left, right, bottom, top, zNear, zFar);
		glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	}
	else if (id == 3) { //Parallel Oblique Cavalier
		radius = 2.0;
		theta = 45.0;
		phi = 45.0;
		zNear = 0.5;
		zFar = 3.0;

		point4  eye((cos(phi) / sin(phi)),
			(cos(phi) / sin(phi)), 0.0,	1.0);
		point4  at(0.0, 0.0, 0.0, 1.0);
		vec4    up(0.0, 1.0, 0.0, 0.0);

		mat4  mv = LookAt(eye, at, up);
		glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

		mat4  p = Ortho(left, right, bottom, top, zNear, zFar);
		glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	}

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void menu2(int id) {
	switch (id) {
		case 1:
			Axis = Xaxis; //Roll
			break;
		case 2:
			Axis = Yaxis; //Yaw
			break;
		case 3:
			Axis = Zaxis; //Pitch
			break;
	}
	glutIdleFunc(idle);
}

//----------------------------------------------------------------------------

void mainmenu(int id) {
	//Do nothing, just show the menu
}

//----------------------------------------------------------------------------

void setupMenu() {
	int submenu1, submenu2;
	submenu1 = glutCreateMenu(menu1);
	glutAddMenuEntry("Perspective", 1);
	glutAddMenuEntry("Parallel Ortographic XY", 2);
	glutAddMenuEntry("Parallel Oblique Cavalier", 3);
	submenu2 = glutCreateMenu(menu2);
	glutAddMenuEntry("Roll", 1);
	glutAddMenuEntry("Yaw", 2);
	glutAddMenuEntry("Pitch", 3);
	glutCreateMenu(mainmenu);
	glutAddSubMenu("Views", submenu1);
	glutAddSubMenu("Transformations", submenu2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//----------------------------------------------------------------------------

void reshape(int width, int height) {
	glViewport(0, 0, width, height);

	aspect = GLfloat(width) / height;
}

//----------------------------------------------------------------------------

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Rotate setup
	mat4  transform = (RotateX(Theta[Xaxis]) *
		RotateY(Theta[Yaxis]) *
		RotateZ(Theta[Zaxis]));

	point4  transformed_points[NumVertices];

	for (int i = 0; i < NumVertices; ++i) {
		transformed_points[i] = transform * points[i];
	}

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(transformed_points),
		transformed_points);

	//Viewing setup
	point4  eye(radius*sin(theta)*cos(phi),
		radius*sin(theta)*sin(phi),
		radius*cos(theta),
		1.0);
	point4  at(0.0, 0.0, 0.0, 1.0);
	vec4    up(0.0, 1.0, 0.0, 0.0);

	mat4  mv = LookAt(eye, at, up);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	mat4  p = Ortho(left, right, bottom, top, zNear, zFar);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;

	case 'x': left *= 1.1; right *= 1.1; break;
	case 'X': left *= 0.9; right *= 0.9; break;
	case 'y': bottom *= 1.1; top *= 1.1; break;
	case 'Y': bottom *= 0.9; top *= 0.9; break;
	case 'z': zNear *= 1.1; zFar *= 1.1; break;
	case 'Z': zNear *= 0.9; zFar *= 0.9; break;
	case 'r': radius *= 2.0; break;
	case 'R': radius *= 0.5; break;
	case 'o': theta += dr; break;
	case 'O': theta -= dr; break;
	case 'p': phi += dr; break;
	case 'P': phi -= dr; break;

	case ' ':  // reset values to their defaults
		left = -1.0;
		right = 1.0;
		bottom = -1.0;
		top = 1.0;
		zNear = 0.5;
		zFar = 3.0;

		radius = 1.0;
		theta = 0.0;
		phi = 0.0;
		break;
	}

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Color Cube");

	glewExperimental = GL_TRUE;
	glewInit();

	init();
	setupMenu();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;
}
