/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/img.h>
#include "../external/jpgd.h"

using namespace abmt;

img_yuyv::img_yuyv(img i){
	type = image_type::YUYV;
	if(i.type == image_type::JPEG){
		img_rgb converted_img(i);
		i = converted_img;
	}
	width = i.width;
	height = i.height;
	if(width % 2 == 1){
		width = 0;
		height = 0;
		return;
	}
	data.realloc(width*height*2);

	if(i.type == image_type::RGB8){
		img_rgb src(i);
		uint8_t* data_ptr = (uint8_t*) data.data;
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x += 2){
				auto px1 = src.at_2(x,   y);
				auto px2 = src.at_2(x+1, y);

				float y1  = (float) 0.299*px1.r + (float)0.587*px1.g +  (float)0.114*px1.b;
				float cb1 = (float)-0.169*px1.r - (float)0.331*px1.g +  (float)0.499*px1.b + 128;
				float cr1 = (float) 0.499*px1.r - (float)0.418*px1.g -  (float)0.0813*px1.b + 128;
				float y2  = (float) 0.299*px2.r + (float)0.587*px2.g +  (float)0.114*px2.b;
				float cb2 = (float)-0.169*px2.r - (float)0.331*px2.g +  (float)0.499*px2.b + 128;
				float cr2 = (float) 0.499*px2.r - (float)0.418*px2.g -  (float)0.0813*px2.b + 128;
				float cb = (cb1 + cb2) / 2 + (float)0.5;
				float cr = (cr1 + cr2) / 2 + (float)0.5;
				if(y1 > 255){
					y1 = 255;
				}
				if(y2 > 255){
					y2 = 255;
				}
				if(cb > 255){
					cb = 255;
				}
				if(cr > 255){
					cr = 255;
				}
				data_ptr[y*width*2+x*2    ] = (uint8_t)y1;
				data_ptr[y*width*2+x*2 + 1] = (uint8_t)cb;
				data_ptr[y*width*2+x*2 + 2] = (uint8_t)y2;
				data_ptr[y*width*2+x*2 + 3] = (uint8_t)cr;
 			}
		}
	}else if(i.type == image_type::RGBA8){
		img_rgba src(i);
		// duplicate code from image_type::RGB8 only src type has changed
		// future me will make an template lambda here to please the ideology.
		uint8_t* data_ptr = (uint8_t*) data.data;
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x += 2){
				auto px1 = src.at_2(x,   y);
				auto px2 = src.at_2(x+1, y);

				float y1  = (float) 0.299*px1.r + (float)0.587*px1.g + (float)0.114*px1.b;
				float cb1 = (float)-0.169*px1.r - (float)0.331*px1.g + (float)0.499*px1.b + 128;
				float cr1 = (float) 0.499*px1.r - (float)0.418*px1.g - (float)0.0813*px1.b + 128;
				float y2  = (float) 0.299*px2.r + (float)0.587*px2.g + (float)0.114*px2.b;
				float cb2 = (float)-0.169*px2.r - (float)0.331*px2.g + (float)0.499*px2.b + 128;
				float cr2 = (float) 0.499*px2.r - (float)0.418*px2.g - (float)0.0813*px2.b + 128;
				float cb = (cb1 + cb2) / 2 + (float)0.5;
				float cr = (cr1 + cr2) / 2 + (float)0.5;
				if(y1 > 255){
					y1 = 255;
				}
				if(y2 > 255){
					y2 = 255;
				}
				if(cb > 255){
					cb = 255;
				}
				if(cr > 255){
					cr = 255;
				}
				data_ptr[y*width*2+x*2    ] = (uint8_t)y1;
				data_ptr[y*width*2+x*2 + 1] = (uint8_t)cb;
				data_ptr[y*width*2+x*2 + 2] = (uint8_t)y2;
				data_ptr[y*width*2+x*2 + 3] = (uint8_t)cr;
			}
		}
	}else if(i.type == image_type::GRAY8){
		img_gray src(i);
		uint8_t* data_ptr = (uint8_t*) data.data;
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x += 2){
				data_ptr[y*width*2+x*2    ] = src.at_2(x,y);
				data_ptr[y*width*2+x*2 + 1] = 128;
				data_ptr[y*width*2+x*2 + 2] = src.at_2(x+1,y);
				data_ptr[y*width*2+x*2 + 3] = 128;
			}
		}
	}else if(i.type == image_type::RGB32){
		img_rgb32 src(i);
		// duplicate code from image_type::RGB8 only src type and scaling has changed
		// future me will make an template lambda here to please the ideology.
		uint8_t* data_ptr = (uint8_t*) data.data;
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x += 2){
				auto px1 = src.at_2(x,   y);
				auto px2 = src.at_2(x+1, y);

				float y1  = (float) 0.299*255*px1.r + (float)0.587*255*px1.g + (float)0.114*255*px1.b;
				float cb1 = (float)-0.169*255*px1.r - (float)0.331*255*px1.g + (float)0.499*255*px1.b + 128;
				float cr1 = (float) 0.499*255*px1.r - (float)0.418*255*px1.g - (float)0.0813*255*px1.b + 128;
				float y2  = (float) 0.299*255*px2.r + (float)0.587*255*px2.g + (float)0.114*255*px2.b;
				float cb2 = (float)-0.169*255*px2.r - (float)0.331*255*px2.g + (float)0.499*255*px2.b + 128;
				float cr2 = (float) 0.499*255*px2.r - (float)0.418*255*px2.g - (float)0.0813*255*px2.b + 128;
				float cb = (cb1 + cb2) / 2 + (float)0.5;
				float cr = (cr1 + cr2) / 2 + (float)0.5;
				if(y1 > 255){
					y1 = 255;
				}
				if(y2 > 255){
					y2 = 255;
				}
				if(cb > 255){
					cb = 255;
				}
				if(cr > 255){
					cr = 255;
				}
				data_ptr[y*width*2+x*2    ] = (uint8_t)y1;
				data_ptr[y*width*2+x*2 + 1] = (uint8_t)cb;
				data_ptr[y*width*2+x*2 + 2] = (uint8_t)y2;
				data_ptr[y*width*2+x*2 + 3] = (uint8_t)cr;
			}
		}
	}else if(i.type == image_type::RGBA32){
		img_rgba32 src(i);
		// duplicate code from image_type::RGB8 only src type and scaling has changed
		// future me will make an template lambda here to please the ideology.
		uint8_t* data_ptr = (uint8_t*) data.data;
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x += 2){
				auto px1 = src.at_2(x,   y);
				auto px2 = src.at_2(x+1, y);

				float y1  = (float) 0.299*255*px1.r + (float)0.587*255*px1.g + (float)0.114*255*px1.b;
				float cb1 = (float)-0.169*255*px1.r - (float)0.331*255*px1.g + (float)0.499*255*px1.b + 128;
				float cr1 = (float) 0.499*255*px1.r - (float)0.418*255*px1.g - (float)0.0813*255*px1.b + 128;
				float y2  = (float) 0.299*255*px2.r + (float)0.587*255*px2.g + (float)0.114*255*px2.b;
				float cb2 = (float)-0.169*255*px2.r - (float)0.331*255*px2.g + (float)0.499*255*px2.b + 128;
				float cr2 = (float) 0.499*255*px2.r - (float)0.418*255*px2.g - (float)0.0813*255*px2.b + 128;
				float cb = (cb1 + cb2) / 2 + (float)0.5;
				float cr = (cr1 + cr2) / 2 + (float)0.5;
				if(y1 > 255){
					y1 = 255;
				}
				if(y2 > 255){
					y2 = 255;
				}
				if(cb > 255){
					cb = 255;
				}
				if(cr > 255){
					cr = 255;
				}
				data_ptr[y*width*2+x*2    ] = (uint8_t)y1;
				data_ptr[y*width*2+x*2 + 1] = (uint8_t)cb;
				data_ptr[y*width*2+x*2 + 2] = (uint8_t)y2;
				data_ptr[y*width*2+x*2 + 3] = (uint8_t)cr;
			}
		}
	}else if(i.type == image_type::GRAY32){
		img_gray32 src(i);
		uint8_t* data_ptr = (uint8_t*) data.data;
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x += 2){
				data_ptr[y*width*2+x*2    ] = src.at_2(x,y)*255;
				data_ptr[y*width*2+x*2 + 1] = 128;
				data_ptr[y*width*2+x*2 + 2] = src.at_2(x+1,y)*255;
				data_ptr[y*width*2+x*2 + 3] = 128;
			}
		}
	}else if(i.type == image_type::YUYV){
		data = i.data;
	}else if(i.type == image_type::BW){
		img_gray src(i);
		uint8_t* data_ptr = (uint8_t*) data.data;
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x += 2){
				data_ptr[y*width*2+x*2    ] = src.at_2(x,y) ? 255 : 0;
				data_ptr[y*width*2+x*2 + 1] = 128;
				data_ptr[y*width*2+x*2 + 2] = src.at_2(x+1,y) ? 255 : 0;
				data_ptr[y*width*2+x*2 + 3] = 128;
			}
		}
	}else{
		// image_type::JPEG handled at the top.
		width = 0;
		height = 0;
	}

}
img_yuyv::img_yuyv(int32_t w, int32_t h, void* data_ptr, bool take_ownership){
	type = image_type::YUYV;
	width = w;
	height = h;
	if(take_ownership){
		data.reset(data_ptr, w*h*2);
	}else{
		data.realloc(data_ptr, w*h*2);
	}
}

