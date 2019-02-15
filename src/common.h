/*
 * common.h
 *
 *  Created on: 2018年12月19日
 *      Author: root
 */

#ifndef COMMON_H_
#define COMMON_H_
#include <string.h>

struct stMasteringDisplayColourVolume
{
	int  display_primaries_x_r;//2
	int  display_primaries_y_r;
	int  display_primaries_x_g;//0
	int  display_primaries_y_g;
	int  display_primaries_x_b;//1
	int  display_primaries_y_b;

	int  white_point_x ;
	int  white_point_y ;

	int  max_display_mastering_luminance ;
	int  min_display_mastering_luminance ;

	stMasteringDisplayColourVolume()
	{
		display_primaries_x_r = 1;
		display_primaries_y_r = 1;
		display_primaries_x_g = 1;
		display_primaries_y_g = 1;
		display_primaries_x_b = 1;
		display_primaries_y_b = 1;

		white_point_x = 1;
		white_point_y = 1;

		max_display_mastering_luminance = 1;
		min_display_mastering_luminance = 1;
	}

};

struct stHDRMetadata
{
	int video_format;//ITU-T H.265,  Table E-2 – Meaning of video_format
	int color_primaries ; //ITU-T H.265, Table E.3 – Colour primaries interpretation using the colour_primaries syntax element
	int transfer_characteristics ; //ITU-T H.265, Table E.4 – Transfer characteristics interpretation using the transfer_characteristics syntax element
	int matrix_coeffs; //ITU-T H.265,Table E.5 – Matrix coefficients interpretation using the matrix_coeffs syntax element
	int chroma_sample_loc_type_top_field;
	int chroma_sample_loc_type_bottom_field ;
	int video_full_range_flag;//ITU-T H.265,

	//Annex A.1 and A.2 of CEA 861.3  or  //ITU-T H.265,SEI  payload type 144
	int  MaxCLL; //max_content_light_level
	int  MaxFall;//max_pic_average_light_level

	stMasteringDisplayColourVolume  displayColorVolume ;

	wchar_t  transfer_gamut_desc[255];//example: "S-Log3/S-Gamut3.Cine"

	stHDRMetadata()
	{
		video_format = 4;
		color_primaries = 2;
		transfer_characteristics = 4;
		matrix_coeffs = 2;
		chroma_sample_loc_type_top_field = 0;
		chroma_sample_loc_type_bottom_field = 0;
		video_full_range_flag = 0;
		MaxCLL = 1;
		MaxFall = 1;
		memset(&transfer_gamut_desc,0,sizeof(wchar_t)*255);
	}
};



#endif /* COMMON_H_ */
