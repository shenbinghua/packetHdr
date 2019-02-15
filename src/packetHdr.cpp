//============================================================================
// Name        : packetHdr.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include "packetHdr.h"

#define _DEBUG_ 0
using namespace std;


int packHdr::packHdrParam(unsigned char * buffer, unsigned int bufferlen,stHDRMetadata & hdrinfo,unsigned char * hdrBuffer,int type){

	switch(type){
	case 0:
		return m_h265_pkt->h265_parser(buffer,bufferlen,hdrinfo,hdrBuffer);
		break;
	case 1:
		return m_h264_pkt->h264_parser(buffer,bufferlen,hdrinfo,hdrBuffer);
		break;
	default:
		return 0;
	}
	return 1;
}
void packHdr::init(){
	m_h265_pkt = new pktH265();
	m_h264_pkt = new pktH264();
}
void packHdr::clear(){

}




int main() {
	//================================
	FILE * fp = fopen("/root/mcloud/eclipse.workspace/TSAnalysixTest/TEST_TS/h265_lunbo.265","rb");
	FILE * fpout = fopen("./out_hdr.h264","wb");
	FILE * fpout1 = fopen("./out_src.h264","wb");
	unsigned char* buffer = new unsigned char[10000000];
	unsigned char* hdrBuffer = new unsigned char[10010000];
	unsigned int bufferlen = fread(buffer,1,100000000,fp);
	fwrite(buffer,10000000,1,fpout1);
	stHDRMetadata info;
//=====================================


	packHdr *pkt = new packHdr();
	pkt->init();
	int len = pkt->packHdrParam(buffer,bufferlen,info,hdrBuffer,0);
	pkt->clear();

	fwrite(hdrBuffer,len,1,fpout);
	fclose(fpout);
	fclose(fpout1);
	cout << "!!!FINISH!!!" << endl;
	return 0;
}

#if _DEBUG_
void Bp(unsigned char n)
{
  int i;
  for (i=7;i>=0;i--)
  {
    printf("%u",(n>>i)&1);
  }
}
int main()
{
  int i;
  unsigned char src[10]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};//{0,0x1F,0x0F,0x0F,0x70,0,0,1,1};
  unsigned char dst[10]={0,0,0,0,0,0,0,0,0};

  printf("%d\n",copybits(src,0,1,dst,0,1,24));

  for(i=0;i<10;++i){
    //printf("\t%2x\t%2x\n",src[i],dst[i]);
    Bp(src[i]);
    putchar(' ');
  }
  putchar('\n');
  for(i=0;i<10;++i){
   // printf("\t%2x\t%2x\n",src[i],dst[i]);
    Bp(dst[i]);
    putchar(' ');
  }
  putchar('\n');
  return 0;
}
#endif
