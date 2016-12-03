
#include "hscom.h"

static struct _HSCOMInt HSCOMInt;

void FUNCDEF HSCOM_Init(HSComFunc Write, HSComFunc Read)
{
	HSCOMInt.Status = HSCOM_CMD_END;
	HSCOMInt.ReadRawDat = Read;
	HSCOMInt.SendRawDat = Write;
	INIT_LIST_HEAD(&HSCOMInt.recvlist);
	INIT_LIST_HEAD(&HSCOMInt.sendlist);
}

void FUNCDEF HSCOM_TaskRecvCmdHandler(HSCOMRawData* RD)
{
}

void FUNCDEF HSCOM_TaskRecvDatHandler(HSCOMRawData* RD)
{

}

void FUNCDEF HSCOM_TaskRecvEndHandler(HSCOMRawData* RD)
{
	while (!list_empty(&HSCOMInt.recvlist))
	{
		struct HSCOMRecvData* recvdat = (struct HSCOMRecvData*)list_entry(HSCOMInt.recvlist.next, struct HSCOMRecvData, list);
		list_del(HSCOMInt.recvlist.next);
		HSCOM_FREE(recvdat);
	}
}

void FUNCDEF HSCOM_TaskRecvHandler(void* rd)
{
	HSCOMRawData* RD = (HSCOMRawData*)rd;
	switch (HSCOM_CMD(RD->cmd))
	{
		case HSCOM_CMD_END:
			HSCOM_TaskRecvEndHandler(RD);
			break;
		case HSCOM_CMD_BEG:
			HSCOM_TaskRecvCmdHandler(RD);
			break;
		case HSCOM_CMD_DAT:
			HSCOM_TaskRecvDatHandler(RD);
			break;
	}
}

void FUNCDEF HSCOM_TaskSendHandler()
{
	;
}

