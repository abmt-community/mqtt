/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */


#ifndef UTIL_MATH_LINE_H_
#define UTIL_MATH_LINE_H_

#include <abmt/math/vec.h>

namespace abmt{

/**
 * Represents a line in parametric form with a point and a direction vector.
 * line_2d (img.h) and line_3d (3d.h) can be converted to a ray.
 */
template<unsigned int N = 3,typename T = double>
class ray{
public:
	vec<N,T> p;
	vec<N,T> dir;

	ray(vec<N,T> p,vec<N,T> d, bool norm_dir = true): p(p),dir(d){
		if(norm_dir){
			dir = dir.norm();
		}
	}

	static ray from_2p(vec<N,T> p1, vec<N,T> p2,bool norm_dir = true){
		vec<N,T> d = p2-p1;
		return ray<N,T>(p1,d,norm_dir);
	}

	vec<N,T> g(T x){
		return p + dir*x;
	}

	/// Returns the point on the ray, that is closest to p2;
	vec<N,T> closest_point(vec<N,T> p2){
		return g(dist_tan(p2));
	}

	T dist_normal(vec<N,T> p2){
		return (p2 - closesd_point(p2)).length();
	}

	T dist_tan(vec<N,T> p2){
		return ((dir*p2).sum()-(p*dir).sum()) / dir.quad_len();
	}

};

typedef ray<3,double> ray3d;
typedef ray<2,double> ray2d;

} // namespace abmt

#endif /* UTIL_MATH_LINE_H_ */
