#ifndef VERTEX_H
#define VERTEX_H

#include <cstring>
#include <exception>
#include <string>
#include <stdio.h>
#include <math.h>

namespace util {
	/**
	 * N-Dimensional vertex of basetype B. 
	 * is used in GFX, world, every POINT can be converted to evth. 
	 * 
	 * Speciality about this version: 
	 * You cannot have a dimension > 4 with a vertex(a,b,c) ctor!
	 * 
	 * Version 1.4 ( 30. December 2012 )
	 * (c) Johannes Walcher, all rights reserved
	 * NOT COMPATIBLE WITH VERSIONS < 1.0!!
	 */
	template <int _DIM, typename _T>
	class vertex {
	private:
		_T f[_DIM];
	public:
		vertex() {
			memset(f, 0, _DIM*sizeof(_T));
		};

		vertex(_T s[_DIM]) {
			memcpy(f, s, _DIM*sizeof(_T));
		};
	
		vertex(const vertex<_DIM, _T> &s) {
			memcpy(f, s.f, _DIM*sizeof(_T));
		};
	
		/**
		 * Now this can be used as vertex(_T, _T, _T)
		 */
		//Initialising the first 4 ctors
		vertex(_T fst) {
			f[0] = fst;
		}
		vertex(_T fst, _T snd) {
			
			if (_DIM < 2) {
				printf("vertex has too few dimensions");
				throw std::exception();
			}
			f[0] = fst;
			f[1] = snd;
		}
		vertex(_T fst, _T snd, _T trd) {
			if (_DIM < 3) {
				printf("vertex has too few dimensions");
				throw std::exception();
			}
			f[0] = fst;
			f[1] = snd;
			f[2] = trd;
		}
		vertex(_T arg1, _T arg2, _T arg3, _T arg4) {
			if (_DIM < 4) {
				printf("vertex has too few dimensions");
				throw std::exception();
			}
			f[0] = arg1;
			f[1] = arg2;
			f[2] = arg3;
			f[4] = arg4;
		}

		template <typename _Rhs_Type>
		vertex<_DIM, _T> &operator=(const vertex<_DIM, _Rhs_Type> &rhs) 
		{
			for (int i= 0; i < _DIM; i++) {
				f[i] = (_T)rhs[i];
			}
			return *this;
		}

		/*
		vertex (_T first, ...) 
		{
			f[0] = first;
			int n = _DIM;
			if (_DIM > 1) {
				va_list params; // Zugriffshandle für Parameter
				va_start(params, n); // Zugriff vorbereiten
				for (int i = 1; i <_DIM; ++i) {
					f[i] = va_arg(params, _T);
				}
				va_end(params);
			}
		};
		*/
		float length() const {
			_T sum = 0;
			for (int i = 0; i < _DIM; ++i) sum += f[i] * f[i];
			return sqrt((float)sum);
		}
		
		double d_length() const {
			double sum = 0;
			for (int i = 0; i < _DIM; ++i) sum += f[i] * f[i];
			return  sqrt((double)sum);
		}

		
		void normalize() {
			float l = length();
			for (int i = 0; i < _DIM; ++i) 
				f[i] = (_T)(f[i]/l);
		}

		_T at(unsigned int p) const {
			if (p < _DIM) return f[p];
			else {
				printf("Vertex index out of bounds");
				throw std::exception();
			}
		}

		operator vertex<_DIM+1, _T>() 
		{
			vertex<_DIM+1, _T> v;
			memcpy(&v[0], f, _DIM*sizeof(_T));
			v[_DIM] = 0;
			return v;
		}

		_T operator[](unsigned int p) const{
			return f[p];
		}

		_T& operator[](unsigned int p) {
			return f[p];
		}

		bool operator == (const vertex<_DIM, _T>& s) const {
			for (int i = 0; i < _DIM; ++i) {
				if (f[i] != s.f[i])
					return false;
			}
			return true;
		};
	
		bool operator < (const vertex<_DIM, _T>& s) const {
			for (int i = 0; i < _DIM; ++i) {
				if (f[i] >= s.f[i])
					return false;
			}
			return true;
		};

		vertex<_DIM, _T> operator+ (const vertex<_DIM, _T>& rhs) const {
			vertex<_DIM, _T> v(*this);
			v += rhs;
			return v;
		};

		vertex<_DIM, _T>& operator+= (const vertex<_DIM, _T>& s) {
			for (int i = 0; i < _DIM; ++i)
				f[i] += s.f[i];
			return *this;
		};

		vertex<_DIM, _T> operator- (const vertex<_DIM, _T>& rhs) const {
			vertex<_DIM, _T> v(*this);
			v -= rhs;
			return v;
		};

		vertex<_DIM, _T>& operator-= (const vertex<_DIM, _T>& s) {
			for (int i = 0; i < _DIM; ++i)
				f[i] -= s.f[i];
			return *this;
		};
		
		template <typename _RHS>
		vertex<_DIM, _T> operator* (const _RHS &d) const {
			vertex<_DIM, _T> r;
			for (int i = 0; i < _DIM; ++i)
				r[i] = _T(f[i] * d);
			return r;
		}

		template <typename _RHS>
		operator vertex<_DIM, _RHS> () {
			vertex<_DIM, _RHS> r;
			for (int i = 0; i < _DIM; ++i)
				r[i] = _RHS(f[i]);
			return r;
		}	

		_T dot(const vertex<_DIM, _T> &v) const {
			_T r = _T();
			for (int i = 0; i < _DIM; i++) {
				r += f[i] * v.f[i];
			}
			return r;
		}
	
		vertex<_DIM, _T> cross(const vertex<_DIM, _T> &rhs) const {
			vertex<_DIM, _T> v;

			v[0] = f[1]*rhs[2] - f[2]*rhs[1];
			v[1] = f[2]*rhs[0] - f[0]*rhs[2];
			v[2] = f[0]*rhs[1] - f[1]*rhs[0];

			return v;
		}

	};
}

typedef util::vertex<2, int>    vertex2i;
typedef util::vertex<3, int>    vertex3i;
typedef util::vertex<2, float>  vertex2f;
typedef util::vertex<3, double> vertex3d;
typedef util::vertex<3, float>  vertex3f;

#endif

