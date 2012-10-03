#include <cctype>
#include "png.h"


int main(int argc, char* argv[]){

  for(int i=1; i < argc; i++){
  
    PNG png(argv[i]);
    png.display();
    png.blur<uint32_t>(5);
    png.write("out.png");

  }
  
}
