#include <cmath>

#include "light.h"


using namespace std;

double DirectionalLight::distanceAttenuation( const Vec3d& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


Vec3d DirectionalLight::shadowAttenuation( const Vec3d& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	Scene* scene = this->getScene();
	Vec3d d = this->getDirection(P);
	Vec3d atten = Vec3d(1, 1, 1);
	isect iDot;
	ray r = ray(P, d, r.SHADOW);  //阴影光

	//透明的物体，中间的kt都点乘起来
	while (scene->intersect(r, iDot) && iDot.t > RAY_EPSILON) {
		const Material& m = iDot.getMaterial();
		atten = prod(atten, m.kt(iDot));
		if (clamp(atten).iszero())
			break;
		r = ray(r.at(iDot.t), d, r.SHADOW);
	}

	return atten;
    //return Vec3d(1,1,1);
}

/*Vec3d DirectionalLight::getColor( const Vec3d& P ) const
{
	// Color doesn't depend on P 
	return color;
}*/

//改正
Vec3d DirectionalLight::getColor() const
{
	// Color doesn't depend on P 
	return color;
}

Vec3d DirectionalLight::getDirection( const Vec3d& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const Vec3d& P ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, we assume no attenuation and just return 1.0
	double d = (P - position).length();
	return min(1, max(0.0, 1 / (constantTerm + linearTerm * d + quadraticTerm * d*d)));
	//return 1.0;
}

/*Vec3d PointLight::getColor( const Vec3d& P ) const
{
	// Color doesn't depend on P 
	return color;
}*/

//改正
Vec3d PointLight::getColor() const
{
	// Color doesn't depend on P 
	return color;
}

Vec3d PointLight::getDirection( const Vec3d& P ) const
{
	Vec3d ret = position - P;
	ret.normalize();
	return ret;
}


Vec3d PointLight::shadowAttenuation(const Vec3d& P) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	Scene* scene = this->getScene(); //场景
	Vec3d d = this->getDirection(P); //方向
	Vec3d atten = Vec3d(1, 1, 1);
	isect iDot; //交点
	ray r = ray(P, d, r.SHADOW);

	double t_light = (position - P).length();
	while (scene->intersect(r, iDot) && iDot.t > RAY_EPSILON) {  //p+t*d是交点
		if (iDot.t < t_light + RAY_EPSILON) {  //点光源多了这一步
			const Material& m = iDot.getMaterial();
			atten = prod(atten, m.kt(iDot)); //transmissive 透射
			if (clamp(atten).iszero())
				break;
			r = ray(r.at(iDot.t), d, r.SHADOW);
		}
		else {
			break;
		}
	}
	return atten;
    //return Vec3d(1,1,1);
}
