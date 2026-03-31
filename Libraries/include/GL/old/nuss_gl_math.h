/*


    This program is distributed for education purposes only and WITHOUT ANY WARRANTY.

    This file is governed by the GNU General Public License.

		Copyright 2014 Doron Nussbaum

*/

#ifndef NUSS_MATH_GL
#define	NUSS_MATH_GL

#include <stdio.h>
#define _USE_MATH_DEFINES // for C++. The define is used to load the constants
#include <math.h>
#include "assert.h"

#define DegreeToRadians(x) (float)(((x) * M_PI / 180.0f))
#define RadianToDegrees(x) (float)(((x) * 180.0f / M_PI))

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
        os <<"("<<v.x <<", "<<  v.y <<", "<< v.z <<", "<< v.w <<")";
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

class Matrix4f;
    
Matrix4f identity();

class Matrix4f
{

public:
	float *m[4];
	Vector4f vm[4];

    Matrix4f()
    {
		int i;
		reset(0.0);
        for (i = 0; i < 4; i++) m[i]= (float *) &vm[i];

		int rc;
		rc = sizeof(m);
		rc= sizeof(vm);
    }

	// changes the matrix as an identity matrix
    inline Matrix4f identity11()
    {
		Matrix4f m1;
        m1 = identity();
		return m1;
		
	}

	// changes the matrix to an identity matrix
    static Matrix4f identity1()
    {
		Matrix4f m1;
        m1.reset(0);
		m1.vm[0].x = m1.vm[1].y = m1.vm[2].z = m1.vm[3].w = 1;
		return(m1);
		
	}

	
	// print the matrix
	// prints the vector
    friend std::ostream& operator << (std::ostream& os, const Matrix4f& m1)
    
    {
		int i;
		for (i = 0; i <=3; i++) {
			os <<m1.vm[i] << std::endl;
		}
        return(os);
    }
    


	// resets all the values of the matrix to "value"
	inline void reset(float value)
		// sets all 16 values of the matrix to value 
	{
		int i;
		for (i=0; i <=3; i++) {
			vm[i]= Vector4f(value, value, value, value);
		}

	}


	// creates a transpose of the matrix
	// retruns a transposed matrix
	// note it does not change the internal values of the matrix
	void setTranspose(void)
	{
		Matrix4f m1;

		m1.vm[0] = Vector4f(vm[0].x, vm[1].x, vm[2].x, vm[3].x);
		m1.vm[1] = Vector4f(vm[0].y, vm[1].y, vm[2].y, vm[3].y);
		m1.vm[2] = Vector4f(vm[0].z, vm[1].z, vm[2].z, vm[3].z);
		m1.vm[3] = Vector4f(vm[0].w, vm[1].w, vm[2].w, vm[3].w);

	}
    
	
	inline Matrix4f operator*(const Matrix4f& rhs) const
    {
        Matrix4f m1,m2;
		int i,j;

 	// check it out	m2 = transpose(rhs);
		m2=rhs;
		m2.setTranspose();
        for (i = 0 ; i < 4 ; i++) {
            for (j = 0 ; j < 4 ; j++) {
                m1.m[i][j] = dot(vm[i], m2.vm[j]);
			}
		}

		return(m1);
    }
 



	// multiply the matrix by a vector 
    Vector4f operator*(const Vector4f& v) const
    {
        Vector4f r;
        
        r.x = dot(vm[0],v);
        r.y = dot(vm[1],v);
		r.y = dot(vm[2],v);
		r.y = dot(vm[3],v);
        return r;
    }
    




	// creates a transpose of the matrix
	// retruns a transposed matrix
	// note it does not transpose the input matrix
	Matrix4f transpose(const Matrix4f& m1)
	{
		Matrix4f m2;
		m2=m1;
		m2.setTranspose();

		return(m2);

	}



	 // set the matrix as a scaling matrix
	 // Note it resets the data first
	void setScale(float scaleX, float scaleY, float scaleZ)
	{
		//setIdentity();
		vm[0].x = scaleX;
		vm[1].y = scaleY;
		vm[2].z = scaleZ;
	}


	// return a scale matrix.  Note internal parameters are not changes
	// Note it resets the data first
	Matrix4f scale(float scaleX, float scaleY, float scaleZ)
	{
		Matrix4f m1;
		//m1.setIdentity();
		m1.vm[0].x = scaleX;
		m1.vm[1].y = scaleY;
		m1.vm[2].z = scaleZ;
		return m1;
	}


	// set the matrix as a pitch rotation matrix
	// if degree is true (!=0) then angle is given in degrees
	// otherwise angle is given in radians
	void setRotateX(float angle, int degree)
	{
		if (degree) angle = DegreeToRadians(angle);
		//setIdentity();
		vm[1].y = vm[2].z=cos(angle);
		vm[1].z = -sin(angle);
		vm[2].y = -vm[1].z;
	}


	// set the matrix as a yaw rotation matrix
	// if degree is true (!=0) then angle is given in degrees
	// otherwise angle is given in radians
	void setRotateY(float angle, int degree)
	{
		if (degree) angle = DegreeToRadians(angle);
		//setIdentity();
		vm[0].x = vm[2].z=cos(angle);
		vm[0].z = sin(angle);
		vm[2].x = -vm[0].z;
	}


	// set the matrix as a roll rotation matrix
	// if degree is true (!=0) then angle is given in degrees
	// otherwise angle is given in radians
	Matrix4f& setRotateZ(float angle, int degree)
	{
		if (degree) angle = DegreeToRadians(angle);
		//setIdentity();
		vm[0].x = vm[1].y = cos(angle);
		vm[0].y = -sin(angle);
		vm[1].x = -vm[0].y;

		return(*this);
	}



	// set the matrix as a roll pitch and yaw rotation matrix
	// the resulting matrix M=Mat(yaw)*mat(pitch)*mat(roll)
	// if degree is true (!=0) then all angles are given in degrees
	// otherwise angle is given in radians
	Matrix4f& setRotateRollPitchYaw(float angleRoll, float anglePitch, float angleYaw, int degree)
	{
		Matrix4f m1;
		if (degree) {
			angleRoll = DegreeToRadians(angleRoll);
			anglePitch = DegreeToRadians(anglePitch);
			angleYaw = DegreeToRadians(angleYaw);
		}
		//setIdentity();
		
//		*this *= m1.setRotationX(angleRoll,degree);

	}
 
	
	Matrix4f& setTranslation(float dx, float dy, float dz);
    void InitCameraTransform(const Vector3f& Target, const Vector3f& Up);
    void InitPersProjTransform(const PersProjInfo& p);
};


// changes the matrix to an identity matrix
Matrix4f identity111()
{
	Matrix4f m1;
    m1.reset(0);
	m1.vm[0].x = m1.vm[1].y = m1.vm[2].z = m1.vm[3].w = 1;
	return(m1);
}






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

