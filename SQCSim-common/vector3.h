#ifndef VECTOR3_H__
#define VECTOR3_H__

#include <iostream>
#include <cmath>

template <class T>
class Vector3
{
public:
	Vector3();
	Vector3(const T& x, const T& y, const T& z);
	~Vector3();

	T Length() const;
	void Normalize();
	void Zero();

	T Dot(const Vector3<T>& v) const;
	Vector3<T> Cross(const Vector3<T>& v) const;

	Vector3<T> operator+(const Vector3<T>& v) const;
	Vector3<T> operator-(const Vector3<T>& v) const;
	Vector3<T> operator-() const;
	Vector3<T> operator+(const T& v) const;
	Vector3<T> operator-(const T& v) const;
	Vector3<T> operator/(const T& v) const;
	Vector3<T> operator*(const T& v) const;

	Vector3<T>& operator=(const Vector3<T>& v);

	Vector3<T>& operator+=(const Vector3<T>& v);
	Vector3<T>& operator-=(const Vector3<T>& v);
	Vector3<T>& operator+=(const T& v);
	Vector3<T>& operator-=(const T& v);
	Vector3<T>& operator/=(const T& v);
	Vector3<T>& operator*=(const T& v);

	bool operator==(const Vector3<T>& v) const;
	bool operator!=(const Vector3<T>& v) const;

	void Afficher() const;

public:
	T x, y, z;
};

typedef Vector3<int> Vector3i;
typedef Vector3<float> Vector3f;

template <class T>
inline std::ostream& operator<<(std::ostream& out, const Vector3<T>& v)
{
	out << "[" << v.x << ", " << v.y << ", " << v.z << "]";
    return out;
}


template <class T>
Vector3<T>::Vector3()
{
}

template <class T>
Vector3<T>::Vector3(const T& x, const T& y, const T& z) : x(x), y(y), z(z)
{
}

template <class T>
Vector3<T>::~Vector3()
{
}

template <class T>
T Vector3<T>::Length() const
{
	return sqrt(x*x + y*y + z*z);
}

template <class T>
void Vector3<T>::Normalize()
{
	T len = Length();
	if (len != 0)
	{
		x /= len;
		y /= len;
		z /= len;
	}
}

template <class T>
void Vector3<T>::Zero()
{
	x = y = z = 0;
}

template <class T>
T Vector3<T>::Dot(const Vector3<T>& v) const
{
	return (x * v.x) + (y * v.y) + (z * v.z);
}

template <class T>
Vector3<T> Vector3<T>::Cross(const Vector3<T>& v) const
{
	return Vector3<T>(
		y * v.z - v.y * z,
		z * v.x - v.z * x,
		x * v.y - v.x * y);
}

template <class T>
Vector3<T> Vector3<T>::operator+(const Vector3<T>& v) const
{
	return Vector3<T>(x + v.x, y + v.y, z + v.z);
}

template <class T>
Vector3<T> Vector3<T>::operator-(const Vector3<T>& v) const
{
	return Vector3<T>(x - v.x, y - v.y, z - v.z);
}

template <class T>
Vector3<T> Vector3<T>::operator-() const
{
	return Vector3<T>(-x, -y, -z);
}

template <class T>
Vector3<T> Vector3<T>::operator+(const T& v) const
{
	return Vector3<T>(x + v, y + v, z + v);
}

template <class T>
Vector3<T> Vector3<T>::operator-(const T& v) const
{
	return Vector3<T>(x - v, y - v, z - v);
}

template <class T>
Vector3<T> Vector3<T>::operator/(const T& v) const
{
	return Vector3<T>(x / v, y / v, z / v);
}

template <class T>
Vector3<T> Vector3<T>::operator*(const T& v) const
{
	return Vector3<T>(x * v, y * v, z * v);
}

template <class T>
Vector3<T>& Vector3<T>::operator=(const Vector3<T>& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

template <class T>
Vector3<T>& Vector3<T>::operator+=(const Vector3<T>& v)
{
	return (*this = *this + v);
}

template <class T>
Vector3<T>& Vector3<T>::operator-=(const Vector3<T>& v)
{
	return (*this = *this - v);
}

template <class T>
Vector3<T>& Vector3<T>::operator+=(const T& v)
{
	return (*this = *this + v);
}

template <class T>
Vector3<T>& Vector3<T>::operator-=(const T& v)
{
	return (*this = *this - v);
}

template <class T>
Vector3<T>& Vector3<T>::operator/=(const T& v)
{
	return (*this = *this / v);
}

template <class T>
Vector3<T>& Vector3<T>::operator*=(const T& v)
{
	return (*this = *this * v);
}

template <class T>
bool Vector3<T>::operator==(const Vector3<T>& v) const
{
	return (x == v.x && y == v.y && z == v.z);
}

template <class T>
bool Vector3<T>::operator!=(const Vector3<T>& v) const
{
	return !(*this == v);
}

template <class T>
void Vector3<T>::Afficher() const
{
	std::cout << "[" << x << ", " << y << ", " << z << "]" << std::endl;
}


#endif // VECTOR3_H__
