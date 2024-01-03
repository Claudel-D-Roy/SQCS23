#ifndef ARRAY2D_H__
#define ARRAY2D_H__

#include "define.h"

template <class T>
class Array2d {
    public:
        Array2d(int x, int y);
        ~Array2d();
        Array2d(const Array2d& array);

        void Set(int x, int y, T type);
        T Get(int x, int y) const;
        T Remove(int x, int y);

        void Reset(T type);

    private:
        int m_x, m_y;
        T* m_array;

        int To1dIndex(int x, int y) const;
};

template <class T>
Array2d<T>::Array2d(int x, int y) : m_x(x), m_y(y) { m_array = new T[m_x * m_y]; }

template <class T>
Array2d<T>::~Array2d() { delete[] m_array; }

template <class T>
Array2d<T>::Array2d(const Array2d<T>& array) : m_x(array.m_x), m_y(array.m_y) {
    m_array = new T[m_x * m_y];
    for (int i = 0; i < m_x * m_y; ++i)
        m_array[i] = array.m_array[i];
}

template <class T>
void Array2d<T>::Set(int x, int y, T type) { m_array[To1dIndex(x, y)] = type; }

template <class T>
T Array2d<T>::Get(int x, int y) const { return m_array[To1dIndex(x, y)]; }

template <class T>
T Array2d<T>::Remove(int x, int y) { 
    T thing = std::move(m_array[To1dIndex(x, y)]);
    m_array[To1dIndex(x, y)] = nullptr;
    return thing;
}

template <class T>
void Array2d<T>::Reset(T type) {
    for (int i = 0; i < m_x * m_y; ++i)
        m_array[i] = type;
}

template <class T>
int Array2d<T>::To1dIndex(int x, int y) const { return x + (y * m_x); }

#endif // ARRAY2D_H__
