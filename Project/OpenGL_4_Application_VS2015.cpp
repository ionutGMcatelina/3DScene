//
//  main.cpp
//  OpenGL Shadows
//
//  Created by CGIS on 05/12/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model3D.hpp"
#include "Mesh.hpp"

int glWindowWidth = 640;
int glWindowHeight = 480;
int width;
int height;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat3 lightDirMatrix;
GLuint lightDirMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

glm::vec3 coneDirection;
GLuint coneDirectionLoc;
glm::vec3 spotPos;
GLuint spotPosLoc;

gps::Camera myCamera(glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f));
GLfloat cameraSpeed = 0.5f;

bool pressedKeys[1024];
GLfloat aim;
GLfloat aimAngle;
GLfloat aimY;
GLfloat bulletFly;

GLfloat angle;
GLfloat myAngle;
GLfloat planeAngle;
GLfloat distHelicopterX = 0.0f;
GLfloat distHelicopterY = 0.0f;
GLfloat distHelicopterZ = 0.0f;
GLfloat lightAngle;
GLfloat dist;
GLfloat newDirection;
GLfloat newZ;
GLfloat newX;
GLfloat carMovement;
GLfloat carSpeed = 1.0f;
GLfloat carMovement2;

GLfloat ballLaunchZ;
GLfloat ballLaunchY;
GLfloat moveHelicopter;
GLfloat carAngle;

GLfloat v[1000];
GLfloat w[1000];
GLboolean spotlightOn;
GLuint spotlightOnLoc;

gps::Model3D myModel;
gps::Model3D soldier;
gps::Model3D soldier2;
gps::Model3D nava;
gps::Model3D tank;
gps::Model3D tank2;
gps::Model3D tank3;
gps::Model3D tankAdversar;
gps::Model3D panzer;
gps::Model3D humvee;
gps::Model3D robot;
gps::Model3D launcher;
gps::Model3D camp;
gps::Model3D bullet;
gps::Model3D trash;
gps::Model3D cannon;
gps::Model3D ball;
gps::Model3D rustedCar;
gps::Model3D car;
gps::Model3D blackOps2Tank;

gps::Model3D road;
gps::Model3D building1;
gps::Model3D building2;
gps::Model3D building3;
gps::Model3D building4;
gps::Model3D tree2;

gps::Model3D helicopter;
gps::Model3D plane;
gps::Model3D elice_mare;
gps::Model3D elice_mica;
gps::Model3D lamp;

gps::Model3D ground;
gps::Model3D tree;
gps::Model3D lightCube;
gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader lampShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

std::vector<const GLchar*> faces;
glm::mat4 specialView;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

glm::vec3 AkPosition;
glm::vec3 AkDirection;

float lastX = 960, lastY = 540;
float yaw = -90.0f, pitch = 0.0f;
bool firstMouse = true;
bool insideTruck = false;
bool forward = false;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	myCustomShader.useShaderProgram();

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.01f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	
	lightShader.useShaderProgram();
	
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}

	if (insideTruck) {
		myCamera.rotate(pitch, yaw);
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
		firstMouse = true;
	}
	else if (!insideTruck) {
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		std::cout << myCamera.getPosition().x << " " << myCamera.getPosition().y << " " << myCamera.getPosition().z << std::endl;

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.05f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;
		myCamera.rotate(pitch, yaw);
	}
}

void updateSpot() {
	spotPos = glm::vec3(0.0f - distHelicopterX * 1.2, 40.0f + 2 * distHelicopterY * 1.2, 0.0f + distHelicopterZ * 1.2);
	spotPosLoc = glGetUniformLocation(myCustomShader.shaderProgram, "spotPos");
	myCustomShader.useShaderProgram();
	glUniform3fv(spotPosLoc, 1, glm::value_ptr(spotPos));
}

bool flag = false;
void fly() {
	if (flag) {
		if (bulletFly < 100) {
			bulletFly += 17;
		}
		else {
			bulletFly -= 100.0f;
			flag = false;
		}
	}
}
void collisionWithBildings() {
	for (int i = 0; i < 4; i++) {
		if (myCamera.getPosition().x <= -10.2f && myCamera.getPosition().x >= -20.2f &&
			myCamera.getPosition().z <= -39.0f + i * 35 && myCamera.getPosition().z >= -61.5f + i * 35 &&
			myCamera.getPosition().y <= 15.0f
			) {
			if (myCamera.getPosition().x >= -20.2f && myCamera.getPosition().x < -19.6f)
				myCamera.changeX(-20.2);
			else if (myCamera.getPosition().x <= -10.2f && myCamera.getPosition().x > -10.9f)
				myCamera.changeX(-10.2);

			if (myCamera.getPosition().z <= -39.0f + i * 35 && myCamera.getPosition().z > -39.6f + i * 35)
				myCamera.changeZ(-39.0 + i * 35);
			else if (myCamera.getPosition().z >= -61.5f + i * 35 && myCamera.getPosition().z < -55.0f + i * 35)
				myCamera.changeZ(-61.5 + i * 35);

			if (myCamera.getPosition().y < 15.0f && myCamera.getPosition().y > 14.5) {
				myCamera.ascend(15.0f);
			}
		}
	}

	if (myCamera.getPosition().y < -0.2f) {
		myCamera.ascend(-0.2f);
	}
}

GLfloat showZ = -100;
GLfloat showY = 70;
bool show;
void moveCamera() {
	glm::vec3 newDir = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(showZ), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(glm::vec3(1.0f, -0.9f, 0.0f), 1.0f));
	myCamera.setDirection(glm::normalize(newDir));
	//myCamera.move(gps::MOVE_RIGHT, cameraSpeed * 4.0f);
	myCamera.changeX(-80.0f);
	myCamera.changeZ(showZ);
	myCamera.ascend(showY);
}

void processMovement()
{
	if (pressedKeys[GLFW_KEY_Z]) {
		flag = true;
	}

	if (pressedKeys[GLFW_KEY_B]) {
		if (aim < 0.2) {
			aim += 0.01;
			aimAngle += 0.5;
			aimY += 0.002;
		}
	}
	else {
		if (aim > 0.0f) {
			aim -= 0.01;
			aimAngle -= 0.5;
			aimY -= 0.002;
		}
	}

	if (pressedKeys[GLFW_KEY_F]) {
		myCamera.changeX(6.14371);
		myCamera.ascend(0.872011);
		myCamera.changeZ(20.7821);
		myCamera.setDirection(glm::vec3(0.0f, 0.0f, -1.0f));
		insideTruck = true;
	}

	if (pressedKeys[GLFW_KEY_Q]) {
		angle += 1.0f;
		if (angle > 360.0f)
			angle -= 360.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle -= 1.0f;
		if (angle < 0.0f)
			angle += 360.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) { 
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		forward = true;
		if (insideTruck)
			myCamera.ascend(0.872011);
	}
	else if (pressedKeys[GLFW_KEY_S]) {
		forward = false;
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
		if (insideTruck)
			myCamera.ascend(0.872011);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		if (!insideTruck) {
			myCamera.move(gps::MOVE_LEFT, cameraSpeed);
		}
		else if (forward){
			carAngle += 0.1f;
			glm::vec3 newDir = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(myCamera.getDirection(), 1.0f));
			myCamera.setDirection(glm::normalize(newDir));
		}
	}

	if (pressedKeys[GLFW_KEY_D]) {
		if (!insideTruck) {
			myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
		}
		else if (forward){
			carAngle -= 0.1f;
			glm::vec3 newDir = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(myCamera.getDirection(), 1.0f));
			myCamera.setDirection(glm::normalize(newDir));
		}
	}

	if (pressedKeys[GLFW_KEY_I]) {
		distHelicopterZ -= 0.1f;
		updateSpot();
	}

	if (pressedKeys[GLFW_KEY_K]) {
		distHelicopterZ += 0.1f;
		updateSpot();
	}

	if (pressedKeys[GLFW_KEY_J]) {
		distHelicopterX -= 0.1f;
		updateSpot();
	}

	if (pressedKeys[GLFW_KEY_L]) {
		distHelicopterX += 0.1f;
		updateSpot();
	}

	if (pressedKeys[GLFW_KEY_U]) {
		distHelicopterY -= 0.1f;
		updateSpot();
	}

	if (pressedKeys[GLFW_KEY_O]) {
		distHelicopterY += 0.1f;
		updateSpot();
	}

	if (pressedKeys[GLFW_KEY_UP]) {
		dist += 0.1f;
	}

	if (pressedKeys[GLFW_KEY_DOWN]) {
		dist -= 0.1f;
	}

	if (pressedKeys[GLFW_KEY_LEFT]) {
		newDirection += 0.1f;
		dist = 0;
	}

	if (pressedKeys[GLFW_KEY_RIGHT]) {
		newDirection -= 0.1f;
		dist = 0;
	}

	if (pressedKeys[GLFW_KEY_N]) {

		lightAngle += 0.3f;
		if (lightAngle > 360.0f)
			lightAngle -= 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	if (pressedKeys[GLFW_KEY_M]) {
		lightAngle -= 0.3f; 
		if (lightAngle < 0.0f)
			lightAngle += 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}	

	if (glfwGetKey(glWindow, GLFW_KEY_1)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (glfwGetKey(glWindow, GLFW_KEY_2)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (glfwGetKey(glWindow, GLFW_KEY_4)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	if (glfwGetKey(glWindow, GLFW_KEY_5)) {
		spotlightOn = false;
		spotlightOnLoc = glGetUniformLocation(myCustomShader.shaderProgram, "spotlightOn");
		myCustomShader.useShaderProgram();
		glUniform1i(spotlightOnLoc, false);
	}

	if (glfwGetKey(glWindow, GLFW_KEY_6)) {
		spotlightOn = true;
		spotlightOnLoc = glGetUniformLocation(myCustomShader.shaderProgram, "spotlightOn");
		myCustomShader.useShaderProgram();
		glUniform1i(spotlightOnLoc, true);
	}

	if (pressedKeys[GLFW_KEY_C]) {
		show = true;
	}

	if (pressedKeys[GLFW_KEY_V]) {
		show = false;
		showZ = -100;
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initFBOs()
{
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix()
{
	const GLfloat near_plane = 1.0f, far_plane = 600.0f;
	glm::mat4 lightProjection = glm::ortho(-1200.0f, 1200.0f, -1200.0f, 1200.0f, near_plane, far_plane);

	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::mat4 lightView = glm::lookAt(myCamera.getCameraTarget() + 1.0f * lightDirTr, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;
}

void initModels()
{
	myModel = gps::Model3D("objects/AK47/scenes/AK47.obj", "objects/AK47/scenes/");
	nava = gps::Model3D("objects/Si-Fi_Freigther/Obj/Freigther_BI_Export.obj", "objects/Si-Fi_Freigther/Obj/");
	tank = gps::Model3D("objects/Tank/tank.obj", "objects/Tank/");
	tank2 = gps::Model3D("objects/Tank2/tank2.obj", "objects/Tank2/");
	tank3 = gps::Model3D("objects/Tank3/tank3.obj", "objects/Tank3/");
	humvee = gps::Model3D("objects/Humvee/truck2.obj", "objects/Humvee/");
	tankAdversar = gps::Model3D("objects/TankAdversar/SU-122-44.obj", "objects/TankAdversar/");
	panzer = gps::Model3D("objects/panzer/panzer.obj", "objects/panzer/");
	robot = gps::Model3D("objects/robot2/Robo_warrior.obj", "objects/robot2/");
	launcher = gps::Model3D("objects/lansator/launcher.obj", "objects/lansator/");
	camp = gps::Model3D("objects/camp/camp.obj", "objects/camp/");
	soldier = gps::Model3D("objects/soldier/soldier.obj", "objects/soldier/");
	soldier2 = gps::Model3D("objects/soldier2/soldier2.obj", "objects/soldier2/");
	cannon = gps::Model3D("objects/cannon/cannon.obj", "objects/cannon/");
	ball = gps::Model3D("objects/ball/ball.obj", "objects/ball/");
	blackOps2Tank = gps::Model3D("objects/BlackOps2KravchenkoTank/Tank2.obj", "objects/BlackOps2KravchenkoTank/");

	plane = gps::Model3D("objects/plane/plane.obj", "objects/plane/");
	helicopter = gps::Model3D("objects/helicopter/helicopter.obj", "objects/helicopter/");
	elice_mica = gps::Model3D("objects/elice_mica/elice_mica.obj", "objects/elice_mica/");
	elice_mare = gps::Model3D("objects/elice_mare/elice_mare.obj", "objects/elice_mare/");
	bullet = gps::Model3D("objects/bullet/bullet.obj", "objects/bullet/");
	rustedCar = gps::Model3D("objects/car/Rusted.obj", "objects/car/");
	car = gps::Model3D("objects/car/car.obj", "objects/car/");

	building1 = gps::Model3D("objects/old_building/old_building.obj", "objects/old_building/");
	building2 = gps::Model3D("objects/deteriorated_buiding_v1/deteriorated_buiding_version_1.obj", "objects/deteriorated_buiding_v1/");
	trash = gps::Model3D("objects/trash/item01.obj", "objects/trash/");
	tree2 = gps::Model3D("objects/tree2/Broad_Leaf_Straight_Trunk.obj", "objects/tree2/");

	ground = gps::Model3D("objects/myGround/myGround.obj", "objects/myGround/");
	tree = gps::Model3D("objects/Tree/tree2.obj", "objects/Tree/");
	road = gps::Model3D("objects/road/myRoad.obj", "objects/road/");
	lightCube = gps::Model3D("objects/sun/13913_Sun_v2_l3.obj", "objects/sun/");
	lamp = gps::Model3D("objects/cube/cube.obj", "objects/cube/");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lampShader.loadShader("shaders/lamp.vert", "shaders/lamp.frag");
	depthMapShader.loadShader("shaders/simpleDepthMap.vert", "shaders/simpleDepthMap.frag");
}

void initUniforms()
{
	myCustomShader.useShaderProgram();

	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	
	lightDirMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix");

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 150.0f, 300.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));



	// spotlight direction
	//coneDirection = glm::vec3(distHelicopterX * 1.2, -1.0f, distHelicopterZ * 1.2);
	coneDirection = glm::vec3(0.0f, -1.0f, 0.0f);
	coneDirectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "ConeDirection");
	glUniform3fv(coneDirectionLoc, 1, glm::value_ptr(coneDirection));

	spotPos = glm::vec3(0.0f, 40.0f, 0.0f);
	spotPosLoc = glGetUniformLocation(myCustomShader.shaderProgram, "spotPos");
	glUniform3fv(spotPosLoc, 1, glm::value_ptr(spotPos));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//spotCosCutoffLoc = glGetUniformLocation(myCustomShader.shaderProgram, "SpotCosCutoff");
	//glUniform3fv(spotCosCutoffLoc, 1, spotCosCutoff);

	//spotExponentLoc = glGetUniformLocation(myCustomShader.shaderProgram, "SpotExponent");
	//glUniform3fv(spotExponentLoc, 1, glm::value_ptr(spotExponent));

	mySkyBox.Load(faces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
	view = myCamera.getViewMatrix(); 
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f); 
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

bool up = false;
bool left = true;
bool rotate = true;
bool upHelicopter = true;


void updateCar() {
	if (!rotate) {
		if (left) {
			carMovement += carSpeed;
		}
		else {
			carMovement -= carSpeed;
		}
	}

	if (carMovement > 100.0f) {
		left = false;
		rotate = true;
	}
	else if (carMovement < 0.0f) {
		left = true;
		rotate = true;
	}

	if (rotate) {
		carMovement2 += 5;
		if (carMovement2 == 180 || carMovement2 == 360) {
			rotate = false;
		}
	}

	if (carMovement2 == 360.0f)
		carMovement2 = 0.0f;
}


void renderScene()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	processMovement();	
	if (show) {
		moveCamera();
		if (showZ < 100)
			showZ += 0.5f;
	}

	//render the scene to the depth buffer (first pass)

	depthMapShader.useShaderProgram();

	//if (lightAngle < 90 || lightAngle > 270) {
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	//}
	glClear(GL_DEPTH_BUFFER_BIT);

	//create model matrix for TANK3

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -1.0f, -4.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		tank3.Draw(depthMapShader);

		//create model matrix for TANK
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -1.0f, -11.5f));
		//send model matrix data to shader	
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		tank.Draw(depthMapShader);


		//create model matrix for TANK2
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, -1.0f, -13.5f));
		//send model matrix data to shader	
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		tank2.Draw(depthMapShader);


		//create model matrix for TANK3
		model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, -1.0f, -4.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		tank3.Draw(depthMapShader);


		//create model matrix for LAUNCHER

		model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.8f, -50.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
		//send model matrix to shader
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		launcher.Draw(depthMapShader);


		//create model matrix for TANK ADVERSAR
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 4.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		tankAdversar.Draw(depthMapShader);


		// BLACK OPS TANK

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 2.2f, 40.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader
		model = glm::scale(model, glm::vec3(0.0004f, 0.0004f, 0.0004f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		blackOps2Tank.Draw(depthMapShader);


		//create model matrix for PANZER
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, -1.0f, 11.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		//send model matrix to shader
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		tankAdversar.Draw(depthMapShader);


		//create model matrix for HUMVEE

		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 2.5f, 20.0f));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
		//send model matrix to shader
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		humvee.Draw(depthMapShader);


		//create model matrix for CAR

		model = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, -1.0f, 80.0f - carMovement));
		model = glm::rotate(model, glm::radians(-180 - carMovement2), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		car.Draw(depthMapShader);


		//create model matrix for ROBOT

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, -1.1f, 3.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		//send model matrix to shader
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		robot.Draw(depthMapShader);


		//create model matrix for PLANE

		model = glm::rotate(glm::mat4(1.0f), glm::radians(planeAngle), glm::vec3(0, 1, 0));
		model = glm::translate(model, glm::vec3(25.0f, 40.0f, -5.9f));
		model = glm::rotate(model, glm::radians(200.0f), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(-25.0f), glm::vec3(0, 0, 1));

		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		//send model matrix to shader
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		plane.Draw(depthMapShader);


		//create model matrix for HELICOPTER

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f - distHelicopterX, 15.0f + distHelicopterY, 0.0f - distHelicopterZ));
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
		//send model matrix to shader
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		helicopter.Draw(depthMapShader);


		//create model matrix for ELICE_MICA

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.38f - distHelicopterX, 17.9f + distHelicopterY, -8.05f - distHelicopterZ));
		model = glm::rotate(model, glm::radians(myAngle), glm::vec3(1, 0, 0));
		model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
		//send model matrix to shader
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		elice_mica.Draw(depthMapShader);


		//create model matrix for ELICE_MARE

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f - distHelicopterX, 15.0f + distHelicopterY, 0.0f - distHelicopterZ));
		model = glm::rotate(model, glm::radians(-myAngle), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
		//send model matrix to shader
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		elice_mare.Draw(depthMapShader);


		//create model matrix for BUILDING1

		for (int i = 0; i < 4; i++) {
			model = glm::translate(glm::mat4(1.0f), glm::vec3(-15.0f, 6.7f, -50.0f + i * 35));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
			//send model matrix data to shader	
			model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
			glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
				1,
				GL_FALSE,
				glm::value_ptr(model));
			building1.Draw(depthMapShader);
		}


		//create model matrix for BUILDING2

		for (int i = 0; i < 5; i++) {
			model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, -1.0f, -30.0f + i * 20));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
			//send model matrix data to shader	
			model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f));
			//send model matrix to shader
			glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
				1,
				GL_FALSE,
				glm::value_ptr(model));
			building2.Draw(depthMapShader);
		}

		//create model matrix for CANNONS
		model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, -0.5f, -80.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		cannon.Draw(depthMapShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, -0.5f, -80.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		cannon.Draw(depthMapShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, -0.5f, 80.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		cannon.Draw(depthMapShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-7.0f, -0.5f, 80.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		cannon.Draw(depthMapShader);


		//create model matrix for BALLS
		model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.95f + ballLaunchY, -78.0f + ballLaunchZ));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		ball.Draw(depthMapShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, 0.95f + ballLaunchY, -78.0f + ballLaunchZ));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		ball.Draw(depthMapShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.95f + ballLaunchY, 78.0f - ballLaunchZ));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));		
		ball.Draw(depthMapShader);
		
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-7.0f, 0.95f + ballLaunchY, 78.0f - ballLaunchZ));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		ball.Draw(depthMapShader);



		//create model matrix for CAMP
		model = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, -2.2f, -150.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		camp.Draw(depthMapShader);


		model = glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, -2.2f, 150.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		camp.Draw(depthMapShader);

		//create model matrix for  TREE2
		model = glm::translate(glm::mat4(1.0f), glm::vec3(25.0f, -1.0f, 0.0f));		// 1
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		tree2.Draw(depthMapShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(27.0f, -1.0f, 50.0f));		// 2
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		tree2.Draw(depthMapShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-25.0f, -1.0f, 1.0f));		// 3
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		tree2.Draw(depthMapShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-28.0f, -1.0f, 45.0f));		// 4
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		tree2.Draw(depthMapShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, -1.0f, 100.0f));		// 5
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		tree2.Draw(depthMapShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-28.0f, -1.0f, 110.0f));		// 6
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		tree2.Draw(depthMapShader);


		//create model matrix for TREES
		model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, -1.0f, 0.0f));
		//send model matrix data to shader	
		//model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		tree.Draw(depthMapShader);


		//create model matrix for TRASH
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-15.0f, -1.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		trash.Draw(depthMapShader);


		//create model matrix for RUSTED CAR
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-35.0f, -1.0f, -5.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		rustedCar.Draw(depthMapShader);


		//glDisable(GL_BACK);
		//create model matrix for ground
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				model = glm::translate(glm::mat4(1.0f), glm::vec3(-500.0f + i * 138, -1.0f, -500.0f + j * 141.3f));
				model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1, 0, 0));
				//send model matrix data to shader
				glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
					1,
					GL_FALSE,
					glm::value_ptr(model));
				ground.Draw(depthMapShader);
			}
		}
		//glEnable(GL_BACK);

		for (int i = 0; i < 500; i++) {
			model = glm::translate(glm::mat4(1.0f), glm::vec3(v[i], -1.0f, w[i]));
			//send model matrix data to shader	
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
				1,
				GL_FALSE,
				glm::value_ptr(model));
			tree.Draw(depthMapShader);
		}


		//create model matrix for SOLDIER
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 5; j++) {
				model = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f + i * 2, -1.0f, -120.0f + j * 2));
				//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
				//send model matrix data to shader	
				model = glm::scale(model, glm::vec3(0.025f, 0.025f, 0.025f));
				glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
					1,
					GL_FALSE,
					glm::value_ptr(model));
				soldier.Draw(depthMapShader);
			}
		}
	//}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//render the scene (second pass)

	myCustomShader.useShaderProgram();

	//send lightSpace matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	//send view matrix to shader
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "view"),
		1,
		GL_FALSE,
		glm::value_ptr(view));

	//compute light direction transformation matrix
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	//send lightDir matrix data to shader
	glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

	glViewport(0, 0, retina_width, retina_height);
	myCustomShader.useShaderProgram();



	//bind the depth map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);
	

	//////////////////////////////////////////////////////////////////////// AK47
	collisionWithBildings();

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f - aim, -0.3f + aimY, 2.0f));
	model = glm::rotate(model, glm::radians(80.0f + aimAngle), glm::vec3(0, 1, 0));
	
	model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//myModel.Draw(myCustomShader);

	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "view"),
		1,
		GL_FALSE,
		glm::value_ptr(specialView));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));

	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	myModel.Draw(myCustomShader);

	fly();

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.73f - aim * 3.7, -0.23f + aimY, 0.0f - bulletFly));
	model = glm::rotate(model, glm::radians(80.0f + aimAngle), glm::vec3(0, 1, 0));

	model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	if (flag) {
		bullet.Draw(myCustomShader);
	}

	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "view"),
		1,
		GL_FALSE,
		glm::value_ptr(view));


	// TANK3

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -1.0f, -4.5f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	//model = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	tank3.Draw(myCustomShader);


	// TANK
	
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -1.0f, -11.5f));
	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	tank.Draw(myCustomShader);


	// TANK2

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -1.0f, -16.5f));
	//send model matrix data to shader	
	//model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	tank2.Draw(myCustomShader);


	// TANK3

	model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, -1.0f, -4.5f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	//model = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	tank3.Draw(myCustomShader);


	// LAUNCHER

	model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.8f, -55.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	launcher.Draw(myCustomShader);


	// TANK ADVERSAR

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 6.5f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader
	//model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	tankAdversar.Draw(myCustomShader);


	// BLACK OPS TANK

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 2.2f, 40.5f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader
	model = glm::scale(model, glm::vec3(0.0004f, 0.0004f, 0.0004f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	blackOps2Tank.Draw(myCustomShader);


	// PANZER

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, -1.0f, 15.5f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	panzer.Draw(myCustomShader);


	// RUSTED CAR

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-35.0f, -1.0f, -5.5f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	rustedCar.Draw(myCustomShader);


	// CAR

	model = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, -1.0f, 80.0f - carMovement));
	model = glm::rotate(model, glm::radians(-180.0f - carMovement2), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	car.Draw(myCustomShader);

	updateCar();


	// HUMVEE

	newZ = glm::cos(newDirection + 90.0f) * dist;
	newX = glm::sin(newDirection + 90.0f) * dist;

	if (insideTruck) {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.1f + myCamera.getPosition().x, 1.65 + myCamera.getPosition().y, 4.2f + myCamera.getPosition().z));
		model = glm::rotate(model, glm::radians(carAngle), glm::vec3(0, 1, 0));
		//glm::vec3 newDir = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(carAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(myCamera.getDirection(), 1.0f));
		//myCamera.setDirection(newDir);
	}
	else {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 2.5f, 25.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	}
	
	//send model matrix data to shader
	model = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	humvee.Draw(myCustomShader);


	// ROBOT

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, -1.1f, 3.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	robot.Draw(myCustomShader);


	// PLANE

	model = glm::rotate(glm::mat4(1.0f), glm::radians(planeAngle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(25.0f, 40.0f, -5.9f));
	model = glm::rotate(model, glm::radians(200.0f), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(-25.0f), glm::vec3(0, 0, 1));

	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	plane.Draw(myCustomShader);
	planeAngle += 1;

	// HELICOPTER

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f - distHelicopterX, 15.0f + distHelicopterY + moveHelicopter, 0.0f - distHelicopterZ));
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	helicopter.Draw(myCustomShader);


	// ELICE_MICA

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.38f - distHelicopterX, 17.9f + distHelicopterY + moveHelicopter, -8.05f - distHelicopterZ));
	model = glm::rotate(model, glm::radians(myAngle), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	elice_mica.Draw(myCustomShader);


	// ELICE_MARE

	
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f - distHelicopterX, 15.0f + distHelicopterY + moveHelicopter, 0.0f - distHelicopterZ));
	model = glm::rotate(model, glm::radians(-myAngle), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	elice_mare.Draw(myCustomShader);

	if (myAngle > 360) {
		myAngle -= 360;
	}
	myAngle += 25;

	if (upHelicopter && moveHelicopter < 1.0f) {
		moveHelicopter += 0.025f;
		if (moveHelicopter >= 1.0) {
			upHelicopter = false;
		}
	}
	else if (!upHelicopter && moveHelicopter > 0.0f) {
		moveHelicopter -= 0.025f;
		if (moveHelicopter <= 0.0) {
			upHelicopter = true;
		}
	}


	// BUILDING1

	for (int i = 0; i < 4; i++) {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-15.0f, 6.7f, -50.0f + i * 35));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		building1.Draw(myCustomShader);
	}


	// BUILDING2

	glDisable(GL_BACK);
	for (int i = 0; i < 5; i++) {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, -1.0f, -30.0f + i * 20));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		building2.Draw(myCustomShader);
	}
	glEnable(GL_BACK);

	// TREE2
	model = glm::translate(glm::mat4(1.0f), glm::vec3(25.0f, -1.0f, 0.0f));		// 1
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	tree2.Draw(myCustomShader);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(27.0f, -1.0f, 50.0f));		// 2
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	tree2.Draw(myCustomShader);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-25.0f, -1.0f, 1.0f));		// 3
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	tree2.Draw(myCustomShader);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-28.0f, -1.0f, 45.0f));		// 4
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	tree2.Draw(myCustomShader);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, -1.0f, 100.0f));		// 5
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	tree2.Draw(myCustomShader);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-28.0f, -1.0f, 110.0f));		// 6
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	tree2.Draw(myCustomShader);



	// CANNONS
	model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, -0.5f, -80.0f));
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	cannon.Draw(myCustomShader);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, -0.5f, -80.0f));
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	cannon.Draw(myCustomShader);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, -0.5f, 80.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	cannon.Draw(myCustomShader);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-7.0f, -0.5f, 80.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	cannon.Draw(myCustomShader);


	// BALLS
	model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.95f + ballLaunchY, -78.0f + ballLaunchZ));
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	ball.Draw(myCustomShader);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, 0.95f + ballLaunchY, -78.0f + ballLaunchZ));
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	ball.Draw(myCustomShader);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.95f + ballLaunchY, 78.0f - ballLaunchZ));
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	ball.Draw(myCustomShader);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-7.0f, 0.95f + ballLaunchY, 78.0f - ballLaunchZ));
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	ball.Draw(myCustomShader);


	if (ballLaunchY >= 5.0f) {
		up = true;
	}
	else if (ballLaunchY <= 0.0f) {
		up = false;
	}

	if (!up) {
		ballLaunchY += 0.1f;
	}
	else {
		ballLaunchY -= 0.1f;
	}

	if (ballLaunchY > 0) {
		ballLaunchZ += 1.0;
	}
	else {
		ballLaunchZ = 0.0f;
	}


	// TRASH
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-15.0f, -1.0f, 0.0f));
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	trash.Draw(myCustomShader);


	
	// CAMP
	model = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, -2.2f, -150.0f));
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDisable(GL_BACK);
	camp.Draw(myCustomShader);
	glEnable(GL_BACK);

	// CAMP
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, -2.2f, 150.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDisable(GL_BACK);
	camp.Draw(myCustomShader);
	glEnable(GL_BACK);


	// SOLDIER
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 5; j++) {
			model = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f + i * 2, -1.0f, -120.0f + j * 2));
			//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
			//send model matrix data to shader	
			model = glm::scale(model, glm::vec3(0.025f, 0.025f, 0.025f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			soldier.Draw(myCustomShader);
		}
	}


	// SOLDIER
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 5; j++) {
			model = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f + i * 2, -1.0f, 120.0f - j * 2));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
			//send model matrix data to shader	
			model = glm::scale(model, glm::vec3(0.027f, 0.027f, 0.027f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			soldier2.Draw(myCustomShader);
		}
	}


	// ROAD
	for (int i = 0; i < 3302; i += 330) {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, -0.89f, -(float)i / 10 + 3302/20));
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		road.Draw(myCustomShader);
	}

	
	//create model matrix for ground
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			model = glm::translate(glm::mat4(1.0f), glm::vec3(-500.0f + i * 138, -1.0f, -500.0f + j * 141.3f));
			//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
			//send model matrix data to shader
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			//create normal matrix
			normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
			//send normal matrix data to shader
			glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

			glDisable(GL_BACK);
			ground.Draw(myCustomShader);
			glEnable(GL_BACK);
		}
	}

	// TREES
	model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, -1.0f, 0.0f));
	//send model matrix data to shader	
	//model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	tree.Draw(myCustomShader);


	for (int i = 0; i < 500; i++) {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(v[i], -1.0f, w[i]));
		//send model matrix data to shader	
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tree.Draw(myCustomShader);
	}


	//draw a white cube around the light

	lightShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	model = glm::rotate(glm::mat4(1.0f), glm::radians(-lightAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, 1.0f * lightDir);
	model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	lightCube.Draw(lightShader);

	mySkyBox.Draw(skyboxShader, view, projection);
}

void initSkyBox() {
	faces.push_back("skybox/miramar_rt.tga");
	faces.push_back("skybox/miramar_lf.tga");
	faces.push_back("skybox/miramar_up.tga");
	faces.push_back("skybox/miramar_dn.tga");
	faces.push_back("skybox/miramar_bk.tga");
	faces.push_back("skybox/miramar_ft.tga");
}

void randomArray() {
	int k = 0;
	int m = 0;
	while (k < 500) {
		int x = rand() % 500 - 250;
		int y = rand() % 500 - 250;
		if (!((x <= 90 & x >= -90) && (y <= 210 && y >= -210))) {
			v[k] = x;
			k++;
			w[m] = y;
			m++;
		}
	}
}

int main(int argc, const char * argv[]) {
	initOpenGLWindow();
	initOpenGLState();
	initFBOs();
	initModels();
	initShaders();
	initSkyBox();
	initUniforms();	
	
	specialView = myCamera.getViewMatrix();

	randomArray();
	glCheckError();
	while (!glfwWindowShouldClose(glWindow)) {
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}
