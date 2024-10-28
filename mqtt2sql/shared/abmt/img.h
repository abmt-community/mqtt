/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef SHARED_ABMT_IMG_H_
#define SHARED_ABMT_IMG_H_

#include <string>
#include <memory>
#include <cstdint>
#include <functional>
#include <abmt/blob.h>
#include <abmt/math/vec.h>
#include <abmt/math/ray.h>
#include <abmt/serialize.h>

namespace abmt{

class img;
template<typename PIXEL> class img_px;
template<typename PIXEL> class region_of_interest;

enum class image_type: uint8_t{
	NO_IMAGE,
	RGB8,
	RGBA8,
	GRAY8,
	RGB32,
	RGBA32,
	GRAY32,
	YUYV, // YUV 4:2:2
	JPEG,
	BW,

	CUSTOM_FORMAT_1 = 0xF0, // for project_internal_use
	CUSTOM_FORMAT_2,        // not supported by view
	CUSTOM_FORMAT_3,
	CUSTOM_FORMAT_4,
	CUSTOM_FORMAT_5,

};

class bool_px;

using pixel_rgb    = vec<3,uint8_t>;
using pixel_rgba   = vec<4,uint8_t>;
using pixel_gray   = uint8_t;
using pixel_rgb32  = vec<3,float>;
using pixel_rgba32 = vec<4,float>;
using pixel_gray32 = float;
using pixel_bw     = bool_px;


using img_rgb    = img_px<pixel_rgb>;
using img_rgba   = img_px<pixel_rgba>;
using img_gray   = img_px<pixel_gray>;
using img_rgb32  = img_px<pixel_rgb32>;
using img_rgba32 = img_px<pixel_rgba32>;
using img_gray32 = img_px<pixel_gray32>;
using img_bw     = img_px<pixel_bw>;

class bool_px{
public:
	char* byte;
	uint8_t bit;

	bool_px(bool i = 1){ // default pixel = true (for write text)
		byte = 0;
		bit = i;
	}

	operator bool(){
		if(byte == 0){
			return bit;
		}
		return *byte >> bit & 1;
	}

	void set(bool b){
		if(byte == 0){
			bit = b;
			return;
		}
		if(b){
			*byte |= 1UL << bit;
		}else{
			*byte &= ~(1UL << bit);
		}
	}

	bool_px& operator=(bool_px b){
		// important! Without this the pixel would not be set but the data copied
		set(b);
		return *this;
	}

	bool_px& operator=(bool b){
		set(b);
		return *this;
	}
};

struct font{
	uint8_t width;
	uint8_t height;
	uint8_t space;
	uint8_t line_space;
	const uint8_t* data;
	std::string charset;
};

extern font default_font;

class img{
public:

	int32_t width;
	int32_t height;
	image_type type;

	abmt::blob_shared data;

	img(int32_t w = 0, int32_t h = 0, image_type t = abmt::image_type::NO_IMAGE, size_t data_size = 0, void* src_data_ptr = 0, bool take_ptr_ownership = false);

	img copy(); // creates a deep copy

	/// Copies all attributes and reallocates memory with the same size
	/// without coping the src data. This is useful for filteralgorithms.
	img copy_for_dst();
	bool operator==(const img&);
	virtual ~img() = default;
};

class img_jpg: public img{
public:
	img_jpg();
	img_jpg(img img, unsigned int quality = 85); // convert here quality between 0 and 100;
	img_jpg(abmt::blob_shared data);
};

class img_yuyv: public img{
public:
	img_yuyv(img img);
	img_yuyv(int32_t w = 0, int32_t h = 0, void* data_ptr = 0, bool take_ptr_ownership = false);
};

template<> abmt::serialize::type abmt::serialize::define_type(img* obj);
template<> abmt::serialize::type abmt::serialize::define_type(img_rgb* obj);
template<> abmt::serialize::type abmt::serialize::define_type(img_rgba* obj);
template<> abmt::serialize::type abmt::serialize::define_type(img_gray* obj);
template<> abmt::serialize::type abmt::serialize::define_type(img_rgb32* obj);
template<> abmt::serialize::type abmt::serialize::define_type(img_rgba32* obj);
template<> abmt::serialize::type abmt::serialize::define_type(img_gray32* obj);
template<> abmt::serialize::type abmt::serialize::define_type(img_yuyv* obj);
template<> abmt::serialize::type abmt::serialize::define_type(img_jpg* obj);
template<> abmt::serialize::type abmt::serialize::define_type(img_bw* obj);


struct line2d{
	vec2 p1;
	vec2 p2;
	ray2d ray();
	double len();
};


template<typename PIXEL>
class img_px: public img{
public:
	img_px(img img); /// Converts the input image to this type. Embedded devices might convert manually to save memory.

	/// Sets type and size. Then calls reset() or realloc() depending on take_ptr_ownership.
	/// When data_ptr == 0 and take_ptr_ownership == false, a new image is create.
	img_px(int32_t w = 0, int32_t h = 0, void* data_ptr = 0, bool take_ptr_ownership = false);

	img_px(int32_t w, int32_t h, const void* data_ptr):img_px(w,h,(void*) data_ptr, false){

	}

	int32_t px_size = sizeof(PIXEL);

	void reset(int32_t w = 0, int32_t h = 0, void* data_ptr_that_will_be_managed = 0){
		width = w;
		height = h;
		size_t data_size = w*h*sizeof(PIXEL);
		data.reset(data_ptr_that_will_be_managed, data_size);
	}

	template<typename CHECK = PIXEL>
	img_px<PIXEL>& realloc(int32_t w = 0, int32_t h = 0, void* src_ptr_to_copy_from = 0,
			std::enable_if_t<std::is_same<CHECK, bool_px>::value == false>* = 0
	){
		width = w;
		height = h;
		size_t data_size = w*h*sizeof(PIXEL);
		data.realloc(src_ptr_to_copy_from, data_size);
		return *this;
	}

	template<typename CHECK = PIXEL>
	img_px<PIXEL>& realloc(int32_t w = 0, int32_t h = 0, void* src_ptr_to_copy_from = 0,
			std::enable_if_t<std::is_same<CHECK, bool_px>::value == true>* = 0
	){
		width = w;
		height = h;
		size_t data_size = ( (w+7)/8 )*h; // (w+7) -> round upward
		data.realloc(src_ptr_to_copy_from, data_size);
		return *this;
	}

	/// Returns the pixel at x,y.
	template<typename CHECK = PIXEL>
	inline PIXEL at(int32_t x, int32_t y,
					 std::enable_if_t<std::is_same<CHECK, bool_px>::value == true>* = 0
	){
		if(x >= width || y >= height){
			abmt::die("img error: pixel out ouf range");
		}
		return at_2(x,y);
	}

	/// Returns the pixel at x,y.
	template<typename CHECK = PIXEL>
	inline PIXEL& at(int32_t x, int32_t y,
					 std::enable_if_t<std::is_same<CHECK, bool_px>::value == false>* = 0
	){
		if(x >= width || y >= height){
			abmt::die("img error: pixel out ouf range");
		}
		return at_2(x,y);
	}

	/// Like at but without limit checking.
	template<typename CHECK = PIXEL>
	inline PIXEL& at_2(int32_t x, int32_t y,
			std::enable_if_t<std::is_same<CHECK, bool_px>::value == false>* = 0
	){
		auto ptr = ( (char*)data.data + sizeof(PIXEL)*x + width*sizeof(PIXEL)*y);
		return *((PIXEL*) ptr);
	}

	/// Like at but without limit checking.
	template<typename CHECK = PIXEL>
	inline PIXEL at_2(int32_t x, int32_t y,
			std::enable_if_t<std::is_same<CHECK, bool_px>::value == true>* = 0
	){
		PIXEL res;
		auto row_len = (width + 7) / 8;
		res.byte  = ( (char*)data.data + x/8 + row_len*y);
		res.bit = x%8;
		return res;
	}

	/// Same as at(x,y)
	template<typename CHECK = PIXEL>
	inline PIXEL& operator()(int32_t x, int32_t y,
			std::enable_if_t<std::is_same<CHECK, bool_px>::value == false>* = 0
	){
		return at(x,y);
	}

	/// Same as at(x,y)
	template<typename CHECK = PIXEL>
	inline PIXEL operator()(int32_t x, int32_t y,
			std::enable_if_t<std::is_same<CHECK, bool_px>::value == true>* = 0
	){
		return at(x,y);
	}

	region_of_interest<PIXEL> roi(int top_left_x = 0, int top_left_y = 0, int _width = -1, int _height = -1){
		if(top_left_x < 0){
			top_left_x = 0;
		}
		if(top_left_x >= width){
			top_left_x = width -1;
		}

		if(top_left_y < 0){
			top_left_y = 0;
		}
		if(top_left_y >= height){
			top_left_y = height -1;
		}

		if(_width < -1){
			_width = 0;
		}
		if(_height < -1){
			_height = 0;
		}
		if(_width == -1){
			_width = width - top_left_x;
		}
		if(_height == -1){
			_height = height - top_left_y;
		}
		if(_width + top_left_x > width){
			_width = width - top_left_x;
		}
		if(_height + top_left_y > height){
			_height = height-top_left_y;
		}
		return abmt::region_of_interest<PIXEL>(*this, top_left_x, top_left_y, _width, _height);
	}

	img_px<PIXEL>& fill(PIXEL color = {}){
		roi().fill(color);
		return *this;
	}

	img_px rotate_90(){ /// clockwise
		img_px res(height, width);
		for(int y = 0; y<height; ++y){
			for(int x = 0; x<width; ++x){
				res.at_2(height -1 -y,x) = at_2(x,y);
			}
		}
		return res;
	}

	img_px rotate_180(){ /// "clockwise"
		img_px res(width, height);
		for(int y = 0; y<height; ++y){
			for(int x = 0; x<width; ++x){
				res.at_2(width -1 -x, height -1 -y) = at_2(x,y);
			}
		}
		return res;
	}

	img_px rotate_270(){ /// clockwise
		img_px res(height, width);
		for(int y = 0; y<height; ++y){
			for(int x = 0; x<width; ++x){
				res.at_2(y,width -1 -x) = at_2(x,y);
			}
		}
		return res;
	}

	img_px flip_hz(){
		img_px res(width, height);
		for(int y = 0; y<height; ++y){
			for(int x = 0; x<width; ++x){
				res.at_2(width -1 -x, y) = at_2(x,y);
			}
		}
		return res;
	}

	img_px flip_vt(){
		img_px res(width, height);
		for(int y = 0; y<height; ++y){
			for(int x = 0; x<width; ++x){
				res.at_2(x, height -1 -y) = at_2(x,y);
			}
		}
		return res;
	}

	/// Draws a line from l.p1 to l.p2. On even line_widths the line has a 1px offset to the left.
	/// Example: Line from (0,2) to (10,2) with line_width 2: Draws 20px with y = 2 and y = 1;
	///          With line_width = 3: Draws 30px with y = 1, 2, 3;
	img_px& draw_line(line2d l, PIXEL color = {}, int line_width = 1){
		return draw_line(l.p1.x, l.p1.y, l.p2.x, l.p2.y, color, line_width);
	}

	img_px& draw_line(int x1, int y1, int x2, int y2, PIXEL color = {}, int line_width = 1){
		int dx = abs(x2-x1);
		int sx = x1 < x2 ? 1 : -1;
		int dy = - abs(y2-y1);
		int sy = y1 < y2 ? 1 : -1;
		int add_x = 0;
		int add_y = 0;
		for(int line = 0; line < line_width; ++line){
			bool mirror = (line % 2 == 0); // drawing a mirrored line
			int add_fz = 1;
			if(mirror){
				add_fz = -1;
			}
			int x = x1 + add_x * add_fz;
			int y = y1 + add_y * add_fz;
			int end_x = x2 + add_x * add_fz;
			int end_y = y2 + add_y * add_fz;
			int error = dx + dy;
			int step = 0;
			while (true){

				if( x >= 0 && x < width &&  y >= 0 && y < height ){
					at_2(x,y) = color;
				}

				if(x == end_x && y == end_y ){
					break;
				}

				int error2 = 2*error;
				if (error2 >= dy){
					error += dy;
					x += sx;
					if(mirror && step == line/2){
						add_y -= sx; // adding sx to add_y is correct
					}
				}else if (error2 <= dx){
					error += dx;
					y += sy;
					if(mirror && step == line/2){
						add_x += sy; // adding sy to add_x is correct
					}
				}
				++step;
			} // while true
		}
		return *this;
	}

	img_px& draw_circle(int center_x, int center_y, int radius, PIXEL color = {}, int line_width = 1){
		int l2 = line_width/2;
		int rd_start = radius - l2;
		if( center_x - radius - l2  < 0 || center_x + radius + l2 >= width || center_y - radius - l2 < 0 || center_y + radius + l2 >= height ){
			return *this;
		}
		for(int i = 0; i < line_width; ++i){
			int rd = rd_start+i;
			int x = -rd;
			int y = 0;
			int error = 2-2*rd;
			do {
				at(center_x - x, center_y + y) = color;
				at(center_x - y, center_y - x) = color;
				at(center_x + x, center_y - y) = color;
				at(center_x + y, center_y + x) = color;
				rd = error;
				if (rd <= y){
					++y;
					error += y*2+1;
				}else if (rd > x || error > y){
					++x;
					error += x*2+1;
				}
			} while (x < 0);
		}
		return *this;
	}

	img_px& draw_circle(vec2 p, double radius, PIXEL color = {}, int line_width = 1){
		return draw_circle(p.x, p.y, radius, color, line_width);
	}

	img_px& draw_circle(double center_x, double center_y, double radius, PIXEL color = {}, int line_width = 1){
		return draw_circle( (int) center_x + 0.5, (int) center_y + 0.5, (int) radius + 0.5, color, line_width);
	}
	/// When arm_len < 0 then it will be set to line_width * 3;
	img_px& draw_cross(vec2 p, PIXEL color = {}, int line_width = 1, int arm_len = -1){
		return draw_cross( (int)(p.x + 0.5), (int)(p.y + 0.5), color, line_width, arm_len);
	}

	/// When arm_len == -1 then it will be set to line_width * 3;
	img_px& draw_cross(int center_x, int center_y, PIXEL color = {}, int line_width = 1, int arm_len = -1){
		if(arm_len < 0){
			arm_len = line_width * 3;
		}
		if( center_x - arm_len  < 0 || center_x + arm_len  >= width || center_y - arm_len < 0 || center_y + arm_len >= height ){
			return *this;
		}
		draw_line(center_x - arm_len, center_y, center_x + arm_len, center_y, color, line_width);
		draw_line(center_x, center_y - arm_len, center_x, center_y + arm_len, color, line_width);
		return *this;
	}

	img_px& draw_icon(img_bw img, int top_left_x, int top_left_y, PIXEL color = {}, int scale = 1){
		for(int y = 0; y < img.height * scale; y++){
			for(int x = 0; x < img.width * scale; x++){
				if(img.at(x / scale, y / scale) == false){
					continue;
				}
				auto p_x = top_left_x + x;
				auto p_y = top_left_y + y;
				if(p_x >= 0 && p_x < width && p_y >= 0 && p_y < height){
					at(p_x , p_y) = color;
				}
			}
		}
		return *this;
	}


	img_px& write_text(std::string text, vec2 p, PIXEL color = {}, int scale = 1, abmt::font font = default_font){
		return write_text(text, p.x, p.y, color, scale, font);
	}

	/// Writes a string. "\n" starts a new line with 2px*scale spacing.
	img_px& write_text(std::string text, int start_x = 0, int start_top_y = 0, PIXEL color = {}, int scale = 1, abmt::font font = default_font){
		int line = 0;
		auto cstr = text.c_str();
		unsigned int str_pos = 0;
		unsigned int str_len = text.length();
		unsigned int line_pos = 0;
		for(unsigned int text_pos = 0; text_pos < str_len;){
			auto c = cstr[text_pos];
			size_t c_len = 1;
			if( (c & 0xE0) == 0xC0 ){
				c_len = 2;
			}
			else if( (c & 0xF0) == 0xE0 ){
				c_len = 3;
			}
			else if( (c & 0xF8) == 0xF0 ){
				c_len = 4;
			}
			if( scale < 0 || start_x + font.width*scale*line_pos < 0 || start_x + font.width*scale*line_pos + font.width*scale  >= width || start_top_y + font.height*scale*line < 0 || start_top_y  + font.height*scale*line + font.height*scale >= height ){
				return *this;
			}
			auto find_me = text.substr(text_pos, c_len);
			auto font_pos = font.charset.find(find_me);
			if(font_pos != std::string::npos){
				int real_pos = 0;
				for (auto c:font.charset.substr(0,font_pos)){
					real_pos += ((c & 0xc0) != 0x80);
				}
				font_pos = real_pos;

				for (int y=0; y < font.height; y++) {
					for (int x=0; x < font.width; x++) {
						auto x_byte = x / 8;
						auto x_rest = x % 8;
						auto data_pos = font_pos*font.height*((font.width + 7)/8) + y*((font.width + 7)/8) + x_byte;
						if( font.data[ data_pos ] & (1 << (7-x_rest)) ){
							for(int s_x = 0; s_x < scale; s_x++){
								for(int s_y = 0; s_y < scale; s_y++){
									at_2(start_x + line_pos*(font.width + font.space)*scale + x*scale + s_x, start_top_y + line*(font.height + font.line_space)*scale + y*scale + s_y) = color;
								}
							}
						}
					}
				}
			}else{
				if(c == '\n'){
					++line;
					line_pos = -1;
				}else if(c != ' '){
					// unknown char -> fill
					for (int y=0; y < font.height; y++) {
						for (int x=0; x < font.width; x++) {
							for(int s_x = 0; s_x < scale; s_x++){
								for(int s_y = 0; s_y < scale; s_y++){
									at_2(start_x + line_pos*(font.width + font.space)*scale + x*scale + s_x, start_top_y + line*(font.height + font.line_space)*scale + y*scale + s_y) = color;
								}
							}
						}
					}
				}
			}
			++line_pos;
			text_pos += c_len;
		}
		return *this;
	}

	img_px& paste(img_px& img, int at_x = 0, int at_y = 0){
		for(int y = 0; y < img.height; y++){
			for(int x = 0; x < img.width; x++){
				auto p_x = at_x + x;
				auto p_y = at_y + y;
				if(p_x >= 0 && p_x < width && p_y >= 0 && p_y < height){
					at(p_x , p_y) = img.at(x, y);
				}
			}
		}
		return *this;
	}

	img_px copy(){
		return img::copy();
	}
};



template<typename PIXEL>
struct pixel_iterator_output{
	PIXEL& color;
	int32_t x;
	int32_t y;
};

template<typename PIXEL>
class inner_pixel_iterator{
public:

	region_of_interest<PIXEL>& r;
	int32_t pos_x;
	int32_t pos_y;

	void operator++(){
		if(pos_x +1 >= r.top_left_x + r.width){
			// when the next x would be out of range
			pos_x = r.top_left_x;
			++pos_y;
		}else{
			++pos_x;
		}
	}

	pixel_iterator_output<PIXEL> operator*(){
		return {r.img.at_2(pos_x,pos_y),pos_x,pos_y};
	}

	bool operator!=(inner_pixel_iterator<PIXEL>& lhs){
		return lhs.pos_x != pos_x || lhs.pos_y != pos_y;
	}

};

template<typename PIXEL>
class region_of_interest{
public:

	img_px<PIXEL>* img;

	int32_t top_left_x = 0;
	int32_t top_left_y = 0;
	int32_t width;
	int32_t height;

	/// for default construction.
	region_of_interest(){
		width = 0;
		height = 0;
		img = 0;
	}


	region_of_interest(img_px<PIXEL>& img, int32_t top_left_x, int32_t top_left_y, int32_t width, int32_t height)
	:img(&img), top_left_x(top_left_x), top_left_y(top_left_y), width(width), height(height)
	{
		correct_to_limit();
	}

	region_of_interest& set_center(int32_t center_x, int32_t center_y, int32_t new_width = -1, int32_t new_height = -1){
		if(new_width > 0){
			width = new_width;
		}
		if(new_height > 0){
			height = new_height;
		}
		top_left_x = center_x - width/2;
		top_left_y = center_y - height/2;
		correct_to_limit();
		return *this;
	}

	region_of_interest& set_size(int32_t new_width = -1, int32_t new_height = -1){
		if(new_width > 0){
			width = new_width;
		}
		if(new_height > 0){
			height = new_height;
		}
		correct_to_limit();
		return *this;
	}

	region_of_interest& correct_to_limit(){
		if(top_left_x < 0){
			width += top_left_x; // += subtracts in this case
			top_left_x = 0;
		}
		if(top_left_x >= img->width){
			top_left_x = img->width -1;
		}
		if(top_left_y < 0){
			height += top_left_y; // += subtracts in this case
			top_left_y = 0;
		}
		if(top_left_y >= img->height){
			top_left_y = img->height -1;
		}
		if(width + top_left_x > img->width){
			width = img->width - top_left_x;
		}
		if(height + top_left_y > img->height){
			height = img->height - top_left_y;
		}
		return *this;
	}

	inner_pixel_iterator<PIXEL> begin(){
		return {*this,top_left_x, top_left_y};
	}
	inner_pixel_iterator<PIXEL> end(){
		if(width == 0 || height == 0){
			return {*this,top_left_x, top_left_y};
		}
		return {*this,top_left_x + width -1, top_left_y + height -1};
	}

	region_of_interest& fill(PIXEL p = {}){
		for(int32_t x = 0; x < width; ++x){
			for(int32_t y = 0; y < height; ++y){
				img->at_2(top_left_x + x,top_left_y + y) = p;
			}
		}
		return *this;
	}

	/// Draws a border on the most outer lines inside the region of interest
	region_of_interest& border(PIXEL color = {}, int line_width = 1){
		int s = line_width / 2; // s = shrink
		img->draw_line(top_left_x,           top_left_y+s,          top_left_x+width-1,   top_left_y+s,          color, line_width);
		img->draw_line(top_left_x+width-s-1, top_left_y,            top_left_x+width-s-1, top_left_y+height-1,   color, line_width);
		img->draw_line(top_left_x+width-1,   top_left_y+height-s-1, top_left_x,           top_left_y+height-s-1, color, line_width);
		img->draw_line(top_left_x+s,         top_left_y+height-1,   top_left_x+s,         top_left_y,            color, line_width);
		return *this;
	}

	region_of_interest& shrink(int32_t px){
		if(px > width){
			top_left_x += width/2;
			width = 0;
		}else{
			top_left_x += px;
			width -= px;
		}

		if(px > height){
			top_left_y += height/2;
			height = 0;
		}else{
			top_left_y += px;
			height -= px;
		}
		return *this;
	}

	region_of_interest& grow(int32_t px){
		if(px >= top_left_x){
			top_left_x = 0;

		}else{
			top_left_x -= px;
		}
		width += px;

		if(px >= top_left_y){
			top_left_y = 0;
		}else{
			top_left_y -= px;
		}
		height += px;
		return *this;
	}

	region_of_interest& write_text(std::string text, vec2 p, PIXEL color = {}, int scale = 1){
		return write_text(text, p.x, p.y, color, scale);
	}

	region_of_interest& write_text(std::string text, int start_x = 0, int start_top_y = 0, PIXEL color = {}, int scale = 1){
		img->write_text(text, start_x + top_left_x, start_top_y+top_left_y, color, scale);
		return *this;
	}

	region_of_interest& draw_line(line2d l, PIXEL color = {}, int line_width = 1){
		return draw_line(l.p1.x, l.p1.y, l.p2.x, l.p2.y, color, line_width);
	}

	region_of_interest& draw_line(double x1, double y1, double x2, double y2, PIXEL color = {}, int line_width = 1){
		return draw_line((int) (x1 + 0.5), (int) (y1 + 0.5), (int) (x2 + 0.5), (int) (y2 + 0.5), color, line_width );
	}

	region_of_interest& draw_line(int x1, int y1, int x2, int y2, PIXEL color = {}, int line_width = 1){
		img->draw_line(x1 + top_left_x, y1 + top_left_y, x2 + top_left_x, y2 + top_left_y, color, line_width);
		return *this;
	}

	region_of_interest& draw_circle(int center_x, int center_y, int radius, PIXEL color = {}, int line_width = 1){
		img->draw_circle(center_x  + top_left_x, center_y  + top_left_y, radius, color, line_width);
		return *this;
	}
	region_of_interest& draw_circle(vec2 p, double radius, PIXEL color = {}, int line_width = 1){
		return draw_circle(p.x, p.y, radius, color, line_width);
	}

	region_of_interest& draw_circle(double center_x, double center_y, double radius, PIXEL color = {}, int line_width = 1){
		return draw_circle( (int) center_x + 0.5, (int) center_y + 0.5, (int) radius + 0.5, color, line_width);
	}
	/// When arm_len < 0 then it will be set to line_width * 3;
	region_of_interest& draw_cross(vec2 p, PIXEL color = {}, int line_width = 1, int arm_len = -1){
		return draw_cross( (int)(p.x + 0.5), (int)(p.y + 0.5), color, line_width, arm_len);
	}

	/// When arm_len == -1 then it will be set to line_width * 3;
	region_of_interest& draw_cross(int center_x, int center_y, PIXEL color = {}, int line_width = 1, int arm_len = -1){
		img->draw_cross(center_x + top_left_x, center_y + top_left_y, color, line_width, arm_len);
		return *this;
	}

	img_px<PIXEL> copy(){
		img_px<PIXEL> res(width,height);
		for(int y = 0; y < height; ++y){
			for(int x = 0; x < width; ++x){
				res(x,y) = img->at(x + top_left_x, y + top_left_y);
			}
		}
		return res;
	}

};


} // namespace abmt
#endif /* SHARED_ABMT_IMG_H_ */
