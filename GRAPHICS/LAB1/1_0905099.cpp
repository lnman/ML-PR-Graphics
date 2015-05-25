#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include"glut.h"


#include<math.h>

#define pi 2*acos(0)

double cameraHeight;
double cameraAngle;

int drawgrid;
int drawaxes;
double angle;
double rotate;
double rad=20;


struct point
{
	double x,y,z;
};


void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);
			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);
			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}


void drawSquare(float a)
{
	glBegin(GL_QUADS);{
		glVertex3f( a, a,2);
		glVertex3f( a,-a,2);
		glVertex3f(-a,-a,2);
		glVertex3f(-a, a,2);
	}glEnd();
}


void drawss()
{
	glPushMatrix();{
		glRotatef(angle,0,0,1);
		glTranslatef(75,0,0);
		glRotatef(2*angle,0,0,1);

				
		glPushMatrix();{
			glRotatef(angle,0,0,1);
			glTranslatef(25,0,0);
			glRotatef(3*angle,0,0,1);
			glColor3f(0.0, 0.0, 1.0);
			drawSquare(5);

		}glPopMatrix();
		glColor3f(1.0, 0.0, 0.0);
		drawSquare(10.0);
	}glPopMatrix();

}

//draws half sphere
void drawsphere(float radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j,k,l;
	double h,r;
	double mx=20;
	double pos=mx-radius;
	
	// all surfaces
	double surfaces[3]={pos,pos,mx};
	for(i=0;i<3;i++){
		glColor3f((i)%2,(i)%2,(i+1)%2);
		glPushMatrix();
		if(i==1)glRotatef(90,1,0,0);
		if(i==2)glRotatef(90,0,1,0);
		glBegin(GL_QUADS);{
			glVertex3f(surfaces[0],surfaces[1],surfaces[2]);
			glVertex3f(surfaces[0],-surfaces[1],surfaces[2]);
			glVertex3f(-surfaces[0],-surfaces[1],surfaces[2]);
			glVertex3f(-surfaces[0],surfaces[1],surfaces[2]);
		}glEnd();
		glColor3f((i+1)%2,(i)%2,(i+1)%2);
		glBegin(GL_QUADS);{
			glVertex3f(surfaces[0],surfaces[1],-surfaces[2]);
			glVertex3f(surfaces[0],-surfaces[1],-surfaces[2]);
			glVertex3f(-surfaces[0],-surfaces[1],-surfaces[2]);
			glVertex3f(-surfaces[0],surfaces[1],-surfaces[2]);
		}glEnd();
		glPopMatrix();
	}
	//return;
	
	// all spheres
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));	
		r=sqrt(radius*radius-h*h);
		for(j=0;j<=slices/4;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}

	
	for(l=0;l<2;l++){
		glPushMatrix();
		if(l)glRotatef(180,1,0,0);
		for(k=0;k<4;k++){
			glPushMatrix();
			glRotatef(90*k,0,0,1);
			glTranslatef(pos,pos,pos);
			for(i=0;i<stacks;i++)
			{
				for(j=0;j<slices/4;j++)
				{
					glColor3f(0,(double)i/(double)slices,0);
					glBegin(GL_QUADS);{
						glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
						glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
						glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
						glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
					}glEnd();
				}
				
			}
			glPopMatrix();
		}
		glPopMatrix();
	}
	

	//return;

	// all cylinders
	for(i=0;i<=2*pos+1;i++)
	{
		h=i-1;	
		r=radius;
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	double arr[4][2]={{pos,pos},{pos,-pos},{-pos,-pos},{-pos,pos}};
	double arr3[]={1,-1,-1,1};

	for(l=0;l<3;l++){
		glPushMatrix();
		if(l==2)glRotatef(180,1,0,0);
		for( k=0;k<4;k++){
			glPushMatrix();
			if(l)glTranslatef(arr[k][0],arr[k][1],pos);
			else glTranslatef(arr[k][0],arr[k][1],-pos);
			if(l)glRotatef(90*arr3[k],(k+1)%2,(k)%2,0);
			for(i=1;i<=2*pos;i++)
			{
				for(j=0;j<slices;j++)
				{
					glColor3f((double)i/(double)(2*pos),0,0);
					glBegin(GL_QUADS);{
						glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
						glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
						glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
						glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
					}glEnd();
				}
				
			}
			glPopMatrix();
		}
		glPopMatrix();

	}
	
}



void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':	
			drawgrid=1-drawgrid;
			break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			cameraHeight -= 3.0;
			break;
		case GLUT_KEY_UP:		// up arrow key
			cameraHeight += 3.0;
			break;

		case GLUT_KEY_RIGHT:
			cameraAngle += 0.03;
			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;
			break;

		case GLUT_KEY_PAGE_UP:
			if(rad)rad--;
			break;
		case GLUT_KEY_PAGE_DOWN:
			if(rad<20)rad++;
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;	
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/ 
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	gluLookAt(100*cos(cameraAngle), 100*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	//gluLookAt(0,-1,150,	0,0,0,	0,0,1);
	
	
	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();
	glColor3f(1,0,0);

	
	//drawsphere(20,20,20);
	//glRotatef(180,1,0,0);
	//drawsphere(20,20,20);
	
	
	//drawss();

	
	//clipping example

	double coef[4];

	coef[0] = -1;	//-1.x
	coef[1] = 0;	//0.y
	coef[2] = 0;	//0.z
	coef[3] = 20;	//10
	//standard format:: ax + by + cz + d >= 0	
	/// the cutting plane equation: x = 10
	/// we will keep the points with
	//		x <= 10
	//OR	-1.x + 1.y + 1.z + 10 >= 0
	glClipPlane(GL_CLIP_PLANE0,coef);
	coef[0] = -1;	//-1.x
	coef[1] = 0;	//0.y
	coef[2] = 0;	//0.z
	coef[3] = -20;
	

	glClipPlane(GL_CLIP_PLANE1,coef);

	//now we enable the clip plane
	//double trans=1;
	
	//trans*=(20-rad);
	
	//glEnable(GL_CLIP_PLANE0);{
		glScalef(2,2,2);
		drawsphere(rad,20,20);
	//}glDisable(GL_CLIP_PLANE0);
		


	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=100.0;
	cameraAngle=1.0;
	angle=0;

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);
	
	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	10000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

void do_nothing(){
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
