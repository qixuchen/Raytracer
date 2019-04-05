#include "ray.h"
#include "material.h"
#include "light.h"
#include <algorithm>
#include <cmath>
// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.
	
	vec3f c = ke;
	vec3f p = r.getPosition();
	vec3f d = r.getDirection();
	vec3f intersect = p + i.t*d;
	// Ia?

	for (list<Light*>::const_iterator light = scene->beginLights(); light != scene->endLights(); light++) {
		//diffuse
		vec3f color = (*light)->getColor(intersect);
		vec3f dir = (*light)->getDirection(intersect);
		vec3f atten = (*light)->distanceAttenuation(intersect)*(*light)->shadowAttenuation(intersect);
		vec3f di = prod(atten ,prod( color,kd))* max(0.0, i.N.dot(dir));
		c += di;

		//sepcular
		
		vec3f ref = dir - 2*dir.dot(i.N)*i.N;//reflection vector
		vec3f obs = -d;//observe direction
		vec3f sp = prod(atten ,prod(color,ks))*max(0.0, pow(ref.dot(obs),shininess));
		c += sp;
	}
	return c;
}
