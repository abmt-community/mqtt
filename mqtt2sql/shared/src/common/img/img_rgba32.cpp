/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/img.h>
#include "../external/jpgd.h"

using namespace abmt;

template<> img_px<pixel_rgba32>::img_px(int32_t w, int32_t h, void* data_ptr, bool take_ptr_ownership){
	type = image_type::RGBA32;
	if(take_ptr_ownership){
		reset(w,h,data_ptr);
	}else{
		realloc(w,h,data_ptr);
	}
}

template<> img_px<pixel_rgba32>::img_px(img i){
	type = image_type::RGBA32;
	width = i.width;
	height = i.height;
	if(i.type == image_type::JPEG){
		img_rgb converted_img(i);
		i = converted_img;
	}
	if(i.type != type){
		realloc(width,height);
	}


	if(i.type == image_type::RGB8){
		img_rgb src(i);
		for(int x = 0; x < width; ++x ){
			for(int y = 0; y < height; ++y ){
				auto& src_px = src.at_2(x,y);
				at_2(x,y) = {src_px.r/(float)255.0, src_px.g/(float)255.0, src_px.b/(float)255.0, 1};
			}
		}
	}else if(i.type == image_type::RGBA8){
		img_rgba src(i);
		for(int x = 0; x < width; ++x ){
			for(int y = 0; y < height; ++y ){
				auto& src_px = src.at_2(x,y);
				at_2(x,y) = {src_px.r/(float)255.0, src_px.g/(float)255.0, src_px.b/(float)255.0, src_px.a/(float)255.0};
			}
		}
	}else if(i.type == image_type::GRAY8){
		img_gray src(i);
		for(int x = 0; x < width; ++x ){
			for(int y = 0; y < height; ++y ){
				auto& src_px = src.at_2(x,y);
				at_2(x,y) = {src_px/(float)255.0, src_px/(float)255.0, src_px/(float)255.0, 1};
			}
		}
	}else if(i.type == image_type::RGB32){
		img_rgb32 src(i);
		for(int x = 0; x < width; ++x ){
			for(int y = 0; y < height; ++y ){
				auto& src_px = src.at_2(x,y);
				at_2(x,y) = {src_px.r, src_px.g, src_px.b, 1};
			}
		}
	}else if(i.type == image_type::RGBA32){
		data = i.data;
	}else if(i.type == image_type::GRAY32){
		img_gray32 src(i);
		for(int x = 0; x < width; ++x ){
			for(int y = 0; y < height; ++y ){
				auto& src_px = src.at_2(x,y);
				at_2(x,y) = {  src_px,  src_px,  src_px, 1};
			}
		}
	}else if(i.type == image_type::YUYV){
		int num_px = width*height;
		uint8_t* src_ptr = (uint8_t*) i.data.data;
		float* dst_ptr = (float*) data.data;
		for(int pos = 0; pos < num_px; pos = pos+2){
			int idx_arr = pos*4;
			int idx_src = pos*2;
			int y1 = src_ptr[idx_src];
			int u  = src_ptr[idx_src+1] - 128;
			int y2 = src_ptr[idx_src+2];
			int v  = src_ptr[idx_src+3] - 128;
			float fr = 1.402*v;
			float fg = -0.344*u -0.714*v;
			float fb = 1.772*u;

			dst_ptr[idx_arr]   = (y1 + fr) <= 255 ? (y1 + fr)/255 : 1 ; // ugly
			dst_ptr[idx_arr+1] = (y1 + fg) <= 255 ? (y1 + fg)/255 : 1 ;
			dst_ptr[idx_arr+2] = (y1 + fb) <= 255 ? (y1 + fb)/255 : 1 ;
			dst_ptr[idx_arr+3] = 1;

			dst_ptr[idx_arr+4] = (y2 + fr) <= 255 ? (y2 + fr)/255 : 1 ;
			dst_ptr[idx_arr+5] = (y2 + fg) <= 255 ? (y2 + fg)/255 : 1 ;
			dst_ptr[idx_arr+6] = (y2 + fb) <= 255 ? (y2 + fb)/255 : 1 ;
			dst_ptr[idx_arr+7] = 1;
		}
	}else if(i.type == image_type::BW){
		img_bw src(i);
		for(int x = 0; x < width; ++x ){
			for(int y = 0; y < height; ++y ){
				auto src_px = src.at_2(x,y);
				float px = src_px ? 1.0 : 0;
				at_2(x,y) = {px, px, px, 1.0};
			}
		}
	}else{
		// image_type::JPEG handled at the top.
		width = 0;
		height = 0;
	}
}

