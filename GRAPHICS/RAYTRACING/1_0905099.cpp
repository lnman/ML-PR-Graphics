#include <bits/stdc++.h>
#include <windows.h>
#include "glut.h"
#include "bitmap_image.hpp"
#include "cam.hpp"

using namespace std;

#define pi 2*acos(0)
#define MX 40
#define Window_MX 1400
#define inf -10000000

CAM Camera;

int drawgrid;
int drawaxes;

double pers_ang=80;
double pers_far=10000;


int recursion_level;
int window_size;
int no_of_obj;

int no_of_lights;

double arr[Window_MX][Window_MX][3];

struct light
{
	double pos_x,pos_y,pos_z;
}lights[MX];

struct object
{
	int type;
	double pos_x,pos_y,pos_z;
	double rad;
	double col_r,col_g,col_b;
	double coef_a,coef_d,coef_s,coef_r;
	double shine;
}obj[MX];


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


bool get_box_t_vals(Vector3d p0,Vector3d d, float t0, float t1,double& vv,int& dir,Vector3d& box_min,Vector3d& box_max) {
	double temp1,temp2,temp3,temp4,temp5,temp6;
	if(d.x>=0){
		dir=-1;
		temp1=(box_min.x-p0.x)/d.x;temp2=(box_max.x-p0.x)/d.x;
	}else {
		dir=1;
		temp1=(box_max.x-p0.x)/d.x;temp2=(box_min.x-p0.x)/d.x;
	}
	for (int i = 0; i < 2; ++i)
	{
		int xx;
		if(i==1){
			if(d.z>=0){
				xx=-3;
				temp5=(box_min.z-p0.z)/d.z;temp6=(box_max.z-p0.z)/d.z;
			}else {
				xx=3;
				temp5=(box_max.z-p0.z)/d.z;temp6=(box_min.z-p0.z)/d.z;
			}
			if((temp1>temp6)||(temp5>temp2))return false;
			if(temp5>temp1){dir=xx;temp1=temp5;}
			if(temp6<temp2)temp2=temp6;
		}else{
			if(d.y>=0){
				xx=-2;
				temp3=(box_min.y-p0.y)/d.y;temp4=(box_max.y-p0.y)/d.y;
			}else {
				xx=2;
				temp3=(box_max.y-p0.y)/d.y;temp4=(box_min.y-p0.y)/d.y;
			}
			if((temp1>temp4)||(temp3>temp2))return false;
			if(temp3>temp1){dir=xx;temp1=temp3;}
			if(temp4<temp2)temp2=temp4;
		}
	}
	
	if(((temp1<t1)&&(temp2>t0)))vv=temp1;
	return ((temp1<t1) && (temp2>t0));
}


int get_sphere_t_vals(Vector3d p0,Vector3d d,Vector3d pc,double r,double res[2]){
	Vector3d diff=p0-pc;
	double c=diff*diff-r*r;
	double a=d*d;
	double b=d*diff*2;

	if(b*b-4*a*c<0.0){
		return 0;
	}
	if(b*b-4*a*c==0.0){
		res[0]=b;
		return 1;
	}
	res[0]=(-b+sqrt(b*b-4*a*c))/(2*a);
	res[1]=(-b-sqrt(b*b-4*a*c))/(2*a);
	if(res[0]<=0&&res[1]<=0)return 0;
	return 2;
}


bool intersect_with_xy(Vector3d p0,Vector3d d,double res[2]){
	double denom=d*get3dVector(0.0,0.0,1.0);
	if(denom==0.0){
		return false;
	}
	res[0]=get3dVector(0.0,0.0,1.0)*(get3dVector(0.0,0.0,0.0)-p0)/denom;
	if(res[0]<0.0)return false;
	return true;
}


void draw_image(){
	bitmap_image image(window_size,window_size);
	image.set_all_channels(0,0,0);
	for (int i = 0; i < window_size; ++i)
	{
		for (int j = 0; j < window_size; ++j)
		{
			image.set_pixel(i,j,arr[i][j][0]*255,arr[i][j][1]*255,arr[i][j][2]*255);
		}
	}
	image.save_image("output.bmp");
	printf("%s\n","done");

}



int find_intersection(Vector3d vvvv,Vector3d norm_dir,double& x,double& y,double& z,Vector3d& dir2,Vector3d& norm2,double& col_r,double& col_g,double& col_b,int& which){
	double lowest=pers_far;
	double rr[2]={inf,inf};
	int okay=0;
	Vector3d nnn,dddd;
	which=-2;
	// check for xy plane
	if(intersect_with_xy(vvvv,norm_dir,rr)&&rr[0]<lowest){
		okay=1;
		lowest=rr[0];
		x=vvvv.x+norm_dir.x*rr[0];
		y=vvvv.y+norm_dir.y*rr[0];
		z=0;
		which=-1;
		if(vvvv.z>=0)nnn=get3dVector(0,0,1);
		else nnn=get3dVector(0,0,-1);
		if(fabs(x)<pers_far && fabs(y)<pers_far){
			if(y>=0.0){
				if((abs((int)(x))/10)%2!=(abs((int)(y))/10)%2){
					col_r=col_g=col_b=1;
				}else col_r=col_g=col_b=0;
			}else {
				if((abs((int)(x))/10)%2==(abs((int)(y))/10)%2){
					col_r=col_g=col_b=1;
				}else col_r=col_g=col_b=0;
			}
		}
	}
	// for each object
	for(int k=0;k<no_of_obj;k++){
		rr[0]=rr[1]=inf;
		if(obj[k].type==1){
			int howmany=get_sphere_t_vals(vvvv,norm_dir,get3dVector(obj[k].pos_x,obj[k].pos_y,obj[k].pos_z),obj[k].rad,rr);
			if(howmany!=0){
				double lll=min(rr[0],rr[1]);
				if(lll<0.0)lll=max(rr[0],rr[1]);
				if(lll<lowest){
					okay=1;
					which=k;
					lowest=lll;
					x=vvvv.x+norm_dir.x*lowest;
					y=vvvv.y+norm_dir.y*lowest;
					z=vvvv.z+norm_dir.z*lowest;
					col_r=obj[k].col_r;
					col_g=obj[k].col_g;
					col_b=obj[k].col_b;
					nnn=Normalize3dVector(get3dVector(x,y,z)-get3dVector(obj[k].pos_x,obj[k].pos_y,obj[k].pos_z));
				}
			}

		}else if(obj[k].type==2){
			Vector3d box_min=get3dVector(obj[k].pos_x,obj[k].pos_y,obj[k].pos_z);
			Vector3d box_max=get3dVector(obj[k].pos_x+obj[k].rad,obj[k].pos_y+obj[k].rad,obj[k].pos_z+obj[k].rad);
			double val1;
			int di;
			if(get_box_t_vals(vvvv,norm_dir,0.0,10000.0,val1,di,box_min,box_max)){
				if(val1<lowest && val1>0.0){
					okay=1;
					which=k;
					lowest=val1;
					x=vvvv.x+norm_dir.x*lowest;
					y=vvvv.y+norm_dir.y*lowest;
					z=vvvv.z+norm_dir.z*lowest;
					col_r=obj[k].col_r;
					col_g=obj[k].col_g;
					col_b=obj[k].col_b;
					if(abs(di)==1){nnn.x=di;nnn.y=0;nnn.z=0;}
					if(abs(di)==2){nnn.x=0;nnn.y=di/2;nnn.z=0;}
					if(abs(di)==3){nnn.x=0;nnn.y=0;nnn.z=di/3;}
				}
			}
		}
	}
	if(okay){
		norm2.x=nnn.x;
		norm2.y=nnn.y;
		norm2.z=nnn.z;
		dddd=Normalize3dVector(norm_dir-norm2*(norm_dir*norm2*2.0));
		dir2.x=dddd.x;
		dir2.y=dddd.y;
		dir2.z=dddd.z;
	}
	
	return okay;
}

double dist1(Vector3d a,Vector3d b){
	Vector3d t=a-b;
	return Getget3dVectorLength(&t);
}


void draw_now(){
	printf("%s\n","started" );
	Vector3d right=Normalize3dVector(Camera.getright());
	Vector3d up=Normalize3dVector(Camera.getup());
	Vector3d v2=Camera.getpos();
	Vector3d dir=Camera.getdir();
	float direction_length = .001*window_size/tan(pers_ang*M_PI/360);
	Vector3d v1=v2-dir*direction_length;
	Vector3d dir2,norm2;
	memset(&arr,0,sizeof(arr));
	double x,y,z,col_r,col_b,col_g;
	int which=-2,show=0,show1=0;
	for (int i = -window_size/2; i < window_size/2; ++i)
	{
		for (int j = -window_size/2; j < window_size/2; ++j)
		{
			Vector3d vvvv=v2+(up*i*.002)+(right*j*.002);
			Vector3d norm_dir=Normalize3dVector(vvvv-v1);
			if(i==0 && j==0)printf("%f %f %f\n",vvvv.x,vvvv.y,vvvv.z);
			arr[j+window_size/2][window_size-(i+window_size/2)][0]=arr[j+window_size/2][window_size-(i+window_size/2)][1]=arr[j+window_size/2][window_size-(i+window_size/2)][2]=0;
			for (int kk = 0; kk < recursion_level; ++kk)
			{
				int okay=find_intersection(vvvv,norm_dir,x,y,z,dir2,norm2,col_r,col_g,col_b,which);
				if(kk==0){
					if(okay==0)break;
					if(which>=0){
						arr[j+window_size/2][window_size-(i+window_size/2)][0]+=col_r*obj[which].coef_a;
						arr[j+window_size/2][window_size-(i+window_size/2)][1]+=col_g*obj[which].coef_a;
						arr[j+window_size/2][window_size-(i+window_size/2)][2]+=col_b*obj[which].coef_a;
						if(recursion_level==1){
							arr[j+window_size/2][window_size-(i+window_size/2)][0]+=col_r*obj[which].coef_r;
							arr[j+window_size/2][window_size-(i+window_size/2)][1]+=col_g*obj[which].coef_r;
							arr[j+window_size/2][window_size-(i+window_size/2)][2]+=col_b*obj[which].coef_r;
						}
					}
					if(which==-1){
						arr[j+window_size/2][window_size-(i+window_size/2)][0]+=col_r*.2;
						arr[j+window_size/2][window_size-(i+window_size/2)][1]+=col_g*.2;
						arr[j+window_size/2][window_size-(i+window_size/2)][2]+=col_b*.2;
						if(recursion_level==1){
							arr[j+window_size/2][window_size-(i+window_size/2)][0]+=col_r*.2;
							arr[j+window_size/2][window_size-(i+window_size/2)][1]+=col_g*.2;
							arr[j+window_size/2][window_size-(i+window_size/2)][2]+=col_b*.2;
						}
					}



					// light options
					for (int mmm = 0; mmm < no_of_lights; ++mmm)
					{
						Vector3d light_pos=get3dVector(lights[mmm].pos_x,lights[mmm].pos_y,lights[mmm].pos_z);
						Vector3d my_pos=get3dVector(x,y,z)+(dir2*.001);
						Vector3d ligth_dir=Normalize3dVector(light_pos-get3dVector(x,y,z));
						double x_,y_,z_,a,b,c;
						int which_;
						Vector3d dir2_,norm2_;
						okay=find_intersection(my_pos,ligth_dir,x_,y_,z_,dir2_,norm2_,a,b,c,which_);
						if(okay!=0){
							if(dist1(my_pos,get3dVector(x_,y_,z_))<dist1(my_pos,light_pos))continue;
						}
						//const
						//double Is=20000/(dist1(my_pos,light_pos)*dist1(my_pos,light_pos));
						// dynamic
						double Is=Getget3dVectorLength(&light_pos)*Getget3dVectorLength(&light_pos)/(dist1(my_pos,light_pos)*dist1(my_pos,light_pos));
						if(Is>1)Is=1;
						double def_ang=ligth_dir*norm2;
						if(def_ang<0.0)def_ang=0;
						// is point?
						def_ang=1;

						if(which==-1){
							arr[j+window_size/2][window_size-(i+window_size/2)][0]+=Is*def_ang*col_r*.3/no_of_lights;
							arr[j+window_size/2][window_size-(i+window_size/2)][1]+=Is*def_ang*col_g*.3/no_of_lights;
							arr[j+window_size/2][window_size-(i+window_size/2)][2]+=Is*def_ang*col_b*.3/no_of_lights;
						}else{
							arr[j+window_size/2][window_size-(i+window_size/2)][0]+=Is*def_ang*col_r*obj[which].coef_d/no_of_lights;
							arr[j+window_size/2][window_size-(i+window_size/2)][1]+=Is*def_ang*col_g*obj[which].coef_d/no_of_lights;
							arr[j+window_size/2][window_size-(i+window_size/2)][2]+=Is*def_ang*col_b*obj[which].coef_d/no_of_lights;
						}


						double spec_ang=0;
						spec_ang=ligth_dir*dir2;
						if(spec_ang<0.0)spec_ang=0;
						else if(which==-1){
							spec_ang=pow(spec_ang,10);
						}else{
							spec_ang=pow(spec_ang,obj[which].shine);
						}

						
						if(which==-1){
							arr[j+window_size/2][window_size-(i+window_size/2)][0]+=Is*spec_ang*.3/no_of_lights;
							arr[j+window_size/2][window_size-(i+window_size/2)][1]+=Is*spec_ang*.3/no_of_lights;
							arr[j+window_size/2][window_size-(i+window_size/2)][2]+=Is*spec_ang*.3/no_of_lights;
						}else{
							arr[j+window_size/2][window_size-(i+window_size/2)][0]+=Is*spec_ang*obj[which].coef_s/no_of_lights;
							arr[j+window_size/2][window_size-(i+window_size/2)][1]+=Is*spec_ang*obj[which].coef_s/no_of_lights;
							arr[j+window_size/2][window_size-(i+window_size/2)][2]+=Is*spec_ang*obj[which].coef_s/no_of_lights;
						}




						
					}
					
				}else{
					if(okay==0)break;
					if(which>=0){
						arr[j+window_size/2][window_size-(i+window_size/2)][0]+=col_r*obj[which].coef_r/(recursion_level-1);
						arr[j+window_size/2][window_size-(i+window_size/2)][1]+=col_g*obj[which].coef_r/(recursion_level-1);
						arr[j+window_size/2][window_size-(i+window_size/2)][2]+=col_b*obj[which].coef_r/(recursion_level-1);
					}
					if(which==-1){
						arr[j+window_size/2][window_size-(i+window_size/2)][0]+=col_r*.2/(recursion_level-1);
						arr[j+window_size/2][window_size-(i+window_size/2)][1]+=col_g*.2/(recursion_level-1);
						arr[j+window_size/2][window_size-(i+window_size/2)][2]+=col_b*.2/(recursion_level-1);
					}
				}
				vvvv=get3dVector(x,y,z)+dir2*.001;
				norm_dir=dir2;
			}
			
		}
	}

	printf("%lf\n",(double)show1/(window_size*window_size) );

	draw_image();
}






void drawGrid()
{
	int i;
	
	if(drawgrid==1)
	{
		
		for(i=-100;i<=100;i++){
			GLfloat a=(GLfloat)(abs(i)%2);
			for (int j=-100; j<=100; ++j)
			{
				glColor3f(a, a, a);
				glBegin(GL_QUADS);
					glVertex3f(i*10, j*10, 0);
					glVertex3f(i*10, (j+1)*10, 0);
					glVertex3f((i+1)*10, (j+1)*10, 0);
					glVertex3f((i+1)*10, j*10, 0);
				glEnd();
				a=1.0-a;
			}
			
		}
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


void keyboardListener(unsigned char key, int x,int y){
	switch(key){
		case '1':	
			Camera.RotateX(5.0);
			break;
		case '2':	
			Camera.RotateX(-5.0);
			break;
		case '3':	
			Camera.RotateY(5.0);
			break;
		case '4':	
			Camera.RotateY(-5.0);
			break;
		case '5':	
			Camera.RotateZ(5.0);
			break;
		case '6':	
			Camera.RotateZ(-5.0);
			break;
		case '7':
			break;
		case '8':
			break;
		case '9':	
			break;
		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:
			Camera.MoveForward( 3.0 ) ;
			break;
		case GLUT_KEY_UP:
			Camera.MoveForward( -3.0 ) ;
			break;
		case GLUT_KEY_RIGHT:
			Camera.StrafeRight(3.0);
			break;
		case GLUT_KEY_LEFT:
			Camera.StrafeRight(-3.0);
			break;
		case GLUT_KEY_PAGE_UP:
			Camera.MoveUpward(3.0);
			break;
		case GLUT_KEY_PAGE_DOWN:
			Camera.MoveUpward(-3.0);
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


void mouseListener(int button, int state, int x, int y){
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN)drawaxes=1-drawaxes;	
			break;
		case GLUT_RIGHT_BUTTON:
			if(state == GLUT_DOWN)draw_now();
			break;
		case GLUT_MIDDLE_BUTTON:
			break;
		default:
			break;
	}
}

void draw_obj(float a,float b,float c,double radius,int type){
	glPushMatrix();
	glTranslatef(a,b,c);
	if(type==1)glutSolidSphere(radius,32,32);
	else {
		glTranslatef(radius/2,radius/2,radius/2);
		glutSolidCube(radius);
	}
	glPopMatrix();
}


void draw_all(){
	GLfloat unset[]={0,0,0,1};
	for (int i = 0; i < no_of_obj; ++i)
	{
		GLfloat a[4]={obj[i].coef_a,obj[i].coef_a,obj[i].coef_a,1.0};
		glMaterialfv(GL_FRONT, GL_AMBIENT,a);
		GLfloat d[4]={obj[i].coef_d,obj[i].coef_d,obj[i].coef_d,1.0};
		glMaterialfv(GL_FRONT, GL_DIFFUSE,d);
		GLfloat s[4]={obj[i].coef_s,obj[i].coef_s,obj[i].coef_s,1.0};
		glMaterialfv(GL_FRONT, GL_SPECULAR,s);
		GLfloat shine[]={obj[i].shine};
		glMaterialfv(GL_FRONT, GL_SHININESS, shine);
		glMaterialfv(GL_FRONT, GL_EMISSION, unset);
		glColor3f(obj[i].col_r,obj[i].col_g,obj[i].col_b);
		draw_obj(obj[i].pos_x,obj[i].pos_y,obj[i].pos_z,obj[i].rad,obj[i].type);
	}
}



void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Camera.Render();
	glMatrixMode(GL_MODELVIEW);


	GLfloat d[]={1,1,1,1.0};
	GLfloat s[]={1,1,1,1.0};

	for (int i = 0; i < no_of_lights; ++i)
	{
		GLfloat p[]={lights[i].pos_x,lights[i].pos_y,lights[i].pos_z, 1.0};
	    glLightfv(GL_LIGHT0+i, GL_DIFFUSE, d);
	    glLightfv(GL_LIGHT0+i, GL_SPECULAR, s);
	    glLightfv(GL_LIGHT0+i, GL_POSITION, p);
	}

	drawAxes();
	drawGrid();
	draw_all();
	glutSwapBuffers();
}

void animate(){
	glutPostRedisplay();
}

void input(){
	ifstream in("description.txt");
	in>>recursion_level;
	in>>window_size;
	in>>no_of_obj;
	for (int i = 0; i < no_of_obj; ++i)
	{
		in>>obj[i].type;
		in>>obj[i].pos_x>>obj[i].pos_y>>obj[i].pos_z;
		in>>obj[i].rad;
		in>>obj[i].col_r>>obj[i].col_g>>obj[i].col_b;
		in>>obj[i].coef_a>>obj[i].coef_d>>obj[i].coef_s>>obj[i].coef_r;
		in>>obj[i].shine;
	}
	in>>no_of_lights;
	for (int i = 0; i < no_of_lights; ++i)
	{
		in>>lights[i].pos_x>>lights[i].pos_y>>lights[i].pos_z;
	}
}

void init(){
	//codes for initialization
	drawgrid=1;
	drawaxes=1;
	Camera.Move(get3dVector(80,0,0));

	glClearColor(0,0,0,0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
    
    glEnable(GL_COLOR_MATERIAL);
   	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	
	//glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
	GLfloat lmodel_ambient[] = {.4,.4,.4,1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	for (int i = 0; i < no_of_lights; ++i)
	{
	    glEnable(GL_LIGHT0+i);
	}
	//glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(pers_ang,1,1,pers_far);
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	input();
	glutInitWindowSize(window_size, window_size);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("My OpenGL Program");
	init();
	glEnable(GL_DEPTH_TEST);	
	glutDisplayFunc(display);
	glutIdleFunc(animate);
	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();
	return 0;
}
