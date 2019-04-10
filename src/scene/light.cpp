#include <cmath>

#include "light.h"

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	
	vec3f d = -orientation.normalize();
	isect i;
	ray r(P, d);

	if (this->scene->intersect(r, i)) {
		if (i.t > RAY_EPSILON) {
			return i.getMaterial().kt;
		}
	}
	return vec3f(1, 1, 1);
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0
	double distance = (position - P).length();
	return min(1.0, 1.0 / (0.25 + 0.25*distance + 0.50*pow(distance, 2.0)));
}

vec3f PointLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	double distance = (position - P).length();
	vec3f d = (position - P).normalize();
	isect i;
	ray r(P, d);
	
	if (this->scene->intersect(r,i)) {
		if (i.t > RAY_EPSILON && i.t < distance) {
			return i.getMaterial().kt;
		}
	}
    return vec3f(1,1,1);
}
