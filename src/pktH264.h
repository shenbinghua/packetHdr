/*
 * H264Parser.h
 *
 *  Created on: 2018年12月11日
 *      Author: root
 */

#ifndef PKTH264_H_
#define PKTH264_H_
//#include <stdint.h>
#include <math.h>
#include "parser_util.h"
#include "common.h"
#include "h2645_paramter.h"

#define H264_FRAME_MAX_LEN (1024*1024)
#define NAL_GET_5BIT(p) (*(p) & 0x1F)
#define NAL_GET_4BYTE(p) (*p<<24 | *(p+1)<<16 | *(p+2)<<8 |*(p+3))
#define NAL_GET_3BYTE(p) (*(p)<<16 | *(p+1)<<8 |*(p+2))

typedef enum {
	NALU_TYPE_SLICE    = 1,
	NALU_TYPE_DPA      = 2,
	NALU_TYPE_DPB      = 3,
	NALU_TYPE_DPC      = 4,
	NALU_TYPE_IDR      = 5,
	NALU_TYPE_SEI      = 6,
	NALU_TYPE_SPS      = 7,
	NALU_TYPE_PPS      = 8,
	NALU_TYPE_AUD      = 9,
	NALU_TYPE_EOSEQ    = 10,
	NALU_TYPE_EOSTREAM = 11,
	NALU_TYPE_FILL     = 12,
} NaluType;

typedef enum
{
	H264_SEI_TYPE_BUFFERING_PERIOD = 0,

	H264_SEI_TYPE_PICTURE_TIMING = 1,

	H264_SEI_TYPE_MASTERING_DISPLAY_COLOUR_VOLUME = 137,

}H264_SEI_TYPE;

class pktH264{

private:
	streamBuffer m_h264_buffer;
	streamBuffer m_outbuffer;
	stHDRMetadata h264info;
	int m_start_code_size;

public:
	int h264_parser(unsigned char * buffer, unsigned int bufferlen,stHDRMetadata & h264info,unsigned char * hdrBuffer);
private:
	int h264_restructure_sps(unsigned char * buffer, unsigned int bufferlen);
	int h264_restructure_mastering_display_colour_volume_sei();
	void h264_init(unsigned char * buffer, unsigned int bufferlen,stHDRMetadata & hdrinfo);
	void h264_clear_outBuffer();
	void h264_uinit();

private:
	int GetAnnexbNALU (NALU_t *nalu);

//======================parser=================
public:

};
#endif /* PKTH264_H_ */
