#include <cmath>
#include <assert.h>

#include "Box.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.


	//get the position and direction
	double p0, p1, p2, d0, d1, d2;
	p0 = r.getPosition()[0];
	p1 = r.getPosition()[1];
	p2 = r.getPosition()[2];
	d0 = r.getDirection()[0];
	d1 = r.getDirection()[1];
	d2 = r.getDirection()[2];

	//  x  = p0 + td0
	//  y  = p1 + td1
	//  x  = p2 + td2
	double intersect = 0.0;
	
	if (d0 != 0) { // not parallel
		double x0, x1;
		//x0<x1
		if (d0 > 0) {
			x0 = (-0.5 - p0) / d0; 
			x1 = (0.5 - p0) / d0;
		}
		else {
			x1 = (-0.5 - p0) / d0;
			x0 = (0.5 - p0) / d0;
		}
		//corresponding y and z
		double y0 = p1 + d1 * x0;
		double y1 = p1 + d1 * x1;
		double z0 = p2 + d2 * x0;
		double z1 = p2 + d2 * x1;

		if (x0 > RAY_EPSILON) { // both are possible intersection
			if (y0 <= 0.5 && y0 >= -0.5 && z0 <= 0.5 && z0 >= -0.5) {
				intersect = x0;
				if (d0 < 0) {
					i.N = vec3f(1.0, 0.0, 0.0);
				}
				else {
					i.N = vec3f(-1.0, 0.0, 0.0);
				}
			}
			else if (y1 <= 0.5 && y1 >= -0.5 && z1 <= 0.5 && z1 >= -0.5) {
				intersect = x1;
				if (d0 < 0) {
					i.N = vec3f(-1.0, 0.0, 0.0);
				}
				else {
					i.N = vec3f(1.0, 0.0, 0.0);
				}
			}
		}
		else { //check x1 only
			if (x1 > RAY_EPSILON) {
				if (y1 <= 0.5 && y1 >= -0.5 && z1 <= 0.5 && z1 >= -0.5) {
					intersect = x1;
					if (d0 < 0) {
						i.N = vec3f(-1.0, 0.0, 0.0);
					}
					else {
						i.N = vec3f(1.0, 0.0, 0.0);
					}
				}
			}
		}
	}
	//do the same for y and z

	//y
	if (d1 != 0) { // not parallel
		double y0, y1;
		//y0<y1
		if (d1 > 0) {
			y0 = (-0.5 - p1) / d1;
			y1 = (0.5 - p1) / d1;
		}
		else {
			y1 = (-0.5 - p1) / d1;
			y0 = (0.5 - p1) / d1;
		}
		//corresponding x and z
		double x0 = p0 + d0 * y0;
		double x1 = p0 + d0 * y1;
		double z0 = p2 + d2 * y0;
		double z1 = p2 + d2 * y1;

		if (y0 > RAY_EPSILON) { // both are possible intersection
			if (x0 <= 0.5 && x0 >= -0.5 && z0 <= 0.5 && z0 >= -0.5) {
				if (intersect > RAY_EPSILON) { //if there's an intersection before...
					if (intersect > y0) { // check if this one is closer
						intersect = y0;
						if (d1 < 0) {
							i.N = vec3f(0.0, 1.0, 0.0);
						}
						else {
							i.N = vec3f(0.0, -1.0, 0.0);
						}
					}
				}
				else {                      //if there's no intersection, this is the intersection
					intersect = y0;
					if (d1 < 0) {
						i.N = vec3f(0.0, 1.0, 0.0);
					}
					else {
						i.N = vec3f(0.0, -1.0, 0.0);
					}
				}
			}
			else if (x1 <= 0.5 && x1 >= -0.5 && z1 <= 0.5 && z1 >= -0.5) {
				if (intersect > RAY_EPSILON) {
					if (intersect > y1) {
						intersect = y1;
						if (d1 < 0) {
							i.N = vec3f(0.0, -1.0, 0.0);
						}
						else {
							i.N = vec3f(0.0, 1.0, 0.0);
						}
					}
				}
				else {
					intersect = y1;
					if (d1 < 0) {
						i.N = vec3f(0.0, -1.0, 0.0);
					}
					else {
						i.N = vec3f(0.0, 1.0, 0.0);
					}
				}
			}
		}
		else { //check y1 only
			if (y1 > RAY_EPSILON) {
				if (x1 <= 0.5 && x1 >= -0.5 && z1 <= 0.5 && z1 >= -0.5) {
					if (intersect > RAY_EPSILON) {
						if (intersect > y1) {
							intersect = y1;
							if (d1 < 0) {
								i.N = vec3f(0.0, -1.0, 0.0);
							}
							else {
								i.N = vec3f(0.0, 1.0, 0.0);
							}
						}
					}
					else {
						intersect = y1;
						if (d1 < 0) {
							i.N = vec3f(0.0, -1.0, 0.0);
						}
						else {
							i.N = vec3f(0.0, 1.0, 0.0);
						}
					}
				}
			}
		}
	}

	//z
	if (d2 != 0) { // not parallel
		double z0, z1;
		//z0<z1
		if (d2 > 0) {
			z0 = (-0.5 - p2) / d2;
			z1 = (0.5 - p2) / d2;
		}
		else {
			z1 = (-0.5 - p2) / d2;
			z0 = (0.5 - p2) / d2;
		}
		//corresponding y and z
		double x0 = p0 + d0 * z0;
		double x1 = p0 + d0 * z1;
		double y0 = p1 + d1 * z0;
		double y1 = p1 + d1 * z1;

		if (z0 > RAY_EPSILON) { // both are possible intersection
			if (x0 <= 0.5 && x0 >= -0.5 && y0 <= 0.5 && y0 >= -0.5) {
				if (intersect > RAY_EPSILON) {
					if (intersect > z0) {
						intersect = z0;
						if (d2 < 0) {
							i.N = vec3f(0.0, 0.0, 1.0);
						}
						else {
							i.N = vec3f(0.0, 0.0, -1.0);
						}
					}
				}
				else {
					intersect = z0;
					if (d2 < 0) {
						i.N = vec3f(0.0, 0.0, 1.0);
					}
					else {
						i.N = vec3f(0.0, 0.0, -1.0);
					}
				}
			}
			else if (x1 <= 0.5 && x1 >= -0.5 && y1 <= 0.5 && y1 >= -0.5) {
				if (intersect > RAY_EPSILON) {
					if (intersect > z1) {
						intersect = z1;
						if (d2 < 0) {
							i.N = vec3f(0.0, 0.0, -1.0);
						}
						else {
							i.N = vec3f(0.0, 0.0, 1.0);
						}
					}
				}
				else {
					intersect = z1;
					if (d2 < 0) {
						i.N = vec3f(0.0, 0.0, -1.0);
					}
					else {
						i.N = vec3f(0.0, 0.0, 1.0);
					}
				}
			}
		}
		else { //check y1 only
			if (z1 > RAY_EPSILON) {
				if (x1 <= 0.5 && x1 >= -0.5 && y1 <= 0.5 && y1 >= -0.5) {
					if (intersect > RAY_EPSILON) {
						if (intersect > z1) {
							intersect = z1;
							if (d2 < 0) {
								i.N = vec3f(0.0, 0.0, -1.0);
							}
							else {
								i.N = vec3f(0.0, 0.0, 1.0);
							}
						}
					}
					else {
						intersect = z1;
						if (d2 < 0) {
							i.N = vec3f(0.0, 0.0, -1.0);
						}
						else {
							i.N = vec3f(0.0, 0.0, 1.0);
						}
					}
				}
			}
		}
	}

	if (intersect < RAY_EPSILON) {
		return false;
	}


	i.obj = this;
	i.t = intersect;
	return true;






}
