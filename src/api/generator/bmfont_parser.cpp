#include <cstring>

#include "bmfont_parser.h"
#include "exceptions.h"

using namespace FWdebugExceptions;
using namespace Grafkit;
using namespace Grafkit;

BMFontParser::BMFontParser(FontMap* _root) : root(_root){
}

void BMFontParser::load(){
//	unsigned char *data = NULL;
//	size_t len = 0, offset = 0, h = 0,i = 0, j=0 ,k=0;
//	unsigned int magicnumber = 0, blockSize = 0;
//	unsigned char version = 0, bitfield = 0, blockID = 0;
//	struct FontMap::char_fmt cfmt;
//	struct FontMap::kerning_fmt kfmt;
//
//	if (!this->root) throw new NullPointerException();
//	if (!this->root->res) throw new NullPointerException();
//
//	// nem torli magatol a ASSET-t
//	this->root->res->load();
//	data = (unsigned char*) this->root->res->getData();
//	len = this->root->res->getLength();
//	offset = 0;
//
//	///// VERZIOSZAM
//	// 1) feltetelezzuk, hogy binaris filerol van szo
//	magicnumber = ((unsigned int*)data + offset)[0]; magicnumber &= 0x00ffffff;
//	version = data[offset+3]; 
//	
//	///@todo detektalni, hogy text-e, vagy xml-e
//	// 1a)
//	// 1b)
//
//	// "BMF#" - # a verzioszam
//	if (magicnumber != 0x00464d42){
//		throw new InvalidBMFVersionException();
//	} else {
//		if (version != 3 && version != 4){
//			throw new InvalidBMFVersionException();
//		} /*else {
//		
//		}*/
//	}
//
//	offset += 4;
//
//	///// INFO
//	blockID = (data+offset)[0];
//	blockSize = ((int*)(data + offset+1))[0]; offset += 5;
//	if (blockID != 1)
//		throw new InvalidBMFVersionException();
//
//	this->root->fontsize = ((short*)(data + offset))[0];
//
//	//bit 0: smooth, bit 1: unicode, bit 2: italic, bit 3: bold, bit 4: fixedHeigth, bits 5-7: reserved
//	bitfield = data[offset + 2];
//	// todo : ++ 
//
//	this->root->charset = data[offset+3];
//	if (this->root->charset == 1) this->root->isUnicode = 1;
//
//	this->root->strechH = (float) ((unsigned short*)(data+offset+4))[0] / 100.;
//	this->root->MSAAlevel =  data[offset+6];
//	offset += 7;
//
//	// ezekhez kesobb hozza kell nyulni majd meg.
//	this->root->padding.x = (float) data[0];	// up
//	this->root->padding.y = (float) data[1];	// right
//	this->root->padding.z = (float) data[2];	// down
//	this->root->padding.w = (float) data[3];	// left
//	offset += 4;
//
//	this->root->spacing.x = (float) data[0];
//	this->root->spacing.y = (float) data[1];
//	offset += 2;
//
//	// outline nem kell egyelore
//	offset ++;
//
//	// csunya megoldas...
//	this->root->face[0] = 0; h = 255; i = 255, k=0;
//	do {
//		i = (data+offset)[k];
//		this->root->face[k] = i;
//		k++;
//	}while(h-- && i);
//	offset += k;
//
//	///// COMMON
//	blockID = (data+offset)[0];
//	blockSize = ((int*)(data + offset+1))[0]; offset += 5;
//	if (blockID != 2)
//		throw new InvalidBMFVersionException();
//
//	this->root->fontsize = ((short*)(data + offset))[0];
//
//	this->root->lineHeight = ((unsigned short*)(data+offset))[0];
//	this->root->base       = ((unsigned short*)(data+offset))[1];
//	this->root->scale.x    = ((unsigned short*)(data+offset))[2];
//	this->root->scale.y    = ((unsigned short*)(data+offset))[3];	// ez valamiert 1
//
//	offset += 8;
//
//	// ellenoriz:
//	// + csak 1:1 texturak lehetnek, es ilyen megkotesek ... 
//
//	this->root->maxPages = ((unsigned short*)(data+offset))[0];
//	offset += 2;
//
//	///@todo bitfield tamogatas
//
//	// + bitfield
//	offset ++;
//	// + alpha
//	offset ++;
//	// + red
//	offset ++;
//	// + green
//	offset ++;
//	// + blue
//	offset ++;
//
//	// do the math
//	// + ezeket tenyleges textura betoltes utan kellene levezenyelni
//	this->root->lineHeight /= this->root->scale.y;
//	this->root->base /= this->root->scale.y;
//	
//	this->root->padding.x /= this->root->scale.y;	// up
//	this->root->padding.y /= this->root->scale.x;	// right
//	this->root->padding.z /= this->root->scale.y;	// down
//	this->root->padding.w /= this->root->scale.x;	// left
//
//	this->root->spacing.x /= this->root->scale.x;
//	this->root->spacing.y /= this->root->scale.x;
//
//	///// TEXTURE PAGES
//	blockID = (data+offset)[0];
//	blockSize = ((int*)(data + offset+1))[0]; offset += 5;
//	if (blockID != 3)
//		throw new InvalidBMFVersionException();
//
//	char *name = (char*)(data+offset); 
//	//k=0; //j=0;
//
//	///@todo Hasznaljon TextureRef-t
//	for (i=0; i<this->root->maxPages; i++){
//		Grafkit::TextureRef tex = new Grafkit::Texture();
//		tex->setName(name);
//		this->root->pages << tex;
//		//k = strlen(name);
//		name += strlen(name)+1;
//		//j+=k+1;
//	}
//
//	offset += blockSize;
//
//	///// CHARACTERS
//	blockID = (data+offset)[0];
//	blockSize = ((int*)(data + offset+1))[0]; offset += 5;
//	if (blockID != 4)
//		throw new InvalidBMFVersionException();
//
//	k = blockSize / 20;
//
//	for(i=0; i<k; i++){
//		cfmt.id = (wchar_t)((unsigned int*)(data+offset+0))[0];	// nem tudom, hogy ez mennyire jo megoldas lesz
//#if 0
//		cfmt.pos.x  = ((float)((unsigned short*)(data+offset+4))[0] + 1 ) / this->root->scale.x;
//		cfmt.pos.y  = ((float)((unsigned short*)(data+offset+4))[1] + 1 ) / this->root->scale.y;
//		cfmt.size.x = ((float)((unsigned short*)(data+offset+4))[2] - 1 ) / this->root->scale.x;
//		cfmt.size.y = ((float)((unsigned short*)(data+offset+4))[3] - 1 ) / this->root->scale.y;
//#else 
//		cfmt.pos.x  = ((float)((unsigned short*)(data+offset+4))[0])/this->root->scale.x;
//		cfmt.pos.y  = ((float)((unsigned short*)(data+offset+4))[1])/this->root->scale.y;
//		cfmt.size.x = ((float)((unsigned short*)(data+offset+4))[2])/this->root->scale.x;
//		cfmt.size.y = ((float)((unsigned short*)(data+offset+4))[3])/this->root->scale.y;
//#endif 	
//		cfmt.offset.x = ((float)((short*)(data+offset+4))[4]) /  this->root->scale.x;
//		cfmt.offset.y = ((float)((short*)(data+offset+4))[5]) /  this->root->scale.y;
//
//		cfmt.advanceX = (float)((short*)(data+offset+4))[6] /  this->root->scale.x;
//
//		cfmt.pageId = data[offset+18];
//		cfmt.channel = data[offset+19];
//
//		this->root->chars.push(cfmt);
//
//		if (cfmt.offset.x > 1. || cfmt.offset.y > 1. || cfmt.offset.x<-1. || cfmt.offset.y<-1.){
//			throw new InvalidBMFVersionException();
//		}
//
//		offset += 20;
//	};
//
//	if (offset == len){
//		this->root->isUseKerning = 0;
//		return;
//	}
//
//	this->root->isUseKerning = 1;
//
//	//return; // ez a kerninges moka meg vacak egy kicsit sajnos.
//
//	///// KERNNING PAIRS
//	blockID = (data+offset)[0];
//	blockSize = ((int*)(data + offset+1))[0]; offset += 5;
//	if (blockID != 5)
//		throw new InvalidBMFVersionException();
//
//	if (blockSize){
//		k = blockSize / 10;
//		for(int i=0; i<k; i++){
//			kfmt.first  = (wchar_t)((unsigned int*)(data+offset+0))[0];	// nem tudom, hogy ez mennyire jo megoldas lesz
//			kfmt.second = (wchar_t)((unsigned int*)(data+offset+0))[1];	
//			kfmt.amount = ((short*)(data+offset+8))[0] / this->root->scale.x;
//
//			this->root->kernings.push(kfmt);
//
//			offset += 10;
//		}
//	}
//
//	// fin.

}