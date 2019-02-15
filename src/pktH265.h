/*
 * pkt_hdr_h265.h
 *
 *  Created on: 2018年12月19日
 *      Author: root
 */

#ifndef PKTH265_H_
#define PKTH265_H_

#include "parser_util.h"
#include "h2645_paramter.h"
#include "common.h"

#define H265_FRAME_MAX_LEN (1024*1024*2)

typedef enum
{
	H265_SEI_TYPE_PICTURE_TIMING = 1,

	H265_SEI_TYPE_ACTIVE_PARAMTER_SETS = 129,

	H265_SEI_TYPE_MASTERING_DISPLAY_COLOUR_VOLUME = 137,

	H265_SEI_TYPE_CONTENT_LIGHT_LEVEL_INFO = 144,

}H265_SEI_TYPE;

class pktH265{
private:
	streamBuffer m_h265_buffer;
	streamBuffer m_outbuffer;
	stHDRMetadata m_h265info;
	int m_start_code_size;
public:
	int h265_parser(unsigned char * buffer, unsigned int bufferlen,stHDRMetadata & h265info,unsigned char * hdrBuffer);
	void h265_init(unsigned char * buffer, unsigned int bufferlen,stHDRMetadata & hdrinfo);
	void h265_uinit();
private:
	int h265_restructure_sps(unsigned char * buffer, unsigned int bufferlen);
	void h265_parse_ptl(unsigned int max_sub_layers_minus1,unsigned char * buffer,unsigned int &StartBit,unsigned int bufLen);
	void h265_parse_scaling_list(unsigned char * buffer,unsigned int &StartBit,unsigned int bufLen);
	void h265_parse_vui(unsigned char * buffer,unsigned int &StartBit,unsigned int bufLen);
	void h265_clear_outBuffer();

	int h265_restructure_content_light_level_info_sei();
	int h265_restructure_mastering_display_colour_volume_sei();
	int h265_sei_message(unsigned char * buf,unsigned int &StartBit);
	int h265_sei_playload(int payload_type,unsigned char * buf,unsigned int &StartBit);
	int h265_mastering_display_colour_volume(unsigned char * buf,unsigned int &StartBit);
	int h265_content_light_level_info(unsigned char * buf,unsigned int &StartBit);
	int h265_active_parameter_sets(unsigned char * buf,unsigned int &StartBit);
	int h265_decode_nal_sei_decoded_picture_hash(unsigned char * buf,unsigned int &StartBit);
	int h265_GetAnnexbNALU (NALU_t *nalu);
};


#endif /* PKTH265_H_ */
