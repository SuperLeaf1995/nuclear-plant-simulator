#ifndef LEAF_H_INCLUDED
#define LEAF_H_INCLUDED

#include <dos.h>

#ifndef _Cdecl
#define _Cdecl _cdecl
#endif

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int32_t;
typedef signed long long int64_t;

uint8_t far *vgaMemory = (uint8_t far *)0xA0000000L;
uint8_t far *textMemory = (uint8_t far *)0xB8000000L;
uint16_t *clock = (uint16_t *)0x0000046C;

union REGS in,out;

struct mouse {
    buttonLeft:1;
    buttonRight:1;
    buttonMiddle:1;
    int16_t x;
    int16_t y;
};

struct bitmap {
    uint32_t wide;
    uint32_t tall;
    uint16_t bitsPerPixel;
    uint8_t *data;
};

#define plotPixelVGA(x,y,color) vgaMemory[x+(y<<8)+(y<<6)] = color
#define fetchPixelVGA(x,y) vgaMemory[x+(y<<8)+(y<<6)]

int16_t _Cdecl setVideo(int16_t video);
void _Cdecl plotLine(int16_t fx, int16_t fy, int16_t tx, int16_t ty, int8_t color);

char _Cdecl initMouse(void);
void _Cdecl setMousePosition(int16_t x,int16_t y);
void _Cdecl showMouse(void);
void _Cdecl hideMouse(void);
void _Cdecl getMouse(struct mouse *m);

uint64_t decodeBitmap(FILE *stream, struct bitmap *bmp, uint8_t *palette);

#include "leaf.c"

#endif /*LEAF_H_INCLUDED*/
