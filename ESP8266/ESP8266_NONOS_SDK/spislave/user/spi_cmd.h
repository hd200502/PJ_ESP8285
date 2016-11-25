
#ifndef _SPI_CMD_H_
#define _SPI_CMD_H_

#include "mem.h"
#include "osapi.h"
#include "c_types.h"
#include "user_interface.h"

typedef struct
{
	uint8  cmd;
	uint8  par;
	uint16 epar;
}spi_cmd_hdr_struct_t;

typedef struct
{
	spi_cmd_hdr_struct_t cmd;
	uint8 data[28];
}spi_cmd_hdr_packet_t;

typedef struct
{
	uint8 idx;
	uint8 data[31];
}spi_dat_hdr_packet_t;

#define SPI_CMD_SIGN_TYPE     0x80
#define SPI_RSP_SIGN_TYPE     0x40
#define SPI_CMD_WIFI_TYPE     0x20
#define SPI_CMD_SOCK_TYPE     0x10

#define SPI_WIFI_CMD_TYPE (SPI_CMD_SIGN_TYPE|SPI_CMD_WIFI_TYPE)
#define SPI_WIFI_RSP_TYPE (SPI_RSP_SIGN_TYPE|SPI_CMD_WIFI_TYPE)

#define SPI_SOCK_CMD_TYPE (SPI_CMD_SIGN_TYPE|SPI_CMD_SOCK_TYPE)
#define SPI_SOCK_RSP_TYPE (SPI_RSP_SIGN_TYPE|SPI_CMD_SOCK_TYPE)


/****************** wifi ******************/

#define SPI_CMD_MODE_INIT_CMD  (SPI_WIFI_CMD_TYPE|0x00)
#define SPI_CMD_MODE_INIT_RSP  (SPI_WIFI_RSP_TYPE|0x00)

typedef struct 
{
	spi_cmd_hdr_struct_t cmd;
	uint8 temp[28];
}wifi_init_cmd_struct_t;

typedef struct
{
	spi_cmd_hdr_struct_t cmd;
	uint8 temp[28];
}wifi_init_rsp_struct_t;

#define SPI_CMD_WIFI_SCAN_CMD  (SPI_WIFI_CMD_TYPE|0x01)
#define SPI_CMD_WIFI_SCAN_RSP  (SPI_WIFI_RSP_TYPE|0x01)

typedef struct
{
	spi_cmd_hdr_struct_t cmd;
	uint8 temp[28];
}wifi_scan_cmd_struct_t;

typedef struct
{
	int8  rssi;
	uint8 mac[6];
	uint8 bssid[32];
}wifi_scan_res_struct_t;

typedef struct
{
	spi_cmd_hdr_struct_t    cmd;
	wifi_scan_res_struct_t* res;
}wifi_scan_rsp_struct_t;

#define SPI_CMD_WIFI_CONN_CMD (SPI_WIFI_CMD_TYPE|0x02)
#define SPI_CMD_WIFI_CONN_RSP (SPI_WIFI_RSP_TYPE|0x02)

#define SPI_CMD_WIFI_CONN_CMD_SSID 0
#define SPI_CMD_WIFI_CONN_CMD_PASS 1
#define SPI_CMD_WIFI_CONN_CMD_CONN 2


typedef struct
{
	spi_cmd_hdr_struct_t cmd;
	uint8 data[28];
}wifi_conn_cmd_struct_t;

typedef struct
{
	spi_cmd_hdr_struct_t cmd;
	uint32 status;
	uint8  temp[24];
}wifi_conn_rsp_struct_t;


/****************** socket ******************/

#define SPI_CMD_SOCK_CONN_CMD (SPI_SOCK_CMD_TYPE|0x00)
#define SPI_CMD_SOCK_CONN_RSP (SPI_SOCK_RSP_TYPE|0x00)

typedef struct
{
	spi_cmd_hdr_struct_t cmd;
	uint16 port;
	uint32 addr;
	uint8  temp[22];
}sock_conn_cmd_struct_t;

typedef struct
{
	spi_cmd_hdr_struct_t cmd;
	uint32 status;
	uint8  temp[24];
}sock_conn_rsp_struct_t;

#define SPI_CMD_SOCK_GETHOST_CMD (SPI_SOCK_CMD_TYPE|0x01)
#define SPI_CMD_SOCK_GETHOST_RSP (SPI_SOCK_RSP_TYPE|0x01)

typedef struct
{
	spi_cmd_hdr_struct_t cmd;
	uint8 data[28];
}sock_gethost_cmd_struct_t;

typedef struct
{
	spi_cmd_hdr_struct_t cmd;
	uint32 addr;
	uint8  temp[24];
}sock_gethost_rsp_struct_t;

#define SPI_CMD_SOCK_WRITE_CMD (SPI_SOCK_CMD_TYPE|0x02)
#define SPI_CMD_SOCK_WRITE_RSP (SPI_SOCK_RSP_TYPE|0x02)

typedef struct
{
	spi_cmd_hdr_struct_t cmd;
	uint8 data[28];
}sock_write_cmd_struct_t;

typedef struct
{
	spi_cmd_hdr_struct_t cmd;
	uint32 status;
	uint8  temp[24];
}sock_write_rsp_struct_t;

#define SPI_CMD_SOCK_READ_CMD (SPI_SOCK_CMD_TYPE|0x03)
#define SPI_CMD_SOCK_READ_RSP (SPI_SOCK_RSP_TYPE|0x03)

typedef struct
{
	spi_cmd_hdr_struct_t cmd;
	uint8 temp[28];
}sock_read_cmd_struct_t;

typedef struct
{
	spi_cmd_hdr_struct_t cmd;;
	uint8 data[28];
}sock_read_rsp_struct_t;

#define SPI_CMD_SOCK_CLOSE_CMD (SPI_SOCK_CMD_TYPE|0x04)
#define SPI_CMD_SOCK_CLOSE_RSP (SPI_SOCK_RSP_TYPE|0x04)

typedef struct
{
	spi_cmd_hdr_struct_t cmd;
	uint8 temp[28];
}sock_close_cmd_struct_t;

typedef struct
{
	spi_cmd_hdr_struct_t cmd;;
	uint8 temp[28];
}sock_close_rsp_struct_t;

#define SPI_CMD_FILL(a, c, p, l) {(a)->cmd=c;(a)->par=p;(a)->epar=l;}

#endif  // _SPI_CMD_H_

