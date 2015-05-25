#include <windows.h>
#include <math.h>
#include <iostream>
#include "glut.h"

#define PI 3.1415
#define piby180 (PI/180.0)

struct Vector3d
{
	GLfloat x,y,z;
};

Vector3d get3dVector(GLfloat x, GLfloat y, GLfloat z )
{
	Vector3d tmp;
	tmp.x = x;
	tmp.y = y;
	tmp.z = z;
	return tmp;
}

GLfloat Getget3dVectorLength(Vector3d * v)
{
	return (GLfloat)(sqrt(v->x*v->x+v->y*v->y+v->z*v->z));
}

Vector3d Normalize3dVector(Vector3d v)
{
	Vector3d res;
	float l =Getget3dVectorLength(&v);
	if (l == 0.0f) return get3dVector(0.0f,0.0f,0.0f);
	res.x = v.x / l;
	res.y = v.y / l;
	res.z = v.z / l;
	return res;
}

Vector3d operator+(Vector3d v, Vector3d u)
{
	Vector3d res;
	res.x = v.x+u.x;
	res.y = v.y+u.y;
	res.z = v.z+u.z;
	return res;
}
Vector3d operator-(Vector3d v, Vector3d u)
{
	Vector3d res;
	res.x = v.x-u.x;
	res.y = v.y-u.y;
	res.z = v.z-u.z;
	return res;
}


Vector3d operator*(Vector3d v, float r)
{
	Vector3d res;
	res.x = v.x*r;
	res.y = v.y*r;
	res.z = v.z*r;
	return res;
}

Vector3d CrossProduct(Vector3d * u, Vector3d * v)
{
	Vector3d resVector;
	resVector.x = u->y*v->z - u->z*v->y;
	resVector.y = u->z*v->x - u->x*v->z;
	resVector.z = u->x*v->y - u->y*v->x;

	return resVector;
}
float operator* (Vector3d v, Vector3d u)
{
	return v.x*u.x+v.y*u.y+v.z*u.z;
}


class CAM
{
private:
	Vector3d dir;
	Vector3d RightVector;	
	Vector3d UpVector;
	Vector3d Position;
	GLfloat anglex, angley, anglez;	
public:
	CAM()
	{
		Position = get3dVector(1.0,0,0);
		dir = get3dVector(-1.0,0,0);
		RightVector = get3dVector(0.0,1.0,0.0);
		UpVector = get3dVector(0.0,0.0,1.0);
		anglex=angley=anglez=0.0;
	}
	void Move (Vector3d Direction)
	{
		Position =Position + Direction;
	}
	
	void MoveForward( GLfloat Distance )
	{
		Position = Position + (dir*-Distance);
	}
	void StrafeRight ( GLfloat Distance )
	{
		Position = Position + (RightVector*Distance);
	}

	void MoveUpward( GLfloat Distance )
	{
		Position = Position + (UpVector*Distance);
	}


	void RotateX (GLfloat Angle)
	{
		anglex += Angle;
		dir = Normalize3dVector(dir*cos(Angle*piby180)+ UpVector*sin(Angle*piby180));
		UpVector = CrossProduct(&dir, &RightVector)*-1;
	}

	void RotateY (GLfloat Angle)
	{
		angley += Angle;
		dir = Normalize3dVector(dir*cos(Angle*piby180)-RightVector*sin(Angle*piby180));
		RightVector = CrossProduct(&dir, &UpVector);
	}

	void RotateZ (GLfloat Angle)
	{
		anglez += Angle;
		RightVector = Normalize3dVector(RightVector*cos(Angle*piby180)+ UpVector*sin(Angle*piby180));
		UpVector = CrossProduct(&dir, &RightVector)*-1;
	}


	void Render( void )
	{
		Vector3d ViewPoint = Position+dir;
		gluLookAt(Position.x,Position.y,Position.z,ViewPoint.x,ViewPoint.y,ViewPoint.z,UpVector.x,UpVector.y,UpVector.z);
	}

};







