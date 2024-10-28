/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef PROJECTS_TEST_DEV_BLOCKS_3D_H_
#define PROJECTS_TEST_DEV_BLOCKS_3D_H_

#include <abmt/img.h>
#include <abmt/serialize.h>
#include <abmt/math/vec.h>
#include <abmt/math/mat.h>
#include <abmt/math/ray.h>

#include <vector>
#include <memory>
#include <string>

namespace abmt{

struct point3d{
    abmt::vec3 pos   = {0,0,0};
    abmt::col4 color = {255,255,255,255};
    float size       = 1;
    
};

struct line3d{
    abmt::vec3 p1    = {0,0,0};
    abmt::vec3 p2    = {0,0,0};
    abmt::col4 color = {255,255,255,255};

    ray3d ray();
    double len();
};


struct cuboid{
	abmt::pose p;
	abmt::col4 color = {255,255,255,255};
	abmt::vec3 size  = {1,1,1};
};

struct model{
    abmt::pose p;
    std::string path;
};

struct spline{
    abmt::pose p;       // center
    float scale = 0.01; // 100px -> 1m
    std::string path;   // data_path
    abmt::img img;
};

struct scene{
	std::vector<std::shared_ptr<cuboid>>  cubes;
	std::vector<std::shared_ptr<model>>   models;
	std::vector<std::shared_ptr<spline>>  splines;
	std::vector<std::shared_ptr<point3d>> points;
	std::vector<std::shared_ptr<line3d>>  lines;

	/// creates a deep copy of the scene. Every element
	scene copy();

	scene operator+(const scene& lhs);
};

scene operator*(pose x, scene& rhs);


//template<> abmt::serialize::type abmt::serialize::define_type(col4*  target);
template<> abmt::serialize::type abmt::serialize::define_type(pose*  target);
template<> abmt::serialize::type abmt::serialize::define_type(scene* target);

} // namspace abmt;

#endif /* PROJECTS_TEST_DEV_BLOCKS_3D_H_ */
