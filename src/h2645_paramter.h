/*
 * h2645_paramter.h
 *
 *  Created on: 2018年12月14日
 *      Author: root
 */

#ifndef HEVCPARSER_H2645_PARAMTER_H_
#define HEVCPARSER_H2645_PARAMTER_H_


typedef struct
{
	int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
	unsigned len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
	unsigned max_size;            //! Nal Unit Buffer max size
	int forbidden_bit;            //! should be always FALSE
	int nal_reference_idc;        //! NALU_PRIORITY_xxxx
	int nal_unit_type;            //! NALU_TYPE_xxxx
	char *buf;                    //! contains the first byte followed by the EBSP(not include startcode)
} NALU_t;

typedef enum
{
   H265_NAL_UNIT_CODED_SLICE_TRAIL_N = 0, // 0
	H265_NAL_UNIT_CODED_SLICE_TRAIL_R,     // 1

	H265_NAL_UNIT_CODED_SLICE_TSA_N,       // 2
	H265_NAL_UNIT_CODED_SLICE_TSA_R,       // 3

	H265_NAL_UNIT_CODED_SLICE_STSA_N,      // 4
	H265_NAL_UNIT_CODED_SLICE_STSA_R,      // 5

	H265_NAL_UNIT_CODED_SLICE_RADL_N,      // 6
	H265_NAL_UNIT_CODED_SLICE_RADL_R,      // 7

	H265_NAL_UNIT_CODED_SLICE_RASL_N,      // 8
	H265_NAL_UNIT_CODED_SLICE_RASL_R,      // 9

	H265_NAL_UNIT_RESERVED_VCL_N10,
	H265_NAL_UNIT_RESERVED_VCL_R11,
	H265_NAL_UNIT_RESERVED_VCL_N12,
	H265_NAL_UNIT_RESERVED_VCL_R13,
	H265_NAL_UNIT_RESERVED_VCL_N14,
	H265_NAL_UNIT_RESERVED_VCL_R15,

	H265_NAL_UNIT_CODED_SLICE_BLA_W_LP,    // 16
	H265_NAL_UNIT_CODED_SLICE_BLA_W_RADL,  // 17
	H265_NAL_UNIT_CODED_SLICE_BLA_N_LP,    // 18
	H265_NAL_UNIT_CODED_SLICE_IDR_W_RADL,  // 19
	H265_NAL_UNIT_CODED_SLICE_IDR_N_LP,    // 20
	H265_NAL_UNIT_CODED_SLICE_CRA,         // 21
	H265_NAL_UNIT_RESERVED_IRAP_VCL22,
	H265_NAL_UNIT_RESERVED_IRAP_VCL23,

	H265_NAL_UNIT_RESERVED_VCL24,
	H265_NAL_UNIT_RESERVED_VCL25,
	H265_NAL_UNIT_RESERVED_VCL26,
	H265_NAL_UNIT_RESERVED_VCL27,
	H265_NAL_UNIT_RESERVED_VCL28,
	H265_NAL_UNIT_RESERVED_VCL29,
	H265_NAL_UNIT_RESERVED_VCL30,
	H265_NAL_UNIT_RESERVED_VCL31,

	H265_NAL_UNIT_VPS,                     // 32
	H265_NAL_UNIT_SPS,                     // 33
	H265_NAL_UNIT_PPS,                     // 34
	H265_NAL_UNIT_ACCESS_UNIT_DELIMITER,   // 35
	H265_NAL_UNIT_EOS,                     // 36
	H265_NAL_UNIT_EOB,                     // 37
	H265_NAL_UNIT_FILLER_DATA,             // 38
	H265_NAL_UNIT_PREFIX_SEI,              // 39
	H265_NAL_UNIT_SUFFIX_SEI,              // 40

	H265_NAL_UNIT_RESERVED_NVCL41,
	H265_NAL_UNIT_RESERVED_NVCL42,
	H265_NAL_UNIT_RESERVED_NVCL43,
	H265_NAL_UNIT_RESERVED_NVCL44,
	H265_NAL_UNIT_RESERVED_NVCL45,
	H265_NAL_UNIT_RESERVED_NVCL46,
	H265_NAL_UNIT_RESERVED_NVCL47,
	H265_NAL_UNIT_UNSPECIFIED_48,
	H265_NAL_UNIT_UNSPECIFIED_49,
	H265_NAL_UNIT_UNSPECIFIED_50,
	H265_NAL_UNIT_UNSPECIFIED_51,
	H265_NAL_UNIT_UNSPECIFIED_52,
	H265_NAL_UNIT_UNSPECIFIED_53,
	H265_NAL_UNIT_UNSPECIFIED_54,
	H265_NAL_UNIT_UNSPECIFIED_55,
	H265_NAL_UNIT_UNSPECIFIED_56,
	H265_NAL_UNIT_UNSPECIFIED_57,
	H265_NAL_UNIT_UNSPECIFIED_58,
	H265_NAL_UNIT_UNSPECIFIED_59,
	H265_NAL_UNIT_UNSPECIFIED_60,
	H265_NAL_UNIT_UNSPECIFIED_61,
	H265_NAL_UNIT_UNSPECIFIED_62,
	H265_NAL_UNIT_UNSPECIFIED_63,
	H265_NAL_UNIT_INVALID,
} H265NalUnitType;

typedef enum {
	NALU_PRIORITY_DISPOSABLE = 0,
	NALU_PRIRITY_LOW         = 1,
	NALU_PRIORITY_HIGH       = 2,
	NALU_PRIORITY_HIGHEST    = 3
} NaluPriority;

#endif /* HEVCPARSER_H2645_PARAMTER_H_ */
