#pragma once

#include <iostream>
#include <igloo/utility/math_vector.hpp>

namespace igloo
{


template<typename Derived, typename T, std::size_t M, std::size_t N> class matrix_facade;


namespace detail
{


template<std::size_t M, std::size_t N> struct adjoint;

template<>
struct adjoint<2,2>
{
  template<typename Derived, typename T>
  inline static Derived do_it(const matrix_facade<Derived,T,2,2> &A)
  {
    Derived result;

    result(0,0) =  A(1,1);
    result(0,1) = -A(0,1);
    result(1,0) = -A(1,0);
    result(1,1) =  A(0,0);

    return result;
  }
};


template<>
struct adjoint<3,3>
{
  template<typename Derived, typename T>
  inline static Derived do_it(const matrix_facade<Derived,T,3,3> &A)
  {
    Derived result;
    
    result(0,0) =  A.minor_(1, 2, 1, 2);
    result(0,1) = -A.minor_(0, 2, 1, 2);
    result(0,2) =  A.minor_(0, 1, 1, 2);
    result(1,0) = -A.minor_(1, 2, 0, 2);
    result(1,1) =  A.minor_(0, 2, 0, 2);
    result(1,2) = -A.minor_(0, 1, 0, 2);
    result(2,0) =  A.minor_(1, 2, 0, 1);
    result(2,1) = -A.minor_(0, 2, 0, 1);
    result(2,2) =  A.minor_(0, 1, 0, 1);

    return result;
  }
};


template<>
struct adjoint<4,4>
{
  template<typename Derived, typename T>
  inline static Derived do_it(const matrix_facade<Derived,T,4,4> &A)
  {
    Derived result;

    result(0,0) =  A.minor_(1, 2, 3, 1, 2, 3);
    result(0,1) = -A.minor_(0, 2, 3, 1, 2, 3);
    result(0,2) =  A.minor_(0, 1, 3, 1, 2, 3);
    result(0,3) = -A.minor_(0, 1, 2, 1, 2, 3);
    result(1,0) = -A.minor_(1, 2, 3, 0, 2, 3);
    result(1,1) =  A.minor_(0, 2, 3, 0, 2, 3);
    result(1,2) = -A.minor_(0, 1, 3, 0, 2, 3);
    result(1,3) =  A.minor_(0, 1, 2, 0, 2, 3);
    result(2,0) =  A.minor_(1, 2, 3, 0, 1, 3);
    result(2,1) = -A.minor_(0, 2, 3, 0, 1, 3);
    result(2,2) =  A.minor_(0, 1, 3, 0, 1, 3);
    result(2,3) = -A.minor_(0, 1, 2, 0, 1, 3);
    result(3,0) = -A.minor_(1, 2, 3, 0, 1, 2);
    result(3,1) =  A.minor_(0, 2, 3, 0, 1, 2);
    result(3,2) = -A.minor_(0, 1, 3, 0, 1, 2);
    result(3,3) =  A.minor_(0, 1, 2, 0, 1, 2);

    return result;
  }
};


template<size_t N> struct determinant;


template<>
struct determinant<2>
{
  template<typename Derived, typename T>
  inline static T do_it(const matrix_facade<Derived,T,2,2> &A)
  {
    return A(0,0)*A(1,1) - A(0,1)*A(1,0);
  }
};


template<>
struct determinant<3>
{
  template<typename Derived, typename T>
  inline static T do_it(const matrix_facade<Derived,T,3,3> &A)
  {
    return A(0,0) * A.minor_(1, 2, 1, 2) -
           A(0,1) * A.minor_(1, 2, 0, 2) +
           A(0,2) * A.minor_(1, 2, 0, 1);
  }
};


template<>
struct determinant<4>
{
  template<typename Derived, typename T>
  inline static T do_it(const matrix_facade<Derived,T,4,4> &A)
  {
    return A(0,0) * A.minor_(1, 2, 3, 1, 2, 3) -
           A(0,1) * A.minor_(1, 2, 3, 0, 2, 3) +
           A(0,2) * A.minor_(1, 2, 3, 0, 1, 3) -
           A(0,3) * A.minor_(1, 2, 3, 0, 1, 2);
  }
};


} // end detail


// M: num_rows
// N: num_columns
template<typename Derived, typename T, std::size_t M, std::size_t N>
  class matrix_facade
{
  public:
    typedef T                               value_type;
    typedef value_type &                    reference;
    typedef const value_type &              const_reference;
    typedef std::size_t                     size_type;
    static const std::size_t                static_size = M * N;
    typedef std::pair<size_type, size_type> extent_type;

    typedef T*       iterator;
    typedef const T* const_iterator;

    matrix_facade(){}

    matrix_facade(const matrix_facade &other)
    {
      for(size_type i = 0; i != static_size; ++i)
      {
        (*this)[i] = other[i];
      }
    }

    template<typename OtherMatrix>
    matrix_facade(const OtherMatrix &other)
    {
      for(size_type i = 0; i != static_size; ++i)
      {
        (*this)[i] = other[i];
      }
    }

    matrix_facade(value_type v)
    {
      std::fill(begin(), end(), v);
    }

    Derived &operator=(const matrix_facade &other)
    {
      for(size_type i = 0; i != static_size; ++i)
      {
        (*this)[i] = other[i];
      }

      return derived();
    }

    template<typename OtherMatrix>
    Derived &operator=(const OtherMatrix &other)
    {
      for(size_type i = 0; i != static_size; ++i)
      {
        (*this)[i] = other[i];
      }

      return derived();
    }

    size_type size() const
    {
      return static_size;
    }

    extent_type extent() const
    {
      return extent_type(M,N);
    }

    iterator begin()
    {
      return reinterpret_cast<iterator>(this);
    }

    iterator end()
    {
      return begin() + size();
    }

    const_iterator begin() const
    {
      return reinterpret_cast<const_iterator>(this);
    }

    const_iterator end() const
    {
      return begin() + size();
    }

    inline operator value_type * ()
    {
      return begin();
    }

    inline operator const value_type * () const
    {
      return begin();
    }

    inline reference operator[](size_type i)
    {
      return begin()[i];
    }

    inline const_reference operator[](size_type i) const
    {
      return begin()[i];
    }

    inline reference operator()(size_type i, size_type j)
    {
      return operator[](i * extent().second + j);
    }

    inline const_reference operator()(size_type i, size_type j) const
    {
      return operator[](i * extent().second + j);
    }

    // XXX extend get to 2D access
    template<size_type i>
    inline reference get()
    {
      return operator[](i);
    }

    template<size_type i>
    inline const_reference get() const
    {
      return operator[](i);
    }

    template<typename OtherDerived, size_type P>
    inline typename OtherDerived::template rebind<value_type,M,P>::type
      operator*(const matrix_facade<OtherDerived,T,N,P> &rhs) const
    {
      typedef typename OtherDerived::template rebind<value_type,M,P>::type result_type;

      result_type result;
      const matrix_facade &lhs = *this;

      value_type temp;
      // iterate over elements of the result:
      for(unsigned int i = 0;
          i < M;
          ++i)
      {
        for(unsigned int j = 0;
            j < P;
            ++j)
        {
          // dot the ith row of lhs
          // with the jth column of rhs
          temp = 0;
          for(unsigned int k = 0;
              k < N;
              ++k)
          {
            temp += lhs(i,k) * rhs(k,j);
          } // end for k

          result(i,j) = temp;
        } // end for j
      } // end for i

      return result;
    } // end operator*()

    // XXX can this be performed in place?
    Derived &operator*=(const matrix_facade &rhs)
    {
      Derived result = (*this) * rhs;

      *this = result;

      return derived();
    }

    Derived &operator*=(value_type rhs)
    {
      for(size_type i = 0; i < static_size; ++i)
      {
        operator[](i) *= rhs;
      } // end for i

      return derived();
    } // end operator*=()

    Derived &operator/=(value_type d)
    {
      value_type reciprocal = value_type(1) / d;
      return operator*=(reciprocal);
    } // end operator/=()

    Derived operator/(value_type d) const
    {
      Derived result = *this;

      result /= d;

      return result;
    } // end operator/()

    Derived inverse() const
    {
      return adjoint() / determinant();
    }

    Derived adjoint() const
    {
      return igloo::detail::adjoint<M,N>::do_it(*this);
    }

    template<typename DerivedVector>
    DerivedVector operator*(const math_vector_facade<DerivedVector,value_type,M> &rhs) const
    {
      DerivedVector result(value_type(0));

      for(unsigned int j = 0; j < M; ++j)
      {
        for(unsigned int i = 0; i < N; ++i)
        {
          result[j] += (*this)(j,i) * rhs[i];
        } // end for i
      } // end for j

      return result;
    } // end operator*()

  private:
    Derived &derived()
    {
      return static_cast<Derived&>(*this);
    } // end derived()

    const Derived &derived() const
    {
      return static_cast<const Derived&>(*this);
    } // end derived()


    // Helper function for adjoint when M is 3.
    // XXX move this to square_matrix?
    value_type minor_(const size_type r0, const size_type r1,
                      const size_type c0, const size_type c1) const
    {
      const matrix_facade &A = *this;
      return A(r0,c0) * A(r1,c1) - A(r1,c0) * A(r0,c1);
    } // end minor()

    // Helper function for adjoint when M is 4.
    // XXX move this to square_matrix?
    value_type minor_(const size_type r0, const size_type r1, const size_type r2,
                      const size_type c0, const size_type c1, const size_type c2) const
    {
      const matrix_facade &A = *this;
      return A(r0,c0) * (A(r1,c1) * A(r2,c2) - A(r2,c1) * A(r1,c2)) -
    	     A(r0,c1) * (A(r1,c0) * A(r2,c2) - A(r2,c0) * A(r1,c2)) +
    	     A(r0,c2) * (A(r1,c0) * A(r2,c1) - A(r2,c0) * A(r1,c1));
    } // end minor()

    // Helper function when M is small
    // XXX move this to square_matrix?
    value_type determinant() const
    {
      return igloo::detail::determinant<M>::do_it(*this);
    } // end determinant()

    template<std::size_t, std::size_t> friend struct detail::adjoint;
    template<size_t> friend struct detail::determinant;
}; // end matrix_facade


template<typename Derived, typename T, std::size_t M, std::size_t N>
std::ostream &operator<<(std::ostream &os, const matrix_facade<Derived,T,M,N> &m)
{
  for(size_t row = 0; row < M; ++row)
  {
    std::cout << "[";

    size_t col = 0;
    for(; col < N-1; ++col)
    {
      std::cout << m(row,col) << ", ";
    }

    std::cout << m(row,col);

    std::cout << "]" << std::endl;
  }

  return os;
}


namespace detail
{


template<typename Derived, typename T, std::size_t N>
class square_matrix : public matrix_facade<Derived, T, N, N>
{
  private:
    typedef matrix_facade<Derived,T,N,N> super_t;

  public:
    typedef typename super_t::value_type value_type;

    square_matrix() : super_t() {}

    square_matrix(const square_matrix &other)
      : super_t(other)
    {}

    template<typename OtherMatrix>
    square_matrix(const OtherMatrix &other)
      : super_t(other)
    {}

    square_matrix(value_type v)
      : super_t(v)
    {}

    inline static Derived identity()
    {
      Derived result(value_type(0));
      for(size_t i = 0; i < N; ++i)
      {
        result(i,i) = value_type(1);
      }

      return result;
    }

    inline Derived transpose() const
    {
      Derived result;

      for(unsigned int i = 0;
          i < N;
          ++i)
      {
        for(unsigned int j = 0;
            j < N;
            ++j)
        {
          result(i,j) = this->operator()(j,i);
        } // end for j
      } // end for i

      return result;
    } // end transpose()
}; // end square_matrix


template<typename Derived, typename T, std::size_t N>
Derived &transpose(const square_matrix<Derived,T,N> &m)
{
  return m.transpose();
} // end transpose()


} // end detail


template<typename T, std::size_t, std::size_t> class matrix;


template<typename T>
class matrix<T,3,3> : public detail::square_matrix<matrix<T,3,3>,T,3>
{
  private:
    typedef detail::square_matrix<matrix<T,3,3>,T,3> super_t;

  public:
    template<typename U, std::size_t M, std::size_t N>
    struct rebind
    {
      typedef matrix<U,M,N> type;
    };

    typedef typename super_t::value_type value_type;

    inline matrix() : super_t() {}

    template<typename OtherMatrix>
    inline matrix(const OtherMatrix &other) : super_t(other) {}

    inline matrix(value_type v) : super_t(v) {}

    inline matrix(value_type m00_, value_type m01_, value_type m02_,
                  value_type m10_, value_type m11_, value_type m12_,
                  value_type m20_, value_type m21_, value_type m22_)
      : m00(m00_), m01(m01_), m02(m02_),
        m10(m10_), m11(m11_), m12(m12_),
        m20(m20_), m21(m21_), m22(m22_)
    {}

    value_type m00, m01, m02, m10, m11, m12, m20, m21, m22;
}; // end matrix<T,3,3>


template<typename T>
class matrix<T,4,4> : public detail::square_matrix<matrix<T,4,4>,T,4>
{
  private:
    typedef detail::square_matrix<matrix<T,4,4>,T,4> super_t;

  public:
    template<typename U, std::size_t M, std::size_t N>
    struct rebind
    {
      typedef matrix<U,M,N> type;
    };

    typedef typename super_t::value_type value_type;

    inline matrix() : super_t() {}

    template<typename OtherMatrix>
    inline matrix(const OtherMatrix &other) : super_t(other) {}

    inline matrix(value_type v) : super_t(v) {}

    inline matrix(value_type m00_, value_type m01_, value_type m02_, value_type m03_,
                  value_type m10_, value_type m11_, value_type m12_, value_type m13_,
                  value_type m20_, value_type m21_, value_type m22_, value_type m23_,
                  value_type m30_, value_type m31_, value_type m32_, value_type m33_)
      : m00(m00_), m01(m01_), m02(m02_), m03(m03_),
        m10(m10_), m11(m11_), m12(m12_), m13(m13_),
        m20(m20_), m21(m21_), m22(m22_), m23(m23_),
        m30(m30_), m31(m31_), m32(m32_), m33(m33_)
    {}

    value_type m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33;
}; // end matrix<T,3,3>


typedef matrix<float,3,3> float3x3;
typedef matrix<float,4,4> float4x4;


} // end igloo

