/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef UTIL_MATH_MAT_H_
#define UTIL_MATH_MAT_H_

#include "vec.h"

namespace abmt{

/// M = Rows; N = Cols
template<unsigned int M,unsigned int N,typename T = double>
class mat{
public:
	vec<N,T> rows[M] = {};

	mat(){};

	mat(T val){
		for(unsigned int i = 0;i<M;++i){
			rows[i] = val;
		}
	};

	mat(std::initializer_list<vec<N,T>> rows_lst) {
		unsigned int i = 0;
		for(auto e:rows_lst){
			rows[i] = e;
			i++;
			if(i >= M){
				break;
			}
		}
	}

	template<typename T2>
	mat(const mat<M,N,T2>& src){
		for(unsigned int i = 0;i<M;++i){
			rows[i] = src.rows[i];
		}
	}

	static mat I(){
		mat res;
		unsigned int j = M;
		if(M > N){
			j = N;
		}
		for(unsigned int i = 0;i<j;++i){
			res[i][i] = 1;
		}
		return res;
	}

	size_t num_rows(){
		return M;
	}

	size_t num_cols(){
		return N;
	}

	vec<N,T> get_col(unsigned int idx){
		vec<N,T> res;
		if(idx < M){
			for(unsigned int i = 0; i < N; ++i){
				res[i] = (*this)[i][idx];
			}
			return res;
		}
		abmt::die("Index out of range!!!");
	}

	mat<M,N,T>& operator=(const T& val){
		for(unsigned int i = 0;i<M;++i){
			rows[i] = val;
		}
		return *this;
	}

	mat<M,N,T>& operator=(const mat<M,N,T>& src){
		for(unsigned int i = 0;i<M;++i){
			rows[i] = src.rows[i];
		}
		return *this;
	}



	mat<M,N,T>& operator+=(const mat<M,N,T>& rhs){
		for(unsigned int i = 0;i<M;++i){
			rows[i] += rhs.rows[i];
		}
		return *this;
	}
	mat<M,N,T> operator+( const mat<M,N,T>& rhs ) const {
		mat<M,N,T> result(*this);
		result += rhs;
		return result;
	}
	mat<M,N,T>& operator+=(const T& v){
		for(unsigned int i = 0;i<M;++i){
			rows[i] += v;
		}
		return *this;
	}
	mat<M,N,T> operator+( const T& v ) const {
		mat<M,N,T> result(*this);
		result += v;
		return result;
	}


	mat<M,N,T>& operator-=(const mat<M,N,T>& rhs){
		for(unsigned int i = 0;i<M;++i){
			rows[i] -= rhs.rows[i];
		}
		return *this;
	}
	mat<M,N,T> operator-( const mat<M,N,T>& rhs ) const {
		mat<M,N,T> result(*this);
		result -= rhs;
		return result;
	}
	mat<M,N,T>& operator-=(const T& v){
		for(unsigned int i = 0;i<M;++i){
			rows[i] -= v;
		}
		return *this;
	}
	mat<M,N,T> operator-( const T& v ) const {
		mat<M,N,T> result(*this);
		result -= v;
		return result;
	}


	mat<M,N,T>& operator*=(const T& v){
		for(unsigned int i = 0;i<M;++i){
			rows[i] *= v;
		}
		return *this;
	}
	mat<M,N,T> operator*( const T& v ) const {
		mat<M,N,T> result(*this);
		result *= v;
		return result;
	}

	vec<M,T> operator*(vec<N,T> v_rhs){
		vec<M,T> res;
		for(unsigned int i = 0;i<M;++i){
			res[i] = 0;
			for(unsigned int j = 0;j<N;++j){
				res[i] += (*this)[i][j] * v_rhs[j];
			}
		}
		return res;
	}

	mat<M,N,T>& operator/=(const mat<M,N,T>& rhs){
		for(unsigned int i = 0;i<M;++i){
			rows[i] /= rhs.rows[i];
		}
		return *this;
	}
	mat<M,N,T> operator/( const mat<M,N,T>& rhs ) const {
		mat<M,N,T> result(*this);
		result /= rhs;
		return result;
	}
	mat<M,N,T>& operator/=(const T& v){
		for(unsigned int i = 0;i<M;++i){
			rows[i] /= v;
		}
		return *this;
	}
	mat<M,N,T> operator/( const T& v ) const {
		mat<M,N,T> result(*this);
		result /= v;
		return result;
	}


	mat<M,N,T> operator+() const {
		mat<M,N,T> result(*this);
		return result;
	}

	mat<M,N,T> operator-() const {
		mat<M,N,T> result(*this);
		for(unsigned int i = 0;i<M;++i){
			result.rows[i] = -rows[i];
		}
		return result;
	}

	mat<M,N,T>& operator++() // ++prefix
	{
		for(unsigned int i = 0;i<M;++i){
			++rows[i];
		}
		return *this;
	}

	mat<M,N,T> operator++(int) // postfix++
	{
		mat<M,N,T> tmp(*this);
		operator++();
		return tmp;
	}

	mat<M,N,T>& operator--()
	{
		for(unsigned int i = 0;i<M;++i){
			--rows[i];
		}
		return *this;
	}

	mat<M,N,T> operator--(int)
	{
		mat<M,N,T> tmp(*this);
		operator--();
		return tmp;
	}

	bool operator==( const mat<M,N,T>& rhs ) const {
		bool res = true;
		for(unsigned int i = 0;i<M;++i){
			if(rhs.rows[i] != rows[i]){
				res = false;
				break;
			}
		}
		return res;
	}

	vec<N,T>& operator[]( unsigned int idx ) {
		if(idx < M){
			return rows[idx];
		}
		abmt::die("Index out of range!!!");
	}

	vec<N,T> operator[]( unsigned int idx ) const { // returns value instead of reference
		if(idx < M){
			return rows[idx];
		}
		abmt::die("Index out of range!!!");
	}


	mat<N,M,T> t() const {
		mat<N,M,T> res;
		for(unsigned int n = 0; n < N; ++n){
			for(unsigned int m = 0; m < M; ++m){
				res.rows[n].data[m] = rows[m].data[n];
			}
		}
		return res;
	}

	struct qr{
		mat<M,N,T> q;
		mat<M,N,T> r;
	};

	/// Returns a struct with the attributes q and r containing the decomposition.
	/// The decomposition is done via Householder transformation.
	qr get_qr(){
		static_assert(M == N);
		qr res;
		res.r = *this;
		res.q = mat<M,N,T>::I();
		for(int k = 0; k < (int)M-1; ++k){
			auto col = res.r.get_col(k);
			for(int i = 0; i < k; ++i){
				col[i] = 0;
			}

			T len = col.len();
			auto u = col;
			auto u_minus = col;
			u[k] += len;
			u_minus[k] -= len;
			if(u_minus.quad_len() > u.quad_len()){ // wichtig: { { 2, 2, 3}, { 0, 4, 5}, { 0, 0, 6} }
				u = u_minus;
			}
			auto v = u.norm();
			mat<M,N,T> q_pre;
			for(int i = 0; i < (int)M; ++i){
				for(int j = 0; j < (int)N; ++j){
					q_pre[i][j] = 2*v[i]*v[j];
				}
			}
			auto q_k = mat<M,N,T>::I() - q_pre;

			res.q = res.q*q_k;
			res.r = q_k*res.r;

		} // for k
		return res;
	}


	mat<M,N,T> inverse(){
		static_assert(M == N);
		mat<N,M,T> res;
		auto qr = get_qr();
		for(int row = M-1; row >= 0; --row){ // rows from bottom to top
			T diag_inv = 1/(qr.r[row][row]);
			res[row][row] = diag_inv;
			//scale rest of row
			for(unsigned int c = row +1; c < M; ++c){
				res[row][c] = res[row][c]*diag_inv;
			}
			// fill top of column
			for(int row2 = row-1; row2 >= 0; --row2){
				auto row_scale = -qr.r[row2][row];
				for(int c = row2; c < (int)M; c++){
					res[row2][c] = res[row2][c] + row_scale*res[row][c];
				}
			}
		}
		return res*qr.q.t();
	}

};

template<unsigned int M,unsigned int N,unsigned int I,unsigned int K,typename T = double>
mat<I,K,T> operator*( const mat<I,N,T>& lhs, const mat<M,K,T>& rhs ){
	mat<I,K,T> result;
	for(unsigned int i = 0; i < I; ++i){
		for(unsigned int k = 0; k < K; ++k){
			T v_ik = 0;
			for(unsigned int j = 0; j < M;++j){
				v_ik += lhs.rows[i].data[j] * rhs.rows[j].data[k];
			}
			result.rows[i].data[k] = v_ik;
		}
	}
	return result;
}

/// creates a mat out of a vec. Include <abmt/math/mat.h>
template<unsigned int N,typename T>
mat<N,1,T> vec<N,T>::m(){
	mat<N,1,T> res;
	for(unsigned int i = 0; i < N; ++i){
		res.rows[i].data[0] = this->data[i];
	}
	return res;
}

/// creates a transposed mat out of a vec. Include <abmt/math/mat.h>
template<unsigned int N,typename T>
mat<1,N,T> vec<N,T>::t(){
	mat<1,N,T> res;
	res.rows[0] = *this;
	return res;
}

using mat2 = mat<2,2,double>;
using mat3 = mat<3,3,double>;
using mat4 = mat<4,4,double>;

using mat2f = mat<2,2,float>;
using mat3f = mat<3,3,float>;
using mat4f = mat<4,4,float>;

using mat2i = mat<2,2,int32_t>;
using mat3i = mat<3,3,int32_t>;
using mat4i = mat<4,4,int32_t>;


mat3 rot_x(double ang_rad);
mat3 rot_y(double ang_rad);
mat3 rot_z(double ang_rad);
mat3 rot_vec(vec3 v, double ang_rad);

mat3f rot_x(float ang_rad);
mat3f rot_y(float ang_rad);
mat3f rot_z(float ang_rad);
mat3f rot_vec(vec3 v, float ang_rad);

/// Extends a mat4 with functionality that is usefull when you use it as a pose (Denavit–Hartenberg parameters).
/// All mat4-operators also work with this class.
class pose: public mat4{
public:

	double& x;
	double& y;
	double& z;

	pose();
	pose(mat4 m);
	pose(vec3 pos);
	pose(pose& p);
	pose(pose&& p);

	pose& operator=(mat3 rhs);
	pose& operator=(pose& rhs);
	pose& operator=(pose&& rhs);

};

pose operator*(pose lhs, mat3 rhs);
pose operator*(mat3 lhs, pose rhs);
vec3 operator*(pose lhs, vec3 rhs);

} //namespace abmt

#endif /* UTIL_MATH_MAT_H_ */
