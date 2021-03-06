// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y ,int maxdepth)
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );
	return traceRay( scene, r, vec3f(1.0,1.0,1.0), 0 ,true, maxdepth, 1).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth , bool air,int max_depth, double curIntensity)
{
	if (depth >= max_depth+1 || curIntensity < minIntensity) {//stop recursion
		return vec3f(0.0, 0.0, 0.0);
	}

	isect i;

	if( scene->intersect( r, i ) ) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		const Material& m = i.getMaterial();
		vec3f color = m.shade(scene, r, i);
		vec3f pos = r.getPosition() + r.getDirection()*i.t;//intersection position
		
		if (m.kr[0] > 0 || m.kr[1] > 0 || m.kr[2] > 0) {//reflection
			vec3f L = -r.getDirection().normalize();
			vec3f R = 2 * L.dot(i.N)*i.N - L; // reflection angle
			ray ref(pos, R);
			double intensity = curIntensity * (m.kr[0] + m.kr[1] + m.kr[2]) / 3;
			isect nextStep;
			if (scene->intersect(ref, nextStep)) {
				vec3f nextPoint = ref.at(nextStep.t);
				double distance = (pos - nextPoint).length();
				intensity /= pow(distance, 2);
			}
			color += prod(traceRay(scene, ref, thresh, depth+1,air, max_depth, intensity), m.kr);
		}
		
		if (m.kt[0] > 0 || m.kt[1] > 0 || m.kt[2] > 0) {//refraction
			double Ri, Rr;
			if (air) {
				Ri = 1.0;
				Rr = m.index;
			}
			else {
				Ri = m.index;
				Rr = 1.0;
				i.N = -i.N;
			}
			vec3f L = -r.getDirection().normalize();
			double Ni = acos(L.dot(i.N));
			double sinNi = sin(Ni);
			if (Ri*sinNi / Rr < 1.0) {//not TIR
				double Nr = asin(Ri*sinNi / Rr);
				double cosNi = cos(Ni);
				double tanNr = tan(Nr);
				
				vec3f refraction_sym = L - (1 - (cosNi*tanNr) / (sinNi+RAY_EPSILON))*(L-L.dot(i.N)*i.N);
				vec3f refraction = -refraction_sym.normalize();
				ray ref(pos, refraction);
				vec3f result = traceRay(scene, ref, thresh, depth+1, !air,max_depth, curIntensity * (m.kt[0]+m.kt[1]+m.kt[2])/3);
				color += prod(result, m.kt);
				

			}

		}
		
		return color;
	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		return vec3f( 0.0, 0.0, 0.0 );
	}
}

RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;

	m_bSceneLoaded = false;
	minIntensity = 0;
	superSample = 1;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines(int max_depth, int start, int stop)
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j,max_depth);
}

void RayTracer::tracePixel( int i, int j ,int max_depth)
{
	vec3f col;
	int previ = i;
	int prevj = j;

	if( !scene )
		return;

	i -= superSample / 2;
	j -= superSample / 2;
	int tempj = j;
	int valid = 0;

	for (int index = 0; index < this->superSample; index++) {
		for (int index2 = 0; index2 < this->superSample; index2++) {
			if (i < 0 || j < 0 || i > buffer_width || j > buffer_width) {
				continue;
				j++;
			}
			double x = double(i) / double(buffer_width);
			double y = double(j) / double(buffer_height);

			col += trace(scene, x, y, max_depth);
			valid++;
			j++;
		}
		i++;
		j = tempj;
	}

	col /= valid;
	i = previ;
	j = prevj;
	
	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}