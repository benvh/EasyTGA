/*
 * Copyright 2011 Ben Van Houtven. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 * 
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 * 
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY BEN VAN HOUTVEN ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BEN VAN HOUTVEN OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of Ben Van Houtven.
 */

#include <stdio.h>
#include <stdlib.h>
#include "tga.h"

#define UNMAPPED_RGB 2
#define RLE_RGB 10

typedef struct TGA_HEADER {
	byte	id_size, colormap_type, image_type;
	short	width, height;
	byte	bpp, img_descriptor;
	
}TgaHeader;

struct TGA_IMAGE {
	short			width, height;
	byte			bpp;
	unsigned char*	data;
};

typedef struct COLOUR {
	unsigned char r,g,b,a;
}Colour;



TgaImage* TgaImage_readRGB(FILE *pFile, TgaHeader *header);
TgaImage* TgaImage_readRLE(FILE *pFile, TgaHeader *header);
Colour TgaImage_decode16(const short c);
Colour TgaImage_decode24(const int c);
Colour TgaImage_decode32(const int c);



TgaImage* TgaImage_new(const char* filename) {

	FILE *pFile = fopen(filename, "rb");
	if( pFile ) {
		TgaImage *this = 0;
		TgaHeader *header = (TgaHeader*)malloc(sizeof(TgaHeader));
		
		fread(header, 3, 1, pFile);
		
		fseek(pFile, 12, SEEK_SET);
		fread(&(header->width), 6, 1, pFile);
		
		fseek(pFile, 18+header->id_size, SEEK_SET);
		
		if( header->image_type == UNMAPPED_RGB ) {
			this = TgaImage_readRGB(pFile, header);
		} else if( header->image_type == RLE_RGB ) {
			this = TgaImage_readRLE(pFile, header);
		}
		
		free(header);
		fclose(pFile);
		return this;
	}
	
	return 0;
	
}

void TgaImage_destroy(TgaImage *this) {
	free(this->data);
	free(this);
}

int	TgaImage_getWidth(TgaImage *this) {
	return this->width;
}
int	TgaImage_getHeight(TgaImage *this) {
	return this->height;
}

byte TgaImage_getBpp(TgaImage *this) {
	return this->bpp;
}

unsigned char* TgaImage_getData(TgaImage *this) {
	return this->data;
}

TgaImage* TgaImage_readRGB(FILE *pFile, TgaHeader *header) {
	TgaImage *this = (TgaImage*)malloc(sizeof(TgaImage));
	
	int data_size = (header->width*4) * header->height;
	this->data = (unsigned char*)malloc( data_size*sizeof(unsigned char) );
	
	this->bpp = 32;
	this->width = header->width;
	this->height = header->height;
	
	int i;
	Colour colour;
	for(i = 0; i < data_size;i+=4) {
		if(header->bpp == 16) {
			short d;
			fread(&d, 2, 1, pFile);
			colour = TgaImage_decode16(d);
		} else if(header->bpp == 24) {
			int d;
			fread(&d, 3, 1, pFile);
			colour = TgaImage_decode24(d);
		} else if(header->bpp == 32) {
			int d;
			fread(&d, 4, 1, pFile);
			colour = TgaImage_decode32(d);
		}
		
		this->data[i] = colour.r;
		this->data[i+1] = colour.g;
		this->data[i+2] = colour.b;
		this->data[i+3] = colour.a;

	}

	return this;
}

TgaImage* TgaImage_readRLE(FILE *pFile, TgaHeader *header) {
	//TODO: Implement this
	
	return 0;
}

Colour TgaImage_decode16(const short c) {
	//TODO: Implement this
}

Colour TgaImage_decode24(const int c) {
	Colour cl;
	
	cl.r = (c>>16)&0xFF;
	cl.g = (c>>8)&0xFF;
	cl.b = c&0xFF;
	cl.a = 0xFF;
	
	return cl;
}

Colour TgaImage_decode32(const int c) {
	Colour cl;
	
	cl.r = c>>24;
	cl.g = (c>>16)&0xFF;
	cl.b = (c>>8)&0xFF;
	cl.a = c&0xFF;
	
	return cl;
}