#include <cmath>

#include "Sphere.h"

using namespace std;


bool Sphere::intersectLocal( const ray& r, isect& i ) const
{
    // YOUR CODE HERE:
    // Add sphere intersection code here.
    // it currently ignores all spheres and just return false.

	Vec3d position = r.getPosition(); 
	Vec3d direction = r.getDirection(); 
	//球心在原点

	//计算3个系数
	double A = direction[0] * direction[0] +
		direction[1] * direction[1] +
		direction[2] * direction[2];
	double B = 2 * (position[0] * direction[0] +
		position[1] * direction[1] +
		position[2] * direction[2]);
	double C = position[0] * position[0] +
		position[1] * position[1] +
		position[2] * position[2] - 1;

	double dis = B * B - 4 * A * C;

	if (dis < 0)
		return false;
	dis = sqrt(dis);

	double t1 = (-B + dis) / (2 * A);
	double t2 = (-B - dis) / (2 * A);
	double t;
	if (t1 < RAY_EPSILON && t2 < RAY_EPSILON)
		return false;
	else if (t1 < RAY_EPSILON)
		t = t2;
	else if (t2 < RAY_EPSILON)
		t = t1;
	else
		t = min(t1, t2);

	i.obj = this;
	i.setT(t);
	i.setN(r.at(t));
	i.N.normalize();
	//得到交点

	Vec3d P = r.at(t);
	i.setUVCoordinates(Vec2d(P[0] + 0.5, P[1] + 0.5));
	return true;
    //return false;
}

