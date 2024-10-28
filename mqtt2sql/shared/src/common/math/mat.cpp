/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/math/mat.h>

namespace abmt{

mat3 rot_x(double ang_rad){
	mat3 res;
	double s = std::sin(ang_rad);
	double c = std::cos(ang_rad);
	res[0][0] =  1;
	res[1][1] =  c;
	res[1][2] = -s;
	res[2][1] =  s;
	res[2][2] =  c;
	return res;
}

mat3 rot_y(double ang_rad){
	mat3 res;
	double s = std::sin(ang_rad);
	double c = std::cos(ang_rad);
	res[0][0] =  c;
	res[0][2] =  s;
	res[1][1] =  1;
	res[2][0] = -s;
	res[2][2] =  c;
	return res;
}

mat3 rot_z(double ang_rad){
	mat3 res;
	double s = std::sin(ang_rad);
	double c = std::cos(ang_rad);
	res[0][0] =  c;
	res[0][1] = -s;
	res[1][0] =  s;
	res[1][1] =  c;
	res[2][2] = 1;
	return res;
}

mat3 rot_vec(vec3 v, double ang_rad){
	mat3 res;
	double s = std::sin(ang_rad);
	double c = std::cos(ang_rad);
	double omc = 1-c; // omc = one minus std::cos
	vec3 n = v.norm();
	res[0][0] = n[0]*n[0]*omc + c;
	res[0][1] = n[0]*n[1]*omc - n[2]*s;
	res[0][2] = n[0]*n[2]*omc + n[1]*s;

	res[1][0] = n[1]*n[0]*omc + n[2]*s;
	res[1][1] = n[1]*n[1]*omc + c;
	res[1][2] = n[1]*n[2]*omc - n[0]*s;

	res[2][0] = n[2]*n[0]*omc - n[1]*s;
	res[2][1] = n[2]*n[1]*omc + n[0]*s;
	res[2][2] = n[2]*n[2]*omc + c;
	return res;
}

mat3f rot_x(float ang_rad){
	mat3f res;
	float s = std::sin(ang_rad);
	float c = std::cos(ang_rad);
	res[0][0] =  1;
	res[1][1] =  c;
	res[1][2] = -s;
	res[2][1] =  s;
	res[2][2] =  c;
	return res;
}

mat3f rot_y(float ang_rad){
	mat3f res;
	float s = std::sin(ang_rad);
	float c = std::cos(ang_rad);
	res[0][0] =  c;
	res[0][2] =  s;
	res[1][1] =  1;
	res[2][0] = -s;
	res[2][2] =  c;
	return res;
}

mat3f rot_z(float ang_rad){
	mat3f res;
	float s = std::sin(ang_rad);
	float c = std::cos(ang_rad);
	res[0][0] =  c;
	res[0][1] = -s;
	res[1][0] =  s;
	res[1][1] =  c;
	res[2][2] = 1;
	return res;
}

mat3f rot_vec(vec3 v, float ang_rad){
	mat3f res;
	float s = std::sin(ang_rad);
	float c = std::cos(ang_rad);
	float omc = 1-c; // omc = one minus std::cos
	vec3 n = v.norm();
	res[0][0] = n[0]*n[0]*omc + c;
	res[0][1] = n[0]*n[1]*omc - n[2]*s;
	res[0][2] = n[0]*n[2]*omc + n[1]*s;

	res[1][0] = n[1]*n[0]*omc + n[2]*s;
	res[1][1] = n[1]*n[1]*omc + c;
	res[1][2] = n[1]*n[2]*omc - n[0]*s;

	res[2][0] = n[2]*n[0]*omc - n[1]*s;
	res[2][1] = n[2]*n[1]*omc + n[0]*s;
	res[2][2] = n[2]*n[2]*omc + c;
	return res;
}

pose::pose():mat4(), x(rows[0][3]), y(rows[1][3]), z(rows[2][3]){
	*((mat4*)this) = mat4::I();
}

pose::pose(mat4 m):mat4(), x(rows[0][3]), y(rows[1][3]), z(rows[2][3]){
	*((mat4*)this) = m;
}

pose::pose(vec3 v):mat4(), x(rows[0][3]), y(rows[1][3]), z(rows[2][3]){
	*((mat4*)this) = mat4::I();
	x = v.x;
	y = v.x;
	z = v.z;
}

pose::pose(pose& p): mat4(), x(rows[0][3]), y(rows[1][3]), z(rows[2][3]){
	for(int i = 0; i < 4; ++i){
		for(int j = 0; j < 4; ++j){
			rows[i][j] = p.rows[i][j];
		}
	}
}


pose::pose(pose&& p): x(rows[0][3]), y(rows[1][3]), z(rows[2][3]){
	for(int i = 0; i < 4; ++i){
		for(int j = 0; j < 4; ++j){
			rows[i][j] = p.rows[i][j];
		}
	}
}

pose& pose::operator=(mat3 rhs){
	for(int i = 0; i < 3; ++i){
		for(int j = 0; j < 3; ++j){
			rows[i][j] = rhs.rows[i][j];
		}
	}
	return *this;
}

pose& pose::operator=(pose& p){
	for(int i = 0; i < 4; ++i){
		for(int j = 0; j < 4; ++j){
			rows[i][j] = p.rows[i][j];
		}
	}
	return *this;
}

pose& pose::operator=(pose&& p){
	for(int i = 0; i < 4; ++i){
		for(int j = 0; j < 4; ++j){
			rows[i][j] = p.rows[i][j];
		}
	}
	return *this;
}

pose operator*(pose lhs, mat3 rhs){
	pose res = lhs;
	for(int i = 0; i < 3; ++i){
		for(int j = 0; j < 3; ++j){
			double val = 0;
			for(int k = 0; k < 3; ++k){
				val += lhs[i][k]*rhs[j][k];
			}
			res[i][j] = val;
		}
	}
	return res;
}

pose operator*(mat3 lhs, pose rhs){
	pose res = rhs;
	// copy and paste from above
	for(int i = 0; i < 3; ++i){
		for(int j = 0; j < 3; ++j){
			double val = 0;
			for(int k = 0; k < 3; ++k){
				val += lhs[i][k]*rhs[j][k];
			}
			res[i][j] = val;
		}
	}
	return res;
}

vec3 operator*(pose lhs, vec3 rhs){
	vec3 res;
	for(int i = 0; i < 3; ++i){
		for(int j = 0; j < 3; ++j){
			res[i] += lhs[i][j]*rhs[j];
		}
		res[i] += lhs[i][3];
	}
	return res;
}

} // namespace abmt

