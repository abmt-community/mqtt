/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/img.h>
#include "../external/jpgd.h"
#include "../external/jpge.h"

using namespace abmt;

img_jpg::img_jpg(){
	width = 0;
	height = 0;
}

img_jpg::img_jpg(img i, unsigned int quality){
	type = image_type::JPEG;
	width = i.width;
	height = i.height;
	jpge::params p;
	if (quality < 1){
		quality = 1;
	}
	if(quality > 100){
		quality = 100;
	}
	p.m_quality = quality;
	if(i.type == image_type::RGB8){
		data.realloc(width*height*3);
		int new_size = data.size;
		bool res = jpge::compress_image_to_jpeg_file_in_memory(data.data, new_size, width, height, 3, (const uint8_t*) i.data.data, p);
		data.size = new_size; // important for serialize::type!
		if(res == false){
			width = 0;
			height = 0;
		}
	}else if(i.type == image_type::RGBA8){
		data.realloc(width*height*3);
		int new_size = data.size;
		bool res = jpge::compress_image_to_jpeg_file_in_memory(data.data, new_size, width, height, 4, (const uint8_t*) i.data.data, p);
		data.size = new_size; // important for serialize::type!
		if(res == false){
			width = 0;
			height = 0;
		}
	}else if(i.type == image_type::GRAY8){
		data.realloc(width*height);
		int new_size = data.size;
		bool res = jpge::compress_image_to_jpeg_file_in_memory(data.data, new_size, width, height, 1, (const uint8_t*) i.data.data, p);
		data.size = new_size; // important for serialize::type!
		if(res == false){
			width = 0;
			height = 0;
		}

	}else if(i.type == image_type::RGB32 || i.type == image_type::RGBA32 || i.type == image_type::YUYV){
		img_rgb src(i); // convert to rgb8
		data.realloc(width*height*3);
		int new_size = data.size;
		bool res = jpge::compress_image_to_jpeg_file_in_memory(data.data, new_size, width, height, 3, (const uint8_t*) src.data.data, p);
		data.size = new_size; // important for serialize::type!
		if(res == false){
			width = 0;
			height = 0;
		}
	}else if(i.type == image_type::GRAY32){
		img_gray src(i); // convert to gray8
		data.realloc(width*height);
		int new_size = data.size;
		bool res = jpge::compress_image_to_jpeg_file_in_memory(data.data, new_size, width, height, 1, (const uint8_t*) src.data.data, p);
		data.size = new_size; // important for serialize::type!
		if(res == false){
			width = 0;
			height = 0;
		}
	}else if(i.type == image_type::BW){
		img_gray src(i); // convert to gray8
		data.realloc(width*height);
		int new_size = data.size;
		bool res = jpge::compress_image_to_jpeg_file_in_memory(data.data, new_size, width, height, 1, (const uint8_t*) src.data.data, p);
		data.size = new_size; // important for serialize::type!
		if(res == false){
			width = 0;
			height = 0;
		}
	}else if(i.type == image_type::JPEG){
		data = i.data;
	}else{
		width = 0;
		height = 0;
	}
}

img_jpg::img_jpg(abmt::blob_shared d){
	data = d;
	width = 0;
	height = 0;
	type = image_type::JPEG;
}

