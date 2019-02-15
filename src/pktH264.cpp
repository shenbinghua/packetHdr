//============================================================================
// Name        : H264PARSER.cpp
// Author      : ss
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser_util.h"
#include "pktH264.h"



int pktH264::GetAnnexbNALU (NALU_t *nalu){
	int info2=0, info3=0;
	int pos = 0;
	int StartCodeFound, rewind;
	unsigned char *Buf;

	if ((Buf = (unsigned char*)calloc (nalu->max_size , sizeof(char))) == NULL)
		printf ("GetAnnexbNALU: Could not allocate Buf memory\n");

	nalu->startcodeprefix_len=3;

	//read_buffer(Buf,1,m_h264_buffer);
	if (3 != read_buffer(Buf,3,m_h264_buffer)){//先读3个字节出来，看文件里面是否有足够的字节
		free(Buf);
		Buf = NULL;
		return 0;
	}
	//int d = get_one_Byte(Buf,m_h264_buffer);
	info2 = FindStartCode2 (Buf);
	if(info2 != 1) {
		if(1 != read_buffer(Buf+3, 1,m_h264_buffer)){
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
		if (0 == m_h264_buffer.length){
			nalu->len = (pos)-nalu->startcodeprefix_len;
			memcpy (nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);
			nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit
			nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
			nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit
			free(Buf);
			Buf = NULL;
			return pos;
		}
		if(m_h264_buffer.length > 0)
			Buf[pos++] = get_char (m_h264_buffer);
		info3 = FindStartCode3(&Buf[pos-4]);
		if(info3 != 1)
			info2 = FindStartCode2(&Buf[pos-3]);
		StartCodeFound = (info2 == 1 || info3 == 1);
	}

	// Here, we have found another start code (and read length of startcode bytes more than we should
	// have.  Hence, go back in the file
	rewind = (info3 == 1)? -4 : -3;

	if (0 != buffer_seek (rewind,m_h264_buffer)){
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
	nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit
	if(Buf != NULL){
		free(Buf);
		Buf = NULL;
	}

	return (pos+rewind);//返回的是整个NALU的长度（nalu.len+StartCodeSizeLen）
}
/**
 * Analysis H.264 Bitstream
 * @param url    Location of input H.264 bitstream file.
 */
void pktH264::h264_init(unsigned char * buffer, unsigned int bufferlen,stHDRMetadata & hdrinfo){

	m_h264_buffer.data = buffer;
	m_h264_buffer.endIdx = bufferlen;
	m_h264_buffer.readIdx = 0;
	m_h264_buffer.length = bufferlen;

	h264info.video_format = hdrinfo.video_format;
	h264info.color_primaries = hdrinfo.color_primaries;
	h264info.transfer_characteristics = hdrinfo.transfer_characteristics;
	h264info.matrix_coeffs = hdrinfo.transfer_characteristics;
	h264info.chroma_sample_loc_type_bottom_field = hdrinfo.chroma_sample_loc_type_bottom_field;
	h264info.chroma_sample_loc_type_top_field = hdrinfo.chroma_sample_loc_type_top_field;
	h264info.video_full_range_flag = hdrinfo.video_full_range_flag;
	h264info.MaxCLL = hdrinfo.MaxCLL;
	h264info.MaxFall = hdrinfo.MaxFall;
	h264info.displayColorVolume.display_primaries_x_r = hdrinfo.displayColorVolume.display_primaries_x_r;//2
	h264info.displayColorVolume.display_primaries_y_r = hdrinfo.displayColorVolume.display_primaries_y_r;
	h264info.displayColorVolume.display_primaries_x_g = hdrinfo.displayColorVolume.display_primaries_x_g;//0
	h264info.displayColorVolume.display_primaries_y_g = hdrinfo.displayColorVolume.display_primaries_y_g;
	h264info.displayColorVolume.display_primaries_x_b = hdrinfo.displayColorVolume.display_primaries_x_b;//1
	h264info.displayColorVolume.display_primaries_y_b = hdrinfo.displayColorVolume.display_primaries_y_b;

	h264info.displayColorVolume.white_point_x = hdrinfo.displayColorVolume.white_point_x;
	h264info.displayColorVolume.white_point_y = hdrinfo.displayColorVolume.white_point_y;

	h264info.displayColorVolume.max_display_mastering_luminance = hdrinfo.displayColorVolume.max_display_mastering_luminance;
	h264info.displayColorVolume.min_display_mastering_luminance = hdrinfo.displayColorVolume.min_display_mastering_luminance;

	m_outbuffer.data = new unsigned char[2048];
	m_outbuffer.endIdx = 0;
	m_outbuffer.readIdx = 0;//写idx
	m_outbuffer.length = 0;
}

void pktH264::h264_uinit(){
	m_start_code_size = 0;
	m_h264_buffer.data = NULL;
	m_h264_buffer.endIdx = 0;
	m_h264_buffer.readIdx = 0;
	m_h264_buffer.length = 0;

	delete[] m_outbuffer.data;
	m_outbuffer.endIdx = 0;
	m_outbuffer.readIdx = 0;//写idx
	m_outbuffer.length = 0;
}

void pktH264::h264_clear_outBuffer(){
	m_outbuffer.endIdx = 0;
	m_outbuffer.readIdx = 0;//写idx
	m_outbuffer.length = 0;
}

int pktH264::h264_parser(unsigned char * buffer, unsigned int bufferlen,stHDRMetadata & h264info,unsigned char * hdrBuffer){

	NALU_t *n;
	h264_uinit();
	h264_init(buffer,bufferlen,h264info);
	n = (NALU_t*)calloc (1, sizeof (NALU_t));
	if (n == NULL){
		printf("Alloc NALU Error\n");
		//return 0;
	}

	n->max_size=H264_FRAME_MAX_LEN;
	n->buf = (char*)calloc (H264_FRAME_MAX_LEN, sizeof (char));
	if (n->buf == NULL){
		free (n);
		printf ("AllocNALU: n->buf");
		return 0;
	}

	unsigned char startCode3[3] = {0x00,0x00,0x01};
	unsigned char startCode4[4] = {0x00,0x00,0x00,0x01};

	int data_offset=0;//数据偏移量
	int hdrBuffer_ofset = 0;
	int nal_num=0;//帧数
	printf("-----+-------- NALU Table ------+---------+\n");
	printf(" NUM |    POS  |    IDC |  TYPE |   LEN   |\n");
	printf("-----+---------+--------+-------+---------+\n");

	while(m_h264_buffer.length > 0)
	{
		int data_lenth;
		data_lenth=GetAnnexbNALU(n);
		h264_clear_outBuffer();

		switch(n->nal_unit_type){
			case NALU_TYPE_SPS:{
				h264_restructure_sps(m_h264_buffer.data+(m_h264_buffer.readIdx-data_lenth+m_start_code_size),(data_lenth-m_start_code_size));
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
			case NALU_TYPE_PPS:{
				memcpy(hdrBuffer+hdrBuffer_ofset,m_h264_buffer.data+(m_h264_buffer.readIdx-data_lenth),data_lenth);
				printf("%d\t%d\n",hdrBuffer_ofset,m_h264_buffer.readIdx-data_lenth);
				hdrBuffer_ofset +=data_lenth;
				int sei_len = h264_restructure_mastering_display_colour_volume_sei();//找到pps，将sei添加到pps后面
				memcpy(hdrBuffer+hdrBuffer_ofset,m_outbuffer.data,m_outbuffer.length);
				hdrBuffer_ofset +=m_outbuffer.length;
				break;
			}
			default:{
				memcpy(hdrBuffer+hdrBuffer_ofset,m_h264_buffer.data+(m_h264_buffer.readIdx-data_lenth),data_lenth);
				//printf("%d\t%d\n",hdrBuffer_ofset,m_h264_buffer.readIdx-data_lenth);
				hdrBuffer_ofset +=data_lenth;
				break;
			}
		}
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
int pktH264::h264_restructure_sps(unsigned char * buf, unsigned int nLen){
	int width = 0;
	int height = 0;
	double fps = 0;

	unsigned int StartBit=0;
    fps=0;
    de_emulation_prevention(buf,&nLen);

    int forbidden_zero_bit=u(1,buf,StartBit);
    int nal_ref_idc=u(2,buf,StartBit);
    int nal_unit_type=u(5,buf,StartBit);
    if(nal_unit_type==7)
    {
        int profile_idc=u(8,buf,StartBit);
        int constraint_set0_flag=u(1,buf,StartBit);//(buf[1] & 0x80)>>7;
        int constraint_set1_flag=u(1,buf,StartBit);//(buf[1] & 0x40)>>6;
        int constraint_set2_flag=u(1,buf,StartBit);//(buf[1] & 0x20)>>5;
        int constraint_set3_flag=u(1,buf,StartBit);//(buf[1] & 0x10)>>4;
        int reserved_zero_4bits=u(4,buf,StartBit);
        int level_idc=u(8,buf,StartBit);

        int seq_parameter_set_id=Ue(buf,nLen,StartBit);

        if( profile_idc == 100 || profile_idc == 110 ||
            profile_idc == 122 || profile_idc == 144 )
        {
            int chroma_format_idc=Ue(buf,nLen,StartBit);
            if( chroma_format_idc == 3 )
                int residual_colour_transform_flag=u(1,buf,StartBit);
            int bit_depth_luma_minus8=Ue(buf,nLen,StartBit);
            int bit_depth_chroma_minus8=Ue(buf,nLen,StartBit);
            int qpprime_y_zero_transform_bypass_flag=u(1,buf,StartBit);
            int seq_scaling_matrix_present_flag=u(1,buf,StartBit);

            int seq_scaling_list_present_flag[8];
            if( seq_scaling_matrix_present_flag )
            {
                for( int i = 0; i < 8; i++ ) {
                    seq_scaling_list_present_flag[i]=u(1,buf,StartBit);
                }
            }
        }
        int log2_max_frame_num_minus4=Ue(buf,nLen,StartBit);
        int pic_order_cnt_type=Ue(buf,nLen,StartBit);
        if( pic_order_cnt_type == 0 )
            int log2_max_pic_order_cnt_lsb_minus4=Ue(buf,nLen,StartBit);
        else if( pic_order_cnt_type == 1 )
        {
            int delta_pic_order_always_zero_flag=u(1,buf,StartBit);
            int offset_for_non_ref_pic=Se(buf,nLen,StartBit);
            int offset_for_top_to_bottom_field=Se(buf,nLen,StartBit);
            int num_ref_frames_in_pic_order_cnt_cycle=Ue(buf,nLen,StartBit);

            int *offset_for_ref_frame=new int[num_ref_frames_in_pic_order_cnt_cycle];
            for( int i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
                offset_for_ref_frame[i]=Se(buf,nLen,StartBit);
            delete [] offset_for_ref_frame;
        }
        int num_ref_frames=Ue(buf,nLen,StartBit);
        int gaps_in_frame_num_value_allowed_flag=u(1,buf,StartBit);
        int pic_width_in_mbs_minus1=Ue(buf,nLen,StartBit);
        int pic_height_in_map_units_minus1=Ue(buf,nLen,StartBit);

        width=(pic_width_in_mbs_minus1+1)*16;
        height=(pic_height_in_map_units_minus1+1)*16;

        int frame_mbs_only_flag=u(1,buf,StartBit);
        if(!frame_mbs_only_flag)
            int mb_adaptive_frame_field_flag=u(1,buf,StartBit);

        int direct_8x8_inference_flag=u(1,buf,StartBit);
        int frame_cropping_flag=u(1,buf,StartBit);
        if(frame_cropping_flag)
        {
            int frame_crop_left_offset=Ue(buf,nLen,StartBit);
            int frame_crop_right_offset=Ue(buf,nLen,StartBit);
            int frame_crop_top_offset=Ue(buf,nLen,StartBit);
            int frame_crop_bottom_offset=Ue(buf,nLen,StartBit);
        }
        int vui_parameter_present_flag=u(1,buf,StartBit);
        if(vui_parameter_present_flag)
        {
            int aspect_ratio_info_present_flag=u(1,buf,StartBit);
            if(aspect_ratio_info_present_flag)
            {
                int aspect_ratio_idc=u(8,buf,StartBit);
                if(aspect_ratio_idc==255)
                {
                    int sar_width=u(16,buf,StartBit);
                    int sar_height=u(16,buf,StartBit);
                }
            }
            int overscan_info_present_flag=u(1,buf,StartBit);
            if(overscan_info_present_flag){
                int overscan_appropriate_flagu=u(1,buf,StartBit);
                }

            //上面部分不变
         copybits(buf,0,1,m_outbuffer.data,m_outbuffer.readIdx/8,1,StartBit);
         m_outbuffer.readIdx = StartBit;

         int video_signal_type_present_flag=u(1,buf,StartBit);
            if(video_signal_type_present_flag)
            {
                int video_format=u(3,buf,StartBit);
                int video_full_range_flag=u(1,buf,StartBit);
                int colour_description_present_flag=u(1,buf,StartBit);
                if(colour_description_present_flag)
                {
                    int colour_primaries=u(8,buf,StartBit);
                    int transfer_characteristics=u(8,buf,StartBit);
                    int matrix_coefficients=u(8,buf,StartBit);
                }
            }

            write_u(1,m_outbuffer.data,m_outbuffer.readIdx,1); //video_signal_type_present_flag = 1 1bit

            write_u(3,m_outbuffer.data,m_outbuffer.readIdx,h264info.video_format);//write video_format 3bit

            write_u(1,m_outbuffer.data,m_outbuffer.readIdx,h264info.video_full_range_flag);//video_full_range_flag 1bit

            write_u(1,m_outbuffer.data,m_outbuffer.readIdx,1);//colour_description_present_flag = 1 1bit

            write_u(8,m_outbuffer.data,m_outbuffer.readIdx,h264info.color_primaries);//color_primaries 8bit

            write_u(8,m_outbuffer.data,m_outbuffer.readIdx,h264info.transfer_characteristics);//transfer_characteristics 8bit

            write_u(8,m_outbuffer.data,m_outbuffer.readIdx,h264info.matrix_coeffs);//matrix_coeffs 8bit

            int chroma_loc_info_present_flag=u(1,buf,StartBit);
            if(chroma_loc_info_present_flag)
            {
                int chroma_sample_loc_type_top_field=Ue(buf,nLen,StartBit);
                int chroma_sample_loc_type_bottom_field=Ue(buf,nLen,StartBit);
            }
            write_u(1,m_outbuffer.data,m_outbuffer.readIdx,1); //chroma_loc_info_present_flag = 1 1bit

            write_Ue(m_outbuffer.data,nLen,m_outbuffer.readIdx,h264info.chroma_sample_loc_type_top_field);//chroma_sample_loc_type_top_field

            write_Ue(m_outbuffer.data,nLen,m_outbuffer.readIdx,h264info.chroma_sample_loc_type_bottom_field);//chroma_sample_loc_type_top_field

            //解析到这里，剩余的sps里面没有hdr参数，因此把sps生于的二进制流拷贝到outbuffer里面
//           unsigned int startPos = StartBit;
//           copybits(buf,(startPos/8),(startPos%8),m_outbuffer.data,(m_outbuffer.readIdx/8),(m_outbuffer.readIdx%8),((8*nLen)-startPos)+1);
//           m_outbuffer.readIdx = (8*nLen);

            while(StartBit<(8*nLen)){
            	int i = u(1,buf,StartBit);
            	write_u(1,m_outbuffer.data,m_outbuffer.readIdx,i);
            }

           unsigned int len = m_outbuffer.readIdx/8+((m_outbuffer.readIdx%8)>0?1:0);
           emulation_prevention(m_outbuffer.data,&len);
           m_outbuffer.length = len;

            int timing_info_present_flag=u(1,buf,StartBit);

//            if(timing_info_present_flag)
//            {
//                int num_units_in_tick=u(32,buf,StartBit);
//                int time_scale=u(32,buf,StartBit);
//                fps=time_scale/num_units_in_tick;
//                int fixed_frame_rate_flag=u(1,buf,StartBit);
//                if(fixed_frame_rate_flag)
//                {
//                    fps=fps/2;
//                }
//            }
        }
        return true;
    }
    else
        return false;
}

int pktH264::h264_restructure_mastering_display_colour_volume_sei(){
	/*	forbidden_zero_bit  All  f(1)
		nal_ref_idc  All  u(2)
		nal_unit_type  All  u(5)*/
	//startCode
	write_u(8,m_outbuffer.data,m_outbuffer.readIdx,0);
	write_u(8,m_outbuffer.data,m_outbuffer.readIdx,0);
	write_u(8,m_outbuffer.data,m_outbuffer.readIdx,1);

	//NALU Header
	write_u(1,m_outbuffer.data,m_outbuffer.readIdx,0);
	write_u(2,m_outbuffer.data,m_outbuffer.readIdx,0);
	write_u(5,m_outbuffer.data,m_outbuffer.readIdx,6);

	//H265_SEI_TYPE_MASTERING_DISPLAY_COLOUR_VOLUME
	write_u(8,m_outbuffer.data,m_outbuffer.readIdx,137);//last_payload_type_byte 8bit
	write_u(8,m_outbuffer.data,m_outbuffer.readIdx,24);//last_playload_size_byte 8bit

	//H265_SEI_TYPE_MASTERING_DISPLAY_COLOUR_VOLUME
	unsigned int display_primaries_x_g = h264info.displayColorVolume.display_primaries_x_g;
	unsigned int display_primaries_y_g = h264info.displayColorVolume.display_primaries_y_g;
	unsigned int display_primaries_x_b = h264info.displayColorVolume.display_primaries_x_b;
	unsigned int display_primaries_y_b = h264info.displayColorVolume.display_primaries_y_b;
	unsigned int display_primaries_x_r = h264info.displayColorVolume.display_primaries_x_r;
	unsigned int display_primaries_y_r = h264info.displayColorVolume.display_primaries_y_r;

	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,display_primaries_x_g);
	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,display_primaries_y_g);
	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,display_primaries_x_b);
	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,display_primaries_y_b);
	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,display_primaries_x_r);
	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,display_primaries_y_r);

	unsigned int white_point_x = h264info.displayColorVolume.white_point_x;//16
	unsigned int white_point_y = h264info.displayColorVolume.white_point_y;//16

	unsigned int max_display_mastering_luminance = h264info.displayColorVolume.max_display_mastering_luminance;//32
	unsigned int min_display_mastering_luminance = h264info.displayColorVolume.min_display_mastering_luminance;//32

	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,white_point_x);
	write_u(16,m_outbuffer.data,m_outbuffer.readIdx,white_point_y);

	write_u(32,m_outbuffer.data,m_outbuffer.readIdx,max_display_mastering_luminance);
	write_u(32,m_outbuffer.data,m_outbuffer.readIdx,min_display_mastering_luminance);

	 unsigned int len = (m_outbuffer.readIdx/8-3);
	 emulation_prevention(m_outbuffer.data+3,&len);
	 m_outbuffer.length = len+3;

	return (m_outbuffer.length);
}





