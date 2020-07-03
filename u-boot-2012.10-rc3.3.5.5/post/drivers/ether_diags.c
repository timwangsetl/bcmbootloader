/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

/*
 * Ethernet test
 *
 * The Serial Communication Controllers (SCC) listed in ctlr_list array below
 * are tested in the loopback ethernet mode.
 * The controllers are configured accordingly and several packets
 * are transmitted. The configurable test parameters are:
 *   MIN_PACKET_LENGTH - minimum size of packet to transmit
 *   MAX_PACKET_LENGTH - maximum size of packet to transmit
 *   TEST_NUM - number of tests
 */

#include <post.h>
#if CONFIG_POST & CONFIG_SYS_POST_ETHER

//#include <command.h>
//#include <net.h>
//#include <serial.h>
//#include <asm/iproc/reg_utils.h>
#include <asm/arch/ethHw_dma.h>
//#include "../../drivers/net/ethHw_data.h"
//#if defined(CONFIG_SABER2)
#include "../../drivers/net/bcmiproc_serdes.h"
#include "../../drivers/net/bcmiproc_phy5461s.h"
//#endif

#define GMAC_NUM    2
#define TEST_NUM		1
#define MIN_PACKET_LENGTH	64
#define MAX_PACKET_LENGTH	256

static uint32_t rxDescAlignedBuf = 0;
static uint32_t rxDataBuf = 0;

extern bcm_eth_t g_eth_data;
extern int ethHw_dmaInit(bcm_eth_t *eth_data);
extern int ethHw_chipAttach(bcm_eth_t *eth_data);
extern int ethHw_dmaTx(size_t len, uint8_t * tx_buf);


static void packet_fill(char *packet, int length)
{
	char c = (char) length;
	int i;

	packet[0] = 0xFF;
	packet[1] = 0xFF;
	packet[2] = 0xFF;
	packet[3] = 0xFF;
	packet[4] = 0xFF;
	packet[5] = 0xFF;

	for (i = 6; i < length; i++) {
		packet[i] = c++;
	}
}

static int packet_check(char *packet, int length)
{
	char c = (char) length;
	int i;

	for (i = 6; i < length; i++) {
		if (packet[i] != c++)
			return -1;
	}

	return 0;
}


static void gmac_core_reset(int index)
{
	uint32 tmp;
	
	if (index == 0)
	{
	    reg32_write((uint32_t*)(AMAC_IDM0_IDM_RESET_CONTROL),0);
	    tmp = reg32_read((uint32_t*)(AMAC_IDM0_IO_CONTROL_DIRECT));
	    tmp &= ~(1<<AMAC_IDM0_IO_CONTROL_DIRECT__CLK_250_SEL);
	    tmp |= (1<<AMAC_IDM0_IO_CONTROL_DIRECT__DIRECT_GMII_MODE);
	    tmp |= (1<<AMAC_IDM0_IO_CONTROL_DIRECT__DEST_SYNC_MODE_EN);
	    reg32_write((uint32_t*)(AMAC_IDM0_IO_CONTROL_DIRECT),tmp);
	} else {
	    reg32_write((uint32_t*)(AMAC_IDM1_IDM_RESET_CONTROL),0);
	    tmp = reg32_read((uint32_t*)(AMAC_IDM1_IO_CONTROL_DIRECT));
	    tmp &= ~(1<<AMAC_IDM1_IO_CONTROL_DIRECT__CLK_250_SEL);
	    tmp |= (1<<AMAC_IDM1_IO_CONTROL_DIRECT__DIRECT_GMII_MODE);
	    tmp |= (1<<AMAC_IDM1_IO_CONTROL_DIRECT__DEST_SYNC_MODE_EN);
	    reg32_write((uint32_t*)(AMAC_IDM1_IO_CONTROL_DIRECT),tmp);
  }
}


static void *dma_getnextrxp(dma_info_t *di, int *index, size_t *len, uint32_t *stat0, uint32_t *stat1)
{
	dma64dd_t *descp = NULL;
	uint i, curr, active;
	void *rxp;

	/* initialize return parameters */
	*index = 0;
	*len = 0;
	*stat0 = 0;
	*stat1 = 0;

	i = di->rxin;

	curr = B2I(((reg32_read(&di->d64rxregs->status0) & D64_RS0_CD_MASK) -
		di->rcvptrbase) & D64_RS0_CD_MASK, dma64dd_t);
	active = B2I(((reg32_read(&di->d64rxregs->status1) & D64_RS0_CD_MASK) -
		di->rcvptrbase) & D64_RS0_CD_MASK, dma64dd_t);

	/* check if any frame */
	if (i == curr)
		return (NULL);

	/* get the packet pointer that corresponds to the rx descriptor */
	rxp = (void*)RX_BUF(i);

	descp = (dma64dd_t *)RX_DESC(i);
	/* invalidate buffer */
	//gmac_cache_inval((u32)rxp, RX_BUF_LEN);
	//gmac_cache_inval((u32)descp, sizeof(dma64dd_t));

	descp->addrlow = 0xdeadbeef;
	descp->addrhigh = 0xdeadbeef;

	*index = i;
	*len = (descp->ctrl2&D64_CTRL2_BC_MASK);
	*stat0 = reg32_read(&di->d64rxregs->status0);
	*stat1 = reg32_read(&di->d64rxregs->status1);

	di->rxin = NEXTRXD(i);

	return (rxp);
}


static void dma_rxrefill(dma_info_t *di, int index)
{
	dma64dd_t *descp = NULL;
	void *bufp;

	/* get the packet pointer that corresponds to the rx descriptor */
	bufp = (void*)RX_BUF(index);
	descp = (dma64dd_t *)RX_DESC(index);

	/* update descriptor that is being added back on ring */
	descp->ctrl2 = cpu_to_le32(RX_BUF_SIZE);
	descp->addrlow = cpu_to_le32((uint32_t)bufp);
	descp->addrhigh = 0;
}


int ether_post_test (int flags)
{
	bcm_eth_t *eth_data = &g_eth_data;
	bcmgmac_t *ch = &eth_data->bcmgmac;
	dma_info_t *di = ch->di[0];
	int i, j, stat, index;
	uint8_t packet_send[MAX_PACKET_LENGTH], packet_recv[MAX_PACKET_LENGTH];
	uint32_t stat0, stat1, control, offset;
	void *bufp, *datap;
	size_t rcvlen, buflen;
	
	for (index = 0; index < GMAC_NUM; ++index)
	{
		 if (index == 0)
		    eth_data->regs = (gmacregs_t *)IPROC_GMAC0_REG_BASE;
		 else
		 	  eth_data->regs = (gmacregs_t *)IPROC_GMAC1_REG_BASE;
		 
		 /* Set loopback */
		 eth_data->loopback = true;
		 if (ethHw_dmaInit(eth_data) < 0)  return -1;
	   
	   /* reset cores */
	   gmac_core_reset(index);
	   
	   stat = ethHw_chipAttach(eth_data);
	   if (stat)  return -1;
	   	
     /* init chip  */
	   // chip_init(eth_data, ET_INIT_FULL);
	   
	   for (i = 0; i < TEST_NUM; i++) {
        for (j = MIN_PACKET_LENGTH; j <= MAX_PACKET_LENGTH; j++) {
			     post_log("Fill packet with length = %d...\n", j);
			     packet_fill((char *)packet_send, j);
			     
			     post_log("Send this packet to eth-%d...\n", index);
			     ethHw_dmaTx(j, (uint8_t *)packet_send);
			     
			     bufp = dma_getnextrxp(ch->di[0], &index, &buflen, &stat0, &stat1);
			     if (bufp)
			     {
				       // get buffer offset
			         control = reg32_read(&di->d64rxregs->control);
			         offset = (control&D64_RC_RO_MASK)>>D64_RC_RO_SHIFT;
			         rcvlen = ltoh16(*(uint16 *)bufp);
				       datap = (void*)((uint32_t)bufp+offset);
				       memcpy(packet_recv, datap, rcvlen);
				       
				       if (rcvlen != j || packet_check((char *)packet_recv, rcvlen) < 0) {
				       	   post_log("Receive a packet length = %d (expected %d) or compare FAIL !!\n", rcvlen, j);
				           return(-1);
			         } else {
				           post_log("Receive a packet (length = %d) and compare content PASS !!\n", rcvlen);
		           }
		       } else {
		           post_log("Don't receive any packets !!\n");
		           return(-1);
		       }
		       dma_rxrefill(ch->di[0], index);
		    }
	   }
  }
	return(0);
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_ETHER */
