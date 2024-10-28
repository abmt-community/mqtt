/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/img.h>
#include "../external/jpgd.h"

using namespace abmt;

template<> img_px<pixel_gray>::img_px(int32_t w, int32_t h, void* data_ptr, bool take_ptr_ownership){
	type = image_type::GRAY8;
	if(take_ptr_ownership){
		reset(w,h,data_ptr);
	}else{
		realloc(w,h,data_ptr);
	}
}

template<> img_px<pixel_gray>::img_px(img i){
	type = image_type::GRAY8;
	width = i.width;
	height = i.height;
	if(i.type != type && i.type != image_type::JPEG){
		realloc(width,height);
	}
	if(i.type == image_type::RGB8){
		img_rgb src(i);
		for(int x = 0; x < width; ++x ){
			for(int y = 0; y < height; ++y ){
				auto& src_px = src.at_2(x,y);
				int gray = (float)0.299*src_px.r + (float)0.587*src_px.g + (float)0.114*src_px.b;
				if(gray > 255){
					gray = 255;
				}
				at_2(x,y) = {(uint8_t)gray};
			}
		}
	}else if(i.type == image_type::RGBA8){
		img_rgba src(i);

		for(int x = 0; x < width; ++x ){
			for(int y = 0; y < height; ++y ){
				auto& src_px = src.at_2(x,y);
				int gray = (float)0.299*src_px.r + (float)0.587*src_px.g + (float)0.114*src_px.b;
				if(gray > 255){
					gray = 255;
				}
				at_2(x,y) = {(uint8_t)gray};
			}
		}
	}else if(i.type == image_type::GRAY8){
		data = i.data;
	}else if(i.type == image_type::RGB32){
		img_rgb32 src(i);
		for(int x = 0; x < width; ++x ){
			for(int y = 0; y < height; ++y ){
				auto& src_px = src.at_2(x,y);
				int gray = (float)0.299*255*src_px.r + (float)0.587*255*src_px.g + (float)0.114*255*src_px.b;
				if(gray > 255){
					gray = 255;
				}
				at_2(x,y) = {(uint8_t)gray};
			}
		}
	}else if(i.type == image_type::RGBA32){
		img_rgba32 src(i);
		for(int x = 0; x < width; ++x ){
			for(int y = 0; y < height; ++y ){
				auto& src_px = src.at_2(x,y);
				int gray = (float)0.299*255*src_px.r + (float)0.587*255*src_px.g + (float)0.114*255*src_px.b;
				if(gray > 255){
					gray = 255;
				}
				at_2(x,y) = {(uint8_t)gray};
			}
		}
	}else if(i.type == image_type::GRAY32){
		img_gray32 src(i);
		for(int x = 0; x < width; ++x ){
			for(int y = 0; y < height; ++y ){
				auto& src_px = src.at_2(x,y);
				uint8_t gray = src_px <= 1 ? src_px*255 : 255;
				at_2(x,y) = {gray};
			}
		}
	}else if(i.type == image_type::YUYV){
		int num_px = width*height;
		uint8_t* src_ptr = (uint8_t*) i.data.data;
		uint8_t* dst_ptr = (uint8_t*) data.data;
		for(int pos = 0; pos < num_px; pos = pos+2){
			int idx_arr = pos;
			int idx_src = pos*2;
			int y1 = src_ptr[idx_src];
			int y2 = src_ptr[idx_src+2];

			dst_ptr[idx_arr]     = (y1);
			dst_ptr[idx_arr + 1] = (y2);
		}
	}else if(i.type == image_type::JPEG){
		int actual_comps;
		void* img_data = jpgd::decompress_jpeg_image_from_memory((const unsigned char*)i.data.data, i.data.size, &width, &height, &actual_comps, 1);
		int size = actual_comps*width*height;
		if(img_data && actual_comps == 3){
			data.reset(img_data, size);
		}else{
			width = 0;
			height = 0;
		}
	}else if(i.type == image_type::BW){
		img_bw src(i);
		for(int x = 0; x < width; ++x ){
			for(int y = 0; y < height; ++y ){
				auto src_px = src.at_2(x,y);
				uint8_t gray = src_px ? 255 : 0;
				at_2(x,y) = {gray};
			}
		}
	}else{
		width = 0;
		height = 0;
	}
}

