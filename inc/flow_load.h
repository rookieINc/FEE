
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : flow_load.h
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2015-12-24 14:09
 * Description : flow_load.h
 * History     : 
 ***********************************************************/


#ifndef _FLOW_LOAD_H_INCLUDED_
#define _FLOW_LOAD_H_INCLUDED_

#include "kdk.h"
#include "flow_parse.h"

#define HEAD            "HEAD"
#define CONTENT         "CONTENT"
#define FLOW_HEAD       "flow_head"
#define FLOW_COUNT      "flow_count"
#define FLOW_COUNT_LEN  "flow_count_len"

kdk_uint32    
flow_config_file_to_flow_coll(kdk_char32 *flow_config_file, flow_coll_t *flow_coll);

#endif /* _FLOW_LOAD_H_INCLUDED_ */
