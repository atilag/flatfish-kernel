/*
************************************************************************************************************************
*                                                         eGON
*                                         the Embedded GO-ON Resumeloader System
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : Resume_head.c
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2007.11.06
*
* Description : This file defines the file head part of Resume, which contains some important
*             infomations such as magic, platform infomation and so on, and MUST be allocted in the
*             head of Resume.
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang       2007.11.06      1.1.0        build the file
*
************************************************************************************************************************
*/


typedef signed char		  __s8;
typedef unsigned char		  __u8;
typedef short int		  __s16;
typedef unsigned short		  __u16;
typedef int			  __s32;
typedef unsigned int		  __u32;


#define RESUMEX_MAGIC             "eGON.BT0"
#define STAMP_VALUE               0x5F0A6C39
#define RESUME_FILE_HEAD_VERSION  "1100"     // X.X.XX
#define RESUME_VERSION            "1100"     // X.X.XX
#define PLATFORM                  "1633"

#define NF_ALIGN_SIZE			1024
#define RESUMEX_ALIGN_SIZE  		NF_ALIGN_SIZE
#define RESUME_PUB_HEAD_VERSION         "1100"    // X.X.XX
#define RESUMEX_FILE_HEAD_VERSION       "1230"    // X.X.XX
#define RESUMEX_VERSION                 "1230"    // X.X.XX
#define EGON_VERSION                    "1100"    // X.X.XX

/******************************************************************************/
/*                              file head of Resume                             */
/******************************************************************************/
typedef struct _Resume_file_head
{
	__u32  jump_instruction;   // one intruction jumping to real code
	__u8   magic[8];           // ="eGON.BT0" or "eGON.BT1",  not C-style string.
	__u32  check_sum;          // generated by PC
	__u32  length;             // generated by PC
	__u32  pub_head_size;      // the size of resume_file_head_t
	__u8   pub_head_vsn[4];    // the version of resume_file_head_t
	__u8   file_head_vsn[4];   // the version of resume0_file_head_t or resume1_file_head_t
	__u8   Resume_vsn[4];      // Resume version
	__u8   eGON_vsn[4];        // eGON version
	__u8   platform[8];        // platform information
}resume_file_head_t;


#pragma  arm section  rodata="head"

const resume_file_head_t  resume_head =
{
		/* jump_instruction */
		( 0xEA000000 | ( ( ( sizeof( resume_file_head_t ) + sizeof( int ) - 1 ) / sizeof( int ) - 2 ) & 0x00FFFFFF ) ),
		//0xe3a0f000,
		//0xe3a0f452,
		RESUMEX_MAGIC,
		STAMP_VALUE,
		RESUMEX_ALIGN_SIZE,
		sizeof( resume_file_head_t ),
		RESUME_PUB_HEAD_VERSION,
		RESUMEX_FILE_HEAD_VERSION,
		RESUMEX_VERSION,
		EGON_VERSION,
};





/*******************************************************************************
*
*                  关于Resume_file_head中的jump_instruction字段
*
*  jump_instruction字段存放的是一条跳转指令：( B  BACK_OF_Resume_file_head )，此跳
*转指令被执行后，程序将跳转到Resume_file_head后面第一条指令。
*
*  ARM指令中的B指令编码如下：
*          +--------+---------+------------------------------+
*          | 31--28 | 27--24  |            23--0             |
*          +--------+---------+------------------------------+
*          |  cond  | 1 0 1 0 |        signed_immed_24       |
*          +--------+---------+------------------------------+
*  《ARM Architecture Reference Manual》对于此指令有如下解释：
*  Syntax :
*  B{<cond>}  <target_address>
*    <cond>    Is the condition under which the instruction is executed. If the
*              <cond> is ommitted, the AL(always,its code is 0b1110 )is used.
*    <target_address>
*              Specified the address to branch to. The branch target address is
*              calculated by:
*              1.  Sign-extending the 24-bit signed(wro's complement)immediate
*                  to 32 bits.
*              2.  Shifting the result left two bits.
*              3.  Adding to the contents of the PC, which contains the address
*                  of the branch instruction plus 8.
*
*  由此可知，此指令编码的最高8位为：0b11101010，低24位根据Resume_file_head的大小动
*态生成，所以指令的组装过程如下：
*  ( sizeof( brom_file_head_t ) + sizeof( int ) - 1 ) / sizeof( int )     求出文件
*                                              头占用的“字”的个数
*  - 2                                         减去PC预取的指令条数
*  & 0x00FFFFFF                                求出signed-immed-24
*  | 0xEA000000                                组装成B指令
*
*******************************************************************************/
