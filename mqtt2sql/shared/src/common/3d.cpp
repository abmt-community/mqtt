/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/3d.h>
#include <abmt/blob.h>

using namespace std;
using namespace abmt;

ray3d line3d::ray(){
	return ray3d::from_2p(p1,p2);
}

double line3d::len(){
	return (p2-p1).len();
}

scene scene::copy(){
	scene res;
	for(auto& c:cubes){
		res.cubes.push_back(std::shared_ptr<cuboid>(new cuboid(*c)));
	}
	for(auto& m:models){
		res.models.push_back(std::shared_ptr<model>(new model(*m)));
	}
	for(auto& s:splines){
		res.splines.push_back(std::shared_ptr<spline>(new spline(*s)));
	}
	for(auto& p:points){
		res.points.push_back(std::shared_ptr<point3d>(new point3d(*p)));
	}
	for(auto& l:lines){
		res.lines.push_back(std::shared_ptr<line3d>(new line3d(*l)));
	}
	return res;
}

scene scene::operator+(const scene& rhs) {
	scene res;
	for(auto& c:cubes){
		res.cubes.push_back(c);
	}
	for(auto& m:models){
		res.models.push_back(m);
	}
	for(auto& s:splines){
		res.splines.push_back(s);
	}
	for(auto& p:points){
		res.points.push_back(p);
	}
	for(auto& l:lines){
		res.lines.push_back(l);
	}

	for(auto& c:rhs.cubes){
		res.cubes.push_back(c);
	}
	for(auto& m:rhs.models){
		res.models.push_back(m);
	}
	for(auto& s:rhs.splines){
		res.splines.push_back(s);
	}
	for(auto& p:rhs.points){
		res.points.push_back(p);
	}
	for(auto& l:rhs.lines){
		res.lines.push_back(l);
	}

	return res;
}

scene abmt::operator*(pose p, scene& rhs){
    auto res = rhs.copy();

	for(auto& c:res.cubes){
		c->p = p*c->p;
	}
	for(auto& m:res.models){
		m->p = p*m->p;
	}
	for(auto& s:res.splines){
		s->p = p*s->p;
	}
	for(auto& po:res.points){
		po->pos = p*po->pos;
	}
	for(auto& l:res.lines){
		l->p1 = p*l->p1;
		l->p2 = p*l->p2;
	}

    return res;
}

template<> abmt::serialize::type abmt::serialize::define_type(col4* target){
	return blob_type({
		/* get_size    */
		[](void* obj) -> size_t {
			return 4;
		},
		/* obj_to_data */
		[](void* obj, void* data, size_t size) {
			abmt::blob blk(data, size);
			abmt::col4*  src = (abmt::col4*)obj;
			blk.set(src->r, 0);
			blk.set(src->g, 1);
			blk.set(src->b, 2);
			blk.set(src->a, 3);
		},
		/* data_to_obj */
		[](void* obj, void* data, size_t size) {
			col4*  dst = (col4*) obj;
			abmt::blob blk(data, size);
			dst->r = blk.get<uint8_t>(0);
			dst->g = blk.get<uint8_t>(1);
			dst->b = blk.get<uint8_t>(2);
			dst->a = blk.get<uint8_t>(3);
		}
	});
}

template<> abmt::serialize::type abmt::serialize::define_type(pose* obj){
	return scene_3d_type({
		/* get_size    */
	    [](void* obj) -> size_t {
	    	return 4*4*4;
	    },
		/* obj_to_data */
	    [](void* obj, void* data, size_t size) {
	    	pose*  src = (pose*)obj;
	    	abmt::blob blk(data, size);
	    	int cnt = 0;
	    	for(int i = 0; i < 4; ++i){
	    		for(int j = 0; j < 4; ++j){
	    			blk.set((float)(*src)[j][i], cnt*sizeof(float)); // column-major because of opengl :(
	    			++cnt;
				}
	    	}
	    },
		/* data_to_obj */
		[](void* obj, void* data, size_t size) {
	    	abmt::blob blk(data, size);
			pose*  dst = (pose*) obj;
			int cnt = 0;
			for(int i = 0; i < 4; ++i){
				for(int j = 0; j < 4; ++j){
					(*dst)[j][i] = blk.get<float>(cnt*sizeof(float)); // column-major because of opengl :(
					++cnt;
				}
			}
	    }
	});
}

// Don't try this at home. It's a mess...
template<> abmt::serialize::type abmt::serialize::define_type(scene* obj){
	return scene_3d_type({
		/* get_size    */
	    [](void* obj) -> size_t {
	        auto& s = *(scene*)obj;
	        auto image_type = abmt::serialize::define_type<abmt::img>(0);
	        size_t size = 0;
	        // cubes
	    	// 4 = uint32_t -> num obj
	    	// pose_size = 4*4*4 = 64;
	    	// color = 4;
	    	// 4*3 = width(x) + height(y) + depth(z)
	    	size +=  4 + (4*4*4 + 4 + 4*3) * s.cubes.size();
	    	
	    	// models
	    	size += 4; // number of models 
	    	for(auto& m: s.models){
	    	    size += 4*4*4 + 4 + m->path.size();
	    	}
	    	
	    	// splines
	    	size += 4; // number of models 
	    	for(auto& sp: s.splines){
	    	    size += 4*4*4 + 4 + 4 + sp->path.size() + 4 + image_type.serialize_info.rexp.get_size(&sp->img); // pose + scale + str_len + str + img_size + img
	    	}
	    	
	    	//points
	    	size += 4 + (4 + 4*3 + 4)*s.points.size(); // 4 = color;  4*3 = pos;  +4 =  size
	    	
	    	//lines
	    	size += 4 + ( 4*3*2 + 4 + 4)*s.lines.size(); // 4*3*2 = points; 4 = color  +4 =  size
	    	
	    	return size;
	    },
		/* obj_to_data */
	    [](void* obj, void* data, size_t size) {
	        abmt::blob blob(data, size);
	    	scene* s = (scene*) obj;
	    	char* data_ptr = (char*)data;
	    	size_t data_pos = 0;
	    	uint32_t* num_obj = (uint32_t*)data;
	    	*num_obj = s->cubes.size();
	    	data_pos += sizeof(uint32_t);
	    	auto mat_type = abmt::serialize::define_type<pose>(0);
	    	auto color_type = abmt::serialize::define_type<col4>(0);
	    	auto image_type = abmt::serialize::define_type<abmt::img>(0);
	    	for(auto& c: s->cubes){
	    		mat_type.serialize_info.rexp.obj_to_data(&(c->p), &(data_ptr[data_pos]), size - data_pos);
	    		data_pos += mat_type.serialize_info.rexp.get_size(&(c->p));
	    		color_type.serialize_info.rexp.obj_to_data(&(c->color), &(data_ptr[data_pos]), size - data_pos);
	    		data_pos += color_type.serialize_info.rexp.get_size(&(c->color));

	    		// Hier wird ein double vector in einen float gewandelt
	    		// dehalb kann nicht der vec3-Exporter verwedet werden
	    		float* s_x = (float*)&data_ptr[data_pos];
	    		float* s_y = (float*)&data_ptr[data_pos + 4];
	    		float* s_z = (float*)&data_ptr[data_pos + 8];
	    		*s_x = c->size.x;
	    		*s_y = c->size.y;
	    		*s_z = c->size.z;

	    		data_pos += 3*4;
	    	}
	    	
	    	// models
	    	*(uint32_t*)&data_ptr[data_pos] = s->models.size();
	    	data_pos += sizeof(uint32_t);
	    	for(auto& m: s->models){
	    	    mat_type.serialize_info.rexp.obj_to_data(&(m->p), &(data_ptr[data_pos]), size - data_pos);
	    	    data_pos += mat_type.serialize_info.rexp.get_size(&(m->p));
	    	    auto str_size = m->path.size();
	    	    *(uint32_t*)&data_ptr[data_pos] = str_size;
	    	    data_pos += sizeof(uint32_t);
	    	    memcpy(&(data_ptr[data_pos]), m->path.c_str(), str_size);
	    	    data_pos += str_size;
	    	}
	    	
	    	// splines
	    	*(uint32_t*)&data_ptr[data_pos] = s->splines.size();
	    	data_pos += sizeof(uint32_t);
	    	for(auto& sp: s->splines){
	    	    mat_type.serialize_info.rexp.obj_to_data(&(sp->p), &(data_ptr[data_pos]), size - data_pos);
	    	    data_pos += mat_type.serialize_info.rexp.get_size(&(sp->p));
	    	    *(float*)&data_ptr[data_pos] = sp->scale;
	    	    data_pos += sizeof(float);
	    	    uint32_t str_size = sp->path.size();
	    	    *(uint32_t*)&data_ptr[data_pos] = str_size;
	    	    data_pos += sizeof(uint32_t);
	    	    memcpy(&(data_ptr[data_pos]), sp->path.c_str(), str_size);
	    	    data_pos += str_size;
	    	    uint32_t img_size = image_type.serialize_info.rexp.get_size(&(sp->img));
	    	    *(uint32_t*)&data_ptr[data_pos] = img_size;
	    	    data_pos += sizeof(uint32_t);
	    	    image_type.serialize_info.rexp.obj_to_data(&(sp->img), &(data_ptr[data_pos]), img_size);
	    	    data_pos += img_size;
	    	}

            // points
            auto num_points = s->points.size();
            *(uint32_t*)&data_ptr[data_pos] = num_points;
	    	data_pos += sizeof(uint32_t);
	    	for(uint32_t i = 0; i < num_points; ++i){
	    	    auto& p = s->points[i];
	    	    blob.set((float) p->pos[0], data_pos);
	    	    blob.set((float) p->pos[1], data_pos + 4);
	    	    blob.set((float) p->pos[2], data_pos + 8);
	    	    data_pos += sizeof(float) * 3;
	    	}
	    	for(uint32_t i = 0; i < num_points; ++i){
	    	    auto& p = s->points[i];
	    	    blob.set((uint8_t) p->color[0], data_pos);
	    	    blob.set((uint8_t) p->color[1], data_pos + 1);
	    	    blob.set((uint8_t) p->color[2], data_pos + 2);
	    	    blob.set((uint8_t) p->color[3], data_pos + 3);
	    	    data_pos += 4;
	    	}
	    	for(uint32_t i = 0; i < num_points; ++i){
	    	    blob.set((float) s->points[i]->size, data_pos);
	    	    data_pos += sizeof(float);
	    	}
	    	
	    	// lines
            auto num_lines = s->lines.size();
            blob.set((uint32_t) num_lines,data_pos);
	    	data_pos += sizeof(uint32_t);
	    	for(uint32_t i = 0; i < num_lines; ++i){
	    	    auto& l = s->lines[i];
				blob.set((float) l->p1[0], data_pos + 0);
				blob.set((float) l->p1[1], data_pos + 4);
				blob.set((float) l->p1[2], data_pos + 8);
				blob.set((float) l->p2[0], data_pos +12);
				blob.set((float) l->p2[1], data_pos +16);
				blob.set((float) l->p2[2], data_pos +20);
	    	    data_pos += sizeof(float) * 3 * 2;
	    	}
	    	for(uint32_t i = 0; i < num_lines; ++i){
	    	    auto& l = s->lines[i];
	    	    blob.set(l->color[0], data_pos);
	    	    blob.set(l->color[1], data_pos + 1);
	    	    blob.set(l->color[2], data_pos + 2);
	    	    blob.set(l->color[3], data_pos + 3);
	    	    data_pos += 4;
	    	}
	    	
	    }, // obj_to_data
	    
		/* data_to_obj */
		// todo: not tested
		[](void* obj, void* data, size_t size) {
		    abmt::blob blob(data, size);
	    	scene* s = (scene*) obj;
	    	s->cubes.clear();
	    	char* data_ptr = (char*)data;
	    	size_t data_pos = 0;
	    	uint32_t num_obj = *(uint32_t*)data;
	    	data_pos = sizeof(uint32_t);
	    	auto mat_type   = abmt::serialize::define_type<pose>(0);
	    	auto color_type = abmt::serialize::define_type<col4>(0);
	    	auto image_type = abmt::serialize::define_type<abmt::img>(0);
	    	s->cubes.clear();
	    	for(uint32_t i = 0; i < num_obj; ++i){
	    		auto p = make_shared<cuboid>();
	    		mat_type.serialize_info.rexp.data_to_obj(&(p->p), &(data_ptr[data_pos]),size - data_pos);
	    		data_pos += mat_type.get_size();
	    		color_type.serialize_info.rexp.data_to_obj(&(p->color), &(data_ptr[data_pos]),size - data_pos);
				data_pos += color_type.get_size();
	    		float* s_x = (float*)&data_ptr[data_pos];
				float* s_y = (float*)&data_ptr[data_pos + 4];
				float* s_z = (float*)&data_ptr[data_pos + 8];
				p->size.x = *s_x;
				p->size.y = *s_y;
				p->size.z = *s_z;
				data_pos += 3*4;
				
				s->cubes.push_back(p);
	    	}
	    	// models
	    	s->models.clear();
	    	num_obj = *(uint32_t*)&data_ptr[data_pos];
	    	data_pos += sizeof(uint32_t);
	    	for(uint32_t i = 0; i < num_obj; ++i){
	    	    auto p = make_shared<model>();
	    	    mat_type.serialize_info.rexp.data_to_obj(&(p->p), &(data_ptr[data_pos]),size - data_pos);
	    		data_pos += mat_type.get_size();
	    		auto str_len = *(uint32_t*)&data_ptr[data_pos];
	    		data_pos += sizeof(uint32_t);
	    		p->path = std::string(&data_ptr[data_pos], str_len);
	    		s->models.push_back(p);
	    	}
	    	
	    	// splintes
	    	s->splines.clear();
	    	num_obj = *(uint32_t*)&data_ptr[data_pos];
	    	data_pos += sizeof(uint32_t);
	    	for(uint32_t i = 0; i < num_obj; ++i){
	    	    auto sp = make_shared<spline>();
	    	    mat_type.serialize_info.rexp.data_to_obj(&(sp->p), &(data_ptr[data_pos]), size - data_pos);
	    	    data_pos += mat_type.serialize_info.rexp.get_size(&(sp->p));
	    	    sp->scale = *(float*)&data_ptr[data_pos];
	    	    data_pos += sizeof(float);
	    	    auto str_len = *(uint32_t*)&data_ptr[data_pos];
	    		data_pos += sizeof(uint32_t);
	    		sp->path = std::string(&data_ptr[data_pos], str_len);
	    		auto img_len = *(uint32_t*)&data_ptr[data_pos];
	    		data_pos += sizeof(uint32_t);
	    		image_type.serialize_info.rexp.data_to_obj(&(sp->img), &(data_ptr[data_pos]), img_len);
	    	    data_pos += image_type.serialize_info.rexp.get_size(&(sp->img));
	    		
	    		s->splines.push_back(sp);
	    	}
	    	
	    	//points
	    	auto num_points = blob.get<uint32_t>(data_pos);
	    	data_pos += sizeof(uint32_t);
	    	s->points.clear();
	    	for(uint32_t i = 0; i < num_points; ++i){
	    	    auto& p = s->points[i];
	    	    p->pos[0] = blob.get<float>(data_pos);
	    	    p->pos[1] = blob.get<float>(data_pos + 4);
	    	    p->pos[2] = blob.get<float>(data_pos + 8);
	    	    data_pos += sizeof(float) * 3;
	    	}
	    	for(uint32_t i = 0; i < num_points; ++i){
	    	    auto p = make_shared<point3d>();
	    	    p->color[0] = blob.get<uint8_t>(data_pos);
	    	    p->color[1] = blob.get<uint8_t>(data_pos + 1);
	    	    p->color[2] = blob.get<uint8_t>(data_pos + 2);
	    	    p->color[3] = blob.get<uint8_t>(data_pos + 3);
	    	    data_pos += sizeof(float) * 4;
	    	    s->points.push_back(p);
	    	}
	    	for(uint32_t i = 0; i < num_points; ++i){
	    	    s->points[i]->size = blob.get<float>(data_pos);
	    	    data_pos += sizeof(float);
	    	}
	    	
	    	//lines
	    	auto num_lines = blob.get<uint32_t>(data_pos);
	    	data_pos += sizeof(uint32_t);
	    	s->lines.clear();
	    	for(uint32_t i = 0; i < num_lines; ++i){
	    	    auto l = make_shared<line3d>();
	    	    l->p1[0] = blob.get<float>(data_pos);
	    	    l->p1[1] = blob.get<float>(data_pos + 4);
	    	    l->p1[2] = blob.get<float>(data_pos + 8);
	    	    l->p2[0] = blob.get<float>(data_pos + 12);
	    	    l->p2[1] = blob.get<float>(data_pos + 16);
	    	    l->p2[2] = blob.get<float>(data_pos + 20);
	    	    data_pos += sizeof(float) * 3 * 2;
	    	    s->lines.push_back(l);
	    	}
	    	for(uint32_t i = 0; i < num_lines; ++i){
	    	    auto& l = s->lines[i];
	    	    l->color[0] = blob.get<uint8_t>(data_pos);
	    	    l->color[1] = blob.get<uint8_t>(data_pos + 1);
	    	    l->color[2] = blob.get<uint8_t>(data_pos + 2);
	    	    l->color[3] = blob.get<uint8_t>(data_pos + 3);
	    	    data_pos +=  4;
	    	   
	    	}
	    	
	    } // data_to_obj
	});
}
