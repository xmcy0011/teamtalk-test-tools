/** @file i_event.h
  * @brief i_event说明
  * @author Captain China
  * @date 2019/01/25
  */

#ifndef _I_EVENT_F4DE1FDA_B436_47F1_8B40_E2EFF67A1DA3_H_
#define _I_EVENT_F4DE1FDA_B436_47F1_8B40_E2EFF67A1DA3_H_

#include "global_define.h"
#include <assert.h>

NAMESPACE_BEGIN(module)

struct Exception : public std::exception
{
public:
	Exception(int errorCode, const std::string& msg)
		:m_ErrorCode(errorCode)
		, m_msg(msg)
	{
	}

public:
	int				m_ErrorCode;
	std::string		m_msg;
};

/** @class IEvent
  * @brief 
  */
struct IEvent
{
public:
	virtual void process() = 0;
	virtual void onException(Exception* e)
	{
		printf("IEvent exception");
		assert(FALSE);
	}
	virtual void release() = 0;
};

/** @class ITimerEvent
  * @brief Timer的Event，目前只是当做tag
  */
struct ITimerEvent : public IEvent
{};

NAMESPACE_END(module)

#endif// _I_EVENT_F4DE1FDA_B436_47F1_8B40_E2EFF67A1DA3_H_
