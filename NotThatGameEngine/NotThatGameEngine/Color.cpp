#include "Color.h"

Color Red = Color(1.0f, 0.0f, 0.0f);
Color Green = Color(0.0f, 1.0f, 0.0f);
Color Blue = Color(0.0f, 0.0f, 1.0f);
Color Purple = Color(0.5f, 0.0f, 0.5f);
Color Black = Color(0.0f, 0.0f, 0.0f);
Color White = Color(1.0f, 1.0f, 1.0f);

Color::Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}

Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

void Color::Set(float r, float g, float b, float a)
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

float* Color::operator & () { return (float*)this; }
