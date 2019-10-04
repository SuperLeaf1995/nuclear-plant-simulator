/*=====================Video Routines=====================*/

int16_t _Cdecl setVideo(int16_t video) { /*Sets the video using int 10h*/
    in.h.ah = 0x00;
    in.h.al = video;
    int86(0x10,&in,&out);
    return out.h.al;
}

void _Cdecl plotLine(int16_t fx, int16_t fy, int16_t tx, int16_t ty, int8_t color) {
	int16_t dx,dy,px,py,x,y,i;
	float sl;
	if(tx >= fx) {
		dx = tx-fx;
	}
	else {
		dx = fx-tx;
	}
	if(ty >= fy) {
		dy = ty-fy;
	}
	else {
		dy = fy-ty;
	}
	if(abs(dy) >= abs(dx)) {
		sl=(float)dy/(float)dx;
		for(i = 0; i != dx; i+= 1) {
			px = i+fx;
			py = sl*i+fy;
			vgaMemory[((py<<8)+(py<<6)+px)] = color;
		}
	}
	else {
		sl=(float)dx/(float)dy;
		for(i = 0; i != dy; i+= 1) {
			py = sl*i+fx;
			px = i+fy;
			vgaMemory[((py<<8)+(py<<6)+px)] = color;
		}
	}
	return;
}

/*=====================Mouse Routines=====================*/

char _Cdecl initMouse(void) {
	in.x.ax = 0x00;
	in.x.bx = 0x00;
	int86(0x33,&in,&out);
	return (out.x.ax != 0) ? 0 : -1; /*If the mouse was initialized return 0, else return -1*/
}

void _Cdecl setMousePosition(int16_t x,int16_t y) {
	in.x.ax = 0x04;
	in.x.cx = x;
	in.x.dx = y;
	int86(0x33,&in,&out);
	return;
}

void _Cdecl showMouse(void) {
	in.x.ax = 0x01;
	int86(0x33,&in,&out);
	return;
}

void _Cdecl hideMouse(void) {
	in.x.ax = 0x02;
	int86(0x33,&in,&out);
	return;
}

void _Cdecl getMouse(struct mouse *m) {
	in.x.ax = 0x03;
	int86(0x33,&in,&out);
	m->x = out.x.cx;
	m->y = out.x.dx;
	m->buttonLeft = (out.x.bx & 1);
	m->buttonRight = (out.x.bx & 2);
	m->buttonMiddle = (out.x.bx & 4);
	return;
}

/*=================Decode/Encode Routines==================*/
/*
*WORD	Identifier	Defaults to BM
*DWORD	Size of file	Size of file, can range from 0 to 65,550
*DWORD	Reserved	?
*DWORD	Offset	Offset from the file data
*DWORD	Header size	The size of the Header, set to 40 [1]
*DWORD	Wide	Wide of the image
*DWORD	Tall	Tall of the image
*WORD	Planes	By default set to 1
*WORD	Bits per pixel	If set 1 becomes monochrome and each bit is a pixel, if set 4 can have up to 16 colours and each byte are two pixels, 8 to 256 colours and each byte is a pixel, and 28 has up to 2^28
and has three bytes per pixel, representing Red, Green and Blue respectively

*DWORD	Compression	If set to 0 RLE is enabled. Else it's raw data
*DWORD	Size of image	Size in bytes of the image
*DWORD	X px/m	X pixels per meter
*DWORD	Y px/m	Y pixels per meter
*DWORD	Number of colors	Number of used colors, max. Depends on bits per pixel
*DWORD	Important colors	Number of relevant colors, i.e the ones that are used most
*/

uint64_t decodeBitmap(FILE *stream, struct bitmap *bmp, uint8_t *palette) {
    uint8_t *skip; /*Will help us to skip some useless parts*/
    uint16_t i;
    uint8_t padding; /*Bitmaps not divisible by 4 are padded*/
    skip = (uint8_t *)malloc(32);
    if(skip == NULL) { /*Return 0 on error*/
        return 0;
    }
    fread((uint8_t *)skip,sizeof(uint8_t),15,stream);
    if(skip[0] != 'B' || skip[1] != 'M') { /*Return 1 on error. Not a valid bitmap*/
        return 1;
    }
    fread((struct bitmap *)bmp->wide,sizeof(uint32_t),1,stream);
    fread((struct bitmap *)bmp->tall,sizeof(uint32_t),1,stream);
    printf("Wide: %x - Tall: %x\n",bmp->wide,bmp->tall);
    printf("%u %u %u %u\n",sizeof(uint8_t),sizeof(uint16_t),sizeof(uint32_t),sizeof(uint64_t));
    if(bmp->wide == 0 || bmp->tall == 0) {
        return 1;
    }
    /*Allocate data*/
    bmp->data = (uint8_t *)malloc(bmp->tall*bmp->wide);
    fread((uint8_t *)skip,sizeof(uint8_t),4,stream);
    fread((struct bitmap *)bmp->bitsPerPixel,sizeof(uint16_t),1,stream);
    if(bmp->bitsPerPixel != 8) {
        printf("VALUE OF BITS PER PIXEL = %u",bmp->bitsPerPixel);
        return 1;
    }
    fread((uint8_t *)skip,sizeof(uint8_t),24,stream);
    if(palette == NULL) { /*No palette provided, skip it!*/
        for(i = 0; i < 1024; i++) {
            fread((uint8_t *)skip[0],sizeof(uint8_t),1,stream);
        }
    }
    else {
        fread(palette,sizeof(uint8_t),1024,stream);
    }
    padding = bmp->wide%4;
    for(i = 0; i < bmp->tall; i++) { /*Read one line at a time*/
        fread(bmp->data,sizeof(uint8_t),bmp->wide+padding,stream);
    }
    free(skip); /*We finished our work, it was time for skip to leave...*/
    return bmp->wide+(bmp->tall<<16); /*Save tall in higher half of the returned 32 bit integer*/
}
