
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>


#define PI 3.14159265  // Should be used from mathlib
inline float sqr(float x) { return x*x; }

using namespace std;


struct lightSource
{
	float xValue;
	float yValue;
	float zValue;
	float redValue;
	float greenValue;
	float blueValue;
};

float centerXSphere = 0;
float centerYSphere = 0;
float centerZSphere = 0;

float kaR = 0;
float kaG = 0;
float kaB = 0;

float kdR = 0;
float kdG = 0;
float kdB = 0;

float ksR = 0;
float ksG = 0;
float ksB = 0;

float powerCoefficient = 0;

int numPointLights = 0;
int numDirectionalLights = 0;

lightSource pointLights[5];
lightSource directionalLights[5];

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
public:
	int w, h; // width and height
};


//****************************************************
// Global Variables
//****************************************************
Viewport	viewport;




//****************************************************
// Simple init function
//****************************************************
void initScene(){

	// Nothing to do here for this simple example.

}


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;

	glViewport (0,0,viewport.w,viewport.h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, viewport.w, 0, viewport.h);

}


//****************************************************
// A routine to set a pixel by drawing a GL point.  This is not a
// general purpose routine as it assumes a lot of stuff specific to
// this example.
//****************************************************

void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
	glColor3f(r, g, b);
	glVertex2f(x + 0.5, y + 0.5);   // The 0.5 is to target pixel
	// centers 
	// Note: Need to check for gap
	// bug on inst machines.
}

//****************************************************
// Draw a filled circle.  
//****************************************************


void circle(float centerX, float centerY, float radius) {
	// Draw inner circle
	glBegin(GL_POINTS);

	// We could eliminate wasted work by only looping over the pixels
	// inside the sphere's radius.  But the example is more clear this
	// way.  In general drawing an object by loopig over the whole
	// screen is wasteful.

	int i,j;  // Pixel indices

	int minI = max(0,(int)floor(centerX-radius));
	int maxI = min(viewport.w-1,(int)ceil(centerX+radius));

	int minJ = max(0,(int)floor(centerY-radius));
	int maxJ = min(viewport.h-1,(int)ceil(centerY+radius));

	for (i=0;i<viewport.w;i++) {
		for (j=0;j<viewport.h;j++) {

			// Location of the center of pixel relative to center of sphere
			float x = (i+0.5-centerX);
			float y = (j+0.5-centerY);

			float dist = sqrt(sqr(x) + sqr(y));

			if (dist<=radius) {

				// This is the front-facing Z coordinate
				float z = sqrt(radius*radius-dist*dist);

				//Normal
				float normalX = x / sqrt(sqr(x)+sqr(y)+sqr(z));
				float normalY = y / sqrt(sqr(x)+sqr(y)+sqr(z));
				float normalZ = z / sqrt(sqr(x)+sqr(y)+sqr(z));


				//Specular

				float specularRed = 0;
				float specularGreen = 0;
				float specularBlue = 0;
				for (int i = 0; i< numPointLights; i++) {
					float pointLightX = pointLights[i].xValue -x;
					float pointLightY = pointLights[i].yValue- y;
					float pointLightZ = pointLights[i].zValue- z;
					float pointLightTotal = sqrt(sqr(pointLightX)+sqr(pointLightY)+sqr(pointLightZ));
					pointLightX = pointLightX / pointLightTotal;
					pointLightY = pointLightY / pointLightTotal;
					pointLightZ = pointLightZ / pointLightTotal;
					float dotProductResult = pointLightX * normalX + pointLightY * normalY + pointLightZ * normalZ;

					float scaledNormalX = dotProductResult*2*normalX;
					float scaledNormalY = dotProductResult*2*normalY;
					float scaledNormalZ = dotProductResult*2*normalZ;

					float reflectiveX = scaledNormalX + (pointLightX * -1);
					float reflectiveY = scaledNormalY + (pointLightY * -1);
					float reflectiveZ = scaledNormalZ + (pointLightZ * -1);

					// assume view  = (0,0,1)

					float dotOfReflectiveViewer= pow(max(reflectiveZ , 0), powerCoefficient);
					specularRed = specularRed + pointLights[i].redValue * ksR * dotOfReflectiveViewer;
					specularGreen = specularGreen+ pointLights[i].greenValue * ksG * dotOfReflectiveViewer;
					specularBlue = specularBlue+ pointLights[i].blueValue * ksB * dotOfReflectiveViewer;
				}
				// Directional

				for (int i = 0; i< numDirectionalLights; i++) {
					float directionLightX = centerXSphere - directionalLights[i].xValue;
					float directionLightY = centerYSphere - directionalLights[i].yValue;
					float directionLightZ = centerZSphere - directionalLights[i].zValue;
					float directionLightTotal = sqrt(sqr(directionLightX)+sqr(directionLightY)+sqr(directionLightZ));
					directionLightX = directionLightX / directionLightTotal;
					directionLightY = directionLightY / directionLightTotal;
					directionLightZ = directionLightZ / directionLightTotal;
					float dotProductResult = directionLightX * normalX + directionLightY * normalY + directionLightZ * normalZ;

					float scaledNormalX = dotProductResult*2*normalX;
					float scaledNormalY = dotProductResult*2*normalY;
					float scaledNormalZ = dotProductResult*2*normalZ;

					float reflectiveX = scaledNormalX + (directionLightX * -1);
					float reflectiveY = scaledNormalY + (directionLightY * -1);
					float reflectiveZ = scaledNormalZ + (directionLightZ * -1);

					// assume view  = (0,0,1)

					float dotOfReflectiveViewer= pow(max(reflectiveZ , 0), powerCoefficient);
					specularRed = specularRed + directionalLights[i].redValue * ksR * dotOfReflectiveViewer;
					specularGreen = specularGreen+ directionalLights[i].greenValue * ksG * dotOfReflectiveViewer;
					specularBlue = specularBlue+ directionalLights[i].blueValue * ksB * dotOfReflectiveViewer;
				}



				/// Diffuse

				float diffuseRed = 0;
				float diffuseGreen = 0;
				float diffuseBlue = 0;
				/// Point Source

				for (int i = 0; i< numPointLights; i++) {
					float pointLightX = pointLights[i].xValue -x;
					float pointLightY = pointLights[i].yValue- y;
					float pointLightZ = pointLights[i].zValue- z;
					float pointLightTotal = sqrt(sqr(pointLightX)+sqr(pointLightY)+sqr(pointLightZ));
					pointLightX = pointLightX / pointLightTotal;
					pointLightY = pointLightY / pointLightTotal;
					pointLightZ = pointLightZ / pointLightTotal;
					float dotProductResult = max(0, pointLightX * normalX + pointLightY * normalY + pointLightZ * normalZ);
					diffuseRed = diffuseRed + pointLights[i].redValue * kdR * dotProductResult;
					diffuseGreen = diffuseGreen+ pointLights[i].greenValue * kdG * dotProductResult;
					diffuseBlue = diffuseBlue+ pointLights[i].blueValue * kdB * dotProductResult;
				}

				//Directional
				for (int i = 0; i< numDirectionalLights; i++) {
					float directionLightX = centerXSphere - directionalLights[i].xValue;
					float directionLightY = centerYSphere - directionalLights[i].yValue;
					float directionLightZ = centerZSphere - directionalLights[i].zValue;
					float directionLightTotal = sqrt(sqr(directionLightX)+sqr(directionLightY)+sqr(directionLightZ));
					directionLightX = directionLightX / directionLightTotal;
					directionLightY = directionLightY / directionLightTotal;
					directionLightZ = directionLightZ / directionLightTotal;
					float dotProductResult = max(0, directionLightX * normalX + directionLightY * normalY + directionLightZ * normalZ);
					diffuseRed = diffuseRed + directionalLights[i].redValue * kdR * dotProductResult;
					diffuseGreen = diffuseGreen+ directionalLights[i].greenValue * kdG * dotProductResult;
					diffuseBlue = diffuseBlue+ directionalLights[i].blueValue * kdB * dotProductResult;
				}
				//directional light

				//// Ambient Term

				float ambientRed = 0;

				float ambientGreen = 0;

				float ambientBlue = 0;

				for (int i = 0; i< numPointLights; i++) {

					ambientRed = ambientRed + ( kaR * pointLights[i].redValue);

					ambientGreen = ambientGreen+ (kaG * pointLights[i].greenValue);

					ambientBlue = ambientBlue +(kaB * pointLights[i].blueValue);

				}


				for (int i = 0; i< numDirectionalLights; i++) {

					ambientRed = ambientRed + ( kaR * directionalLights[i].redValue);

					ambientGreen = ambientGreen+ (kaG * directionalLights[i].greenValue);

					ambientBlue = ambientBlue +(kaB * directionalLights[i].blueValue);

				}





				setPixel(i,j, (specularRed+ambientRed+diffuseRed)/255,(specularGreen+ambientGreen+diffuseGreen)/255, (specularBlue+ambientBlue+diffuseBlue)/255);

				// This is amusing, but it assumes negative color values are treated reasonably.
				// setPixel(i,j, x/radius, y/radius, z/radius );
			}


		}
	}


	glEnd();
}
//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

	glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer

	glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
	glLoadIdentity();				        // make sure transformation is "zero'd"


	// Start drawing
	circle(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) / 3.0);

	glFlush();
	glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}



void keyPressed (unsigned char key, int x, int y) {  

	if (key == ' ') {
		exit(0);
	}

}  

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {   // first argument is the program running
	//This initializes glut
	glutInit(&argc, argv);

	cout <<"num of args " << argc<< endl;

	int i = 1;

	while (i < argc) {

		string lineArg = argv [i];

		if (lineArg == "-ka" || lineArg == "-kd" || lineArg == "-ks") {
			if (i + 3 < argc  == false) {
				cout << "Badly formatted command line arguments for ka, kd, or ks" << endl;
				break;
			}

			i++;
			string redString = argv [i];
			float redValue = 255* (float)atof(redString.c_str());

			i++;
			string greenString = argv [i];
			float greenValue = 255*  (float)atof(greenString.c_str());

			i++;
			string blueString = argv [i];
			float blueValue = 255* (float)atof(blueString.c_str());

			if (redValue < 0 || redValue > 255 || greenValue < 0 || greenValue > 255 || blueValue < 0 || blueValue > 255) {
				cout << "Badly formatted command line arguments. ka, kd, ks value not within range" << endl;
				break;
			}

			if (lineArg == "-ka") {

				kaR = redValue;
				kaG = greenValue;
				kaB = blueValue;

			} else if ( lineArg == "-kd") {

				kdR = redValue;
				kdG = greenValue;
				kdB = blueValue;

			}  else {   //-ks

				ksR = redValue;
				ksG = greenValue;
				ksB = blueValue;

			}

		} else if (lineArg == "-sp") {
			if (i + 1 < argc  == false) {
				cout << "Badly formatted command line arguments for sp" << endl;
				break;
			}
			i++;
			string powerString = argv [i];
			float powerValue = (float)atof(powerString.c_str());

			powerCoefficient = powerValue;

		} else if (lineArg == "-pl" || lineArg == "-dl" ) {
			if (i + 6 < argc  == false) {
				cout << "Badly formatted command line arguments for pl or dl" << endl;
				break;
			}

			i++;
			string xString = argv [i];
			float xValue = (float)atof(xString.c_str());

			i++;
			string yString = argv [i];
			float yValue = (float)atof(yString.c_str());

			i++;
			string zString = argv [i];
			float zValue = (float)atof(zString.c_str());

			i++;
			string redString = argv [i];
			float redValue = (float)atof(redString.c_str());

			i++;
			string greenString = argv [i];
			float greenValue = (float)atof(greenString.c_str());

			i++;
			string blueString = argv [i];
			float blueValue = (float)atof(blueString.c_str());

			lightSource s = {xValue, yValue, zValue, redValue, greenValue, blueValue};

			if (lineArg == "-pl") {

				if (numPointLights > 4) {
					cout << "Badly formatted command line arguments. " << endl;
					break;
				}

				pointLights[numPointLights] = s;
				numPointLights++;

			} else {  //-dl

				if (numDirectionalLights > 4) {
					cout << "Badly formatted command line arguments. " << endl;
					break;
				}

				directionalLights[numDirectionalLights] = s;
				numDirectionalLights++;

			}
		} else {
			cout << "Badly formatted command line arguments. " << endl;
			break;
		}

		i++;
	}

	//This tells glut to use a double-buffered window with red, green, and blue channels 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	// Initalize theviewport size
	viewport.w = 400;
	viewport.h = 400;

	//The size and position of the window
	glutInitWindowSize(viewport.w, viewport.h);
	glutInitWindowPosition(0,0);
	glutCreateWindow(argv[0]);
	glutKeyboardFunc(keyPressed);


	initScene();							// quick function to set up scene

	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
	glutReshapeFunc(myReshape);				// function to run when the window gets resized

	glutMainLoop();							// infinite loop that will keep drawing and resizing
	// and whatever else

	return 0;
}









