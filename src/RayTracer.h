#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/scene.h"
#include "scene/ray.h"

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    vec3f trace( Scene *scene, double x, double y , int max_depth);
	vec3f traceRay( Scene *scene, const ray& r, const vec3f& thresh, int depth ,bool air, int max_depth, double curIntensity);


	void getBuffer( unsigned char *&buf, int &w, int &h );
	double aspectRatio();
	void traceSetup( int w, int h );
	void traceLines(int max_depth,int start = 0, int stop = 10000000);
	void tracePixel( int i, int j, int max_depth);

	void changeAmbient(double deg) {
		scene->changeDegreeOfAmbient(deg);
	}

	void setConstAttenCoeff(double const_coeff) {
		this->scene->setConstAttenCoeff(const_coeff);
	}
	void setLinearAttenCoeff(double const_coeff) {
		this->scene->setLinearAttenCoeff(const_coeff);
	}
	void setQuadraticAttenCoeff(double const_coeff) {
		this->scene->setQuadraticAttenCoeff(const_coeff);
	}
	void setMinIntensity(double min) {
		minIntensity = min;
	}

	void setSuperRes(int res) {
		superSample = res;
	}

	bool loadScene( char* fn );

	bool sceneLoaded();

private:
	unsigned char *buffer;
	int buffer_width, buffer_height;
	int bufferSize;
	int superSample;
	Scene *scene;

	bool m_bSceneLoaded;
	double minIntensity;
};

#endif // __RAYTRACER_H__
