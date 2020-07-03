/*
 * $Copyright Open Broadcom Corporation$
 *
 * These routines provide access to the serdes
 *
 */

/* ---- Include Files ---------------------------------------------------- */
//#include <bcmutils.h>
//#include <bcmenetphy.h>
//#include <siutils.h>
#include <asm/arch/bcmrobo.h>
#include "bcmiproc_robo_serdes.h"
#include "bcmiproc_serdes_def.h"

/* ---- External Variable Declarations ----------------------------------- */
/* ---- External Function Prototypes ------------------------------------- */
/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */

/* debug/trace */
//#define BCMDBG
//#define BCMDBG_ERR
#ifdef BCMDBG
#define	NET_ERROR(args) printf args
#define	NET_TRACE(args) printf args
#elif defined(BCMDBG_ERR)
#define	NET_ERROR(args) printf args
#define NET_TRACE(args)
#else
#define	NET_ERROR(args)
#define	NET_TRACE(args)
#endif /* BCMDBG */
#define	NET_REG_TRACE(args)


#ifndef ASSERT
#define ASSERT(exp)
#endif

/* ==== Public Functions ================================================= */

uint16
robo_serdes_get_id(robo_info_t *robo, uint page, uint off)
{
	uint16 data;

	/* read the id high */
	srab_sgmii_rreg(robo, page, XGXS16G_SERDESID_SERDESID0r+off, &data);
	return data;
}


void
robo_serdes_reset(robo_info_t *robo, uint page)
{
	uint16 ctrl;

	/* set reset flag */
	srab_sgmii_rreg(robo, page, XGXS16G_IEEE0BLK_IEEECONTROL0r, &ctrl);
	ctrl |= IEEE0BLK_IEEECONTROL0_RST_HW_MASK;
	srab_sgmii_wreg(robo, page, XGXS16G_IEEE0BLK_IEEECONTROL0r, &ctrl);
	udelay(100);
	/* check if out of reset */
	srab_sgmii_rreg(robo, page, XGXS16G_IEEE0BLK_IEEECONTROL0r, &ctrl);
	if (ctrl & IEEE0BLK_IEEECONTROL0_RST_HW_MASK) {
		NET_ERROR(("%s page (0x%x) reset not complete\n", __FUNCTION__, page));
	}
}


int
robo_serdes_reset_core(robo_info_t *robo, uint page)
{
	uint16		data16;
	uint16		serdes_id2;

	/* get serdes id */
	serdes_id2 = robo_serdes_get_id(robo, page, 2);
	printf("%s page(0x%x) id2(0x%x)\n", __FUNCTION__, page, serdes_id2);

	/* unlock lane */
	srab_sgmii_rreg(robo, page, 0x833c, &data16);
	data16 &= ~(0x0040);
	srab_sgmii_wreg(robo, page, 0x833c, &data16);

	if ( page == PAGE_P5_SGMII ) {
		/* Reset the core */
		/* Stop PLL Sequencer and configure the core into correct mode */
		data16 = (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane <<
					XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT) |
				XGXSBLK0_XGXSCONTROL_HSTL_MASK |
				XGXSBLK0_XGXSCONTROL_CDET_EN_MASK |
				XGXSBLK0_XGXSCONTROL_EDEN_MASK |
				XGXSBLK0_XGXSCONTROL_AFRST_EN_MASK |
				XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK;
		srab_sgmii_wreg(robo, page, XGXS16G_XGXSBLK0_XGXSCONTROLr, &data16);

		/* Disable IEEE block select auto-detect. 
		* The driver will select desired block as necessary.
		* By default, the driver keeps the XAUI block in
		* IEEE address space.
		*/
		srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_MISCCONTROL1r, &data16);
		if (XGXS16G_2p5G_ID(serdes_id2)) {
			data16 &= ~(	XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_AUTODET_MASK |
							XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK);
		} else {
			data16 &= ~(	XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_AUTODET_MASK |
							XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK);
			data16 |= XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK;
		}
		srab_sgmii_wreg(robo, page, XGXS16G_XGXSBLK0_MISCCONTROL1r, &data16);

#if 0 //dgb
		/* disable in-band MDIO. PHY-443 */
		srab_sgmii_rreg(robo, page, 0x8111, &data16);
		/* rx_inBandMdio_rst */
		data16 |= 1 << 3;
		srab_sgmii_wreg(robo, page, 0x8111, &data16);
#endif
	}
	return 0;
}


int
robo_serdes_start_pll(robo_info_t *robo, uint page)
{
	uint16		data16;

	if ( page == PAGE_P5_SGMII ) {
		uint32		count=250;
		 /* Start PLL Sequencer and wait for PLL to lock */
		srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_XGXSCONTROLr, &data16);
		data16 |= XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK;
		srab_sgmii_wreg(robo, page, XGXS16G_XGXSBLK0_XGXSCONTROLr, &data16);

		/* wait for PLL to lock */
		while (count!=0) {
			srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_XGXSSTATUSr, &data16);
			if ( data16 & XGXSBLK0_XGXSSTATUS_TXPLL_LOCK_MASK ) {
	            break;
			}
			/* wait 1 usec then dec counter */
			udelay(10);
			count--;
		}
		if (count == 0) {
			NET_ERROR(("%s TXPLL did not lock\n", __FUNCTION__));
		}
	}
	return 0;
}


/*
 * Function:
 *      robo_serdes_init
 * Purpose:
 *      Initialize xgxs6 phys
 * Parameters:
 *      robo - robo handle
 *      page - port page
 * Returns:
 *      0
 */
int
robo_serdes_init(robo_info_t *robo, uint page)
{
	uint16		data16;
	uint16		serdes_id0, serdes_id1, serdes_id2;
	uint		locpage=page;

	if ( page == PAGE_P4_SGMII ) {
		printf("%s access page4 via page5 lane1\n", __FUNCTION__);
		locpage = PAGE_P5_SGMII;
		// try to access p4 registers via lane1
		data16 = 1;
		srab_sgmii_wreg(robo, locpage, 0xffde, &data16);
	}

	/* get serdes id */
	serdes_id0 = robo_serdes_get_id(robo, locpage, 0);
	serdes_id1 = robo_serdes_get_id(robo, locpage, 1);
	serdes_id2 = robo_serdes_get_id(robo, locpage, 2);
	printf("%s page(0x%x) id0(0x%x) id1(0x%x) id2(0x%x)\n", __FUNCTION__, locpage, serdes_id0, serdes_id1, serdes_id2);

	/* get more ids */
	srab_sgmii_rreg(robo, locpage, 2, &serdes_id0);
	srab_sgmii_rreg(robo, locpage, 3, &serdes_id1);
	//printf("%s locpage(0x%x) SERDES PhyID_MS(0x%x) PhyID_LS(0x%x)\n", __FUNCTION__, locpage, serdes_id0, serdes_id1);

	/* unlock lane */
	srab_sgmii_rreg(robo, locpage, 0x833c, &data16);
	data16 &= ~(0x0040);
	srab_sgmii_wreg(robo, locpage, 0x833c, &data16);

	/* Must Enable TX polarity flip */
	srab_sgmii_rreg(robo, locpage, XGXS16G_TX0_TX_ACONTROL0r, &data16);
	data16 |= TX0_TX_ACONTROL0_TXPOL_FLIP_MASK;
	srab_sgmii_wreg(robo, locpage, XGXS16G_TX0_TX_ACONTROL0r, &data16);

	/* disable CL73 BAM */
	srab_sgmii_rreg(robo, locpage, 0x8372, &data16);
	data16 &= ~(CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK);
	srab_sgmii_wreg(robo, locpage, 0x8372, &data16);

	/* Set Local Advertising Configuration */
	data16 = MII_ANA_C37_FD | MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE;
	srab_sgmii_wreg(robo, locpage, XGXS16G_COMBO_IEEE0_AUTONEGADVr, &data16);

	/* Disable BAM in Independent Lane mode. Over1G AN not supported  */
	data16 = 0;
	srab_sgmii_wreg(robo, locpage, XGXS16G_BAM_NEXTPAGE_MP5_NEXTPAGECTRLr, &data16);
	srab_sgmii_wreg(robo, locpage, XGXS16G_BAM_NEXTPAGE_UD_FIELDr, &data16);

	data16 = SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK |
				SERDESDIGITAL_CONTROL1000X1_DISABLE_PLL_PWRDWN_MASK |
				SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;

//	data16 |= SERDESDIGITAL_CONTROL1000X1_REMOTE_LOOPBACK_MASK;
	/*
	* Put the Serdes in SGMII mode
	* bit0 = 0; in SGMII mode
	*/
	srab_sgmii_wreg(robo, locpage, XGXS16G_SERDESDIGITAL_CONTROL1000X1r, &data16);

	/* set autoneg */
	data16 = MII_CTRL_AE | MII_CTRL_RAN;
//	data16 = MII_CTRL_SS_MSB | MII_CTRL_FD;
	srab_sgmii_wreg(robo, locpage, XGXS16G_COMBO_IEEE0_MIICNTLr, &data16);

	/* Disable 10G parallel detect */
	data16 = 0;
	srab_sgmii_wreg(robo, locpage, XGXS16G_AN73_PDET_PARDET10GCONTROLr, &data16);

	/* Disable BAM mode and Teton mode */
	srab_sgmii_wreg(robo, locpage, XGXS16G_BAM_NEXTPAGE_MP5_NEXTPAGECTRLr, &data16);

	/* Enable lanes */
	srab_sgmii_rreg(robo, locpage, XGXS16G_XGXSBLK1_LANECTRL0r, &data16);
	data16 |= XGXSBLK1_LANECTRL0_CL36_PCS_EN_RX_MASK |
				XGXSBLK1_LANECTRL0_CL36_PCS_EN_TX_MASK;
	srab_sgmii_wreg(robo, locpage, XGXS16G_XGXSBLK1_LANECTRL0r, &data16);

    /* set elasticity fifo size to 13.5k to support 12k jumbo pkt size*/
	srab_sgmii_rreg(robo, locpage, XGXS16G_SERDESDIGITAL_CONTROL1000X3r, &data16);
	data16 &= SERDESDIGITAL_CONTROL1000X3_FIFO_ELASICITY_TX_RX_MASK;
	data16 |= (1 << 2);
	srab_sgmii_wreg(robo, locpage, XGXS16G_SERDESDIGITAL_CONTROL1000X3r, &data16);

    /* Enabble LPI passthru' for native mode EEE */
	srab_sgmii_rreg(robo, locpage, XGXS16G_REMOTEPHY_MISC5r, &data16);
	data16 |= 0xc000;
	srab_sgmii_wreg(robo, locpage, XGXS16G_REMOTEPHY_MISC5r, &data16);
	srab_sgmii_rreg(robo, locpage, XGXS16G_XGXSBLK7_EEECONTROLr, &data16);
	data16 |= 0x0007;
	srab_sgmii_wreg(robo, locpage, XGXS16G_XGXSBLK7_EEECONTROLr, &data16);

	if ( page == PAGE_P4_SGMII ) {
		printf("%s set back to lane0\n", __FUNCTION__);
		// set back to lane 0
		data16 = 0;
		srab_sgmii_wreg(robo, locpage, 0xffde, &data16);
	}

	return 0;
}


/*
 * Function:
 *      robo_serdes_enable_set
 * Purpose:
 *      Enable/Disable phy
 * Parameters:
 *      robo - robo handle
 *      page - port page
 *      enable - on/off state to set
 * Returns:
 *      0
 */
int
robo_serdes_enable_set(robo_info_t *robo, uint page, int enable)
{
	uint16	data16, mask16;

	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK1_LANECTRL3r, &data16);
	if (page == PAGE_P5_SGMII)
		/* lane 0 */
		mask16 = 1;		/* rx lane */
	else
		/* lane 1 */
		mask16 = 2;		/* rx lane */
	mask16 |= (mask16 << 4);	/* add tx lane */
	mask16 |= 0x800;
	if (enable) {
		data16 &= ~(mask16);
	} else {
		data16 &= ~(mask16);
		data16 |= mask16;
	}
	srab_sgmii_wreg(robo, page, XGXS16G_XGXSBLK1_LANECTRL3r, &data16);

	return 0;
}


/*
 * Function:
 *      robo_serdes_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      eth_num - ethernet data
 *      phyaddr - physical address
 *      speed - link speed in Mbps
 * Returns:
 *      0
 */
int
robo_serdes_speed_set(robo_info_t *robo, uint page, int speed)
{
	uint16		speed_val, mask;
	uint16		data16;
	uint16		speed_mii;

	if (speed > 1000) {
		return -1;
	}

	speed_val = 0;
	speed_mii = 0;
	mask      = SERDESDIGITAL_MISC1_FORCE_SPEED_SEL_MASK |
				SERDESDIGITAL_MISC1_FORCE_SPEED_MASK;

	switch (speed) {
	case 0:
		/* Do not change speed */
		return 0;
	case 10:
		speed_mii = MII_CTRL_SS_10;
		break;
	case 100:
		speed_mii = MII_CTRL_SS_100;
		break;
	case 1000:
		speed_mii = MII_CTRL_SS_1000;
		break;
	default:
		return -1;
	}

	/* Hold rxSeqStart */
	srab_sgmii_rreg(robo, page, XGXS16G_RX0_RX_CONTROLr, &data16);
	data16 |= DSC_2_0_DSC_CTRL0_RXSEQSTART_MASK;
	srab_sgmii_wreg(robo, page, XGXS16G_RX0_RX_CONTROLr, &data16);

	/* hold TX FIFO in reset */
	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_CONTROL1000X3r, &data16);
	data16 |= SERDESDIGITAL_CONTROL1000X3_TX_FIFO_RST_MASK;
	srab_sgmii_wreg(robo, page, XGXS16G_SERDESDIGITAL_CONTROL1000X3r, &data16);

	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_MISC1r, &data16);
	data16 &= ~(mask);
	data16 |= speed_val;
	srab_sgmii_wreg(robo, page, XGXS16G_SERDESDIGITAL_MISC1r, &data16);

	srab_sgmii_rreg(robo, page, XGXS16G_COMBO_IEEE0_MIICNTLr, &data16);
	data16 &= ~(MII_CTRL_SS_LSB | MII_CTRL_SS_MSB);
	data16 |= speed_mii;
	srab_sgmii_wreg(robo, page, XGXS16G_COMBO_IEEE0_MIICNTLr, &data16);

	/* release rxSeqStart */
	srab_sgmii_rreg(robo, page, XGXS16G_RX0_RX_CONTROLr, &data16);
	data16 &= ~(DSC_2_0_DSC_CTRL0_RXSEQSTART_MASK);
	srab_sgmii_wreg(robo, page, XGXS16G_RX0_RX_CONTROLr, &data16);
                                                                               
	/* release TX FIFO reset */
	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_CONTROL1000X3r, &data16);
	data16 &= ~(SERDESDIGITAL_CONTROL1000X3_TX_FIFO_RST_MASK);
	srab_sgmii_wreg(robo, page, XGXS16G_SERDESDIGITAL_CONTROL1000X3r, &data16);

	return 0;
}


/*
 * Function:
 *      robo_serdes_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      robo - robo handle
 *      page - port page
 *      speed - current link speed in Mbps
 * Returns:
 *      0
 */
int
robo_serdes_speed_get(robo_info_t *robo, uint page, int *speed)
{
	uint16	data16;

	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_STATUS1000X1r, &data16);

	data16 &= SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_MASK;
	data16 >>= SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_SHIFT;

	if (data16 == 3) {
		*speed= 2500;
	} else if (data16 == 2) {
		*speed= 1000;
	} else if (data16 == 1) {
		*speed= 100;
	} else {
		*speed= 10;
	}

	return 0;
}


#ifdef BCMINTERNAL
/*
 * Function:
 *      robo_serdes_lb_set
 * Purpose:
 *      Put XGXS6/FusionCore in PHY loopback
 * Parameters:
 *      robo - robo handle
 *      page - port page
 *      enable - binary value for on/off (1/0)
 * Returns:
 *      0
 */
int
robo_serdes_lb_set(robo_info_t *robo, uint gpage, int enable)
{
	uint16      misc_ctrl, data16;
	uint16      lb_bit;
	uint16      lb_mask;


	uint		page=gpage;

	if ( page == PAGE_P4_SGMII ) {
		printf("%s access page4 via page5 lane1\n", __FUNCTION__);
		page = PAGE_P5_SGMII;
		// try to access p4 registers via lane1
		data16 = 1;
		srab_sgmii_wreg(robo, page, 0xffde, &data16);
	}


	/* Configure Loopback in XAUI */
	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_MISCCONTROL1r, &misc_ctrl);
	if (misc_ctrl & XGXSBLK0_MISCCONTROL1_PCS_DEV_EN_OVERRIDE_MASK) {
		/* PCS */
		lb_bit  = (enable) ? IEEE0BLK_IEEECONTROL0_GLOOPBACK_MASK : 0;
		lb_mask = IEEE0BLK_IEEECONTROL0_GLOOPBACK_MASK;
	} else if (misc_ctrl & XGXSBLK0_MISCCONTROL1_PMD_DEV_EN_OVERRIDE_MASK) {
		/* PMA/PMD */
		lb_bit  = (enable) ? 1 : 0;
		lb_mask = 1;
	} else {
		/* PHY XS, DTE XS */
		lb_bit  = (enable) ? IEEE0BLK_IEEECONTROL0_GLOOPBACK_MASK : 0;
		lb_mask = IEEE0BLK_IEEECONTROL0_GLOOPBACK_MASK;
	}

	srab_sgmii_rreg(robo, page, XGXS16G_IEEE0BLK_IEEECONTROL0r, &data16);
	data16 &= ~(lb_mask);
	data16 |= lb_bit;
	srab_sgmii_wreg(robo, page, XGXS16G_IEEE0BLK_IEEECONTROL0r, &data16);

	/* Configure Loopback in SerDes */
	lb_bit  = (enable) ? MII_CTRL_LE : 0;
	lb_mask = MII_CTRL_LE;
	srab_sgmii_rreg(robo, page, XGXS16G_COMBO_IEEE0_MIICNTLr, &data16);
	data16 &= ~(lb_mask);
	data16 |= lb_bit;
	srab_sgmii_wreg(robo, page, XGXS16G_COMBO_IEEE0_MIICNTLr, &data16);

	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_XGXSCONTROLr, &data16);
//	data16 |= 0x10;
	data16 |= 0x50;
	srab_sgmii_wreg(robo, page, XGXS16G_XGXSBLK0_XGXSCONTROLr, &data16);

	srab_sgmii_rreg(robo, page, 0x8017, &data16);
	data16 = 0xff0f;
	srab_sgmii_wreg(robo, page, 0x8017, &data16);

    return 0;
}

void
robo_serdes_disp_status(robo_info_t *robo, uint page)
{
	uint16		tmp0, tmp1, tmp2, tmp3;

	printf("%s: page:%d\n", __FUNCTION__, page);

	tmp0 = robo_serdes_get_id(robo, page, 0);
	tmp1 = robo_serdes_get_id(robo, page, 1);
	tmp2 = robo_serdes_get_id(robo, page, 2);
	printf("  id0(0x%x) id1(0x%x) id2(0x%x)\n", tmp0, tmp1, tmp2);

	srab_sgmii_rreg(robo, page, XGXS16G_IEEE0BLK_IEEECONTROL0r, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_IEEE0BLK_IEEECONTROL0r+1, &tmp1);
	printf("  MII-Control(0): 0x%x; MII-Status(1): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, 2, &tmp0);
	srab_sgmii_rreg(robo, page, 3, &tmp1);
	printf("  Phy ChipID(2:3): 0x%04x:0x%04x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, 4, &tmp0);
	srab_sgmii_rreg(robo, page, 5, &tmp1);
	srab_sgmii_rreg(robo, page, 0xf, &tmp2);
	printf("  AN AD(4): 0x%x; AN LNK PARTNER(5): 0x%x; EXT STAT(f): 0x%x\n", tmp0, tmp1, tmp2);

	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_XGXSCONTROLr, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_XGXSSTATUSr, &tmp1);
	printf("  XGXS-Control(8000): 0x%x; XGXS-Status(8001): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_MMDSELECTr, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_MISCCONTROL1r, &tmp1);
	printf("  XGXS BLK0 MMD Select(800d): 0x%x; XGXS BLK0 MISC CTRL(800e): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK1_LANECTRL0r, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK1_LANECTRL3r, &tmp1);
	printf("  XGXS BLK1 LNCTRL0(8015): 0x%x; XGXS BLK1_LNCTRL3(8018): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_RX0_RX_CONTROLr, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_AN73_PDET_PARDET10GCONTROLr, &tmp1);
	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK7_EEECONTROLr, &tmp2);
	printf("  XGXS RX0 CTRL(80b1): 0x%x; XGXS AN73 PARDET CTRL(8131): 0x%x; XGXS BLK7 EEECTRL(8150): 0x%x\n", tmp0, tmp1, tmp2);

	srab_sgmii_rreg(robo, page, 0x8111, &tmp0);
	srab_sgmii_rreg(robo, page, 0x8372, &tmp1);
	printf("  (8111): 0x%x; (8372): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_CONTROL1000X1r, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_CONTROL1000X2r, &tmp1);
	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_CONTROL1000X3r, &tmp2);
	printf("  XGXS SERDES DIG CTRL 1000X1(8300): 0x%x; XGXS SERDES DIG CTRL 1000X2(8301): 0x%x; XGXS SERDES DIGITAL CTRL 1000X3r(8302): 0x%x\n", tmp0, tmp1, tmp2);

	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_STATUS1000X1r, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_MISC1r, &tmp1);
	printf("  XGXS SERDES DIG STATUS 1000X1(8304): 0x%x; XGXS SERDES DIG MISC1(8308): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_SERDESID_SERDESID0r, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_SERDESID_SERDESID1r, &tmp1);
	printf("  XGXS SERDESID0(8310): 0x%x; XGXS SERDESID1(8311): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_SERDESID_SERDESID2r, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_SERDESID_SERDESID3r, &tmp1);
	printf("  XGXS SERDESID0(8312): 0x%x; XGXS SERDESID1(8313): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_REMOTEPHY_MISC3r, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_REMOTEPHY_MISC5r, &tmp1);
	printf("  XGXS REMOTEPHY MISC3(833c): 0x%x; XGXS REMOTEPHY MISC5(833e): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_BAM_NEXTPAGE_MP5_NEXTPAGECTRLr, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_BAM_NEXTPAGE_UD_FIELDr, &tmp1);
	printf("  XGXS BAM MP5_NEXTPAGECTRL(8350): 0x%x; XGXS BAM NP UD FIELDr(8357): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_COMBO_IEEE0_MIICNTLr, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_COMBO_IEEE0_AUTONEGADVr, &tmp1);
	printf("  XGXS COMBO IEEE0 MIICNTL(ffe0): 0x%x; XGXS COMBO IEEE0 AUTONEGADVr(ffe4): 0x%x\n", tmp0, tmp1);

   srab_sgmii_rreg(robo, page, 0x8050, &tmp0);
   srab_sgmii_rreg(robo, page, 0x8122, &tmp1);
	printf("  (8050): 0x%x; (8122): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, 0x80b0, &tmp0);
	srab_sgmii_rreg(robo, page, 0x80c0, &tmp1);
	srab_sgmii_rreg(robo, page, 0x80d0, &tmp2);
	srab_sgmii_rreg(robo, page, 0x80e0, &tmp3);
	printf("  (80b0): 0x%x; (80c0): 0x%x; (80d0): 0x%x, (80e0): 0x%x\n", tmp0, tmp1, tmp2, tmp3);

	srab_sgmii_rreg(robo, page, 0xffe1, &tmp0);
	printf("  (ffe1): 0x%x\n", tmp0);

}
#endif /* BCMINTERNAL */

