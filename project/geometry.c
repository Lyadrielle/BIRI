/**
 * @file		geometry.c
 *       		display functions library. Centralise all basic display functions,
 * 			    basic shapes non dependent of the gameplay.
 * @author	Calmels Gaëlle, Gallet Adrian
 * @version	1.0
 * @date		2017-04-17
 */

#include <assert.h>
#include <math.h>
#include <SDL/SDL.h>

#include "headers.h"

/**
 * Initialise a 2D 	point with x and y float params.
 * @param	Point2D*	p	the point pointer to be initialised
 * @param	float			x	x value
 * @param	float			y	y value
 */
void initPoint2D(Point2D *p, float x, float y) {
	p->x = x;
	p->y = y;
}

/**
 * Initialise a RGB color. (return normalized values between 0.0 an 1.0)
 * @param	Color3f*	c	the color pointer to be initialised
 * @param	float			r	red value
 * @param	float			g	green value
 * @param	float			b	blue value
 */
void initColor3f(Color3f *c, float r, float g, float b) {
	c->r = r / 255;
	c->g = g / 255;
	c->b = b / 255;
}

/**
 * Initialise a 2D vector (same as the 2D point struct but applied to speeds)
 * @param	Vector2D*	v	the vector pointer to be initialised
 * @param	float			x	the x value
 * @param	float			y	the y value
 */
void initVector2D (Vector2D *v, float x, float y){
	v->x = x;
	v->y = y;
}

/**
 * Define a 2D vector from two 2D points.
 * @param		Point2D		A	start 2D point
 * @param		Point2D		B	end 2D point
 * @return	Vector2D		return a 2D vector
 */
Vector2D defineVector(Point2D a, Point2D b) {
	Vector2D vector;
	vector.x = b.x - a.x;
	vector.y = b.y - a.y;
	return vector;
}

/**
 * Add a 2D vector to a 2D point. Return a new point
 * @param		Point2D		p	the 2D point
 * @param		Vector2D	v	the 2D vector
 * @return	Point2D			the new 2D point
 */
Point2D pointPlusVector( Point2D p, Vector2D v){
	Point2D point;
	point.x = p.x + v.x;
	point.y = p.y + v.y;
	return point;
}

/**
 * Add two 2D vectors.
 * @param		Vector2D	vA	first 2D vector
 * @param		Vector2D	vB	second 2D vector
 * @return	Vector2D			vector	return the new 2D vector
 */
Vector2D addVectors(Vector2D vA, Vector2D vB){
	Vector2D vector;
	vector.x = vA.x + vB.x;
	vector.y = vA.y + vB.y;
	return vector;
}

/**
 * Substract two 2D vectors.
 * @param		Vector2D	vA			first 2D vector
 * @param		Vector2D	vB			second 2D vector
 * @return	Vector2D	vector	return the new 2D vector
 */
Vector2D subVectors(Vector2D vA, Vector2D vB){
	Vector2D vector;
	vector.x = vA.x - vB.x;
	vector.y = vA.y - vB.y;
	return vector;
}

/**
 * Multiply a 2D vectors with a constant.
 * @param		Vector2D	v				the 2D vector
 * @param		float			a				the constant
 * @return	Vector2D	vector	return the new 2D vector
 */
Vector2D multVector(Vector2D v, float a){
	Vector2D vector;
	vector.x = v.x * a;
	vector.y = v.y * a;
	return vector;
}

/**
 * Divide a 2D vectors with a constant.
 * @param		Vector2D	v				the 2D vector
 * @param		float			a				the constant
 * @return	Vector2D	vector	return the new 2D vector
 */
Vector2D divVector(Vector2D v, float a){
	assert(a != 0.f);
	Vector2D vector;
	vector.x = v.x / a;
	vector.y = v.y / a;
	return vector;
}

/**
 * Calculate the dot product of two 2D vectors.
 * @param		Vector2D	vA	first 2D vector
 * @param		Vector2D	vB	second 2D vector
 * @return	float					return the dot product value
 */
float dotPRoduct(Vector2D vA, Vector2D vB){
	return vA.x * vB.x + vA.y * vB.y;
}

/**
 * Calculate the norm of a 2D vector.
 * @param		Vector2D	v	2D vector
 * @return	float				return sqrt(x²+y²)
 */
float norm(Vector2D v){
	return sqrt(v.x * v.x + v.y * v.y);
}

/**
 * Normalise a 2D vector.
 * @param		Vector2D	v	2D vector
 * @return	Vector2D		return the normalized 2D vector
 */
Vector2D normalize(Vector2D v){
	return divVector(v, norm(v));
}

/**
 * Calculate the distance between two 2D points.
 * @param		Point2D	a	first 2D point
 * @param		Point2D	b	second 2D point
 * @return	float			return the distance between A & B
 */
float distance(Point2D a, Point2D b) {
	Vector2D v;
	v.x = b.x - a.x;
	v.y = b.y - a.y;
	return norm(v);
}
