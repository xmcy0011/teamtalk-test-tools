/*******************************************************************************
 *  @file      GlobalConfig.h 2014\7\16 11:43:48 $
 *  @author    快刀<kuaidao@mogujie.com>
 *  @summary   
 ******************************************************************************/

#ifndef _GLOBAL_CONFIG_5D2BB312_93D5_4439_B91E_132C84AE1B51_H_
#define _GLOBAL_CONFIG_5D2BB312_93D5_4439_B91E_132C84AE1B51_H_

#define UTIL_LOG_APPFILE			_T("applog.log")
#define UTIL_MULTILIGNUAL			_T("chinese.ini")

#define APP		"app"
#define NET		"net"
#define DEBG	"debug"
#define ERR		"error"
#define SOCK	"socket"

/* 主窗口类名，多个项目（module, speexdec)需要用到该窗口类名 */
#ifdef _DEBUG
#define MAIN_DIALOG_WND_CLASS_NAME					_T("MainDialog_debug")
#else
#define MAIN_DIALOG_WND_CLASS_NAME					_T("MainDialog")
#endif

#define LOGIN_DLG_WND_CLASS_NAME                    _T("wnd_login")

// speexdec 解码用到的窗口
#define	AUDIO_SPEEXDEC_WINDOW_CLASS_NAME			_T("audio-speexdecWindow")

/* 自定义windows 消息 */
#define WM_START_HUIXIN_NEW_INSTANCE             WM_USER + 101


#endif// _GLOBAL_CONFIG_5D2BB312_93D5_4439_B91E_132C84AE1B51_H_
