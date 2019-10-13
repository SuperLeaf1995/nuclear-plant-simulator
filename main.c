#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define _DEBUG_LEAF

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

struct bitmapFileHeader bf;
struct bitmapInfoHeader bi;
struct image img;
struct mouse m;

struct specializedGameData {
	struct image leftSideIcons;
	struct image generalMenuIcons;
	struct image _64per64tiles;
	
	uint8_t menuRefresh;
	uint8_t fullRefresh;
};

struct specializedGameData mainGameData;

int main(void) {
    int64_t i;
    setVideo(0x13);
    srand(*clock);
    if(initMouse(&m) != 0) {
		fprintf(stderr,"Could not init mouse");
		return -2;
	}
	
	getVideoAdapter();
    /*AVAILABLE FILE POOL:
     *ct1 - 64x64 tiles
     *ig1 - "Build" menu icons
     *ig2 - Standard menu icons*/
	fp = fopen("img//ig1.bmp","rb"); /*ig1*/
    if(!fp) {
        fprintf(stderr,"Could not open bitmap");
        return -1;
    }
	readBitmapFileHeader(fp,&bf);
	readBitmapInformationHeader(fp,&bi);
	for(i = 0; i < 1022; i++) {
		fgetc(fp);
	}
	mainGameData.leftSideIcons.data = readBitmapData(fp,bi.wide,bi.tall);
	fclose(fp);
	
	fp = fopen("img//ig2.bmp","rb"); /*ig2*/
    if(!fp) {
        fprintf(stderr,"Could not open bitmap");
        return -1;
    }
	readBitmapFileHeader(fp,&bf);
	readBitmapInformationHeader(fp,&bi);
	for(i = 0; i < 1022; i++) {
		fgetc(fp);
	}
	mainGameData.generalMenuIcons.data = readBitmapData(fp,bi.wide,bi.tall);
	fclose(fp);
	
	fp = fopen("img//ct1.bmp","rb"); /*ct1*/
    if(!fp) {
        fprintf(stderr,"Could not open bitmap");
        return -1;
    }
	readBitmapFileHeader(fp,&bf);
	readBitmapInformationHeader(fp,&bi);
	for(i = 0; i < 1022; i++) { /*We cant read palettes yet, just, drop it out!*/
		fgetc(fp);
	}
	mainGameData._64per64tiles.data = readBitmapData(fp,bi.wide,bi.tall);
	fclose(fp);
	
	/*Set normal values*/
	mainGameData.fullRefresh = 1;
	mainGameData.menuRefresh = 1;
	
	setMousePosition(0,0);
	showMouse();
	
	for(i = 0; i < 64000; i++) {
		plotLinearPixel(i,54);
	}
    while(!kbhit()) {
		if(mainGameData.menuRefresh == 1) {
			/*Display build menu*/
			for(i = 0; i < 4; i++) {
				displayImageTile(mainGameData.leftSideIcons.data,0,32+(32*i),32,32,i);
			}
			/*Clickable arrows, to scroll*/
			displayImageTile(mainGameData.generalMenuIcons.data,0,0,32,32,0);
			displayImageTile(mainGameData.generalMenuIcons.data,0,32+(32*i),32,32,1);
			mainGameData.menuRefresh = 0;
		}
		/*:0*/
		getMouseStatus(&m);
		/*Evaluate, we clicked on something?*/
		for(i = 0; i < 7; i++) {
			if((m.x/2) >= 0 && (m.x/2) <= 32
			&& (m.y) >= (i*32) && (m.y) <= 32+(i*32)) {
				/*Button left was clicked?*/
				if(m.buttonLeft != 0) {
					switch(i) {
						case 1:
							break;
					}
				}
			}
		}
	}
	
    free(mainGameData.leftSideIcons.data);
    free(mainGameData._64per64tiles.data);
    free(mainGameData.generalMenuIcons.data);
    hideMouse();
    setVideo(oldVideo);
    return 0;
}
