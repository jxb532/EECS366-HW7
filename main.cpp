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

using namespace std;

const GLenum LIGHT [] = {
	GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3,
	GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7
};


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
bool parseLayoutFile(char* path);
void shootRay(Ray *ray);

typedef struct _faceStruct {
  int v1,v2,v3;
  int n1,n2,n3;
} faceStruct;

int verts, faces, norms;    // Number of vertices, faces and normals in the system
point *vertList, *normList; // Vertex and Normal Lists
faceStruct *faceList;	    // Face List
DisplayObject *objects;
DisplayObject *sphereObjects;
DisplayObject *polygonObjects;
Light *lighting;

// The mesh reader itself
// It can read *very* simple obj files
void meshReader (char *filename,int sign)
{
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
    exit(0);
  }

  // Count the number of vertices and faces
  while(!feof(fp))
    {
      fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
      if (letter == 'v')
	{
	  verts++;
	}
      else
	{
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

  fp = fopen(filename, "r");

  // Read the veritces
  for(i = 0;i < verts;i++)
    {
      fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
      vertList[i].x = x;
      vertList[i].y = y;
      vertList[i].z = z;
    }

  // Read the faces
  for(i = 0;i < faces;i++)
    {
      fscanf(fp,"%c %d %d %d\n",&letter,&ix,&iy,&iz);
      faceList[i].v1 = ix - 1;
      faceList[i].v2 = iy - 1;
      faceList[i].v3 = iz - 1;
    }
  fclose(fp);


  // The part below calculates the normals of each vertex
  normCount = (int *)malloc(sizeof(int)*verts);
  for (i = 0;i < verts;i++)
    {
      normList[i].x = normList[i].y = normList[i].z = 0.0;
      normCount[i] = 0;
    }

  for(i = 0;i < faces;i++)
    {
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
  for (i = 0;i < verts;i++)
    {
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

	// Load projections and viewing transforms
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Add the lights.
	glEnable(GL_LIGHTING);
	for (int i = 0; i < 8; i++) glDisable(LIGHT[i]);
	for (int i = 0; i < lights; i++) {
		GLfloat color[4] = { values[i][4], values[i][5], values[i][6], 1 };
		GLfloat position[4] = { values[i][1], values[i][2], values[i][3], 1 };
		glLightfv(LIGHT[i], values[i][0] == 0 ? GL_SPOT_DIRECTION : GL_POSITION, position);
		glLightfv(LIGHT[i], GL_DIFFUSE, color);
		glLightfv(LIGHT[i], GL_SPECULAR, color);
		glEnable(LIGHT[i]);
	}

	// Add the spheres.
	for (int i = lights; i < lights + spheres; i++) {
		glLoadIdentity();
		glTranslatef(values[i][0], values[i][1], values[i][2]);
		GLUquadric* quad = gluNewQuadric();
		gluSphere(quad, values[i][3], SLICES, STACKS);
		// TODO: Apply material elements
		// TODO: delete quad?
	}
	
	// Add the meshes.
	// TODO: Add the meshes.

   
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
		fb->Resize(fb->GetHeight()/2, fb->GetWidth()/2);
		BresenhamLine(fb, fb->GetWidth()*0.1, fb->GetHeight()*0.1, fb->GetWidth()*0.9, fb->GetHeight()*0.9, Color(1,0,0));
		break;
	case '=':
		fb->Resize(fb->GetHeight()*2, fb->GetWidth()*2);
		BresenhamLine(fb, fb->GetWidth()*0.1, fb->GetHeight()*0.1, fb->GetWidth()*0.9, fb->GetHeight()*0.9, Color(1,0,0));
		break;
	case ']':
		// TODO: move the image plane farther from the origin along the z axis
		imagePlaneZ -= Z_STEP;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-imagePlaneXY,imagePlaneXY,-imagePlaneXY,imagePlaneXY,0,imagePlaneZ);
		break;
	case '[':
		// TODO: move the image plane closer to the origin along the z axis
		imagePlaneZ += Z_STEP;
		imagePlaneZ = imagePlaneZ > 0 ? 0 : imagePlaneZ;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-imagePlaneXY,imagePlaneXY,-imagePlaneXY,imagePlaneXY,0,imagePlaneZ);
		break;
	case '.':
		// TODO: increase the x,y dimensions of the image plane
		imagePlaneXY += XY_STEP;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-imagePlaneXY,imagePlaneXY,-imagePlaneXY,imagePlaneXY,0,imagePlaneZ);
		break;
	case ',':
		// TODO: decrease the x,y dimensions of the image plane
		imagePlaneXY -= XY_STEP;
		imagePlaneXY = imagePlaneXY < 1 ? 1 : imagePlaneXY;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-imagePlaneXY,imagePlaneXY,-imagePlaneXY,imagePlaneXY,0,imagePlaneZ);
		break;
	case 'r':
		// TODO redraw the image
		break;
    default:
		break;
    }

    // Schedule a new display event
    glutPostRedisplay();
}


int main(int argc, char* argv[])
{    

	fb = new FrameBuffer(INITIAL_RES, INITIAL_RES);

	BresenhamLine(fb, fb->GetWidth()*0.1, fb->GetHeight()*0.1, fb->GetWidth()*0.9, fb->GetHeight()*0.9, Color(1,0,0));

	

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
	glOrtho(-imagePlaneXY,imagePlaneXY,-imagePlaneXY,imagePlaneXY,0,imagePlaneZ);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);

	initObjectsAndLights();

    // Switch to main loop
    glutMainLoop();
    return 0;        
}

void shootRay(Ray *ray) {
	// if ray intersects an object
		//get normal at intersection point
		//calculate local intensity (I_local)
		// decrement current depth of trace
		// if depth of trace > 0
			// if object is a reflecting object
				// calculate reflectin vector and include in the ray structure
				// Ray Origin = intersection point
				// Attenuate the ray (multiply the current k_rg by its value at the previous intersection)
				// shootRay(reflected ray structure)
				// if reflected ray intersects an object
					// combine colors (k_rg I) with I_local
			// if object is a refracting object
				// if ray is entering object
					// accumulate the refractive index
					// increment number of objects that the ray is currently inside
					// calculate refraction vector and include in refracted ray structure
				// else
					// de-accumulate refractive index
					// decrement number of objects that the ray is currently inside
					// calculate refraction vector and include in refracted ray structure
				// Ray Origin = intersection point
				// Attenuate ray (k_tg)
				// shootRay(refracted ray structure)
				// if refracted ray intersects an object
					// combine colors (k_tg I) with I_local
}

void initObjectsAndLights() {
	// TODO move these guys into the main directory
	parseLayoutFile("samples/redsphere.rtl");

	//TODO grab the filename as a string rather than a float
	char* meshFile = "teapot.obj";
	//values[lights + spheres][0];
	meshReader(meshFile, 1.0);

	lighting = static_cast<Light*>( ::operator new ( sizeof Light * lights ) );

	// Add the lights.
	int index = 0;
	for (int i = 0; i < lights; i++) {
		new (&lighting[index++]) Light((int)values[i][0], &values[i][1], &values[i][4]);
	}

	// TODO make sure this works
	sphereObjects = static_cast<DisplayObject*>( ::operator new ( sizeof Sphere * spheres ) );

	// Add the spheres.
	index = 0;
	for (int i = lights; i < lights + spheres; i++) {
		new (&sphereObjects[index++]) Sphere(&values[i][0], values[i][3], &values[i][4], &values[i][7], &values[i][10], values[i][13], values[i][14], values[i][15], values[i][16], values[i][17], values[i][18], values[i][19]);
	}
	

	polygonObjects = static_cast<DisplayObject*>( ::operator new ( sizeof Polygon * spheres ) );

	// Add the meshes.
	index = 0;
	for (int i = lights + spheres; i < lights + spheres + meshes; i++) {
		for (int j = 0; j < faces; j++) {

			float scale = values[i][1];

			Vector3* vertex1 = new Vector3(vertList[faceList[j].v1].x, vertList[faceList[j].v1].y, vertList[faceList[j].v1].z);
			Vector3* vertex2 = new Vector3(vertList[faceList[j].v2].x, vertList[faceList[j].v2].y, vertList[faceList[j].v2].z);
			Vector3* vertex3 = new Vector3(vertList[faceList[j].v3].x, vertList[faceList[j].v3].y, vertList[faceList[j].v3].z);

			Matrix* rotateX = rotateMatrix(values[i][2], 'x');
			Matrix* rotateY = rotateMatrix(values[i][3], 'y');
			Matrix* rotateZ = rotateMatrix(values[i][4], 'z');

			Vector3* translate = new Vector3(values[i][5], values[i][6], values[i][7]);

			//TODO rotate and scale vertices
			float vertices[9] = {vertex1->vector[0], vertex1->vector[1], vertex1->vector[2], vertex2->vector[0], vertex2->vector[1], vertex2->vector[2], vertex3->vector[0], vertex3->vector[1], vertex3->vector[2]};

			new (&polygonObjects[index++]) Polygon(vertices, &values[i][8], &values[i][11], &values[i][14], values[i][17], values[i][18], values[i][19], values[i][20], values[i][21], values[i][22], values[i][23]);

			delete vertex1, vertex2, vertex3, rotateX, rotateY, rotateZ, translate;
		}
	}
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

	// TODO this loop is gone through one too many times, I think we need to handle a newline at the end of the file
	// it's actually hitting case 'S' twice, when there is only one sphere in the file (the final fp >> type isn't actually changing type)
	while (!fp.eof()) {
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
			fp >> meshPaths[curMesh];
			count = 24;
			index = lights + spheres + curMesh++;
			break;
		default:
			cout << "Failed to parse " << path << endl;
			return false;
		}

		if (curLight > lights || curSphere > spheres || curMesh > meshes) {
			cout << "Failed to parse " << path << endl;
			return false;
		}

		values[index] = new float [count];
		for (int i = 0; i < count; i++) {
			fp >> values[index][i];
		}
	}

	fp.close();
	return true;
}
