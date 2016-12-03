/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
//#include "ets_sys.h"
#include "osapi.h"
#include "spi_main.h"
#include "user_interface.h"
//#include "os_type.h"
#include "mem.h"
#include "driver/spi_interface.h"

#include "spi_cmd.h"

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void ICACHE_FLASH_ATTR user_rf_pre_init(void)
{
}


void ICACHE_FLASH_ATTR disp_spi_data(uint32 data)
{
    uint8 i = 0;
    for(i=0;i<32;i++){
        os_printf("%02X ",((uint8 *)data)[i]);
    }
	os_printf("\r\n");
}

typedef struct
{
	uint8 cmd;
	union
	{
		uint8 data;
	}data;
}SpiCmdDataPart_t;


extern void spi_parse_data(uint32 d);

typedef struct
{
	uint8* rxbuf;
	uint16 rxlen;
	uint8* txbuf;
	uint16 txlen;
}SpiPortInfo_t;

static SpiPortInfo_t SpiInfo;

void ICACHE_FLASH_ATTR spi_task(os_event_t *e)
{
    uint8 data;
	os_printf("%s\n", __FUNCTION__);
    switch(e->sig)
	{
    case SPI_SIG_MOSI:
        disp_spi_data(e->par);
		spi_parse_data(e->par);
        break;
	case SPI_SIG_MISO:
		break;
	case SPI_SIG_STATUS_R_IN_WR :
		os_printf("SR ERR in WRPR,Reg:%08x \n",e->par);
		break;
	case SPI_SIG_STATUS_W:
		os_printf("SW ERR,Reg:%08x\n",e->par);
		break;	
	case SPI_SIG_TR_DONE_ALONE:
		os_printf("TD ALO ERR,Reg:%08x\n",e->par);
		break;	
	case SPI_SIG_WR_RD:
		os_printf("WR&RD ERR,Reg:%08x\n",e->par);
		//set_miso_data();
		break;	
	case SPI_SIG_DATA_ERROR:
		os_printf("Data ERR,Reg:%08x\n",e->par);
		break;
	case SPI_SIG_STATUS_R_IN_RD :
		os_printf("SR ERR in RDPR,Reg:%08x\n",e->par);
		break;
	case SPI_SIG_TEST_CMD:
		spi_parse_data(e->par);
		os_free((void*)e->par);
		break;
    default:
        break;
    }
}

static os_timer_t spi_task_timer;

#define SPI_QUEUE_LEN 8
static os_event_t * spiQueue;

void ICACHE_FLASH_ATTR spi_task_init(void)
{
    spiQueue = (os_event_t*)os_malloc(sizeof(os_event_t)*SPI_QUEUE_LEN);
    system_os_task(spi_task,USER_TASK_PRIO_1,spiQueue,SPI_QUEUE_LEN);
}

void ICACHE_FLASH_ATTR app_test_spi_cmd_wifi_conn(void* arg)
{
	wifi_conn_cmd_struct_t* cmd;

	cmd = (wifi_conn_cmd_struct_t*)os_malloc(sizeof(*cmd));
	if (!cmd)
		return;
	os_memset(cmd, 0, sizeof(*cmd));
	cmd->cmd.cmd = SPI_CMD_WIFI_CONN_CMD;
	cmd->cmd.par = SPI_CMD_WIFI_CONN_CMD_SSID;
	os_strncpy(cmd->data, "Lianyun", sizeof(cmd->data)-1);
	cmd->cmd.epar= os_strlen(cmd->data);
	system_os_post(USER_TASK_PRIO_1, (os_signal_t)SPI_SIG_TEST_CMD, (os_param_t)cmd);

	cmd = (wifi_conn_cmd_struct_t*)os_malloc(sizeof(*cmd));
	if (!cmd)
		return;
	os_memset(cmd, 0, sizeof(*cmd));
	cmd->cmd.cmd = SPI_CMD_WIFI_CONN_CMD;
	cmd->cmd.par = SPI_CMD_WIFI_CONN_CMD_PASS;
	os_strncpy(cmd->data, "poiuytrewq", sizeof(cmd->data)-1);
	cmd->cmd.epar= os_strlen(cmd->data);
	system_os_post(USER_TASK_PRIO_1, (os_signal_t)SPI_SIG_TEST_CMD, (os_param_t)cmd);

	cmd = (wifi_conn_cmd_struct_t*)os_malloc(sizeof(*cmd));
	if (!cmd)
		return;
	os_memset(cmd, 0, sizeof(*cmd));
	cmd->cmd.cmd = SPI_CMD_WIFI_CONN_CMD;
	cmd->cmd.par = SPI_CMD_WIFI_CONN_CMD_CONN;
	system_os_post(USER_TASK_PRIO_1, (os_signal_t)SPI_SIG_TEST_CMD, (os_param_t)cmd);
}

void ICACHE_FLASH_ATTR app_test_spi_cmd_wifi_scan(void* arg)
{
	wifi_scan_cmd_struct_t* cmd;

	cmd = (wifi_scan_cmd_struct_t*)os_malloc(sizeof(*cmd));
	if (!cmd)
		return;
	os_memset(cmd, 0, sizeof(*cmd));
	cmd->cmd.cmd = SPI_CMD_WIFI_SCAN_CMD;
	system_os_post(USER_TASK_PRIO_1, (os_signal_t)SPI_SIG_TEST_CMD, (os_param_t)cmd);

	START_TIMER(spi_task_timer, app_test_spi_cmd_wifi_conn, 5000);
}

void ICACHE_FLASH_ATTR app_test_spi_cmd_mode_init(void* arg)
{
	wifi_init_cmd_struct_t* cmd;
	cmd = (wifi_init_cmd_struct_t*)os_malloc(sizeof(*cmd));
	if (!cmd)
		return;
	os_memset(cmd, 0, sizeof(*cmd));
	cmd->cmd.cmd = SPI_CMD_MODE_INIT_CMD;
	system_os_post(USER_TASK_PRIO_1, (os_signal_t)SPI_SIG_TEST_CMD, (os_param_t)cmd);

	START_TIMER(spi_task_timer, app_test_spi_cmd_wifi_scan, 3000);
}

void ICACHE_FLASH_ATTR user_init(void)
{
	UART_SetBaudrate(0, 74880);

	wifi_set_opmode_current(STATION_MODE);

	os_memset(&SpiInfo, 0, sizeof(SpiInfo));

    //os_printf("compile time:%s %s %s",__DATE__,__TIME__, __FUNCTION__);
    spi_interface_init();

	os_printf("spi task init \n\r");
	spi_task_init();

	//START_TIMER(spi_task_timer, app_test_spi_cmd_mode_init, 3000);
}

