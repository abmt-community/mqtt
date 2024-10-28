/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/img.h>
#include "../external/jpgd.h"


using namespace abmt;

template<> img_px<bool_px>::img_px(int32_t w, int32_t h, void* data_ptr, bool take_ptr_ownership){
	type = image_type::BW;
	if(take_ptr_ownership){
		reset(w,h,data_ptr);
	}else{
		realloc(w,h,data_ptr);
	}
}

template<> img_px<bool_px>::img_px(img i){
	type = image_type::BW;
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
				at_2(x,y) = { gray > 127 };
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
				at_2(x,y) = { gray > 127 };
			}
		}
	}else if(i.type == image_type::GRAY8){
		img_gray src(i);
		for(int x = 0; x < width; ++x ){
			for(int y = 0; y < height; ++y ){
				auto& src_px = src.at_2(x,y);
				bool gray = src_px >=  127;
				at_2(x,y) = {gray};
			}
		}
	}else if(i.type == image_type::RGB32){
		img_rgb32 src(i);
		for(int x = 0; x < width; ++x ){
			for(int y = 0; y < height; ++y ){
				auto& src_px = src.at_2(x,y);
				int gray = (float)0.299*255*src_px.r + (float)0.587*255*src_px.g + (float)0.114*255*src_px.b;
				if(gray > 255){
					gray = 255;
				}
				at_2(x,y) = {gray > 127};
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
				at_2(x,y) = {gray > 127};
			}
		}
	}else if(i.type == image_type::GRAY32){
		img_gray32 src(i);
		for(int x = 0; x < width; ++x ){
			for(int y = 0; y < height; ++y ){
				auto& src_px = src.at_2(x,y);
				bool gray = src_px <= 1 ? src_px*255 : 255;
				at_2(x,y) = {gray};
			}
		}
	}else if(i.type == image_type::YUYV){
		uint8_t* src_ptr = (uint8_t*) i.data.data;
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				int pos = y* width + x;
				int idx_src = pos*2;
				at_2(x,y) = src_ptr[idx_src];
			}
		}
	}else if(i.type == image_type::JPEG){
			img_gray src(i); // convert jpg to gray
			for(int x = 0; x < width; ++x ){
				for(int y = 0; y < height; ++y ){
					auto& src_px = src.at_2(x,y);
					bool gray = src_px >=  127;
					at_2(x,y) = {gray};
				}
			}
	}else if(i.type == image_type::BW){
		data = i.data;
	}else{
		width = 0;
		height = 0;
	}
}

