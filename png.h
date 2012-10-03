#ifndef __PNG_H
#define __PNG_H

#include <iostream>
#include <cctype>
#include <libpng15/png.h>

class PNG {
  private:
    png_structp data;
    png_infop   meta;
    uint8_t bit_depth, color_type, bytes_per_pixel;
    uint8_t* pixels;
    int w, h, wh;
    void* rows();
    
  private:
    enum error {
      NO_ERROR = 0,
      FILE_OPEN_ERROR,
      FILE_FORMAT_ERROR
    };
    template<class T> void do_display (uint8_t);
    
  public:
    PNG();
    ~PNG();
    PNG(const char*);
    int load(const char*);
    int write(const char*);
    void display();
    template<class T> void blur(int);
    
};

template<class T> void PNG::blur(int n){

  if(n < 1) return;
  
  T* rows = (T*)pixels;

  for(int i=0; i < wh; i++){
    
    int prev=i+n;
    if((i/w+n)<0||prev > wh) continue;
    for(char field = 0; field < 4; field++){
    
      uint8_t shift = field*bit_depth;
      T mask = 0xff<<shift;
      
      uint8_t c = ((rows[i]&mask)>>shift)/2;
      c += ((rows[prev]&mask)>>shift)/2;
      rows[i]&=~mask;
      rows[i]|=c<<shift;
      
    }
    
  }
  blur<T>(n-1);
  
}

#endif
