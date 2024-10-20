// ----------------------------------------------------
// Point class    -----------
// ----------------------------------------------------

#ifndef __P2POINT_H__
#define __P2POINT_H__

#include <math.h>

template<class TYPE>
class vec2
{
public:

	TYPE x, y;

	vec2()
	{}

	vec2(const vec2& p) : x(p.x), y(p.y)
	{}

	vec2(TYPE x, TYPE y) : x(x), y(y)
	{}

	// Math ------------------------------------------------
	vec2 operator -(const vec2 &v) const
	{
		vec2<TYPE> r;

		r.x = x - v.x;
		r.y = y - v.y;

		return(r);
	}

	vec2 operator + (const vec2 &v) const
	{
		vec2<TYPE> r;

		r.x = x + v.x;
		r.y = y + v.y;

		return(r);
	}

	const vec2& operator -=(const vec2 &v)
	{
		x -= v.x;
		y -= v.y;

		return(*this);
	}

	const vec2& operator +=(const vec2 &v)
	{
		x += v.x;
		y += v.y;

		return(*this);
	}

	bool operator ==(const vec2& v) const
	{
		return (x == v.x && y == v.y);
	}

	bool operator !=(const vec2& v) const
	{
		return (x != v.x || y != v.y);
	}

	// Utils ------------------------------------------------
	bool IsZero() const
	{
		return (x == 0 && y == 0);
	}

	vec2& SetToZero()
	{
		x = y = 0;
		return(*this);
	}

	vec2& Negate()
	{
		x = -x;
		y = -y;

		return(*this);
	}

	// Distances ---------------------------------------------
	TYPE DistanceTo(const vec2& v) const
	{
		TYPE fx = x - v.x;
		TYPE fy = y - v.y;

		return (TYPE) sqrtf(float((fx*fx) + (fy*fy)));
	}

	void Normalize()
	{
		float m = x * x + y * y;
		if (m > 0.0f)
		{
			float inv_m = 1 / sqrtf(m);
			x *= inv_m;
			y *= inv_m;
		}
	}

	float Length() const
	{
		return sqrtf(float((x * x) + (y * y)));
	}

	TYPE dot(const vec2<TYPE>& v) const
	{
		return(x * v.x + y * v.y);
	}

	vec2<TYPE>& operator*=(float a)
	{
		x *= a;
		y *= a;
		return(*this);
	}

	vec2<TYPE>& operator*(float a)
	{
		p2Vector<TYPE> r;

		r.x = x * a;
		r.y = y * a;

		return r;
	}

	vec2<TYPE>& operator/=(float a)
	{
		float f = 1.0f / a;
		x *= f;
		y *= f;
		return(*this);
	}
};

using vec2i = vec2<int>;
using vec2f = vec2<float>;


#endif // __P2POINT_H__