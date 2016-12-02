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

#include "driver/spi_interface.h"
#include "eagle_soc.h"
#include "osapi.h"
#include "user_interface.h"


// Show the spi registers.
#define SHOWSPIREG(i) __ShowRegValue(__func__, __LINE__);

/**
 * @brief Print debug information.
 *
 */
void __ShowRegValue(const char * func, uint32_t line)
{

    int i;
    uint32_t regAddr = 0x60000140; // SPI--0x60000240, HSPI--0x60000140;
    os_printf("\r\n FUNC[%s],line[%d]\r\n", func, line);
    os_printf(" SPI_ADDR      [0x%08x]\r\n", READ_PERI_REG(SPI_ADDR(SpiNum_HSPI)));
    os_printf(" SPI_CMD       [0x%08x]\r\n", READ_PERI_REG(SPI_CMD(SpiNum_HSPI)));
    os_printf(" SPI_CTRL      [0x%08x]\r\n", READ_PERI_REG(SPI_CTRL(SpiNum_HSPI)));
    os_printf(" SPI_CTRL2     [0x%08x]\r\n", READ_PERI_REG(SPI_CTRL2(SpiNum_HSPI)));
    os_printf(" SPI_CLOCK     [0x%08x]\r\n", READ_PERI_REG(SPI_CLOCK(SpiNum_HSPI)));
    os_printf(" SPI_RD_STATUS [0x%08x]\r\n", READ_PERI_REG(SPI_RD_STATUS(SpiNum_HSPI)));
    os_printf(" SPI_WR_STATUS [0x%08x]\r\n", READ_PERI_REG(SPI_WR_STATUS(SpiNum_HSPI)));
    os_printf(" SPI_USER      [0x%08x]\r\n", READ_PERI_REG(SPI_USER(SpiNum_HSPI)));
    os_printf(" SPI_USER1     [0x%08x]\r\n", READ_PERI_REG(SPI_USER1(SpiNum_HSPI)));
    os_printf(" SPI_USER2     [0x%08x]\r\n", READ_PERI_REG(SPI_USER2(SpiNum_HSPI)));
    os_printf(" SPI_PIN       [0x%08x]\r\n", READ_PERI_REG(SPI_PIN(SpiNum_HSPI)));
    os_printf(" SPI_SLAVE     [0x%08x]\r\n", READ_PERI_REG(SPI_SLAVE(SpiNum_HSPI)));
    os_printf(" SPI_SLAVE1    [0x%08x]\r\n", READ_PERI_REG(SPI_SLAVE1(SpiNum_HSPI)));
    os_printf(" SPI_SLAVE2    [0x%08x]\r\n", READ_PERI_REG(SPI_SLAVE2(SpiNum_HSPI)));

    for (i = 0; i < 16; ++i) {
        os_printf(" ADDR[0x%08x],Value[0x%08x]\r\n", regAddr, READ_PERI_REG(regAddr));
        regAddr += 4;
    }

}

#define SPI_DATA_MAX 10

static uint8 spi_data[SPI_DATA_MAX][32] = {0};
static uint8 spi_idx = 0;


// SPI interrupt callback function.
void spi_slave_isr_sta(void *para)
{
    uint32 regvalue;
    uint32 statusW, statusR, counter;
    if (READ_PERI_REG(0x3ff00020)&BIT4)
	{
        //following 3 lines is to clear isr signal
        CLEAR_PERI_REG_MASK(SPI_SLAVE(SpiNum_SPI), 0x3ff);
    } 
	else if (READ_PERI_REG(0x3ff00020)&BIT7)
    { //bit7 is for hspi isr,
        regvalue = READ_PERI_REG(SPI_SLAVE(SpiNum_HSPI));
        os_printf("spi_slave_isr_sta SPI_SLAVE[0x%08x]\n\r", regvalue);
        SPIIntClear(SpiNum_HSPI);
        SET_PERI_REG_MASK(SPI_SLAVE(SpiNum_HSPI), SPI_SYNC_RESET);
        SPIIntClear(SpiNum_HSPI);
        SPIIntEnable(SpiNum_HSPI, SpiIntSrc_WrStaDone
                 | SpiIntSrc_RdStaDone 
                 | SpiIntSrc_WrBufDone 
                 | SpiIntSrc_RdBufDone);

        if (regvalue & SPI_SLV_WR_BUF_DONE) 
		{
			uint8 i, idx=spi_idx;
            // User can get data from the W0~W7
			//os_printf("spi_slave_isr_sta : SPI_SLV_WR_BUF_DONE\n\r");
			GPIO_OUTPUT_SET(0, 0); // GPIO0 set 0
			for(i=0; i<8; i++)
			{
				uint32 recv_data=READ_PERI_REG(SPI_W0(SpiNum_HSPI)+(i<<2));
				spi_data[idx][i<<2] = recv_data&0xff;
				spi_data[idx][(i<<2)+1] = (recv_data>>8)&0xff;
				spi_data[idx][(i<<2)+2] = (recv_data>>16)&0xff;
				spi_data[idx][(i<<2)+3] = (recv_data>>24)&0xff;
			}
			system_os_post(USER_TASK_PRIO_1, (os_signal_t)SPI_SIG_MOSI, (os_param_t)spi_data[idx]);
			if (++spi_idx >= SPI_DATA_MAX)
				spi_idx=0;

			GPIO_OUTPUT_SET(0, 1); //GPIO0 set 1
        } 
		else if (regvalue & SPI_SLV_RD_BUF_DONE)
        {
            // TO DO 
            GPIO_OUTPUT_SET(2, 0); //GPIO2 set 0
            os_printf("spi_slave_isr_sta : SPI_SLV_RD_BUF_DONE\n\r");
        }
        if (regvalue & SPI_SLV_RD_STA_DONE)
		{
            statusR = READ_PERI_REG(SPI_RD_STATUS(SpiNum_HSPI));
            statusW = READ_PERI_REG(SPI_WR_STATUS(SpiNum_HSPI));
            os_printf("spi_slave_isr_sta : SPI_SLV_RD_STA_DONE[R=0x%08x,W=0x%08x]\n\r", statusR, statusW);
        }

        if (regvalue & SPI_SLV_WR_STA_DONE)
		{
            statusR = READ_PERI_REG(SPI_RD_STATUS(SpiNum_HSPI));
            statusW = READ_PERI_REG(SPI_WR_STATUS(SpiNum_HSPI));
            os_printf("spi_slave_isr_sta : SPI_SLV_WR_STA_DONE[R=0x%08x,W=0x%08x]\n\r", statusR, statusW);

			//GPIO_OUTPUT_SET(0, 0);
			//add system_os_post here
			//system_os_post(USER_TASK_PRIO_1, (os_signal_t)SPI_SIG_MOSI, (os_param_t)spi_data);
			//GPIO_OUTPUT_SET(0, 1);
        }
        if ((regvalue & SPI_TRANS_DONE) && ((regvalue & 0xf) == 0))
		{
            os_printf("spi_slave_isr_sta : SPI_TRANS_DONE\n\r");
        }
        //SHOWSPIREG(SpiNum_HSPI);
    }
}

static os_timer_t spi_timer_test;

void ICACHE_FLASH_ATTR spi_slave_test_send_data()
{
	static uint16 cnt=0;

    uint32_t sndData[8] = { 0 };
    sndData[0] = 0x35343332;
    sndData[1] = 0x39383736;
    sndData[2] = 0x3d3c3b3a;
    sndData[3] = 0x11103f3e;
    sndData[4] = 0x15141312;
    sndData[5] = 0x19181716;
    sndData[6] = 0x1d1c1b1a;
    sndData[7] = 0x21201f1e;

    GPIO_OUTPUT_SET(2, 1); //GPIO2 set 1
    SPISlaveSendData(SpiNum_HSPI, sndData, 8);

    WRITE_PERI_REG(SPI_RD_STATUS(SpiNum_HSPI), 0x8A);
    WRITE_PERI_REG(SPI_WR_STATUS(SpiNum_HSPI), 0x83);

	os_timer_disarm(&spi_timer_test);
    os_timer_setfn(&spi_timer_test, (os_timer_func_t *)spi_slave_test_send_data, NULL);//wjl
    os_timer_arm(&spi_timer_test,5000,1);
	os_printf("spi_slave_test_send_data %d\n", cnt++);
}

// Test spi slave interfaces.
void ICACHE_FLASH_ATTR _spi_slave_init()
{
	spi_idx = 0;
	os_memset(spi_data, 0, sizeof(spi_data));
    //
    SpiAttr hSpiAttr;
    hSpiAttr.bitOrder = SpiBitOrder_MSBFirst;
    hSpiAttr.speed = 0;
    hSpiAttr.mode = SpiMode_Slave;
    hSpiAttr.subMode = SpiSubMode_3;

    // Init HSPI GPIO
    WRITE_PERI_REG(PERIPHS_IO_MUX, 0x105);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2);//configure io to spi mode
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2);//configure io to spi mode
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2);//configure io to spi mode
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2);//configure io to spi mode

    os_printf("\r\n ============= spi init slave =============\r\n");
    SPIInit(SpiNum_HSPI, &hSpiAttr);
    
    // Set spi interrupt information.
    SpiIntInfo spiInt;
    spiInt.src = (SpiIntSrc_TransDone 
        |SpiIntSrc_WrStaDone 
        |SpiIntSrc_RdStaDone 
        |SpiIntSrc_WrBufDone 
        |SpiIntSrc_RdBufDone);
    spiInt.isrFunc = spi_slave_isr_sta;
    SPIIntCfg(SpiNum_HSPI, &spiInt);
   // SHOWSPIREG(SpiNum_HSPI);
    
    SPISlaveRecvData(SpiNum_HSPI);
#if 0
    uint32_t sndData[8] = { 0 };
    sndData[0] = 0x35343332;
    sndData[1] = 0x39383736;
    sndData[2] = 0x3d3c3b3a;
    sndData[3] = 0x11103f3e;
    sndData[4] = 0x15141312;
    sndData[5] = 0x19181716;
    sndData[6] = 0x1d1c1b1a;
    sndData[7] = 0x21201f1e;

    SPISlaveSendData(SpiNum_HSPI, sndData, 8);
#else
	//spi_slave_test_send_data();
#endif
    WRITE_PERI_REG(SPI_RD_STATUS(SpiNum_HSPI), 0x8A);
    WRITE_PERI_REG(SPI_WR_STATUS(SpiNum_HSPI), 0x83);
}

void spi_interface_init(void)
{
    // Test spi interfaces.
    os_printf("\r\n =======================================================\r\n");
    os_printf("\t ESP8266 %s application \n\r", __func__);
    os_printf("\t\t SDK version:%s    \n\r", system_get_sdk_version());
    os_printf("\t\t Complie time:%s  \n\r", __TIME__);
    os_printf("\r\n =======================================================\r\n");

    _spi_slave_init();
}

