
#ifndef _HSCOM_H_
#define _HSCOM_H_

#include "osapi.h"
#include "mem.h"
#include "os_type.h"

#define INLINE inline
#include "list.h"

#define FUNCDEF ICACHE_FLASH_ATTR

#define HSCOM_MALLOC os_malloc
#define HSCOM_FREE   os_free

#define HSCOM_CMD_END 0x00
#define HSCOM_CMD_BEG 0x01
#define HSCOM_CMD_DAT 0x02

#define HSCOM_DAT_CMD 0x00
#define HSCOM_DAT_SOC 0x01

#define HSCOM_MAX_RAW_LEN 32
#define HSCOM_MAX_DAT_LEN 255


typedef struct
{
	uint8 cmd;
	union
	{
		struct
		{
			uint8 len;
			uint8 data[HSCOM_MAX_RAW_LEN-2];
		}header;
		uint8 data[HSCOM_MAX_RAW_LEN-1];
	}RawData;
}HSCOMRawData;

typedef uint32 (*HSComFunc)(HSCOMRawData* RD);

struct HSCOMRecvData
{
	struct list_head list;
	uint8 data[HSCOM_MAX_RAW_LEN];
};

struct _HSCOMInt
{
	uint8     Status;
	struct list_head recvlist;
	struct list_head sendlist;
	HSComFunc SendRawDat;
	HSComFunc ReadRawDat;
};


#define HSCOM_CMD(cmd) (((cmd)&0xF0) >> 4)
#define HSCOM_IDX(cmd) ((cmd)&0x0F)

#endif

