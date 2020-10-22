#pragma once

struct Color
{
	float r, g, b, a;
	
	Color();

	Color(float r, float g, float b, float a = 1.0f);

	void Set(float r, float g, float b, float a = 1.0f);

	float* operator & ();

};

extern Color Red;
extern Color Green;
extern Color Blue;
extern Color Black;
extern Color White;
extern Color Purple;
