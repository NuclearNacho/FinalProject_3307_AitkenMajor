/*


    This program is distributed for education purposes only and WITHOUT ANY WARRANTY.

    This file is governed by the GNU General Public License.

		Copyright 2014 Doron Nussbaum

*/

/**********************************************************************************************/
/*
REVISIONS
1. November 23 2015 - avoid normalizing vector(0,0,0)

*/

/***********************************************************************************************/

#ifndef NUSS_POINT
#define	NUSS_POINT

#include <stdio.h>
#define _USE_MATH_DEFINES // for C++. The define is used to load the constants
#include <math.h>
#include "nuss_vector.h"
#include "assert.h"
#include "iostream"

#define DegreeToRadians(x) (float)(((x) * M_PI / 180.0f))
#define RadianToDegrees(x) (float)(((x) * 180.0f / M_PI))

//float RandomFloat();


struct Point2f
{
    float x;
    float y;

    Point2f()
    {
        x=0;
        y=0;
    }

	Point2f(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

	// unary operators
	Point2f operator-()
	{
		return(Point2f(-x, -y));
	}

};

////////////////////////////////////////////////////////////////////////////////////////

struct Point3f
{
	float x;
	float y;
	union {
		float z;
		float w;
	};

	// constructors 
	Point3f()
	{
		x = 0;
		y = 0;
		z = 0;
	}


	Point3f(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Point3f(Point2f u, float z)
	{
		this->x = u.x;
		this->y = u.y;
		this->z = z;
	}

	// unary operators
	Point3f operator-()
	{
		return(Point3f(-x, -y, -z));
	}


	 
	Point3f operator+(const Point3f& u) const
	{
		return(Point3f(x + u.x, y + u.y, z + u.z));
	}

	// binary operators 
	Point3f operator+(const Vector3f& u) const
	{
		return(Point3f(x + u.x, y + u.y, z + u.z));
	}


	Vector3f operator-(const Point3f& u) const
	{
		return(Vector3f(x - u.x, y - u.y, z - u.z));
	}



	Point3f operator/(const float &r) const
	{
		return(Point3f(x/r, y/r, z/r));
	}




	Point3f& operator+=(const Vector3f& r)
	{
		x += r.x;
		y += r.y;
		z += r.z;

		return *this;
	}
	

	Point3f& operator+=(const Point3f& r)
	{
		x += r.x;
		y += r.y;
		z += r.z;

		return *this;
	}

	Point3f& operator/=(const float& r)
	{
		x /= r;
		y /= r;
		z /= r;

		return *this;
	}






	// prints the vector 
	friend std::ostream& operator << (std::ostream& os, const Point3f& v)

	{
		os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
		return(os);
	}

};

typedef struct Point3f Point3f;


///////////////////////////////////////////////////////////////////////////////////

/* Vector4f is geared towards homogenous coordinates.  Therefore, it is not the same as a four dimension vectors.
For example if v=(1,1,0,1) and u=(1,0,0,1) the result of v-u should not be (0,1,0,0) since this is not defined as
homogenous coordinates.  The result of v-u should be (0,1,0,1).
Similarly u+v is (2,1,0,1) and not (2,1,0,2).  The latter vector is not the same.
*/

// #define VECTOR4F_HOMOGENOUS(v) {assert(v.w);v = Vector4f(v.x/v.w, v.y/v.w,v.z/v.w,1)}
struct Point4f
{
    float x;
    float y;
    float z;
    float w;
    
    
    
	// constructors
	Point4f()
    {

    }

	Point4f(float f)
    {
        x= y= z= w=f;
    }
    
    
	Point4f(float x, float y, float z, float w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    
	Point4f(Point2f u, float z, float w)
    {
        this->x = u.x;
        this->y = u.y;
        this->z = z;
        this->w = w;
    }
    
	Point4f(Point3f u, float w)
    {
        this->x = u.x;
        this->y = u.y;
        this->z = u.z;
        this->w = w;
    }

	Point4f setValue(float f)
    {
        x= y= z= w=f;
    }



	// unary operators
	Point4f operator-()
    {
        return(Point4f(-x,-y,-z,-w));
    }
    
    
    // binary operators
	Point4f operator+(const Vector4f& u) const
    {
        return(Point4f(x+u.x,y +u.y, z+u.z, w+u.w));
    }
    
    
	Vector4f operator-(const Point4f& u) const
    {
        return(Vector4f(x-u.x, y-u.y, z-u.z, w-u.w));
    }
    
    
      
    
	Point4f& operator+=(const Vector4f& r)
    {
        x += r.x;
        y += r.y;
        z += r.z;
        w += r.w;
        return *this;
    }
    
    
	Point4f& operator-=(const Vector4f& r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;
        w -= r.w;
        
        return *this;
    }
    
 
    
	Point3f to3D()
	{
		assert(this->w);
		return(Point3f(this->x/this->w, this->y/ this->w, this->z/ this->w));
	}
    
  
    
	
	// prints the vector
    friend std::ostream& operator << (std::ostream& os, const Point4f& v)
    
    {
        os <<"("<<v.x <<", "<<  v.y <<", "<< v.z <<", "<< v.w <<")";
        return(os);
    }
   

    
};

typedef struct Point4f Point4f;





///////////////////////////////////////////////////////////////////////////////



#endif