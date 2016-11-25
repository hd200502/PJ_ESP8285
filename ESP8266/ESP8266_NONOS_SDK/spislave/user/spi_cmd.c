
#include "spi_cmd.h"




static void ICACHE_FLASH_ATTR wifi_scan_done(void *arg, STATUS status)
{
    int ret;

    if (status == OK)
	{
        struct bss_info *bss_link = (struct bss_info *)arg;
        while (bss_link != NULL)
		{
		    //if (bss_link->simple_pair)
			{
	            os_printf("%02x:%02x:%02x:%02x:%02x:%02x, %4d, %s\r\n", 
					bss_link->bssid[0], bss_link->bssid[1], bss_link->bssid[2],
					bss_link->bssid[3], bss_link->bssid[4], bss_link->bssid[5],
					bss_link->rssi,
					bss_link->ssid);
				//simple_pair_set_peer_ref(bss_link->bssid, tmpkey, NULL);
				//ret = simple_pair_sta_start_negotiate();
				//if (ret)
				//	os_printf("Simple Pair: STA start NEG Failed\n");
				//else
				//	os_printf("Simple Pair: STA start NEG OK\n");
				//break;
	    	}
            bss_link = bss_link->next.stqe_next;
        }
    }
	else
	{
        os_printf("err, scan status %d\n", status);
    }
}


void ICACHE_FLASH_ATTR spi_cmd_mode_init(wifi_init_cmd_struct_t* cmd)
{
	os_printf("%s\n", __FUNCTION__);
}

void ICACHE_FLASH_ATTR spi_cmd_wifi_scan(wifi_scan_cmd_struct_t* cmd)
{
	os_printf("%s\n", __FUNCTION__);
	wifi_station_scan(NULL, wifi_scan_done);
}


static struct station_config SpiStationConf;
static ETSTimer SpiWifiLinkTimer;
static uint8_t wifiStatus = STATION_IDLE;

static void ICACHE_FLASH_ATTR spi_wifi_check_ip(void *arg)
{
	struct ip_info ipConfig;

	os_printf("%s\n", __FUNCTION__);

	wifi_get_ip_info(STATION_IF, &ipConfig);

	os_printf("ip %d.%d.%d.%d\n",  
		(ipConfig.ip.addr>>24)&0xff,
		(ipConfig.ip.addr>>16)&0xff,
		(ipConfig.ip.addr>>8)&0xff,
		(ipConfig.ip.addr)&0xff);


	os_printf("gw %d.%d.%d.%d\n",  
		(ipConfig.gw.addr>>24)&0xff,
		(ipConfig.gw.addr>>16)&0xff,
		(ipConfig.gw.addr>>8)&0xff,
		(ipConfig.gw.addr)&0xff);


	os_printf("netmask %d.%d.%d.%d\n",  
		(ipConfig.netmask.addr>>24)&0xff,
		(ipConfig.netmask.addr>>16)&0xff,
		(ipConfig.netmask.addr>>8)&0xff,
		(ipConfig.netmask.addr)&0xff);



	wifiStatus = wifi_station_get_connect_status();
	if (wifiStatus == STATION_GOT_IP && ipConfig.ip.addr != 0)
	{
		START_TIMER(SpiWifiLinkTimer, spi_wifi_check_ip, 2000);
	}
	else
	{
		if(wifi_station_get_connect_status() == STATION_WRONG_PASSWORD)
		{

			os_printf("STATION_WRONG_PASSWORD\r\n");
			wifi_station_connect();


		}
		else if(wifi_station_get_connect_status() == STATION_NO_AP_FOUND)
		{

			os_printf("STATION_NO_AP_FOUND\r\n");
			wifi_station_connect();


		}
		else if(wifi_station_get_connect_status() == STATION_CONNECT_FAIL)
		{

			os_printf("STATION_CONNECT_FAIL\r\n");
			wifi_station_connect();

		}
		else
		{
			os_printf("STATION_IDLE\r\n");
		}

		START_TIMER(SpiWifiLinkTimer, spi_wifi_check_ip, 2000);
	}
}

void ICACHE_FLASH_ATTR spi_cmd_wifi_conn(wifi_conn_cmd_struct_t* cmd)
{
	os_printf("%s\n", __FUNCTION__);

	switch(cmd->cmd.par)
	{
		case SPI_CMD_WIFI_CONN_CMD_SSID:
			os_sprintf(SpiStationConf.ssid, "%s", cmd->data);
			break;
		case SPI_CMD_WIFI_CONN_CMD_PASS:
			os_sprintf(SpiStationConf.password, "%s", cmd->data);
			break;
		case SPI_CMD_WIFI_CONN_CMD_CONN:
			SpiStationConf.bssid_set = 0;
			wifi_station_set_config_current(&SpiStationConf);
			wifi_station_connect();
			START_TIMER(SpiWifiLinkTimer, spi_wifi_check_ip, 2000);
			break;
		default:
			break;
	}
}

void ICACHE_FLASH_ATTR spi_cmd_sock_conn(sock_conn_cmd_struct_t* cmd)
{
	os_printf("%s\n", __FUNCTION__);

}

void ICACHE_FLASH_ATTR spi_cmd_sock_gethost(sock_gethost_cmd_struct_t* cmd)
{
	os_printf("%s\n", __FUNCTION__);

}

void ICACHE_FLASH_ATTR spi_cmd_sock_write(sock_write_cmd_struct_t* cmd)
{
	os_printf("%s\n", __FUNCTION__);

}

void ICACHE_FLASH_ATTR spi_cmd_sock_read(sock_read_cmd_struct_t* cmd)
{
	os_printf("%s\n", __FUNCTION__);

}

void ICACHE_FLASH_ATTR spi_cmd_sock_close(sock_close_cmd_struct_t* cmd)
{
	os_printf("%s\n", __FUNCTION__);

}

void ICACHE_FLASH_ATTR spi_parse_data(uint32 d)
{
	uint8* data = (uint8*)d;

	switch (data[0])
	{
		case SPI_CMD_MODE_INIT_CMD:
			spi_cmd_mode_init((wifi_init_cmd_struct_t*)d);
			break;

		case SPI_CMD_WIFI_SCAN_CMD: //scan
			spi_cmd_wifi_scan((wifi_scan_cmd_struct_t*)d);
			break;

		case SPI_CMD_WIFI_CONN_CMD:
			spi_cmd_wifi_conn((wifi_conn_cmd_struct_t*)d);
			break;

		case SPI_CMD_SOCK_CONN_CMD:
			spi_cmd_sock_conn((sock_conn_cmd_struct_t*)d);
			break;

		case SPI_CMD_SOCK_GETHOST_CMD:
			spi_cmd_sock_gethost((sock_gethost_cmd_struct_t*)d);
			break;

		case SPI_CMD_SOCK_WRITE_CMD:
			spi_cmd_sock_write((sock_write_cmd_struct_t*)d);
			break;

		case SPI_CMD_SOCK_READ_CMD:
			spi_cmd_sock_read((sock_read_cmd_struct_t*)d);
			break;

		case SPI_CMD_SOCK_CLOSE_CMD:
			spi_cmd_sock_close((sock_close_cmd_struct_t*)d);
			break;

		default:
			break;
	}
}



