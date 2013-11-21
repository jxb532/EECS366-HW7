#include <stdio.h>
#include <stdlib.h>
#include "glut.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

#include "frame_buffer.h"
#include "primitives.h"
#include "color.h"
#include "Ray.h"
#include "DisplayObject.h"
#include "Light.h"
#include "MatrixMakers.h"
#include <vector>

#include <iostream>
#include <fstream>

#define ON 1
#define OFF 0
#define Z_STEP 1
#define XY_STEP 1
#define SLICES 100
#define STACKS 100
#define LAYOUT_FILE "samples/red_sphere_and_teapot.rtl"
#define RAY_DEPTH 1

using namespace std;

// Global variables
int window_width, window_height;    // Window dimensions
int imagePlaneZ = -8;
int imagePlaneXY = 5;
const int INITIAL_RES = 400;
float** values;
char** meshPaths;
int lights, spheres, meshes;

FrameBuffer* fb;

void initObjectsAndLights();
void redraw();
bool parseLayoutFile(char* path);
bool shootRay(Ray *ray);
bool shootRay(Ray *ray, int depth, int objectsRayIsInside);
void updateBoundingBox(Vector3* v);
void finishBoundingBox();
bool first = true;

typedef struct _faceStruct {
  int v1,v2,v3;
  int n1,n2,n3;
} faceStruct;

int verts, faces, norms;    // Number of vertices, faces and normals in the system
int totalFaces = 0;
point *vertList, *normList; // Vertex and Normal Lists
faceStruct *faceList;	    // Face List
DisplayObject *objects;
//DisplayObject *sphereObjects;
//DisplayObject *polygonObjects;
vector<Sphere*> sphereObjects;
vector<Polygon*> polygonObjects;
vector<Light*> lighting;
vector<Polygon*> boundingBoxPolygons;
Vector3** boundingBox = new Vector3*[8];
float MinimumX, MaximumX, MinimumY, MaximumY, MinimumZ, MaximumZ;

// The mesh reader itself
// It can read *very* simple obj files
bool meshReader (char *filename, int sign) {
	float x,y,z,len;
	int i;
	char letter;
	point v1,v2,crossP;
	int ix,iy,iz;
	int *normCount;
	FILE *fp;

	fp = fopen(filename, "r");
	if (fp == NULL) { 
		printf("Cannot open %s\n!", filename);
		return false;
	}

	// Count the number of vertices and faces
	while(!feof(fp)){
		fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
		if (letter == 'v') {
			verts++;
		} else {
			faces++;
		}
	}

	fclose(fp);

	printf("verts : %d\n", verts);
	printf("faces : %d\n", faces);

	// Dynamic allocation of vertex and face lists
	faceList = (faceStruct *)malloc(sizeof(faceStruct)*faces);
	vertList = (point *)malloc(sizeof(point)*verts);
	normList = (point *)malloc(sizeof(point)*verts);

	// Reopen to start at beginning
	fp = fopen(filename, "r");

	// Read the vertices
	for(i = 0;i < verts;i++) {
		fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
		vertList[i].x = x;
		vertList[i].y = y;
		vertList[i].z = z;
	}

	// Read the faces
	for(i = 0;i < faces;i++) {
		fscanf(fp,"%c %d %d %d\n",&letter,&ix,&iy,&iz);
		faceList[i].v1 = ix - 1;
		faceList[i].v2 = iy - 1;
		faceList[i].v3 = iz - 1;
	}

	fclose(fp);


	// The part below calculates the normals of each vertex
	normCount = (int *)malloc(sizeof(int)*verts);
	for (i = 0;i < verts;i++) {
		normList[i].x = normList[i].y = normList[i].z = 0.0;
		normCount[i] = 0;
	}

	for(i = 0;i < faces;i++) {
		v1.x = vertList[faceList[i].v2].x - vertList[faceList[i].v1].x;
		v1.y = vertList[faceList[i].v2].y - vertList[faceList[i].v1].y;
		v1.z = vertList[faceList[i].v2].z - vertList[faceList[i].v1].z;
		v2.x = vertList[faceList[i].v3].x - vertList[faceList[i].v2].x;
		v2.y = vertList[faceList[i].v3].y - vertList[faceList[i].v2].y;
		v2.z = vertList[faceList[i].v3].z - vertList[faceList[i].v2].z;

		crossP.x = v1.y*v2.z - v1.z*v2.y;
		crossP.y = v1.z*v2.x - v1.x*v2.z;
		crossP.z = v1.x*v2.y - v1.y*v2.x;

		len = sqrt(crossP.x*crossP.x + crossP.y*crossP.y + crossP.z*crossP.z);

		crossP.x = -crossP.x/len;
		crossP.y = -crossP.y/len;
		crossP.z = -crossP.z/len;

		normList[faceList[i].v1].x = normList[faceList[i].v1].x + crossP.x;
		normList[faceList[i].v1].y = normList[faceList[i].v1].y + crossP.y;
		normList[faceList[i].v1].z = normList[faceList[i].v1].z + crossP.z;
		normList[faceList[i].v2].x = normList[faceList[i].v2].x + crossP.x;
		normList[faceList[i].v2].y = normList[faceList[i].v2].y + crossP.y;
		normList[faceList[i].v2].z = normList[faceList[i].v2].z + crossP.z;
		normList[faceList[i].v3].x = normList[faceList[i].v3].x + crossP.x;
		normList[faceList[i].v3].y = normList[faceList[i].v3].y + crossP.y;
		normList[faceList[i].v3].z = normList[faceList[i].v3].z + crossP.z;
		normCount[faceList[i].v1]++;
		normCount[faceList[i].v2]++;
		normCount[faceList[i].v3]++;
	}

	for (i = 0;i < verts;i++) {
		normList[i].x = (float)sign*normList[i].x / (float)normCount[i];
		normList[i].y = (float)sign*normList[i].y / (float)normCount[i];
		normList[i].z = (float)sign*normList[i].z / (float)normCount[i];
	}
}

void drawRect(double x, double y, double w, double h)
{
	glVertex2f(x,y);
	glVertex2f(x+w,y);
	glVertex2f(x+w,y+h);
	glVertex2f(x, y+h);
}


// The display function. It is called whenever the window needs
// redrawing (ie: overlapping window moves, resize, maximize)
// You should redraw your polygons here
void	display(void)
{
    // Clear the background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	double w = 10/double(fb->GetWidth());
	double h = 10/double(fb->GetHeight());
	
	Color cl;
	glColor3f(0,0,1);
	
	glBegin(GL_QUADS);

	printf("width %d, height %d\n", fb->GetWidth(), fb->GetHeight());

	for(int y = 0; y < fb->GetHeight(); y++)
	{
		for(int x = 0; x < fb->GetHeight(); x++)
		{
			cl = fb->buffer[x][y].color;
			glColor3f(cl.r, cl.g, cl.b);

			drawRect(w*x, h*y, w, h);
		}
	}

	glEnd();
    glutSwapBuffers();
}


// This function is called whenever the window is resized. 
// Parameters are the new dimentions of the window
void	resize(int x,int y)
{
    glViewport(0,0,x,y);
    window_width = x;
    window_height = y;
    
    printf("Resized to %d %d\n",x,y);
}


// This function is called whenever the mouse is pressed or released
// button is a number 0 to 2 designating the button
// state is 1 for release 0 for press event
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseButton(int button,int state,int x,int y)
{
   ;
}


//This function is called whenever the mouse is moved with a mouse button held down.
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseMotion(int x, int y)
{
	;
}


// This function is called whenever there is a keyboard input
// key is the ASCII value of the key pressed
// x and y are the location of the mouse
void	keyboard(unsigned char key, int x, int y)
{
    switch(key) {
    case 'q':                           /* Quit */
		exit(1);
		break;
	case '-':
		fb->Resize(fb->GetHeight()/2 < 1 ? 1 : fb->GetHeight()/2, fb->GetWidth()/2 < 1 ? 1 : fb->GetWidth()/2);
		BresenhamLine(fb, fb->GetWidth()*0.1, fb->GetHeight()*0.1, fb->GetWidth()*0.9, fb->GetHeight()*0.9, Color(1,0,0));
		break;
	case '=':
		fb->Resize(fb->GetHeight()*2, fb->GetWidth()*2);
		BresenhamLine(fb, fb->GetWidth()*0.1, fb->GetHeight()*0.1, fb->GetWidth()*0.9, fb->GetHeight()*0.9, Color(1,0,0));
		break;
	case ']':
		imagePlaneZ -= Z_STEP;
		break;
	case '[':
		imagePlaneZ += Z_STEP;
		imagePlaneZ = imagePlaneZ > 0 ? 0 : imagePlaneZ;
		break;
	case '.':
		imagePlaneXY += XY_STEP;
		break;
	case ',':
		imagePlaneXY -= XY_STEP;
		imagePlaneXY = imagePlaneXY < 1 ? 1 : imagePlaneXY;
		break;
	case 'r':
		redraw();
		break;
    default:
		break;
    }

    // Schedule a new display event
    glutPostRedisplay();
}

void test() {
	Vector3* v1 = new Vector3(1, 1, 0);
	Vector3* v2 = new Vector3(-1, 1, 0);
	Vector3* v3 = new Vector3(0, -1, 0);
	Polygon* polygon = new Polygon(v1, v2, v3, v1, v1, v1, new Material());

	float center[] = {0.0, 0.0, 0.0};
	Sphere* sphere = new Sphere(center, 1.0, new Material());

	//Vector3* origin = new Vector3(0, 0, 1);
	//Vector3* direction = new Vector3(0, 0, -1);
	//Ray* ray = new Ray(origin, direction);

	Vector3* origin = new Vector3(0.5, 0, 3);
	Vector3* direction = new Vector3(0, 0, -1);
	Ray* ray = new Ray(origin, direction);

	Vector3* curIntersect = new Vector3();
	float dist = 0;
	//polygon->intersects(ray, curIntersect, dist);
	sphere->intersects(ray, curIntersect, dist);
}

int main(int argc, char* argv[])
{    
	//test();

	fb = new FrameBuffer(INITIAL_RES, INITIAL_RES);

	BresenhamLine(fb, fb->GetWidth()*0.1, fb->GetHeight()*0.1, fb->GetWidth()*0.9, fb->GetHeight()*0.9, Color(1,0,0));

	initObjectsAndLights();
	redraw();

    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Raytracer");
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);

    // Initialize GL
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(0,10,0,10,-10000,10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);

    // Switch to main loop
    glutMainLoop();
    return 0;        
}

void redraw() {
	for (int i = 0; i < fb->GetWidth(); i++) {
		if (i % 50 == 0) {
			printf("Calculating pixel column %d out of %d\n", i, fb->GetWidth());
		}
		for (int j = 0; j < fb->GetHeight(); j++) {

			float x = (((float)imagePlaneXY * 2) / (float)fb->GetWidth()) * (float)i - (float)imagePlaneXY;
			float y = (((float)imagePlaneXY * 2) / (float)fb->GetHeight()) * (float)j - (float)imagePlaneXY;

			Vector3* origin = new Vector3(x, y, 0.0);
			Vector3* direction = new Vector3(0.0, 0.0, -1.0);
			Ray* ray = new Ray(origin, direction);

			shootRay(ray, RAY_DEPTH, 0);
			fb->SetPixel(i, j, ray->color, 0);

			delete origin, direction, ray;
			origin = direction = NULL;
			ray = NULL;
		}
	}
}

bool shootRay(Ray *ray, int depth = 5, int objectsRayIsInside = 0) {
	if (depth < 1) {
		return false;
	}

	float lowestDist = FLT_MAX;
	DisplayObject* obj = NULL;
	Vector3* intersect = NULL;

	bool hitsBoundingBox = totalFaces == 0;

	if (!hitsBoundingBox) {
		float dist = 0;
		Vector3* curIntersect = new Vector3();
		for (int i = 0; i < 12; i++) {
			Polygon* currentObj = boundingBoxPolygons[i];
			if (currentObj->intersects(ray, curIntersect, dist)) {
				hitsBoundingBox = true;
				break;
			}
		}
		delete curIntersect;
	}
	
	for (int i = 0; i < spheres + totalFaces; i++) {

		if (i >= spheres && !hitsBoundingBox) {
			break;
		}

		DisplayObject* currentObj = i < spheres ? dynamic_cast<DisplayObject*>(sphereObjects[i]) : dynamic_cast<DisplayObject*>(polygonObjects[i - spheres]);
		Vector3* curIntersect = new Vector3();
		float dist = 0;
		if (currentObj->intersects(ray, curIntersect, dist) && dist > 0.00001 && dist < lowestDist) {
			lowestDist = dist;
			// TODO find out what was breaking this
			//delete obj;
			obj = currentObj;

			delete intersect;
			intersect = curIntersect;
		} else {
			delete curIntersect;
		}
	}

	if (lowestDist == FLT_MAX) {
		return false;
	}

	// Calculate local intensity (I_local)
	Vector3* norm = obj->normalAtPoint(intersect);
	Vector3 V = *ray->direction * -1.0;
	Color* localIntensity = obj->calculateIntensityAtPoint(intersect, &V, norm, lighting, lights);
	Color* totalColor = new Color(localIntensity);
	delete localIntensity, norm;

	// if object is a reflecting object
	if (obj->material->k_reflective > 0 /*&& objectsRayIsInside == 0*/) {
		// calculate reflection vector and include in new ray structure
		Vector3 reflection = *ray->direction - (*norm * (2 * ray->direction->dot(norm)));
		reflection.normalize();
		Ray* reflectedRay = new Ray(intersect, &reflection);

		// if reflected ray intersects an object
		if (shootRay(reflectedRay, depth - 1, objectsRayIsInside)) {
			// combine colors (k_rg I) with I_local
			totalColor = &((*reflectedRay->color * obj->material->k_reflective) + *totalColor);
		}

		delete reflectedRay;
	}

	// if object is a refracting object
	if (obj->material->k_refractive > 0) {
		Vector3 refractionVector;
		float refIndex = 0;
		int objsInside = 0;

		// if ray is entering object
		if (ray->direction->dot(norm) <= 0) {
			// accumulate the refractive index
			refIndex = 1.0 / obj->material->refraction_index;
			objsInside = objectsRayIsInside + 1;

			// calculate refraction vector and include in refracted ray structure
			float cosTheta = ray->direction->dot(norm) * -1.0;
			float sinSqTheta = refIndex * refIndex * (1 - (cosTheta * cosTheta));
			refractionVector = (*ray->direction * refIndex) + (*norm * (refIndex * cosTheta - sqrtf(1 - sinSqTheta)));
		} else {
			// de-accumulate refractive index
			refIndex = obj->material->refraction_index;

			// decrement number of objects that the ray is currently inside
			objsInside = objectsRayIsInside - 1;

			// calculate refraction vector and include in refracted ray structure
			float cosTheta = ray->direction->dot(norm) * -1.0;
			float sinSqTheta = refIndex * refIndex * (1 - (cosTheta * cosTheta));
			refractionVector = (*ray->direction * refIndex) + (*norm * (refIndex * cosTheta - sqrtf(1 - sinSqTheta)));
		}

		Ray* refractedRay = new Ray(intersect, &refractionVector);

		// if refracted ray intersects an object
		if (shootRay(refractedRay, depth - 1, objsInside)) {
			// combine colors (k_tg I) with I_local
			Color* temp = totalColor;
			totalColor = new Color((*refractedRay->color * obj->material->k_reflective) + *totalColor);
			// TODO reinstate this, it was killing things
			//delete temp;
		}

		delete refractedRay;
	}

	ray->color = totalColor;

	return true;
}

Vector3 rotscaletrans(float x, float y, float z, Matrix* rot, float scale, Vector3* trans) {
	float sx = scale * x;
	float sy = scale * y;
	float sz = scale * z;
	return Vector3(rot->matrix[0][0] * sx + rot->matrix[0][1] * sy + rot->matrix[0][2] * sz + trans->vector[0],
		rot->matrix[1][0] * sx + rot->matrix[1][1] * sy + rot->matrix[1][2] * sz + trans->vector[1],
		rot->matrix[2][0] * sx + rot->matrix[2][1] * sy + rot->matrix[2][2] * sz + trans->vector[2]);
}

Vector3 rotscaletrans(float x, float y, float z, Matrix* rot, float scale) {
	float sx = scale * x;
	float sy = scale * y;
	float sz = scale * z;
	return Vector3(rot->matrix[0][0] * sx + rot->matrix[0][1] * sy + rot->matrix[0][2] * sz,
		rot->matrix[1][0] * sx + rot->matrix[1][1] * sy + rot->matrix[1][2] * sz,
		rot->matrix[2][0] * sx + rot->matrix[2][1] * sy + rot->matrix[2][2] * sz);
}

void initObjectsAndLights() {
	parseLayoutFile(LAYOUT_FILE);

	// Add the lights.
	//lighting = static_cast<Light*>( ::operator new ( sizeof Light * lights ) );
	int index = 0;
	for (vector<Light*>::size_type i = 0; i < lights; i++) {
		//new (&lighting[index++]) Light((int)values[i][0], &values[i][1], &values[i][4]);
		Light* light = new Light((int)values[i][0], &values[i][1], &values[i][4]);
		lighting.push_back(light);
	}

	// Add the spheres.
	//sphereObjects = static_cast<DisplayObject*>( ::operator new ( sizeof Sphere * spheres ) );
	//sphereObjects = new Sphere[spheres];
	//vector<Sphere> sphereObjects
	index = 0;
	for (vector<DisplayObject*>::size_type i = lights; i < lights + spheres; i++) {
		Material* mat = new Material(&values[i][4], &values[i][7], &values[i][10], values[i][13], values[i][14], values[i][15], values[i][16], values[i][17], values[i][18], values[i][19]);
		//new (&sphereObjects[index++]) Sphere(&values[i][0], values[i][3], mat);
		Sphere* sphere = new Sphere(&values[i][0], values[i][3], mat);
		sphereObjects.push_back(sphere);
	}
	
	// Add the meshes.
	//polygonObjects = static_cast<DisplayObject*>( ::operator new ( sizeof Polygon * spheres ) );
	index = 0;
	for (vector<DisplayObject*>::size_type i = lights + spheres; i < lights + spheres + meshes; i++) {
		meshReader(meshPaths[i - lights - spheres], 1.0);
		totalFaces += faces;

		Material* mat = new Material(&values[i][8], &values[i][11], &values[i][14], values[i][17], values[i][18], values[i][19], values[i][20], values[i][21], values[i][22], values[i][23]);
		
		// Transform matrices.
		float scale = values[i][0];
		Vector3* translate = new Vector3(values[i][4], values[i][5], values[i][6]);
		Matrix* rx = rotateMatrix(values[i][1], 'x');
		Matrix* ry = rotateMatrix(values[i][2], 'y');
		Matrix* rz = rotateMatrix(values[i][3], 'z');
		Matrix* ro = *rx * rz;
		Matrix* tr = *ry * ro;
		delete rx, ry, rz, ro;
		rx = ry = rz = ro = NULL;

		Vector3* temp;
		Matrix *t1, *t2;
		for (int j = 0; j < faces; j++) {
			// Apply the transforms.
			Vector3 vertex1 = rotscaletrans(vertList[faceList[j].v1].x, vertList[faceList[j].v1].y, vertList[faceList[j].v1].z, tr, scale, translate);
			Vector3 vertex2 = rotscaletrans(vertList[faceList[j].v2].x, vertList[faceList[j].v2].y, vertList[faceList[j].v2].z, tr, scale, translate);
			Vector3 vertex3 = rotscaletrans(vertList[faceList[j].v3].x, vertList[faceList[j].v3].y, vertList[faceList[j].v3].z, tr, scale, translate);
			Vector3 normal1 = rotscaletrans(normList[faceList[j].v1].x, normList[faceList[j].v1].y, normList[faceList[j].v1].z, tr, scale);
			Vector3 normal2 = rotscaletrans(normList[faceList[j].v2].x, normList[faceList[j].v2].y, normList[faceList[j].v2].z, tr, scale);
			Vector3 normal3 = rotscaletrans(normList[faceList[j].v3].x, normList[faceList[j].v3].y, normList[faceList[j].v3].z, tr, scale);

			// Save the calculated polygon.
			//new (&polygonObjects[index++]) Polygon(&vertex1, &vertex2, &vertex3, &normal1, &normal2, &normal3, mat);
			updateBoundingBox(&vertex1);
			updateBoundingBox(&vertex2);
			updateBoundingBox(&vertex3);
			Polygon* poly = new Polygon(&vertex1, &vertex2, &vertex3, &normal1, &normal2, &normal3, mat);
			polygonObjects.push_back(poly);
		}

		delete translate, tr;
		translate = NULL; tr = NULL;
	}

	finishBoundingBox();
}

void updateBoundingBox(Vector3* v) {
	float a = v->vector[0];
	float b = v->vector[1];
	float c = v->vector[2];

	if (first) {
		MaximumX = MinimumX = a;
		MaximumY = MinimumY = b;
		MaximumZ = MinimumZ = c;
		first = false;
	}

	MaximumX = a > MaximumX ? a : MaximumX;
	MinimumX = a < MinimumX ? a : MinimumX;
	MaximumY = b > MaximumY ? b : MaximumY;
	MinimumY = b < MinimumY ? b : MinimumY;
	MaximumZ = c > MaximumZ ? c : MaximumZ;
	MinimumZ = c < MinimumZ ? c : MinimumZ;
}

void finishBoundingBox() {
	
	boundingBox[0] = new Vector3(MaximumX, MaximumY, MaximumZ);
	boundingBox[1] = new Vector3(MinimumX, MaximumY, MaximumZ);
	boundingBox[2] = new Vector3(MinimumX, MinimumY, MaximumZ);
	boundingBox[3] = new Vector3(MaximumX, MinimumY, MaximumZ);
	boundingBox[4] = new Vector3(MaximumX, MaximumY, MinimumZ);
	boundingBox[5] = new Vector3(MinimumX, MaximumY, MinimumZ);
	boundingBox[6] = new Vector3(MinimumX, MinimumY, MinimumZ);
	boundingBox[7] = new Vector3(MaximumX, MinimumY, MinimumZ);

	Vector3* emptyPtr = new Vector3();
	Material* mat = new Material();
					
	boundingBoxPolygons.push_back(new Polygon(boundingBox[0], boundingBox[1], boundingBox[2], emptyPtr, emptyPtr, emptyPtr, mat));
	boundingBoxPolygons.push_back(new Polygon(boundingBox[0], boundingBox[2], boundingBox[3], emptyPtr, emptyPtr, emptyPtr, mat));
	boundingBoxPolygons.push_back(new Polygon(boundingBox[5], boundingBox[4], boundingBox[7], emptyPtr, emptyPtr, emptyPtr, mat));
	boundingBoxPolygons.push_back(new Polygon(boundingBox[5], boundingBox[7], boundingBox[6], emptyPtr, emptyPtr, emptyPtr, mat));
	boundingBoxPolygons.push_back(new Polygon(boundingBox[4], boundingBox[0], boundingBox[3], emptyPtr, emptyPtr, emptyPtr, mat));
	boundingBoxPolygons.push_back(new Polygon(boundingBox[4], boundingBox[3], boundingBox[7], emptyPtr, emptyPtr, emptyPtr, mat));
	boundingBoxPolygons.push_back(new Polygon(boundingBox[6], boundingBox[2], boundingBox[1], emptyPtr, emptyPtr, emptyPtr, mat));
	boundingBoxPolygons.push_back(new Polygon(boundingBox[6], boundingBox[1], boundingBox[5], emptyPtr, emptyPtr, emptyPtr, mat));
	boundingBoxPolygons.push_back(new Polygon(boundingBox[5], boundingBox[1], boundingBox[0], emptyPtr, emptyPtr, emptyPtr, mat));
	boundingBoxPolygons.push_back(new Polygon(boundingBox[5], boundingBox[0], boundingBox[4], emptyPtr, emptyPtr, emptyPtr, mat));
	boundingBoxPolygons.push_back(new Polygon(boundingBox[7], boundingBox[3], boundingBox[2], emptyPtr, emptyPtr, emptyPtr, mat));
	boundingBoxPolygons.push_back(new Polygon(boundingBox[7], boundingBox[2], boundingBox[6], emptyPtr, emptyPtr, emptyPtr, mat));
	delete emptyPtr;
}

bool parseLayoutFile(char* path) {
	ifstream fp(path, ios::in);
	if(!fp || !fp.is_open()) {
		cout<< "Failed to load " << path << endl;
		return false;
	}

	// Get the basic information from the file.
	fp >> lights >> spheres >> meshes;
	values = new float* [lights + spheres + meshes];
	meshPaths = new char* [meshes];

	if (lights > 8) {
		cout << "Cannot have more than 8 lights." << endl;
		return false;
	}

	// Read the objects.
	int curLight = 0;
	int curSphere = 0;
	int curMesh = 0;
	char type;
	int index, count;

	while (!fp.eof() && (curLight < lights || curSphere < spheres || curMesh < meshes)) {
		fp >> type;
		switch (type) {
		case 'L':
			count = 7;
			index = curLight++;
			break;
		case 'S':
			count = 20;
			index = lights + curSphere++;
			break;
		case 'M':
			meshPaths[curMesh] = new char[50];
			fp >> meshPaths[curMesh];
			count = 24;
			index = lights + spheres + curMesh++;
			break;
		default:
			cout << "Failed to parse " << path << endl;
			return false;
		}

		values[index] = new float[count];
		for (int i = 0; i < count; i++) {
			fp >> values[index][i];
		}
	}

	fp.close();
	cout << "Successfully read " << path << endl;
	return true;
}
