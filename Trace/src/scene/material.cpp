#include "ray.h"
#include "material.h"
#include "light.h"

#include "../fileio/imageio.h"

using namespace std;
extern bool debugMode;


// Apply the Phong model to this point on the surface of the object, returning
// the color of that point.
Vec3d Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	//这个函数返回了物体反射的颜色
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the Phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.
	if( debugMode )
		std::cout << "Debugging the Phong code (or lack thereof...)" << std::endl;

	// When you're iterating through the lights,
	// you'll want to use code that looks something
	// like this:
	//
	// for ( vector<Light*>::const_iterator litr = scene->beginLights(); 
	// 		litr != scene->endLights(); 
	// 		++litr )
	// {
	// 		Light* pLight = *litr;
	// 		.
	// 		.
	// 		.
	// }
	//phong照明模型
	Vec3d light = ke(i); 
	Vec3d normal = i.N;   //交点处的法向量
	Vec3d iDot = r.at(i.t); //p+t*d,起始于p，传播t*d的距离
	//L=ke+kala
	//prod是两个三维向量的乘积，环境光
	if (r.getDirection() * normal > 0) {
		normal = -normal;
		light += prod(prod(scene->ambient(), ka(i)), kt(i));  //ambient 环境 transmissive 透射
	}
	else {
		light += prod(scene->ambient(), ka(i));
	}

	//对每一个光源
	for (vector<Light*>::const_iterator litr = scene->beginLights();
		litr != scene->endLights();
		++litr)
	{
		Light* pLight = *litr;
		//L=ke+kala+As*Ad*L*(kdIl(N*L)+ksIl(N*H)^ns)
		//物体表面上点P?的法向为N?，从点P指向光源的向量为L
		//ns为镜面高光指数，H为L和V的中间单位向量
		double distAttenuation = pLight->distanceAttenuation(iDot); // 距离衰减
		Vec3d shadowAttenuation = pLight->shadowAttenuation(iDot);
		Vec3d atten = distAttenuation * shadowAttenuation;
		Vec3d L = pLight->getDirection(iDot);  //光源到交点


		if (L * normal > 0) {
			// H是光源方向和视线(r)方向的中线
			Vec3d H = (L + -1 * r.getDirection());
			if (H.length() != 0)
				H.normalize();

			double sDot = max(0.0, normal * H);
			Vec3d dTerm = kd(i) * (normal * L); //diffuse 漫反射
			Vec3d sTerm = ks(i) * (pow(sDot, shininess(i))); //specular 镜面反射
			Vec3d newLight = dTerm + sTerm;
			newLight = prod(newLight,pLight->getColor());

			light += prod(atten, newLight);
		}
	}
	return light;

	//return kd(i);
}


TextureMap::TextureMap( string filename )
{
    data = load( filename.c_str(), width, height );
    if( 0 == data )
    {
        width = 0;
        height = 0;
        string error( "Unable to load texture map '" );
        error.append( filename );
        error.append( "'." );
        throw TextureMapException( error );
    }
}

Vec3d TextureMap::getMappedValue( const Vec2d& coord ) const
{
	// YOUR CODE HERE

    // In order to add texture mapping support to the 
    // raytracer, you need to implement this function.
    // What this function should do is convert from
    // parametric space which is the unit square
    // [0, 1] x [0, 1] in 2-space to Image coordinates,
    // and use these to perform bilinear interpolation
    // of the values.
	// 双线性插值

	//线性插值：已知数据 (x0, y0) 与 (x1, y1)，要计算 [x0, x1] 区间内某一位置 x 在直线上的y值
	//(y-y0)/(x-x0)=(y1-y0)/(x1-x0)
	//双线性插值：在数学上，双线性插值是有两个变量的插值函数的线性插值扩展，其核心思想是在两个方向分别进行一次线性插值

	double xReal = coord[0] * width; // 0 <= xReal <= N-1
	double yReal = coord[1] * height; // 0 <= yReal <= M-1
	int x0 = int(xReal), y0 = int(yReal);
	double dx = xReal - x0, dy = yReal - y0, omdx = 1 - dx, omdy = 1 - dy;
	//f(x,y)=f(0,0)(1-x0(1-y)+f(0,1)(1-x)y+f(1,0)(1-y)x+f(1,1)xy
	Vec3d bilinear = omdx * omdy*getPixelAt(x0, y0) + omdx * dy*getPixelAt(x0, y0 + 1) +
		dx * omdy*getPixelAt(x0 + 1, y0) + dx * dy*getPixelAt(x0 + 1, y0 + 1);
	return bilinear;


    //return Vec3d(1.0, 1.0, 1.0);
}


Vec3d TextureMap::getPixelAt( int x, int y ) const
{
    // This keeps it from crashing if it can't load
    // the texture, but the person tries to render anyway.
    if (0 == data)
      return Vec3d(1.0, 1.0, 1.0);

    if( x >= width )
       x = width - 1;
    if( y >= height )
       y = height - 1;

    // Find the position in the big data array...
    int pos = (y * width + x) * 3;
    return Vec3d( double(data[pos]) / 255.0, 
       double(data[pos+1]) / 255.0,
       double(data[pos+2]) / 255.0 );
}

Vec3d MaterialParameter::value( const isect& is ) const
{
    if( 0 != _textureMap )
        return _textureMap->getMappedValue( is.uvCoordinates );
    else
        return _value;
}

double MaterialParameter::intensityValue( const isect& is ) const
{
    if( 0 != _textureMap )
    {
        Vec3d value( _textureMap->getMappedValue( is.uvCoordinates ) );
        return (0.299 * value[0]) + (0.587 * value[1]) + (0.114 * value[2]);
    }
    else
        return (0.299 * _value[0]) + (0.587 * _value[1]) + (0.114 * _value[2]);
}

