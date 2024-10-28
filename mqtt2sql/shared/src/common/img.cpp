/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/img.h>
#include "external/jpgd.h"
#include "external/jpge.h"

using namespace std;

namespace abmt{

img::img(int32_t w, int32_t h, image_type t, size_t data_size, void* src_data_ptr, bool take_ownership){
	width = w;
	height = h;
	type = t;
	if(data_size != 0){
		if(take_ownership){
			data.reset(src_data_ptr, data_size);
		}else{
			data.realloc(src_data_ptr, data_size);
		}
	}
}

img img::copy(){
	img res;
	res.width = width;
	res.height = height;
	res.type = type;
	res.data.realloc(data.data, data.size);
	return res;
}

img img::copy_for_dst(){
	img res;
	res.width = width;
	res.height = height;
	res.type = type;
	res.data.realloc(data.size);
	return res;
}

bool img::operator==(const img& rhs){
	return rhs.type == type && rhs.width == width && rhs.height == height && rhs.data.data == data.data;
}


template<> abmt::serialize::type abmt::serialize::define_type(img* obj){
	return scene_2d_type({
		/* get_size    */
		[](void* obj) -> size_t {
			return (*(img*)obj).data.size + 9;
		},
		/* obj_to_data */
		[](void* obj, void* data, size_t size) {
			abmt::blob blob(data, size);
			img* img = (abmt::img*)obj;
			blob.set((uint8_t) img->type, 0);
			blob.set((int32_t) img->width, 1);
			blob.set((int32_t) img->height, 5);
			blob.set_mem(img->data.data, img->data.size,9);
		},
		/* data_to_obj */
		[](void* obj, void* data, size_t size) {
			abmt::blob blob(data, size);
			img* img = (abmt::img*)obj;
			img->type = (image_type) blob.get<uint8_t>(0);
			img->width = blob.get<int32_t>(1);
			img->height = blob.get<int32_t>(5);
			img->data.realloc((char*)blob.data + 9, size-9);
		}
	});
}
template<> abmt::serialize::type abmt::serialize::define_type(img_rgb* obj){
	return abmt::serialize::define_type((img*) obj);
}

template<> abmt::serialize::type abmt::serialize::define_type(img_rgba* obj){
	return abmt::serialize::define_type((img*) obj);
}

template<> abmt::serialize::type abmt::serialize::define_type(img_gray* obj){
	return abmt::serialize::define_type((img*) obj);
}

template<> abmt::serialize::type abmt::serialize::define_type(img_rgb32* obj){
	return abmt::serialize::define_type((img*) obj);
}

template<> abmt::serialize::type abmt::serialize::define_type(img_rgba32* obj){
	return abmt::serialize::define_type((img*) obj);
}

template<> abmt::serialize::type abmt::serialize::define_type(img_gray32* obj){
	return abmt::serialize::define_type((img*) obj);
}

template<> abmt::serialize::type abmt::serialize::define_type(img_yuyv* obj){
	return abmt::serialize::define_type((img*) obj);
}

template<> abmt::serialize::type abmt::serialize::define_type(img_jpg* obj){
	return abmt::serialize::define_type((img*) obj);
}

template<> abmt::serialize::type abmt::serialize::define_type(img_bw* obj){
	return abmt::serialize::define_type((img*) obj);
}

ray2d line2d::ray(){
	return ray2d::from_2p(p1, p2);
}

double line2d::len(){
	return (p2-p1).len();
}


}// namespace


