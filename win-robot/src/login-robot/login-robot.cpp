// login-robot.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "leonetwork/im_core.h"
#include "yalog/yaolog.h"
#include "api/tcp_client.h"
#include <shlwapi.h>

std::wstring getAppPath()
{
	static std::wstring g_sDllPath = _T("");

	if (g_sDllPath.empty())
	{
		TCHAR	buffer[MAX_PATH];
		ZeroMemory(buffer, sizeof(TCHAR)* MAX_PATH);
		HMODULE h = GetModuleHandle(NULL);
		::GetModuleFileName(h, buffer, MAX_PATH);
		::PathRemoveFileSpec(buffer);
		g_sDllPath = buffer;
		g_sDllPath += _T("\\");
	}
	return g_sDllPath;
}
const std::string ws2s(const std::wstring& src)
{
	std::locale sys_locale("");

	const wchar_t* data_from = src.c_str();
	const wchar_t* data_from_end = src.c_str() + src.size();
	const wchar_t* data_from_next = 0;

	int wchar_size = 4;
	char* data_to = new char[(src.size() + 1) * wchar_size];
	char* data_to_end = data_to + (src.size() + 1) * wchar_size;
	char* data_to_next = 0;

	memset(data_to, 0, (src.size() + 1) * wchar_size);

	typedef std::codecvt<wchar_t, char, mbstate_t> convert_facet;
	mbstate_t out_state = 0;
	auto result = std::use_facet<convert_facet>(sys_locale).out(
		out_state, data_from, data_from_end, data_from_next,
		data_to, data_to_end, data_to_next);
	if (result == convert_facet::ok)
	{
		std::string dst = data_to;
		delete[] data_to;
		return dst;
	}
	else
	{
		printf("convert error!\n");
		delete[] data_to;
		return std::string("");
	}
}
const std::wstring s2ws(const std::string& src)
{
	std::locale sys_locale("");

	const char* data_from = src.c_str();
	const char* data_from_end = src.c_str() + src.size();
	const char* data_from_next = 0;

	wchar_t* data_to = new wchar_t[src.size() + 1];
	wchar_t* data_to_end = data_to + src.size() + 1;
	wchar_t* data_to_next = 0;

	wmemset(data_to, 0, src.size() + 1);

	typedef std::codecvt<wchar_t, char, mbstate_t> convert_facet;
	mbstate_t in_state = 0;
	auto result = std::use_facet<convert_facet>(sys_locale).in(
		in_state, data_from, data_from_end, data_from_next,
		data_to, data_to_end, data_to_next);
	if (result == convert_facet::ok)
	{
		std::wstring dst = data_to;
		delete[] data_to;
		return dst;
	}
	else
	{
		printf("convert error!\n");
		delete[] data_to;
		return std::wstring(L"");
	}
}

void _InitLog()
{
	std::string logConfig = ws2s(getAppPath()) + "ttlogconfig.ini";

	YAOLOG_INIT;
	YAOLOG_CREATE(APP, true, YaoUtil::LOG_TYPE_TEXT);
	YAOLOG_CREATE(NET, true, YaoUtil::LOG_TYPE_TEXT);
	YAOLOG_CREATE(DEBG, true, YaoUtil::LOG_TYPE_TEXT);
	YAOLOG_CREATE(ERR, true, YaoUtil::LOG_TYPE_TEXT);
	YAOLOG_CREATE(SOCK, true, YaoUtil::LOG_TYPE_FORMATTED_BIN);
	YAOLOG_SET_ATTR_FROM_CONFIG_FILE(APP, logConfig.c_str());
	YAOLOG_SET_ATTR_FROM_CONFIG_FILE(NET, logConfig.c_str());
	YAOLOG_SET_ATTR_FROM_CONFIG_FILE(DEBG, logConfig.c_str());
	YAOLOG_SET_ATTR_FROM_CONFIG_FILE(ERR, logConfig.c_str());
	YAOLOG_SET_ATTR_FROM_CONFIG_FILE(SOCK, logConfig.c_str());
}

int main()
{
	_InitLog();
	//start imcore lib
	//在这里启动任务队列和网络IO线程
	imcore::IMLibCoreRunEvent();
	
	TcpClient client;
	std::string ip = "192.168.31.44";
	std::string user_name = "xuyc";
	std::string pwd = "123456";
	IM::Login::IMLoginRes* res = client.doLogin(ip, 8000, user_name, pwd);
	wprintf(_T("login result:%d,%s"), res->result_code(), res->result_string());
	LOG__("robot", _T("login result:%d,%s"), res->result_code(), res->result_string());

	if (res->result_code() == 0){
		client.startHeartbeat();
		while (true)
		{
			Sleep(5 * 1000);
		}
	}
	return 0;
}

