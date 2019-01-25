/*******************************************************************************
 *  @file      versioninfo.h 2015\3\23 20:01:28 $
 *  @author    ¿ìµ¶<kuaidao@mogujie.com>
 *  @brief     
 ******************************************************************************/

#ifndef _VERSIONINFO_512063D3_0451_4836_AAAA_539A3020E202_H_
#define _VERSIONINFO_512063D3_0451_4836_AAAA_539A3020E202_H_


#define MK_STR_(v)                      #v
#define MK_STR(v)                       MK_STR_(v)
#define MK_VERSION(A, B, C, D)          A.B.C.D

#define APP_VER_MAJOR                   0             // Ö÷°æ±¾ºÅ
#define APP_VER_MINOR                   6             // ´Î°æ±¾ºÅ
#define APP_VER_REVISION                1             // ÐÞ¶©°æ±¾ºÅ
#define APP_VER_SVN                     165         // svn °æ±¾ºÅ
#define APP_VER_SVN_STR                 MK_STR(APP_VER_SVN)

#define APP_VER                         MK_VERSION(APP_VER_MAJOR, APP_VER_MINOR, APP_VER_REVISION, APP_VER_SVN)
#define APP_VER_STR	                    MK_STR(APP_VER)

/******************************************************************************/
#endif// _VERSIONINFO_512063D3_0451_4836_AAAA_539A3020E202_H_
