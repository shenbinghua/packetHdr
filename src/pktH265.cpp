/*
 * h265Parser.cpp
 *
 *  Created on: 2018年12月14日
 *      Author: root
 */

#include "pktH265.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser_util.h"
#define _SHOW_TYPE_ 0

/*返回0：成功，
 * 1：失败*/
int pktH265::h265_parser(unsigned char * buffer, unsigned int bufferlen,stHDRMetadata & h265info,unsigned char * hdrBuffer){
	h265_uinit();
	h265_init(buffer,bufferlen,h265info);
	NALU_t *n;
	FILE *myout=stdout;
	n = (NALU_t*)calloc (1, sizeof (NALU_t));
	if (n == NULL){
		printf("Alloc NALU Error\n");
		return 0;
	}
	unsigned char startCode3[3] = {0x00,0x00,0x01};
	unsigned char startCode4[4] = {0x00,0x00,0x00,0x01};
	n->max_size=H265_FRAME_MAX_LEN;
	n->buf = (char*)calloc (H265_FRAME_MAX_LEN, sizeof (char));
	if (n->buf == NULL){
		free (n);
		printf ("AllocNALU: n->buf");
		return 0;
	}

	int data_offset=0;//数据偏移量
	int hdrBuffer_ofset = 0;
	int nal_num=0;//帧数
#if _SHOW_TYPE_
	printf("-----+-------- H265 NALU Table--+---------+\n");
	printf(" NUM |    POS  |    IDC |  TYPE |   LEN   |\n");
	printf("-----+---------+--------+-------+---------+\n");
#endif
	while(m_h265_buffer.length > 0)
	{
		int data_lenth = 0;
		data_lenth=h265_GetAnnexbNALU(n);//data_lenth整个NALU的长度，包括startCode
		h265_clear_outBuffer();
		char type_str[20]={0};
		switch(n->nal_unit_type){
			case H265_NAL_UNIT_SPS:{//修改sps
				sprintf(type_str,"SPS");
				h265_restructure_sps(m_h265_buffer.data+(m_h265_buffer.readIdx-data_lenth+m_start_code_size),(data_lenth-m_start_code_size));
				//把新的sps放到视频流里面，m_outbuffer.data(不包含骑士字节)，所以要先拷贝骑士字节
				if(3 == n->startcodeprefix_len){
					memcpy(hdrBuffer+hdrBuffer_ofset,startCode3,3);
					hdrBuffer_ofset +=3;
				}
				else if(4 == n->startcodeprefix_len){
					memcpy(hdrBuffer+hdrBuffer_ofset,startCode4,4);
					hdrBuffer_ofset +=4;
				}else{
					return 1;
				}
				memcpy(hdrBuffer+hdrBuffer_ofset,m_outbuffer.data,m_outbuffer.length);
				hdrBuffer_ofset +=m_outbuffer.length;
				break;
			}
			case H265_NAL_UNIT_PPS:{
				memcpy(hdrBuffer+hdrBuffer_ofset,m_h265_buffer.data+(m_h265_buffer.readIdx-data_lenth),data_lenth);
				printf("%d\t%d\n",hdrBuffer_ofset,m_h265_buffer.readIdx-data_lenth);
				hdrBuffer_ofset +=data_lenth;

				int sei_len = h265_restructure_mastering_display_colour_volume_sei();//找到pps，将sei插入到pps后面
				memcpy(hdrBuffer+hdrBuffer_ofset,m_outbuffer.data,m_outbuffer.length);
				hdrBuffer_ofset +=m_outbuffer.length;
				//
				h265_clear_outBuffer();
				sei_len = h265_restructure_content_light_level_info_sei();//找到pps，将sei插入到pps后面
				memcpy(hdrBuffer+hdrBuffer_ofset,m_outbuffer.data,m_outbuffer.length);
				hdrBuffer_ofset +=m_outbuffer.length;

				break;
			}
			default:{
				sprintf(type_str,"Reserved");
				//copy
				memcpy(hdrBuffer+hdrBuffer_ofset,m_h265_buffer.data+(m_h265_buffer.readIdx-data_lenth),data_lenth);
				//printf("%d\t%d\n",hdrBuffer_ofset,m_h265_buffer.readIdx-data_lenth);
				hdrBuffer_ofset +=data_lenth;
				break;
			}
		}
#if _SHOW_TYPE_
		fprintf(myout,"%5d| %8d| %6s| %15s| %8d|\n",nal_num,data_offset,idc_str,type_str,n->len);
#endif
		data_offset=data_offset+data_lenth;
		nal_num++;
	}

	//Free
	if (n){
		if (n->buf){
			free(n->buf);
			n->buf=NULL;
		}
		free (n);
	}
	return hdrBuffer_ofset;


}

int pktH265::h265_GetAnnexbNALU (NALU_t *nalu){
	int info2=0, info3=0;
	int pos = 0;
	int StartCodeFound, rewind;
	unsigned char *Buf;

	if ((Buf = (unsigned char*)calloc (nalu->max_size , sizeof(char))) == NULL)
		printf ("GetAnnexbNALU: Could not allocate Buf memory\n");

	nalu->startcodeprefix_len=3;

	//read_buffer(Buf,1,m_h264_buffer);
	if (3 != read_buffer(Buf,3,m_h265_buffer)){//先读3个字节出来，看文件里面是否有足够的字节
		free(Buf);
		Buf = NULL;
		return 0;
	}
	//int d = get_one_Byte(Buf,m_h264_buffer);
	info2 = FindStartCode2 (Buf);
	if(info2 != 1) {
		if(1 != read_buffer(Buf+3, 1,m_h265_buffer)){
			free(Buf);
			Buf = NULL;
			return 0;
		}
		info3 = FindStartCode3 (Buf);
		if (info3 != 1){
			free(Buf);
			Buf = NULL;
			return -1;
		}
		else {
			pos = 4;
			nalu->startcodeprefix_len = 4;
		}
	}
	else{
		nalu->startcodeprefix_len = 3;
		pos = 3;
	}
	StartCodeFound = 0;
	info2 = 0;
	info3 = 0;
	m_start_code_size = nalu->startcodeprefix_len;
	while (!StartCodeFound){
		if (0 == m_h265_buffer.length){
			nalu->len = (pos)-nalu->startcodeprefix_len;//nalu->len = (pos-1)-nalu->startcodeprefix_len;
			memcpy (nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);
			nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit
			nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
			nalu->nal_unit_type = (nalu->buf[0]& 0x7E)>>1;// 5 bit
			free(Buf);
			Buf = NULL;
			return pos;
		}
		if(m_h265_buffer.length > 0)
			Buf[pos++] = get_char (m_h265_buffer);
		info3 = FindStartCode3(&Buf[pos-4]);
		if(info3 != 1)
			info2 = FindStartCode2(&Buf[pos-3]);
		StartCodeFound = (info2 == 1 || info3 == 1);
	}

	// Here, we have found another start code (and read length of startcode bytes more than we should
	// have.  Hence, go back in the file
	rewind = (info3 == 1)? -4 : -3;

	if (0 != buffer_seek (rewind,m_h265_buffer)){
		free(Buf);
		Buf = NULL;
		printf("GetAnnexbNALU: Cannot fseek in the bit stream file");
	}

	// Here the Start code, the complete NALU, and the next start code is in the Buf.
	// The size of Buf is pos, pos+rewind are the number of bytes excluding the next
	// start code, and (pos+rewind)-startcodeprefix_len is the size of the NALU excluding the start code

	nalu->len = (pos+rewind)-nalu->startcodeprefix_len;
	memcpy (nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);//
	nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit
	nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
	nalu->nal_unit_type = (nalu->buf[0]& 0x7E)>>1;// & 0x7E)>>1;
	if(Buf != NULL){
		free(Buf);
		Buf = NULL;
	}

	return (pos+rewind);//返回的是整个NALU的长度（nalu.len+StartCodeSizeLen）
}

void pktH265::h265_init(unsigned char * buffer, unsigned int bufferlen,stHDRMetadata & hdrinfo){
	m_h265_buffer.data = buffer;
	m_h265_buffer.endIdx = bufferlen;
	m_h265_buffer.readIdx = 0;
	m_h265_buffer.length = bufferlen;
	m_h265info.video_format = 5;
	m_outbuffer.data = new unsigned char[4096];
	m_outbuffer.endIdx = 0;
	m_outbuffer.readIdx = 0;//写idx
	m_outbuffer.length = 0;

	m_h265info.video_format = hdrinfo.video_format;
	m_h265info.color_primaries = hdrinfo.color_primaries;
	m_h265info.transfer_characteristics = hdrinfo.transfer_characteristics;
	m_h265info.matrix_coeffs = hdrinfo.transfer_characteristics;
	m_h265info.chroma_sample_loc_type_bottom_field = hdrinfo.chroma_sample_loc_type_bottom_field;
	m_h265info.chroma_sample_loc_type_top_field = hdrinfo.chroma_sample_loc_type_top_field;
	m_h265info.video_full_range_flag = hdrinfo.video_full_range_flag;
	m_h265info.MaxCLL = hdrinfo.MaxCLL;
	m_h265info.MaxFall = hdrinfo.MaxFall;
	m_h265info.displayColorVolume.display_primaries_x_r = hdrinfo.displayColorVolume.display_primaries_x_r;//2
	m_h265info.displayColorVolume.display_primaries_y_r = hdrinfo.displayColorVolume.display_primaries_y_r;
	m_h265info.displayColorVolume.display_primaries_x_g = hdrinfo.displayColorVolume.display_primaries_x_g;//0
	m_h265info.displayColorVolume.display_primaries_y_g = hdrinfo.displayColorVolume.display_primaries_y_g;
	m_h265info.displayColorVolume.display_primaries_x_b = hdrinfo.displayColorVolume.display_primaries_x_b;//1
	m_h265info.displayColorVolume.display_primaries_y_b = hdrinfo.displayColorVolume.display_primaries_y_b;

	m_h265info.displayColorVolume.white_point_x = hdrinfo.displayColorVolume.white_point_x;
	m_h265info.displayColorVolume.white_point_y = hdrinfo.displayColorVolume.white_point_y;

	m_h265info.displayColorVolume.max_display_mastering_luminance = hdrinfo.displayColorVolume.max_display_mastering_luminance;
	m_h265info.displayColorVolume.min_display_mastering_luminance = hdrinfo.displayColorVolume.min_display_mastering_luminance;

}

void pktH265::h265_clear_outBuffer(){
	m_outbuffer.endIdx = 0;
	m_outbuffer.readIdx = 0;//写idx
	m_outbuffer.length = 0;
}

void pktH265::h265_uinit(){
	m_start_code_size = 0;
	m_h265_buffer.data = NULL;
	m_h265_buffer.endIdx = 0;
	m_h265_buffer.readIdx = 0;
	m_h265_buffer.length = 0;

	delete[] m_outbuffer.data;
	m_outbuffer.endIdx = 0;
	m_outbuffer.readIdx = 0;//写idx
	m_outbuffer.length = 0;
}

int pktH265::h265_restructure_sps(unsigned char * buffer, unsigned int bufferlen){
	unsigned int StartBit=0;
   de_emulation_prevention(buffer,&bufferlen);

    uint32_t    sps_video_parameter_set_id = 0;
    uint32_t    sps_max_sub_layers_minus1 = 0;
    bool        sps_temporal_id_nesting_flag;
    uint32_t    sps_seq_parameter_set_id = 0;
    uint32_t    chroma_format_idc;
    bool        separate_colour_plane_flag = false;
    uint32_t    pic_width_in_luma_samples;
    uint32_t    pic_height_in_luma_samples;
    bool        conformance_window_flag;
    uint32_t    conf_win_left_offset;
    uint32_t    conf_win_right_offset;
    uint32_t    conf_win_top_offset;
    uint32_t    conf_win_bottom_offset;
    uint32_t    bit_depth_luma_minus8;
    uint32_t    bit_depth_chroma_minus8;
    uint32_t    log2_max_pic_order_cnt_lsb_minus4;
    bool        sps_sub_layer_ordering_info_present_flag;
    bool        rbsp_stop_one_bit;

    u(16,buffer,StartBit);//forbidden_zero_bit  f(1),nal_unit_type  u(6),nuh_layer_id  u(6),nuh_temporal_id_plus1  u(3)
    sps_video_parameter_set_id      = u(4,buffer,StartBit);
    sps_max_sub_layers_minus1       = u(3,buffer,StartBit);
    sps_temporal_id_nesting_flag    = u(1,buffer,StartBit);

    h265_parse_ptl(sps_max_sub_layers_minus1,buffer,StartBit,bufferlen);

    sps_seq_parameter_set_id    = Ue(buffer,bufferlen,StartBit);
    //p_sps = &sps[sps_seq_parameter_set_id];

    chroma_format_idc           = Ue(buffer,bufferlen,StartBit);

    if (3 == chroma_format_idc)
    {
        separate_colour_plane_flag = u(1,buffer,StartBit);
    }

    pic_width_in_luma_samples   = Ue(buffer,bufferlen,StartBit);
    pic_height_in_luma_samples  = Ue(buffer,bufferlen,StartBit);

    conformance_window_flag = u(1,buffer,StartBit);

    if (conformance_window_flag)
    {
        conf_win_left_offset    = Ue(buffer,bufferlen,StartBit);
        conf_win_right_offset   = Ue(buffer,bufferlen,StartBit);
        conf_win_top_offset     = Ue(buffer,bufferlen,StartBit);
        conf_win_bottom_offset  = Ue(buffer,bufferlen,StartBit);
    }

    bit_depth_luma_minus8               = Ue(buffer,bufferlen,StartBit);
    bit_depth_chroma_minus8             = Ue(buffer,bufferlen,StartBit);
    log2_max_pic_order_cnt_lsb_minus4   = Ue(buffer,bufferlen,StartBit);

    sps_sub_layer_ordering_info_present_flag = u(1,buffer,StartBit);

    int i;
    uint32_t *sps_max_dec_pic_buffering_minus1   = new uint32_t[sps_max_sub_layers_minus1 + 1];
    uint32_t *sps_max_num_reorder_pics           = new uint32_t[sps_max_sub_layers_minus1 + 1];
    uint32_t *sps_max_latency_increase_plus1     = new uint32_t[sps_max_sub_layers_minus1 + 1];

    for (i = (sps_sub_layer_ordering_info_present_flag ? 0 : sps_max_sub_layers_minus1); i <= sps_max_sub_layers_minus1; i++ )
    {
        sps_max_dec_pic_buffering_minus1[i] = Ue(buffer,bufferlen,StartBit);
        sps_max_num_reorder_pics[i]         = Ue(buffer,bufferlen,StartBit);
        sps_max_latency_increase_plus1[i]   = Ue(buffer,bufferlen,StartBit);
    }

    uint32_t log2_min_luma_coding_block_size_minus3;
    uint32_t log2_diff_max_min_luma_coding_block_size;
    uint32_t log2_min_transform_block_size_minus2;
    uint32_t log2_diff_max_min_transform_block_size;
    uint32_t max_transform_hierarchy_depth_inter;
    uint32_t max_transform_hierarchy_depth_intra;
    bool     scaling_list_enabled_flag;

    log2_min_luma_coding_block_size_minus3      = Ue(buffer,bufferlen,StartBit);
    log2_diff_max_min_luma_coding_block_size    = Ue(buffer,bufferlen,StartBit);
    log2_min_transform_block_size_minus2        = Ue(buffer,bufferlen,StartBit);
    log2_diff_max_min_transform_block_size      = Ue(buffer,bufferlen,StartBit);
    max_transform_hierarchy_depth_inter         = Ue(buffer,bufferlen,StartBit);
    max_transform_hierarchy_depth_intra         = Ue(buffer,bufferlen,StartBit);
    scaling_list_enabled_flag                   = u(1,buffer,StartBit);

    if (scaling_list_enabled_flag)
    {
        bool sps_scaling_list_data_present_flag;

        sps_scaling_list_data_present_flag = u(1,buffer,StartBit);

        if (sps_scaling_list_data_present_flag)
        {
        	h265_parse_scaling_list(buffer,StartBit,bufferlen);
        }
    }

    bool amp_enabled_flag;
    bool sample_adaptive_offset_enabled_flag;
    bool pcm_enabled_flag;
    uint32_t pcm_sample_bit_depth_luma_minus1;
    uint32_t pcm_sample_bit_depth_chroma_minus1;
    uint32_t log2_min_pcm_luma_coding_block_size_minus3;
    uint32_t log2_diff_max_min_pcm_luma_coding_block_size;
    bool pcm_loop_filter_disabled_flag;

    amp_enabled_flag = u(1,buffer,StartBit);
    sample_adaptive_offset_enabled_flag = u(1,buffer,StartBit);
    pcm_enabled_flag = u(1,buffer,StartBit);

    if (pcm_enabled_flag)
    {
        pcm_sample_bit_depth_luma_minus1    = u(4,buffer,StartBit);
        pcm_sample_bit_depth_chroma_minus1  = u(4,buffer,StartBit);
        log2_min_pcm_luma_coding_block_size_minus3 = Ue(buffer,bufferlen,StartBit);
        log2_diff_max_min_pcm_luma_coding_block_size = Ue(buffer,bufferlen,StartBit);
        pcm_loop_filter_disabled_flag       = u(1,buffer,StartBit);
    }

    uint32_t num_short_term_ref_pic_sets = 0;

    num_short_term_ref_pic_sets = Ue(buffer,bufferlen,StartBit);

//    createRPSList(p_sps, num_short_term_ref_pic_sets);

//     for (i = 0; i < num_short_term_ref_pic_sets; i++)
//     {
//         ReferencePictureSet_t *rps = &p_sps->m_RPSList.m_referencePictureSets[i];
//         parse_short_term_ref_pic_set(p_sps, rps, i);
//     }

    bool long_term_ref_pics_present_flag = false;

    long_term_ref_pics_present_flag = u(1,buffer,StartBit);

    if (long_term_ref_pics_present_flag)
    {
        uint32_t num_long_term_ref_pics_sps;

        num_long_term_ref_pics_sps = Ue(buffer,bufferlen,StartBit);

        uint32_t *lt_ref_pic_poc_lsb_sps = new uint32_t[num_long_term_ref_pics_sps];
        uint32_t *used_by_curr_pic_lt_sps_flag = new uint32_t[num_long_term_ref_pics_sps];

        for (i = 0; i < num_long_term_ref_pics_sps; i++)
        {
        		int varible = (log2_max_pic_order_cnt_lsb_minus4+4)>16?16:(log2_max_pic_order_cnt_lsb_minus4+4);
            lt_ref_pic_poc_lsb_sps[i]       = u(varible,buffer,StartBit);
            used_by_curr_pic_lt_sps_flag[i] = u(1,buffer,StartBit);
        }
    }

    bool    sps_temporal_mvp_enabled_flag;
    bool    strong_intra_smoothing_enabled_flag;
    bool    vui_parameters_present_flag;

    sps_temporal_mvp_enabled_flag       = u(1,buffer,StartBit);
    strong_intra_smoothing_enabled_flag = u(1,buffer,StartBit);
    vui_parameters_present_flag         = u(1,buffer,StartBit);
    copybits(buffer,0,1,m_outbuffer.data,m_outbuffer.readIdx/8,1,StartBit);
    m_outbuffer.readIdx = StartBit;
    if (vui_parameters_present_flag)
    {
    	h265_parse_vui(buffer,StartBit,bufferlen);
    }
    //copy剩下的一部分
    //copybits(buffer,0,1);
    unsigned int len = m_outbuffer.readIdx/8+((m_outbuffer.readIdx%8)>0?1:0);
    emulation_prevention(m_outbuffer.data,&len);
    m_outbuffer.length = len;
    return 1;
}

/* profile_tier_level */
void pktH265::h265_parse_ptl(uint32_t max_sub_layers_minus1,unsigned char * buffer,unsigned int &StartBit,unsigned int bufLen)
{
    unsigned char general_profile_space;
    bool    general_tier_flag;
    unsigned char general_profile_idc;
    unsigned char general_profile_compatibility_flag[32];
    bool    general_progressive_source_flag;
    bool    general_interlaced_source_flag;
    bool    general_non_packed_constraint_flag;
    bool    general_frame_only_constraint_flag;
    unsigned long int general_reserved_zero_44bits;
    unsigned char general_level_idc;

    bool    *sub_layer_profile_present_flag = new bool[max_sub_layers_minus1];
    bool    *sub_layer_level_present_flag = new bool[max_sub_layers_minus1];
    unsigned char *sub_layer_profile_space = new unsigned char[max_sub_layers_minus1];
    bool    *sub_layer_tier_flag = new bool[max_sub_layers_minus1];
    unsigned char *sub_layer_profile_idc = new unsigned char[max_sub_layers_minus1];

    uint32_t i;
    uint32_t j;

    general_profile_space   = u(2,buffer,StartBit);
    general_tier_flag       = u(1,buffer,StartBit);
    general_profile_idc     = u(5,buffer,StartBit);

    for (j = 0; j < 32; j++)
    {
        general_profile_compatibility_flag[j] = u(1,buffer,StartBit);
    }

    general_progressive_source_flag     = u(1,buffer,StartBit);
    general_interlaced_source_flag      = u(1,buffer,StartBit);
    general_non_packed_constraint_flag  = u(1,buffer,StartBit);
    general_frame_only_constraint_flag  = u(1,buffer,StartBit);

    u(16,buffer,StartBit);
    u(16,buffer,StartBit);
    u(12,buffer,StartBit);

    general_level_idc = u(8,buffer,StartBit);

    for (i = 0; i < max_sub_layers_minus1; i++)
    {
        sub_layer_profile_present_flag[i]   = u(1,buffer,StartBit);
        sub_layer_level_present_flag[i]     = u(1,buffer,StartBit);
    }

    if (max_sub_layers_minus1 > 0)
    {
        for (i = max_sub_layers_minus1; i < 8; i++)
        {
        	u(2,buffer,StartBit);
        }
    }

    for (i = 0; i < max_sub_layers_minus1; i++)
    {
        if (sub_layer_profile_present_flag[i])
        {
            sub_layer_profile_space[i]  = u(2,buffer,StartBit);
            sub_layer_tier_flag[i]      = u(1,buffer,StartBit);
            sub_layer_profile_idc[i]    = u(5,buffer,StartBit);
        }

        if (sub_layer_level_present_flag[i])
        {
        }
    }
}
/** decode quantization matrix */
static uint32_t ScalingList[4][6][64];
void pktH265::h265_parse_scaling_list(unsigned char * buffer,unsigned int &StartBit,unsigned int bufLen)
{
    int         sizeId;
    int         matrixId;
    bool        scaling_list_pred_mode_flag[4][6];
    uint32_t    scaling_list_pred_matrix_id_delta[4][6];

    for (sizeId = 0; sizeId < 4; sizeId++)
    {
        for (matrixId = 0; matrixId < ( (sizeId == 3) ? 2 : 6 ); matrixId++)
        {
            scaling_list_pred_mode_flag[sizeId][matrixId] = u(1,buffer,StartBit);

            if (!scaling_list_pred_mode_flag[sizeId][matrixId])
            {
                scaling_list_pred_matrix_id_delta[sizeId][matrixId] = Ue(buffer,bufLen,StartBit);
            }
            else
            {
                uint32_t    nextCoef;
                uint32_t    coefNum;
                int32_t     scaling_list_dc_coef_minus8[4][6];

                nextCoef    = 8;
                coefNum     = parser_min(64, (1 << (4 + (sizeId << 1))));

                if (sizeId > 1)
                {
                    scaling_list_dc_coef_minus8[sizeId - 2][matrixId] = Se(buffer,bufLen,StartBit);

                    nextCoef = scaling_list_dc_coef_minus8[sizeId - 2][matrixId] + 8;
                }

                uint32_t i;
                for (i = 0; i < coefNum; i++)
                {
                    int32_t scaling_list_delta_coef;

                    scaling_list_delta_coef = Se(buffer,bufLen,StartBit);
                    nextCoef = (nextCoef + scaling_list_delta_coef + 256) % 256;
                    ScalingList[sizeId][matrixId][i] = nextCoef;
                }
            }
        }
    }
}

void pktH265::h265_parse_vui(unsigned char * buffer,unsigned int &StartBit,unsigned int bufLen)
{
    bool aspect_ratio_info_present_flag;
    int  aspect_ratio_idc = 0;
    int startPos = StartBit;
    unsigned short int        sar_width;
    unsigned short int        sar_height;

    aspect_ratio_info_present_flag = u(1,buffer,StartBit);

    if (aspect_ratio_info_present_flag)
    {
        aspect_ratio_idc = u(8,buffer,StartBit);

        if (aspect_ratio_idc == 255)
        {
            sar_width   = u(16,buffer,StartBit);
            sar_height  = u(16,buffer,StartBit);
        }
    }

    bool overscan_info_present_flag;
    bool overscan_appropriate_flag;

    overscan_info_present_flag = u(1,buffer,StartBit);

    if (overscan_info_present_flag)
    {
        overscan_appropriate_flag = u(1,buffer,StartBit);
    }

    bool    video_signal_type_present_flag;
    unsigned char video_format;
    bool    video_full_range_flag;
    bool    colour_description_present_flag;
    unsigned char colour_primaries;
    unsigned char transfer_characteristics;
    unsigned char matrix_coeffs;

    copybits(buffer,startPos/8,startPos%8,m_outbuffer.data,startPos/8,startPos%8,(StartBit-startPos));
    m_outbuffer.readIdx = StartBit;
    startPos = StartBit;
    video_signal_type_present_flag = u(1,buffer,StartBit);
    if (video_signal_type_present_flag)
    {
        video_format            = u(3,buffer,StartBit);
        video_full_range_flag   = u(1,buffer,StartBit);
        colour_description_present_flag = u(1,buffer,StartBit);
        if (colour_description_present_flag)
        {
            colour_primaries            = u(8,buffer,StartBit);
            transfer_characteristics    = u(8,buffer,StartBit);
            matrix_coeffs               = u(8,buffer,StartBit);

        }
    }
    write_u(1,m_outbuffer.data,m_outbuffer.readIdx,1); //video_signal_type_present_flag = 1 1bit

    	write_u(3,m_outbuffer.data,m_outbuffer.readIdx,m_h265info.video_format);//write video_format 3bit

    	write_u(1,m_outbuffer.data,m_outbuffer.readIdx,m_h265info.video_full_range_flag);//video_full_range_flag 1bit

    	write_u(1,m_outbuffer.data,m_outbuffer.readIdx,1);//colour_description_present_flag = 1 1bit

    		write_u(8,m_outbuffer.data,m_outbuffer.readIdx,m_h265info.color_primaries);//color_primaries 8bit

   		write_u(8,m_outbuffer.data,m_outbuffer.readIdx,m_h265info.transfer_characteristics);//transfer_characteristics 8bit

    		write_u(8,m_outbuffer.data,m_outbuffer.readIdx,m_h265info.matrix_coeffs);//matrix_coeffs 8bit


    bool        chroma_loc_info_present_flag;
    uint32_t    chroma_sample_loc_type_top_field;
    uint32_t    chroma_sample_loc_type_bottom_field;

    chroma_loc_info_present_flag = u(1,buffer,StartBit);

    if (chroma_loc_info_present_flag)
    {
        chroma_sample_loc_type_top_field    = Ue(buffer,bufLen,StartBit);
        chroma_sample_loc_type_bottom_field = Ue(buffer,bufLen,StartBit);
    }

   write_u(1,m_outbuffer.data,m_outbuffer.readIdx,1); //chroma_loc_info_present_flag = 1 1bit
//
   	   write_Ue(m_outbuffer.data,bufLen,m_outbuffer.readIdx,m_h265info.chroma_sample_loc_type_top_field);//chroma_sample_loc_type_top_field
//
   	   write_Ue(m_outbuffer.data,bufLen,m_outbuffer.readIdx,m_h265info.chroma_sample_loc_type_bottom_field);//chroma_sample_loc_type_top_field

    int        neutral_chroma_indication_flag;
    int        field_seq_flag;
    int        frame_field_info_present_flag;
    int        default_display_window_flag;
    uint32_t    def_disp_win_left_offset;
    uint32_t    def_disp_win_right_offset;
    uint32_t    def_disp_win_top_offset;
    uint32_t    def_disp_win_bottom_offset;

    neutral_chroma_indication_flag  = u(1,buffer,StartBit);
    write_u(1,m_outbuffer.data,m_outbuffer.readIdx,neutral_chroma_indication_flag); //neutral_chroma_indication_flag 1bit

    field_seq_flag                  = u(1,buffer,StartBit);
    write_u(1,m_outbuffer.data,m_outbuffer.readIdx,field_seq_flag);

    frame_field_info_present_flag   = u(1,buffer,StartBit);
    write_u(1,m_outbuffer.data,m_outbuffer.readIdx,frame_field_info_present_flag);

    default_display_window_flag     = u(1,buffer,StartBit);
    write_u(1,m_outbuffer.data,m_outbuffer.readIdx,default_display_window_flag);

    if (default_display_window_flag)
    {
        def_disp_win_left_offset    = Ue(buffer,bufLen,StartBit);
        write_Ue(m_outbuffer.data,bufLen,m_outbuffer.readIdx,def_disp_win_left_offset);

        def_disp_win_right_offset   = Ue(buffer,bufLen,StartBit);
        write_Ue(m_outbuffer.data,bufLen,m_outbuffer.readIdx,def_disp_win_right_offset);

        def_disp_win_top_offset     = Ue(buffer,bufLen,StartBit);
        write_Ue(m_outbuffer.data,bufLen,m_outbuffer.readIdx,def_disp_win_top_offset);

        def_disp_win_bottom_offset  = Ue(buffer,bufLen,StartBit);
        write_Ue(m_outbuffer.data,bufLen,m_outbuffer.readIdx,def_disp_win_bottom_offset);
    }


    bool        vui_timing_info_present_flag;
    uint32_t    vui_num_units_in_tick;
    uint32_t    vui_time_scale;
    bool        vui_poc_proportional_to_timing_flag;
    uint32_t    vui_num_ticks_poc_diff_one_minus1;
    bool        vui_hrd_parameters_present_flag;

    vui_timing_info_present_flag = u(1,buffer,StartBit);
    write_u(1,m_outbuffer.data,m_outbuffer.readIdx,vui_timing_info_present_flag);

    if (vui_timing_info_present_flag)
    {
        vui_num_units_in_tick   = u(32,buffer,StartBit);
        write_u(32,m_outbuffer.data,m_outbuffer.readIdx,vui_num_units_in_tick);

        vui_time_scale          = u(32,buffer,StartBit);
        write_u(32,m_outbuffer.data,m_outbuffer.readIdx,vui_time_scale);

        vui_poc_proportional_to_timing_flag = u(1,buffer,StartBit);
        write_u(1,m_outbuffer.data,m_outbuffer.readIdx,vui_poc_proportional_to_timing_flag);

        double fps = (double)vui_time_scale/(double)vui_num_units_in_tick;

        if (vui_poc_proportional_to_timing_flag)
        {
            vui_num_ticks_poc_diff_one_minus1 = Ue(buffer,bufLen,StartBit);
            write_Ue(m_outbuffer.data,bufLen,m_outbuffer.readIdx,vui_num_ticks_poc_diff_one_minus1);
        }

        vui_hrd_parameters_present_flag = u(1,buffer,StartBit);
        write_u(1,m_outbuffer.data,m_outbuffer.readIdx,vui_hrd_parameters_present_flag);
        //解析到这里，剩余的sps里面没有hdr参数，因此把sps生于的二进制流拷贝到outbuffer里面
        int cnt = 0;
        while(StartBit<(8*bufLen)){
        	int i = u(1,buffer,StartBit);
        	write_u(1,m_outbuffer.data,m_outbuffer.readIdx,i);
        }

 //       startPos = StartBit;
//       copybits(buffer,(startPos/8),(startPos%8),m_outbuffer.data,(m_outbuffer.readIdx/8),(m_outbuffer.readIdx%8),((8*bufLen)-startPos)+1);
    }

}

int pktH265::h265_restructure_content_light_level_info_sei(){
	/*	forbidden_zero_bit  f(1)
		nal_unit_type  u(6)
		nuh_layer_id  u(6)
		nuh_temporal_id_plus1  u(3)*/
//	unsigned int sei_forbidden_zero_bit  = 0;//1bit
//	unsigned int sei_nal_unit_type = 39;//6bit
//	unsigned int sei_nuh_layer_id = 0;//6bit
//	unsigned int nuh_temporal_id_plus1 = 1;//3bit
	write_u(8,m_outbuffer.data,m_outbuffer.readIdx,0);
	write_u(8,m_outbuffer.data,m_outbuffer.readIdx,0);
	write_u(8,m_outbuffer.data,m_outbuffer.readIdx,1);
	write_u(8,m_outbuffer.data,m_outbuffer.readIdx,78);
	write_u(8,m_outbuffer.data,m_outbuffer.readIdx,1);

	//H265_SEI_TYPE_CONTENT_LIGHT_LEVEL_INFO
	write_u(8,m_outbuffer.data,m_outbuffer.readIdx,144);//last_payload_type_byte 8bit
	write_u(8,m_outbuffer.data,m_outbuffer.readIdx,4);//last_playload_size_byte 8bit
	unsigned int max_content_light_level = m_h265info.MaxCLL;//16bit
	unsigned int max_pic_average_light_level = m_h265info.MaxFall;//16bit
	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,max_content_light_level);//MaxCLL
	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,max_pic_average_light_level);//MaxFall
	m_outbuffer.length = (m_outbuffer.readIdx/8);

	unsigned int len = (m_outbuffer.readIdx/8-3);
	emulation_prevention(m_outbuffer.data+3,&len);
	m_outbuffer.length = len+3;

	return (m_outbuffer.length);
}

int pktH265::h265_restructure_mastering_display_colour_volume_sei(){
	/*	forbidden_zero_bit  f(1)
		nal_unit_type  u(6)
		nuh_layer_id  u(6)
		nuh_temporal_id_plus1  u(3)*/
//	unsigned int sei_forbidden_zero_bit  = 0;//1bit
//	unsigned int sei_nal_unit_type = 39;//6bit
//	unsigned int sei_nuh_layer_id = 0;//6bit
//	unsigned int nuh_temporal_id_plus1 = 1;//3bit
	//startCode
	write_u(8,m_outbuffer.data,m_outbuffer.readIdx,0);
	write_u(8,m_outbuffer.data,m_outbuffer.readIdx,0);
	write_u(8,m_outbuffer.data,m_outbuffer.readIdx,1);

	//MALU header
	write_u(1,m_outbuffer.data,m_outbuffer.readIdx,0);
	write_u(6,m_outbuffer.data,m_outbuffer.readIdx,39);
	write_u(6,m_outbuffer.data,m_outbuffer.readIdx,0);
	write_u(3,m_outbuffer.data,m_outbuffer.readIdx,1);

	//H265_SEI_TYPE_MASTERING_DISPLAY_COLOUR_VOLUME
	write_u(8,m_outbuffer.data,m_outbuffer.readIdx,137);//last_payload_type_byte 8bit
	write_u(8,m_outbuffer.data,m_outbuffer.readIdx,24);//last_playload_size_byte 8bit

	unsigned int display_primaries_x_g = m_h265info.displayColorVolume.display_primaries_x_g;
	unsigned int display_primaries_y_g = m_h265info.displayColorVolume.display_primaries_y_g;
	unsigned int display_primaries_x_b = m_h265info.displayColorVolume.display_primaries_x_b;
	unsigned int display_primaries_y_b = m_h265info.displayColorVolume.display_primaries_y_b;
	unsigned int display_primaries_x_r = m_h265info.displayColorVolume.display_primaries_x_r;
	unsigned int display_primaries_y_r = m_h265info.displayColorVolume.display_primaries_y_r;

	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,display_primaries_x_g);
	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,display_primaries_y_g);
	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,display_primaries_x_b);
	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,display_primaries_y_b);
	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,display_primaries_x_r);
	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,display_primaries_y_r);

	unsigned int white_point_x = m_h265info.displayColorVolume.white_point_x;//16
	unsigned int white_point_y = m_h265info.displayColorVolume.white_point_y;//16

	unsigned int max_display_mastering_luminance = m_h265info.displayColorVolume.max_display_mastering_luminance;//32
	unsigned int min_display_mastering_luminance = m_h265info.displayColorVolume.min_display_mastering_luminance;//32

	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,white_point_x);
	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,white_point_y);

	write_u(32,m_outbuffer.data,m_outbuffer.readIdx,max_display_mastering_luminance);
	write_u(32,m_outbuffer.data,m_outbuffer.readIdx,min_display_mastering_luminance);


	unsigned int len = (m_outbuffer.readIdx/8-3);
	emulation_prevention(m_outbuffer.data+3,&len);
	m_outbuffer.length = len+3;

	return (m_outbuffer.length);
}



