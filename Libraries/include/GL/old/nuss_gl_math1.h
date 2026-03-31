/*


    This program is distributed for education purposes only and WITHOUT ANY WARRANTY.

    This file is governed by the GNU General Public License.

		Copyright 2014 Doron Nussbaum

*/

#ifndef NUSS_MATH_GL
#define	NUSS_MATH_GL


#include <stdio.h>
#include <math.h>
#include "assert.h"

#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))

//float RandomFloat();


struct Vector2f
{
    float x;
    float y;

    Vector2f()
    {
        x=0;
        y=0;
    }

    Vector2f(float x, float y)
    {
        this->x = x;
        this->y = y;
    }
};

////////////////////////////////////////////////////////////////////////////////////////

struct Vector3f
{
    float x;
    float y;
    float z;

	// constructors 
    Vector3f()
    {
        x=0;
        y=0;
        z=0;
    }

    
    Vector3f(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    
    Vector3f(Vector2f u, float z)
    {
        this->x = u.x;
        this->y = u.y;
        this->z = z;
    }
    
	// unary operators
    Vector3f operator-()
    {
        return(Vector3f(-x,-y,-z));
    }


    // binary operators 
    Vector3f operator+(const Vector3f& u) const
    {
        return(Vector3f(x+u.x,y +u.y, z+u.z));
    }
    
    
    Vector3f operator-(const Vector3f& u) const
    {
        return(Vector3f(x-u.x, y-u.y, z-u.z));
    }
    

    Vector3f operator*(const Vector3f& u) const
    {
        return(Vector3f(x*u.x, y*u.y, z*u.z));
    }
    

    Vector3f operator*(const float f) const
    {
        return(Vector3f(x*f, y*f, z*f));
    }
    
    Vector3f operator/(const float f) const
    {
        assert(f!=0);
        return(Vector3f(x/f, y/f, z/f));
    }
    

    friend Vector3f operator*(const float f, const Vector3f& u)
    {
        return(u*f);
    }
    

    Vector3f& operator+=(const Vector3f& r)
    {
        x += r.x;
        y += r.y;
        z += r.z;
        
        return *this;
    }

    
    Vector3f& operator-=(const Vector3f& r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;

        return *this;
    }

    Vector3f& operator*=(float f)
    {
        x *= f;
        y *= f;
        z *= f;

        return *this;
    }

	Vector3f& operator/=(float f)
    {
        assert(f!=0);
        x /= f;
        y /= f;
        z /= f;

        return *this;
    }



   // returns the magnitude of the vector    
    float length()
    {
        return(sqrt(this->x*this->x+this->y*this->y+this->z*this->z));
        
    }


	// normalizes the vector
    void normalize()
    {
        float length = this->length();
        *this /= length;
    }
    

	
	// prints the vector 
    friend std::ostream& operator << (std::ostream& os, const Vector3f& v)
    
    {
        os <<"("<<v.x <<", "<<  v.y <<", "<< v.z<<")";
        return(os);
    }


    
};

/*
inline Vector3f operator+(const Vector3f& l, const Vector3f& r)
{
    Vector3f Ret(l.x + r.x,
                 l.y + r.y,
                 l.z + r.z);

    return Ret;
}
*/
/*
inline Vector3f operator-(const Vector3f& l, const Vector3f& r)
{
    Vector3f Ret(l.x - r.x,
                 l.y - r.y,
                 l.z - r.z);

    return Ret;
}
*/

/*
inline Vector3f operator*(const Vector3f& l, float f)
{
    Vector3f Ret(l.x * f,
                 l.y * f,
                 l.z * f);

    return Ret;
}
*/

/*
inline Vector3f operator*(float f, const Vector3f& l)
{
    Vector3f Ret(l.x * f,
                 l.y * f,
                 l.z * f);
    
    return Ret;
}
*/

/*
inline Vector3f operator/(const Vector3f& l, float f)
{
    Vector3f Ret(l.x / f,
                 l.y / f,
                 l.z / f);
    
    return Ret;
}
*/
 
 
inline float dot(const Vector3f& u, const Vector3f& v)
{
    return(u.x * v.x + u.y*v.y + u.z*v.z);
}


inline float length(const Vector3f& u)
{
    return(sqrt(dot(u,u)));
    ;
}


Vector3f normalize(const Vector3f& u)
{
    float len = length(u);
    return(u/len);
}

Vector3f cross(const Vector3f& u,  const Vector3f& v)
{
    Vector3f vv;
    vv.x =u.y*v.z-v.y*u.z;
    vv.y=-1*(u.x*v.z-v.x*u.z);
    vv.z =u.x*v.y-v.x*u.y;
    return(Vector3f(u.y*v.z-v.y*u.z, -1*(u.x*v.z-v.x*u.z), u.x*v.y-v.x*u.y));
}


float cross_value_value(const Vector3f& u,  const Vector3f& v)
{
    return(u.y*v.z-v.y*u.z + -1*(u.x*v.z-v.x*u.z) + u.x*v.y-v.x*u.y);
    
}


///////////////////////////////////////////////////////////////////////////////////

struct Vector4f
{
    float x;
    float y;
    float z;
    float w;
    
    
    
	// constructors
    Vector4f()
    {

    }

    Vector4f(float f)
    {
        x= y= z= w=f;
    }
    
    
    Vector4f(float x, float y, float z, float w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    
    Vector4f(Vector2f u, float z, float w)
    {
        this->x = u.x;
        this->y = u.y;
        this->z = z;
        this->w = w;
    }
    
    Vector4f(Vector3f u, float w)
    {
        this->x = u.x;
        this->y = u.y;
        this->z = u.z;
        this->w = w;
    }

	Vector4f setValue(float f)
    {
        x= y= z= w=f;
    }



	// unary operators
    Vector4f operator-()
    {
        return(Vector4f(-x,-y,-z,-w));
    }
    
    
    // binary operators
    Vector4f operator+(const Vector4f& u) const
    {
        return(Vector4f(x+u.x,y +u.y, z+u.z, w+u.w));
    }
    
    
    Vector4f operator-(const Vector4f& u) const
    {
        return(Vector4f(x-u.x, y-u.y, z-u.z, w-u.w));
    }
    
    
    Vector4f operator*(const Vector4f& u) const
    {
        return(Vector4f(x*u.x, y*u.y, z*u.z, w*u.w));
    }
    
    
    Vector4f operator*(const float f) const
    {
        return(Vector4f(x*f, y*f, z*f, w*f));
    }
    
    Vector4f operator/(const float f) const
    {
        assert(f!=0);
        return(Vector4f(x/f, y/f, z/f, w/f));
    }
    
    
    friend Vector4f operator*(const float f, const Vector4f& u)
    {
        return(u*f);
    }
    
    
    Vector4f& operator+=(const Vector4f& r)
    {
        x += r.x;
        y += r.y;
        z += r.z;
        w += r.w;
        return *this;
    }
    
    
    Vector4f& operator-=(const Vector4f& r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;
        w -= r.w;
        
        return *this;
    }
    
    Vector4f& operator*=(float f)
    {
        x *= f;
        y *= f;
        z *= f;
        w *= f;
        return *this;
    }
    
	Vector4f& operator/=(float f)
    {
        assert(f!=0);
        x /= f;
        y /= f;
        z /= f;
        w /= f;
        
        return *this;
    }
    
    
    
    // returns the magnitude of the vector
    float length()
    {
        return(sqrt(this->x*this->x+this->y*this->y+this->z*this->z+this->w*this->w));
        
    }
    
    
	// normalizes the vector
    void normalize()
    {
        float length = this->length();
        *this /= length;
    }
    
    
	
	// prints the vector
    friend std::ostream& operator << (std::ostream& os, const Vector4f& v)
    
    {
        os <<"("<<v.x <<", "<<  v.y <<", "<< v.z<< v.w <<")";
        return(os);
    }
    
    
    
};


inline float dot(const Vector4f& u, const Vector4f& v)
{
    return(u.x * v.x + u.y*v.y + u.z*v.z + u.w*v.w);
}


inline float length(const Vector4f& u)
{
    return(sqrt(dot(u,u)));
    ;
}


Vector4f normalize(const Vector4f& u)
{
    float len = length(u);
    return(u/len);
}

Vector3f cross(const Vector4f& u,  const Vector4f& v)
{
    Vector3f vv;
    vv.x =u.y*v.z-v.y*u.z;
    vv.y=-1*(u.x*v.z-v.x*u.z);
    vv.z =u.x*v.y-v.x*u.y;
    return(Vector3f(u.y*v.z-v.y*u.z, -1*(u.x*v.z-v.x*u.z), u.x*v.y-v.x*u.y));
}


float cross_value_value(const Vector4f& u,  const Vector4f& v)
{
    return(u.y*v.z-v.y*u.z + -1*(u.x*v.z-v.x*u.z) + u.x*v.y-v.x*u.y);
    
}





/////////////////////////////////////////////////////////////////////

struct PersProjInfo
{
    float FOV;
    float Width; 
    float Height;
    float zNear;
    float zFar;
};


///////////////////////////////////////////////////////////////////////////////

struct mMatrix4f {
    float m[4][4];
    Vector4f vm[4];
	    
	mMatrix4f()
    {
        vm[0] = Vector4f(0,0,0,0);

    }
};
    

class Matrix4f
{
public:
	float *m[4];
	Vector4f vm[4];

    

    Matrix4f()
    {
		int i;
        for (i = 0; i < 4; i++) {
			vm[i] = Vector4f(0,0,0,0);
			m[i]= (float *) &vm[i];
		}
		int rc;
		rc = sizeof(m);
		rc= sizeof(vm);
    }


    inline void InitIdentity()
    {
        vm[0] = Vector4f(1,0,0,0);
        m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
    }

    inline Matrix4f operator*(const Matrix4f& Right) const
    {
        Matrix4f Ret;

        for (unsigned int i = 0 ; i < 4 ; i++) {
            for (unsigned int j = 0 ; j < 4 ; j++) {
                Ret.m[i][j] = m[i][0] * Right.m[0][j] +
                              m[i][1] * Right.m[1][j] +
                              m[i][2] * Right.m[2][j] +
                              m[i][3] * Right.m[3][j];
            }
        }

        return Ret;
    }
    
    Vector4f operator*(const Vector4f& v) const
    {
        Vector4f r;
        
        r.x = m[0][0]* v.x + m[0][1]* v.y + m[0][2]* v.z + m[0][3]* v.w;
        r.y = m[1][0]* v.x + m[1][1]* v.y + m[1][2]* v.z + m[1][3]* v.w;
        r.z = m[2][0]* v.x + m[2][1]* v.y + m[2][2]* v.z + m[2][3]* v.w;
        r.w = m[3][0]* v.x + m[3][1]* v.y + m[3][2]* v.z + m[3][3]* v.w;
        
        return r;
    }
    
    void Print()
    {
        for (int i = 0 ; i < 4 ; i++) {
            printf("%f %f %f %f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
        }       
    }

    void InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
    void InitRotateTransform(float RotateX, float RotateY, float RotateZ);
    void InitTranslationTransform(float x, float y, float z);
    void InitCameraTransform(const Vector3f& Target, const Vector3f& Up);
    void InitPersProjTransform(const PersProjInfo& p);
};

////////////////////////////////////////////////////////////////////////////////////////////////


struct Quaternion
{
    float x, y, z, w;

    Quaternion(float _x, float _y, float _z, float _w);

    void Normalize();

    Quaternion Conjugate();  
 };

Quaternion operator*(const Quaternion& l, const Quaternion& r);

Quaternion operator*(const Quaternion& q, const Vector3f& v);

#endif	/* MATH_3D_H */

