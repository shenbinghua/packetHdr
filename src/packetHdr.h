/*
 * packetHdr.h
 *
 *  Created on: 2019年1月18日
 *      Author: root
 */

#ifndef PACKETHDR_H_
#define PACKETHDR_H_
#include "common.h"
#include "pktH265.h"
#include "pktH264.h"

class packHdr{
private:
	pktH265 *m_h265_pkt;
	pktH264 *m_h264_pkt;

public:
	void init();
	void clear();
int packHdrParam(unsigned char * buffer, unsigned int bufferlen,stHDRMetadata & hdrinfo,
		unsigned char * hdrBuffer,int type);
};
#endif /* PACKETHDR_H_ */
