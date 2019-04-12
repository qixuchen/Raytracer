#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"

class Light
	: public SceneElement
{
public:
	virtual vec3f shadowAttenuation(const vec3f& P) const = 0;
	virtual double distanceAttenuation( const vec3f& P ) const = 0;
	virtual vec3f getColor( const vec3f& P ) const = 0;
	virtual vec3f getDirection( const vec3f& P ) const = 0;
	void set_const_coeff(double);
	void set_linear_coeff(double);
	void set_quadratic_coeff(double);
	double get_const_coeff() {
		return constant_attenuation_coeff;
	}
	double get_linear_coeff() {
		return linear_attenuation_coeff;
	}
	double get_quadratic_coeff() {
		return quadratic_attenuation_coeff;
	}
protected:
	Light( Scene *scene, const vec3f& col )
		: SceneElement( scene ), color( col ), constant_attenuation_coeff(0.25), linear_attenuation_coeff(0.25), quadratic_attenuation_coeff(0.5) {}

	vec3f 		color;
	double constant_attenuation_coeff;
	double linear_attenuation_coeff;
	double quadratic_attenuation_coeff;
};

class DirectionalLight
	: public Light
{
public:
	DirectionalLight( Scene *scene, const vec3f& orien, const vec3f& color )
		: Light( scene, color ), orientation( orien ) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f 		orientation;
};

class PointLight
	: public Light
{
public:
	PointLight( Scene *scene, const vec3f& pos, const vec3f& color )
		: Light( scene, color ), position( pos ) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;



protected:
	vec3f position;

};

#endif // __LIGHT_H__
