/*
 * parser_util.h
 *
 *  Created on: 2018年12月14日
 *      Author: root
 */

#ifndef PARSER_UTIL_H_
#define PARSER_UTIL_H_
#include <string.h>
#include <stdio.h>
#include <math.h>

#define parser_min(a, b) (((a) < (b)) ? (a) : (b))
#define parser_max(a, b) (((a) > (b)) ? (a) : (b))

//=============buffer struct==========
struct streamBuffer
{
	unsigned char * data;
	unsigned int length;
	unsigned int readIdx;
	unsigned int endIdx;
	streamBuffer(){
		data = NULL;
		length = 0;
		readIdx = 0;
		endIdx = 0;
	}
};

//=================parser function====
unsigned int static Ue(unsigned char *pBuff, unsigned int nLen, unsigned int &nStartBit);

int static Se(unsigned char *pBuff, unsigned int nLen, unsigned int &nStartBit);

unsigned int static u(unsigned int BitCount,unsigned char * buf,unsigned int &nStartBit);

unsigned int static next_bits(unsigned int BitCount,unsigned char * buf,unsigned int &nStartBit);

unsigned int static write_u(unsigned int BitCount,unsigned char * buf,unsigned int &nStartBit,unsigned int value);
//去除NAL起始码防竞争机制
void static de_emulation_prevention(unsigned char* buf,unsigned int* buf_size);

void static emulation_prevention(unsigned char* buf,unsigned int* buf_size);

static int copybits(const unsigned char* src,int sbb/*source begin byte*/,int ssb/*source skip bit*/,
      unsigned char* dest,int dbb/*dest begin byte*/,int dsb/*dest skip bit*/,int nbits);

void static dectobin( int n );

//=================NALU function=====
/*func:00 00 01 起始字节
 *成功则返回：1*/
static int FindStartCode2 (unsigned char *Buf);

/*func:00 00 00 01 起始字节
 *成功则返回：1*/
static int FindStartCode3 (unsigned char *Buf);

/*func：从src中读取n个字节到dec中，指针向后移动n个字节
 *return：返回实际读取的字节数*/
static int read_buffer(unsigned char *dec,unsigned int n,streamBuffer &src);

/*func:从src中读取1个字节并返回，指针向后移动1个字节*/
static unsigned char get_char(streamBuffer &src);

/*func:移动src指针，与fseek()功能类似*/
static int buffer_seek(unsigned int n,streamBuffer &src);


unsigned int static Ue(unsigned char *pBuff, unsigned int nLen, unsigned int &nStartBit)
{
	    //计算0bit的个数
		unsigned int nZeroNum = 0;
	    while (nStartBit < nLen * 8)
	    {
	        if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8))) //&:按位与，%取余
	        {
	            break;
	        }
	        nZeroNum++;
	        nStartBit++;
	    }
	    nStartBit ++;


	    //计算结果
	    unsigned int dwRet = 0;
	    for (unsigned int i=0; i<nZeroNum; i++)
	    {
	        dwRet <<= 1;
	        if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8)))
	        {
	            dwRet += 1;
	        }
	        nStartBit++;
	    }
	    return (1 << nZeroNum) - 1 + dwRet;
}


int static Se(unsigned char *pBuff, unsigned int nLen, unsigned int &nStartBit)
{
	    int UeVal=Ue(pBuff,nLen,nStartBit);
	    double k=UeVal;
	    int nValue=ceil(k/2);//ceil函数：ceil函数的作用是求不小于给定实数的最小整数。ceil(2)=ceil(1.2)=cei(1.5)=2.00
	    if (UeVal % 2==0)
	        nValue=-nValue;
	    return nValue;
}


unsigned int static u(unsigned int BitCount,unsigned char * buf,unsigned int &nStartBit)
{
		unsigned int dwRet = 0;
	    for (unsigned int i=0; i<BitCount; i++)
	    {
	        dwRet <<= 1;
	        if (buf[nStartBit / 8] & (0x80 >> (nStartBit % 8)))//
	        {
	            dwRet += 1;
	        }
	        nStartBit++;
	    }
	    return dwRet;
}

void static SetN1(unsigned char *pBuf, int n)
{
	(*pBuf) |= 1<<n;
}

void static SetN0(unsigned char *pBuf, int n)
{
	(*pBuf) &= ~(1<<n);
}

unsigned int static write_u(unsigned int BitCount,unsigned char * buf,unsigned int &nStartBit,unsigned int value){
	unsigned int dwRet = 0;
      //buf[nStartBit / 8] & (0x80 >> (nStartBit % 8)) = value&(0x80 >> (nStartBit % 8));
        int i = 0,key = 0;
        unsigned int offset = 1;
        while(key < BitCount-1){
        	offset <<= 1;
        	key++;
        }

	    for (unsigned int i=0; i<BitCount; i++)
	    {
	        dwRet <<= 1;
	    		int j = (value&(offset >> i));//&(0x80>>i);
	    		j = j > 0?1:0;
	    		int restBitPos = 7-(nStartBit % 8);
	    		if(1 == j){
	    			SetN1(&buf[nStartBit / 8],restBitPos);
	    		}
	    		else{
	    			SetN0(&buf[nStartBit / 8],restBitPos);
	    		}
	        nStartBit++;
	    }

    return 1;

}

void static dectobin( int n ){
	int result=0,k=1,i,temp;
	temp = n;
	while(temp){
		i = temp%2;
		result = k * i + result;
		k = k*10;
		temp = temp/2;
	}
	//printf("%d\n", result);
}

unsigned int static write_Ue(unsigned char *pBuff, unsigned int nLen, unsigned int &nStartBit,unsigned int value){
	/*无符号指数哥伦布熵编码的过程*/
//step 1: value+1;(4加1)
   int v = value+1;
   int v_copy = v;
//step 2:将4加1(为5)转换为最小的二进制序列即 101 (此是M=3)
   int i = 0;
   int M = 0;
   int result = 0,k = 1;
   while(v){
	   i = v%2;
	   result = k * i + result;
	   k = k *10;
	   v = v/2;
	   M++;
	   //printf("%d\t", i);
   }
   k = k/10;
//step 3:此二进制序列前面补充M-1个0(即两个0)
   int nZeroNum = M-1;
//step 4:先写入M-1个0；
   while(nZeroNum > 0){
  		int restBitPos = 7-(nStartBit % 8);
  		SetN0(&pBuff[nStartBit / 8],restBitPos);
      nStartBit++;
      nZeroNum--;
   }
//step 4:在写入v序列
   while(k>0){
	   i = result/k;
	   result = result%k;
	   k = k/10;
 		int restBitPos = 7-(nStartBit % 8);
	   if(i==1){
	  		SetN1(&pBuff[nStartBit / 8],restBitPos);
	   }
	   else{
		   SetN0(&pBuff[nStartBit / 8],restBitPos);
	   }
	   nStartBit++;
   }
   return 1;
}

unsigned int static next_bits(unsigned int BitCount,unsigned char * buf,unsigned int &nStartBit)
{
		unsigned int dwRet = 0;
		unsigned int startBit = nStartBit;
	   for (unsigned int i=0; i<BitCount; i++){
		   dwRet <<= 1;
			if (buf[nStartBit / 8] & (0x80 >> (nStartBit % 8))){
				dwRet += 1;
			}
			    nStartBit++;
	    }
	   /*nStartBit回退到之前的位置，比特流的指针不移动*/
	   nStartBit = startBit;
	   return dwRet;
}

	/**
	 * 去除NAL起始码防竞争机制
	 *
	 * @param buf SPS数据内容
	 *
	 * @无返回值
	 */
void static de_emulation_prevention(unsigned char* buf,unsigned int* buf_size)
{
	    int i=0,j=0;
	    unsigned char* tmp_ptr=NULL;
	    unsigned int tmp_buf_size=0;
	    int val=0;

	    tmp_ptr=buf;
	    tmp_buf_size=*buf_size;
	    for(i=0;i<(tmp_buf_size-2);i++)
	    {
	        //check for 0x000003
	        val=(tmp_ptr[i]^0x00) +(tmp_ptr[i+1]^0x00)+(tmp_ptr[i+2]^0x03);
	        if(val==0)
	        {
	            //kick out 0x03
	            for(j=i+2;j<tmp_buf_size-1;j++)
	                tmp_ptr[j]=tmp_ptr[j+1];

	            //and so we should devrease bufsize
	            (*buf_size)--;
	        }
	    }
}

void static emulation_prevention(unsigned char* buf,unsigned int* buf_size)
{
	    int i=0,j=0;
	    unsigned char* tmp_ptr=NULL;
	    unsigned int size = 0;
	    size = *buf_size;
	    unsigned char *tmp_src = new unsigned char[size];
	    memcpy(tmp_src,buf,size);
	    unsigned int tmp_buf_size=0;
	    int lastVal=1;

	    tmp_ptr=buf;
	    tmp_buf_size=*buf_size;
	    for(i=0,j=0;i<tmp_buf_size;i++,j++)
	    {
	    	if((lastVal == 0)&&(tmp_src[i] == 0)){
	    		tmp_ptr[j] = tmp_src[i];j++;
	    		//printf("%x\t",tmp_ptr[j]);
	    		tmp_ptr[j] = 0x03;
	    		//printf("%x\t",tmp_ptr[j]);
	    		(*buf_size)++;
	    		lastVal = 3;
	    	}
	    	else{
	    		tmp_ptr[j] = tmp_src[i];
	    		//printf("%x\t",tmp_ptr[j]);
	    		lastVal = tmp_ptr[j];
	    	}
	    	//printf("%x\t",tmp_ptr[j]);
	    }
}

static int FindStartCode2 (unsigned char *Buf){
	if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=1) return 0; //0x000001?
	else return 1;
}

static int FindStartCode3 (unsigned char *Buf){
	if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=0 || Buf[3] !=1) return 0;//0x00000001?
	else return 1;
}

static int read_buffer(unsigned char *dec,unsigned int n,streamBuffer &src){
	int ret = 0;
	if(src.length < n){
		ret = n-src.length;
	}
	else{
		ret = n;
	}
	memcpy(dec,src.data+src.readIdx,ret);
	src.length = src.length - ret;
	src.readIdx = src.readIdx + ret;
	return ret;
}

static unsigned char get_char(streamBuffer &src){
	unsigned char ret = *(src.data+src.readIdx);
	src.length = src.length - 1;
	src.readIdx = src.readIdx + 1;
	return ret;
}

static int buffer_seek(unsigned int n,streamBuffer &src){
	int ret = 0;
	int m = -n;
	if(src.length < m){
		ret = m-src.length;
		return 1;
	}
	else{
		src.length = src.length + m;
		src.readIdx = src.readIdx - m;
		return 0;
	}
}

//按比特位拷贝
// 从src数组首地址跳过sbb个字节，又跳过ssb个比特位，拷贝nbits个比特位的数据到
//   dest数组首地址跳过dbb个字节，又跳过dsb个比特位位置
static int copybits(const unsigned char* src,int sbb/*source begin byte*/,int ssb/*source skip bit*/,
      unsigned char* dest,int dbb/*dest begin byte*/,int dsb/*dest skip bit*/,int nbits)
{
  // assert(src && dest && sbb>=0 && ssb>=0 && dbb>=0 && dsb>=0 && nbits>=0);
  if(ssb == 0){
	  sbb = sbb -1;
	  ssb = 8;
  }
  if(dsb == 0){
	  dbb = dbb -1;
	  dsb = 8;
  }
  if(src ==NULL || dest == NULL)return -1;
  if(sbb < 0 || ssb < 0 || dbb < 0 || dsb <0)return -2;
  if(nbits==0)return 0;

  if(ssb == dsb){
    //边界对其情况
    //1拷贝对齐部分
    int copybyte=(nbits -(8-ssb))/8;
    memmove(dest+dbb+1,src+sbb+1,copybyte);
    //2拷贝前端不完整字节
    if(ssb != 0){
      unsigned char s=src[sbb];
      s &= 0xFF>>ssb;
      dest[dbb] &= 0xFF<<(8-ssb);
      dest[dbb] |= s;
    }
    //拷贝后端不完整字节
    int endbit=(nbits - (8- ssb))%8;
    if(endbit != 0){
      unsigned char s=src[sbb+1+copybyte];
      s &= 0xFF<<(8-endbit);
      dest[dbb+1 + copybyte] &= 0xFF>>endbit;
      dest[dbb+1 + copybyte] |= s;
    }
    return (8 - endbit);
  }
  //-------------------------------------------------

  int sbgb = sbb*8 + ssb;  //源开始的比特位置
  int dbgb = dbb*8 + dsb;  //目标开始比特位置
  int i,ret;
  int k1,m1,k2,m2;
  unsigned char s;
  if(((dest - src)*8+dbgb) < sbgb  ){
    // 目标开始位置在源开始位置左边
    for(i=0;i<nbits;++i){
      //拷贝某个位
      //1、源位置      目标位置
      k1=(sbgb+i)>>3;    k2=(dbgb+i)>>3;
      m1=(sbgb+i)&0x7;  m2=(dbgb+i)&0x7;
      s=src[k1];
      s &= 0x80>>m1;  //获取源比特位
      if(m1!=m2){  //偏移位
        s = m1<m2? (s>>(m2-m1)):(s<<(m1-m2));
      }
      dest[k2] &= (~(0x80>>m2));  //目标位置0
      dest[k2] |= s;  //目标位赋值
    }
  }
  else{
    for(i=nbits-1; i >=0 ;--i){
      //拷贝某个位
      //1、源位置      目标位置
      k1=(sbgb+i)>>3;    k2=(dbgb+i)>>3;
      m1=(sbgb+i)&0x7;  m2=(dbgb+i)&0x7;
      s=src[k1];
      s &= 0x80>>m1;  //获取源比特位
      if(m1!=m2){  //偏移位
        s = m1<m2? (s>>(m2-m1)):(s<<(m1-m2));
      }
      dest[k2] &= (~(0x80>>m2));  //目标位置0
      dest[k2] |= s;  //目标位赋值
    }

  }
  return (8 - (dbgb+nbits)%8);
}

#endif /* PARSER_UTIL_H_ */
