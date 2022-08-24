#pragma once

#ifdef USE_SIMD
#include <immintrin.h>
#endif

#include <algorithm>
#include <array>

#include "Vector3.hpp"

using Matrix4d = Matrix4<f64>;
using Matrix4f = Matrix4<f32>;
using Matrix4i = Matrix4<isize>;

#ifdef USE_SIMD
static void sse_matrix4x4_mul(const float* lhs, const float* rhs, float* result)
{
    __m128 row1 = _mm_load_ps(&rhs[0]);
    __m128 row2 = _mm_load_ps(&rhs[4]);
    __m128 row3 = _mm_load_ps(&rhs[8]);
    __m128 row4 = _mm_load_ps(&rhs[12]);
    for(usize i = 0; i < 4; ++i)
    {
        __m128 ai1 = _mm_set1_ps(lhs[i * 4 + 0]);
        __m128 ai2 = _mm_set1_ps(lhs[i * 4 + 1]);
        __m128 ai3 = _mm_set1_ps(lhs[i * 4 + 2]);
        __m128 ai4 = _mm_set1_ps(lhs[i * 4 + 3]);

        __m128 row = _mm_add_ps
        (
            _mm_add_ps
            (
                _mm_mul_ps(ai1, row1),
                _mm_mul_ps(ai2, row2)
            ), 
            _mm_add_ps
            (
                _mm_mul_ps(ai3, row3),
                _mm_mul_ps(ai4, row4)
            )
        );
        _mm_store_ps(&result[i * 4], row);
    }
}
#endif

template <arithmetic T>
struct Matrix4
{
private:
    T data[16];

public:
    constexpr Matrix4() = default;
    constexpr auto operator <=> (const Matrix4<T>&) const = default;

    template <arithmetic... U>
    constexpr Matrix4(U... args) : data{ static_cast<T>(args)... } {}

    constexpr Vector3<T> column(usize i) const
    {
        return {data[i], data[4 + i], data[8 + i]}; 
    }

    constexpr Vector3<T> row(usize i) const
    {
        return {data[i * 4], data[i * 4 + 1], data[i * 4 + 2]}; 
    }

    constexpr Matrix4<T>& operator *= (T rhs)
    {
        for(auto& i : data) i *= rhs;
        return *this;
    }

    constexpr Matrix4<T> operator * (T rhs) const
    {
        return
        {
            data[0]  * rhs, data[1]  * rhs, data[2]  * rhs, data[3]  * rhs,
            data[4]  * rhs, data[5]  * rhs, data[6]  * rhs, data[7]  * rhs,
            data[8]  * rhs, data[9]  * rhs, data[10] * rhs, data[11] * rhs,
            data[12] * rhs, data[13] * rhs, data[14] * rhs, data[15] * rhs
        };
    }

    Matrix4<T> operator * (const Matrix4<T>& rhs) const
    {
        Matrix4<T> ret;
#ifdef USE_SIMD
        if constexpr(std::is_same_v<f32, T>)
            sse_matrix4x4_mul(data, rhs.data, ret.data);
        else
            static_assert(false, "simd only support float type");
#else
        const Matrix4<T>& lhs = *this;
        T sum;
        for(usize i = 0; i < 4; i++)
        {
            for(usize j = 0; j < 4; j++)
            {
                sum = 0;
                for(usize k = 0; k < 4; k++)
                {
                    sum += lhs[i][k] * rhs[k][j];
                }
                ret[i][j] = sum;
            }
        }
#endif
        return ret;
    }

    struct Index
    {
        Matrix4<T>& m;
        usize i;

        Index(const Index&) = delete;
        Index(Index&&) = delete;
        ~Index() = default;
        Index& operator = (const Index&) = delete;
        Index& operator = (Index&&) = delete;

        Index(Matrix4<T>& m, usize i): m(m), i(i) { assert(i < 4); }

        operator T () const { return m.data[i]; }

        Index& operator [] (usize index)
        {
            assert(index < 4);
            i = i * 4 + index;
            return *this;
        }

        Index& operator = (T x)
        {
            m.data[i] = x;
            return *this;
        }

        Index& operator += (T x)
        {
            m.data[i] += x;
            return *this;
        }
    };

    Index operator [] (usize index) { return { *this, index }; }

    Index operator [] (usize index) const
    {
        return
        {
            const_cast<Matrix4<T>&>(*this),
            index
        };
    }

     static constexpr Matrix4<T> identity()
    {
        return
        {
            ONE<T>,  ZERO<T>, ZERO<T>, ZERO<T>,
            ZERO<T>, ONE<T>,  ZERO<T>, ZERO<T>,
            ZERO<T>, ZERO<T>, ONE<T>,  ZERO<T>,
            ZERO<T>, ZERO<T>, ZERO<T>, ONE<T>
        };
    }

    static constexpr Matrix4<T> fill(T value)
    {
        return
        {
            value, value, value, value,
            value, value, value, value,
            value, value, value, value,
            value, value, value, value
        };
    }

    constexpr Matrix4<T> transpose() const
    {
        return
        {
            data[0], data[4], data[8],  data[12],
            data[1], data[5], data[9],  data[13],
            data[2], data[6], data[10], data[14],
            data[3], data[7], data[11], data[15]
        };
    }

    constexpr Matrix4<T> adjugate() const
    {
        Matrix4<T> adj;
        const Matrix4<T>& m = *this;
        adj[0][0] = +m[1][1] * m[2][2] * m[3][3] +
                    m[1][2] * m[2][3] * m[3][1] +
                    m[1][3] * m[2][1] * m[3][2] -
                    m[1][1] * m[2][3] * m[3][2] -
                    m[1][2] * m[2][1] * m[3][3] -
                    m[1][3] * m[2][2] * m[3][1];
        adj[1][0] = -m[1][0] * m[2][2] * m[3][3] -
                    m[1][2] * m[2][3] * m[3][0] -
                    m[1][3] * m[2][0] * m[3][2] +
                    m[1][0] * m[2][3] * m[3][2] +
                    m[1][2] * m[2][0] * m[3][3] +
                    m[1][3] * m[2][2] * m[3][0];
        adj[2][0] = +m[1][0] * m[2][1] * m[3][3] +
                    m[1][1] * m[2][3] * m[3][0] +
                    m[1][3] * m[2][0] * m[3][1] -
                    m[1][0] * m[2][3] * m[3][1] -
                    m[1][1] * m[2][0] * m[3][3] -
                    m[1][3] * m[2][1] * m[3][0];
        adj[3][0] = -m[1][0] * m[2][1] * m[3][2] -
                    m[1][1] * m[2][2] * m[3][0] -
                    m[1][2] * m[2][0] * m[3][1] +
                    m[1][0] * m[2][2] * m[3][1] +
                    m[1][1] * m[2][0] * m[3][2] +
                    m[1][2] * m[2][1] * m[3][0];
        adj[0][1] = -m[0][1] * m[2][2] * m[3][3] -
                    m[0][2] * m[2][3] * m[3][1] -
                    m[0][3] * m[2][1] * m[3][2] +
                    m[0][1] * m[2][3] * m[3][2] +
                    m[0][2] * m[2][1] * m[3][3] +
                    m[0][3] * m[2][2] * m[3][1];
        adj[1][1] = +m[0][0] * m[2][2] * m[3][3] +
                    m[0][2] * m[2][3] * m[3][0] +
                    m[0][3] * m[2][0] * m[3][2] -
                    m[0][0] * m[2][3] * m[3][2] -
                    m[0][2] * m[2][0] * m[3][3] -
                    m[0][3] * m[2][2] * m[3][0];
        adj[2][1] = -m[0][0] * m[2][1] * m[3][3] -
                    m[0][1] * m[2][3] * m[3][0] -
                    m[0][3] * m[2][0] * m[3][1] +
                    m[0][0] * m[2][3] * m[3][1] +
                    m[0][1] * m[2][0] * m[3][3] +
                    m[0][3] * m[2][1] * m[3][0];
        adj[3][1] = +m[0][0] * m[2][1] * m[3][2] +
                    m[0][1] * m[2][2] * m[3][0] +
                    m[0][2] * m[2][0] * m[3][1] -
                    m[0][0] * m[2][2] * m[3][1] -
                    m[0][1] * m[2][0] * m[3][2] -
                    m[0][2] * m[2][1] * m[3][0];
        adj[0][2] = +m[0][1] * m[1][2] * m[3][3] +
                    m[0][2] * m[1][3] * m[3][1] +
                    m[0][3] * m[1][1] * m[3][2] -
                    m[0][1] * m[1][3] * m[3][2] -
                    m[0][2] * m[1][1] * m[3][3] -
                    m[0][3] * m[1][2] * m[3][1];
        adj[1][2] = -m[0][0] * m[1][2] * m[3][3] -
                    m[0][2] * m[1][3] * m[3][0] -
                    m[0][3] * m[1][0] * m[3][2] +
                    m[0][0] * m[1][3] * m[3][2] +
                    m[0][2] * m[1][0] * m[3][3] +
                    m[0][3] * m[1][2] * m[3][0];
        adj[2][2] = +m[0][0] * m[1][1] * m[3][3] +
                    m[0][1] * m[1][3] * m[3][0] +
                    m[0][3] * m[1][0] * m[3][1] -
                    m[0][0] * m[1][3] * m[3][1] -
                    m[0][1] * m[1][0] * m[3][3] -
                    m[0][3] * m[1][1] * m[3][0];
        adj[3][2] = -m[0][0] * m[1][1] * m[3][2] -
                    m[0][1] * m[1][2] * m[3][0] -
                    m[0][2] * m[1][0] * m[3][1] +
                    m[0][0] * m[1][2] * m[3][1] +
                    m[0][1] * m[1][0] * m[3][2] +
                    m[0][2] * m[1][1] * m[3][0];
        adj[0][3] = -m[0][1] * m[1][2] * m[2][3] -
                    m[0][2] * m[1][3] * m[2][1] -
                    m[0][3] * m[1][1] * m[2][2] +
                    m[0][1] * m[1][3] * m[2][2] +
                    m[0][2] * m[1][1] * m[2][3] +
                    m[0][3] * m[1][2] * m[2][1];
        adj[1][3] = +m[0][0] * m[1][2] * m[2][3] +
                    m[0][2] * m[1][3] * m[2][0] +
                    m[0][3] * m[1][0] * m[2][2] -
                    m[0][0] * m[1][3] * m[2][2] -
                    m[0][2] * m[1][0] * m[2][3] -
                    m[0][3] * m[1][2] * m[2][0];
        adj[2][3] = -m[0][0] * m[1][1] * m[2][3] -
                    m[0][1] * m[1][3] * m[2][0] -
                    m[0][3] * m[1][0] * m[2][1] +
                    m[0][0] * m[1][3] * m[2][1] +
                    m[0][1] * m[1][0] * m[2][3] +
                    m[0][3] * m[1][1] * m[2][0];
        adj[3][3] = +m[0][0] * m[1][1] * m[2][2] +
                    m[0][1] * m[1][2] * m[2][0] +
                    m[0][2] * m[1][0] * m[2][1] -
                    m[0][0] * m[1][2] * m[2][1] -
                    m[0][1] * m[1][0] * m[2][2] -
                    m[0][2] * m[1][1] * m[2][0];
        return adj;
    }

    constexpr Matrix4<T> inverse() const
    {
        Matrix4<T> adj = adjugate();
        const Matrix4<T>& m = *this;
        const f32 det = m[0][0] * adj[0][0]
                      + m[0][1] * adj[1][0]
                      + m[0][2] * adj[2][0]
                      + m[0][3] * adj[3][0];
        return adj * reciprocal(det);
    }
};

template <arithmetic T>
std::ostream& operator << (std::ostream& os, const Matrix4<T>& m)
{
    for(usize i = 0; i < 4; ++i)
    {
        os << '[';
        for (usize j = 0; j < 4; ++j)
        {
            os << m[i][j] << (j != 3 ? ", " : "]\n");
        }
    }
    return os;
}
