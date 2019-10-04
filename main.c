#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "leaf.h"

struct nuclearPlant {
    int64_t uranium;
    int64_t uraniumRods;
    int64_t moderators;
    int64_t controlRods;
    int64_t electricOutput;
};

int oldVideo;
FILE *fp;
struct bitmap b;

int main(void) {
    /*int16_t x,y;
    int8_t color;*/
    int64_t i;
    /*oldVideo = setVideo(0x13);*/
    srand(*clock);
    fp = fopen("bitmap.bmp","rb");
    if(!fp) {
        fprintf(stderr,"Could not open bitmap");
        return -1;
    }
    while(!kbhit()) {
        /*x = rand()%320;
        y = rand()%200;
        color = rand()%255;
        plotPixelVGA(x,y,color);*/
        i = decodeBitmap(fp,&b,NULL);
        if(i == 0) {
            fprintf(stderr,"General IO error");
            return -1;
        }
        else if(i == 1) {
            fprintf(stderr,"Wrong bitmap configurations");
            return -2;
        }
    }
    fclose(fp);
    /*setVideo(oldVideo);*/
    return 0;
}
