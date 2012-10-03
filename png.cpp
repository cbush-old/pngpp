#include "png.h"
#include <iostream>
#include <cctype>

using std::cout;
using std::endl;

PNG::PNG():data(NULL),meta(NULL),pixels(NULL){}
PNG::PNG(const char* filename):data(NULL),meta(NULL),pixels(NULL){

  load(filename);
  
}

int PNG::load(const char* filename){
  
  FILE *fp = fopen(filename, "rb");
  
  if(!fp) return FILE_OPEN_ERROR;
  
  png_byte header[8];
  
  fread(header, 1, 8, fp);
  
  if(png_sig_cmp(header, 0, 8)){
  
    fclose(fp);
    return FILE_FORMAT_ERROR;
    
  }
  
  fseek(fp,0,SEEK_SET);
  
  data = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  meta = png_create_info_struct(data);
  
  png_init_io(data, fp);
  png_read_png(data, meta, 0, NULL);
  fclose(fp);

  h = png_get_image_height(data, meta);
  w = png_get_image_width(data, meta);  
  wh = w*h;

  bit_depth   = (uint8_t)png_get_bit_depth(data, meta);
  color_type  = (uint8_t)png_get_color_type(data, meta);

  png_bytepp rows = png_get_rows(data, meta);
  
  bytes_per_pixel = bit_depth/2;
  
  pixels = new uint8_t[wh*bytes_per_pixel];
  for(int i=0; i < h; i++)
    for(int j=0; j < w*bytes_per_pixel; j++)
      pixels[i*w*bytes_per_pixel+j] = rows[i][j];

  png_destroy_read_struct(&data, &meta, NULL);
  data = NULL;
  meta = NULL;

  return NO_ERROR;
  
}

int PNG::write(const char* filename){

  FILE *fp = fopen(filename, "wb");
  
  if (!fp) return (FILE_OPEN_ERROR);

  data = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!data){
    fclose(fp);
    return 1;
  }

  meta = png_create_info_struct(data);
  if (!meta){
  
    fclose(fp);
    png_destroy_write_struct(&data,  NULL);
    return 2;
    
  }
  png_init_io(data, fp);

  png_set_IHDR(data, meta, w, h, bit_depth, PNG_COLOR_TYPE_RGB_ALPHA,
    PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(data, meta);

  uint8_t** rows = new uint8_t*[h];
  for(int i=0; i < h; i++){
    rows[i] = new uint8_t[w*bytes_per_pixel];
    for(int j=0; j < w*bytes_per_pixel; j++)
      rows[i][j] = pixels[i*w*bytes_per_pixel+j];
      
  }
  png_write_image(data, rows);
  
  for(int i=0; i < h; i++) 
    delete[] rows[i];
  delete[] rows;
  
  png_write_end(data, meta);
  
  png_destroy_write_struct(&data, &meta);

  data = NULL;
  meta = NULL;
  fclose(fp);

  return NO_ERROR;
  
}

PNG::~PNG(){

  if(pixels) delete pixels;
  
}

template<class T> void PNG::do_display(uint8_t field){

  T* rows = (T*)pixels;
  
  uint8_t shift = sizeof(T)*8 - field*bit_depth;
  T mask = 0xff<<shift;
  
  for(int i=0; i < wh; i++){
    
    if(!(i%w)) cout << endl;
    
    uint8_t c = (rows[i]&mask) >> shift;
    cout << (
      c > 0xf0 ? "##" : c > 0xe0 ? "$$" : c > 0xd0 ? "88" : c > 0xc0 ? "AA" :
      c > 0xb0 ? "dd" : c > 0xa0 ? "hh" : c > 0x09 ? "<<" : c > 0x08 ? ">>" :
      c > 0x07 ? "++" : c > 0x06 ? "//" : c > 0x05 ? "==" : c > 0x04 ? ";;" :
      c > 0x03 ? "--" : c > 0x02 ? "''" : c > 0x01 ? "``" : "  "
    );
  }
  
}

void PNG::display(){
  switch(bit_depth){
    case 2:   do_display<uint8_t>(4);    break;
    case 4:   do_display<uint16_t>(4);   break;
    case 8:   do_display<uint32_t>(4);   break;
  }
  
}
