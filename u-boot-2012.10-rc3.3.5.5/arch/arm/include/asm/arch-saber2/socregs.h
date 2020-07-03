/*
 * $Copyright Open Broadcom Corporation$
 */
 
#ifndef __SOCREGS_H
#define __SOCREGS_H

#define ICFG_CHIP_ID_REG                                    0x18000000
#define ICFG_CHIP_REVISION_ID                               0x18000004
#define CMIC_DEV_REV_ID                                     0x03210224

#define DMU_PCU_IPROC_STRAPS_CAPTURED                       0x1800f028
#define DMU_PCU_IPROC_STRAPS_CAPTURED__strap_boot_dev_R     9
#define DMU_PCU_IPROC_STRAPS_CAPTURED__strap_nand_type_R    5
#define DMU_PCU_IPROC_STRAPS_CAPTURED__strap_nand_page_R    3

#define NAND_IDM_IDM_IO_CONTROL_DIRECT                      0xf8105408
#define NAND_nand_flash_INTFC_STATUS                        0x18046014
#define NAND_ro_ctlr_ready                                  0x18046f10
#define NAND_nand_flash_CMD_ADDRESS                         0x1804600c
#define NAND_nand_flash_CMD_EXT_ADDRESS                     0x18046008
#define NAND_nand_flash_INIT_STATUS                         0x18046144
#define NAND_nand_flash_FLASH_DEVICE_ID                     0x18046194
#define NAND_nand_flash_ONFI_STATUS                         0x18046148
#define NAND_nand_flash_CONFIG_CS1                          0x18046064
#define NAND_nand_flash_CONFIG_CS0                          0x18046054
#define NAND_nand_flash_ACC_CONTROL_CS1                     0x18046060
#define NAND_nand_flash_ACC_CONTROL_CS0                     0x18046050
#define NAND_nand_flash_FLASH_CACHE0                        0x18046400
#define NAND_nand_flash_ECC_UNC_ADDR                        0x18046118
#define NAND_nand_flash_CS_NAND_SELECT                      0x18046018
#define NAND_nand_flash_SPARE_AREA_READ_OFS_0               0x18046200
#define NAND_nand_flash_SPARE_AREA_WRITE_OFS_0              0x18046280
#define NAND_nand_flash_FLASH_DEVICE_ID_EXT                 0x18046198
#define NAND_nand_flash_CMD_START                           0x18046004
#define NAND_nand_flash_UNCORR_ERROR_COUNT                  0x180460fc
#define NAND_IDM_IDM_RESET_CONTROL                          0xf8105800

#define QSPI_bspi_registers_REVISION_ID                     0x18047000
#define CRU_control                                         0x1800e000

#define ICFG_CHIP_ID_REG	                                0x18000000

#define ICFG_IPROC_IOPAD_SW_OVERRIDE_CTRL                   0x18000c8c
#define ICFG_IPROC_IOPAD_SW_OVERRIDE_CTRL__iproc_pnor_sel            1
#define ICFG_IPROC_IOPAD_SW_OVERRIDE_CTRL__iproc_pnor_sel_sw_ovwr    0
#define ICFG_PNOR_STRAPS                                    0x18000a5c
#define ICFG_PNOR_STRAPS__PNOR_SRAM_MW_R                    0
#define PNOR_set_opmode                                     0x18045018
#define PNOR_set_opmode__set_mw_R                           0
#define PNOR_direct_cmd                                     0x18045010
#define PNOR_direct_cmd__cmd_type_R                         21

#define AMAC_IDM0_IO_CONTROL_DIRECT	                        0x18110408
#define AMAC_IDM0_IO_CONTROL_DIRECT__CLK_250_SEL            6
#define AMAC_IDM0_IO_CONTROL_DIRECT__DIRECT_GMII_MODE       5
#define AMAC_IDM0_IO_CONTROL_DIRECT__DEST_SYNC_MODE_EN      3
#define AMAC_IDM1_IO_CONTROL_DIRECT	                        0x1811f408
#define AMAC_IDM1_IO_CONTROL_DIRECT__CLK_250_SEL            6
#define AMAC_IDM1_IO_CONTROL_DIRECT__DIRECT_GMII_MODE       5
#define AMAC_IDM1_IO_CONTROL_DIRECT__DEST_SYNC_MODE_EN      3

#define AMAC_IDM0_IDM_RESET_CONTROL							0x18110800
#define AMAC_IDM1_IDM_RESET_CONTROL							0x1811f800

#define ChipcommonG_MII_Management_Control	                0x18002000
#define ChipcommonG_MII_Management_Command_Data             0x18002004
#define ChipcommonB_MII_Management_Control					ChipcommonG_MII_Management_Control
#define ChipcommonB_MII_Management_Command_Data				ChipcommonG_MII_Management_Command_Data
#define ChipcommonB_MII_Management_Control__BSY				8
#define ChipcommonB_MII_Management_Control__PRE				7
#define ChipcommonB_MII_Management_Control__EXT				9
#define ChipcommonB_MII_Management_Command_Data__SB_R		30
#define ChipcommonB_MII_Management_Command_Data__PA_R		23
#define ChipcommonB_MII_Management_Command_Data__RA_R		18
#define ChipcommonB_MII_Management_Command_Data__TA_R		16
#define ChipcommonB_MII_Management_Command_Data__OP_R		28

#define IPROC_WRAP_MISC_CONTROL                             0x1800fc40
#define IPROC_WRAP_MISC_CONTROL__IPROC_MDIO_SEL	            1

#define DMU_CRU_RESET                                       0x1800f200

#define DDR_S0_IDM_RESET_CONTROL                            0xf8101800
#define DDR_S1_IDM_IO_CONTROL_DIRECT                        0xf8102408
#define DDR_S1_IDM_IO_STATUS                                0xf8102500
#define DDR_S1_IDM_IO_STATUS__o_phy_ready                   2
#define DDR_S1_IDM_IO_STATUS__o_phy_pwrup_rsb               3
#define DDR_S1_IDM_RESET_CONTROL                            0xf8102800
#define DDR_S2_IDM_RESET_CONTROL                            0xf8103800
#define DDR_S2_IDM_IO_CONTROL_DIRECT                        0xf8103408
#define DDR_S2_IDM_IO_CONTROL_DIRECT__mode_32b              1
#define ROM_S0_IDM_IO_STATUS                                0xf8104500

#define DDR_BistConfig                                      0x18010c00
#define DDR_BistConfig__axi_port_sel                        1
#define DDR_BistGeneralConfigurations                       0x18010c08
#define DDR_BistConfigurations                              0x18010c0c
#define DDR_BistConfigurations__WriteWeight_R               0
#define DDR_BistConfigurations__ReadWeight_R                8
#define DDR_BistConfigurations__IndWrRdAddrMode             19
#define DDR_BistConfigurations__ConsAddr8Banks              21
#define DDR_BistConfigurations__BistEn                      25
#define DDR_BistNumberOfActions                             0x18010c10
#define DDR_BistStartAddress                                0x18010c14
#define DDR_BistEndAddress                                  0x18010c18
#define DDR_BistEndAddress__BistEndAddress_WIDTH            26
#define DDR_BistPatternWord7                                0x18010c20
#define DDR_BistPatternWord6                                0x18010c24
#define DDR_BistPatternWord5                                0x18010c28
#define DDR_BistPatternWord4                                0x18010c2c
#define DDR_BistPatternWord3                                0x18010c30
#define DDR_BistPatternWord2                                0x18010c34
#define DDR_BistPatternWord1                                0x18010c38
#define DDR_BistPatternWord0                                0x18010c3c
#define DDR_BistStatuses                                    0x18010c60
#define DDR_BistStatuses__BistFinished                      0
#define DDR_BistErrorOccurred                               0x18010c6c

#define DDR_DENALI_CTL_00                                   0x18010000
#define DDR_DENALI_CTL_146                                  0x18010248
#define DDR_DENALI_CTL_146__ECC_EN                          0
#define DDR_DENALI_CTL_148                                  0x18010250
#define DDR_DENALI_CTL_148__ECC_DISABLE_W_UC_ERR            0
#define DDR_DENALI_CTL_175                                  0x180102bc
#define DDR_DENALI_CTL_177                                  0x180102c4
#define DDR_DENALI_CTL_177_ECC_MASK                         0x78

#define CRU_ddrphy_pwr_ctrl                                 0x1800e024
#define CRU_ddrphy_pwr_ctrl__i_hw_reset_n                   5
#define CRU_ddrphy_pwr_ctrl__i_pwrokin_phy                  4
#define CRU_ddrphy_pwr_ctrl__i_pwronin_phy                  3
#define CRU_ddrphy_pwr_ctrl__i_iso_phy_dfi                  2
#define CRU_ddrphy_pwr_ctrl__i_iso_phy_regs                 1
#define CRU_ddrphy_pwr_ctrl__i_iso_phy_pll                  0

#define DDR_PHY_CONTROL_REGS_DRIVE_PAD_CTL                  0x18011038
#define DDR_PHY_CONTROL_REGS_STATIC_PAD_CTL                 0x18011040
#define DDR_PHY_CONTROL_REGS_DRAM_CONFIG	                  0x18011044
#define DDR_PHY_CONTROL_REGS_VDL_CALIB_STATUS1              0x1801105c
#define DDR_PHY_CONTROL_REGS_VDL_CONTROL_PAR                0x180110e0
#define DDR_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0           0x1801147c
#define DDR_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1           0x18011480
#define DDR_PHY_BYTE_LANE_0_RD_EN_DLY_CYC                   0x1801148c
#define DDR_PHY_BYTE_LANE_0_READ_CONTROL	                  0x18011494
#define DDR_PHY_BYTE_LANE_0_DRIVE_PAD_CTL                   0x180114b0
#define DDR_PHY_BYTE_LANE_0_DQSP_DRIVE_PAD_CTL              0x180114b4
#define DDR_PHY_BYTE_LANE_0_DQSN_DRIVE_PAD_CTL              0x180114b8
#define DDR_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL             0x180114c0
#define DDR_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE                0x180114c8
#define DDR_PHY_CONTROL_REGS_VDL_CALIBRATE	                0x18011058
#define DDR_PHY_CONTROL_REGS_VDL_CONTROL_CKE                0x180110ec
#define DDR_PHY_CONTROL_REGS_STANDBY_CONTROL	              0x180111d0
#define DDR_PHY_CONTROL_REGS_DFI_CNTRL                      0x180111dc
#define DDR_PHY_CONTROL_REGS_VREF_DAC_CONTROL               0x1801119c
#define DDR_PHY_CONTROL_REGS_REVISION                       0x18011000
#define DDR_PHY_CONTROL_REGS_PLL_DIVIDERS                   0x18011018
#define DDR_PHY_CONTROL_REGS_PLL_DIVIDERS__MDIV_R           20
#define DDR_PHY_CONTROL_REGS_PLL_DIVIDERS__MDIV_WIDTH       8
#define DDR_PHY_CONTROL_REGS_PLL_DIVIDERS__PDIV_R           12
#define DDR_PHY_CONTROL_REGS_PLL_DIVIDERS__PDIV_WIDTH       4
#define DDR_PHY_CONTROL_REGS_PLL_DIVIDERS__NDIV_INT_R       0
#define DDR_PHY_CONTROL_REGS_PLL_DIVIDERS__NDIV_INT_WIDTH   10

#define ICFG_ROM_STRAPS                                     0x18000aa0
#define IPROC_WRAP_IPROC_PLL_CTRL_0                         0x1800fc50
#define IPROC_WRAP_IPROC_PLL_CTRL_0__FAST_LOCK              27
#define IPROC_WRAP_IPROC_PLL_CTRL_1                         0x1800fc54
#define IPROC_WRAP_IPROC_PLL_CTRL_1__PDIV_R                 27
#define IPROC_WRAP_IPROC_PLL_CTRL_1__PDIV_WIDTH             4
#define IPROC_WRAP_IPROC_PLL_CTRL_1__NDIV_FRAC_R            7
#define IPROC_WRAP_IPROC_PLL_CTRL_1__NDIV_FRAC_WIDTH	    20
#define IPROC_WRAP_IPROC_PLL_CTRL_2                         0x1800fc58
#define IPROC_WRAP_IPROC_PLL_CTRL_3                         0x1800fc5c
#define IPROC_WRAP_IPROC_PLL_CTRL_3__NDIV_INT_R             20
#define IPROC_WRAP_IPROC_PLL_CTRL_3__NDIV_INT_WIDTH         10
#define IPROC_WRAP_IPROC_PLL_CTRL_4                         0x1800fc60
#define IPROC_WRAP_IPROC_PLL_CTRL_5                         0x1800fc64
#define IPROC_WRAP_IPROC_PLL_CTRL_5__CH1_MDIV_R	            8
#define IPROC_WRAP_IPROC_PLL_CTRL_5__CH1_MDIV_WIDTH	        8

#define IHOST_PROC_CLK_WR_ACCESS                            0x19000000
#define IHOST_PROC_CLK_POLICY_FREQ                          0x19000008
#define IHOST_PROC_CLK_POLICY_FREQ__policy0_freq_R          0
#define IHOST_PROC_CLK_POLICY_FREQ__policy1_freq_R          8
#define IHOST_PROC_CLK_POLICY_FREQ__policy2_freq_R          16
#define IHOST_PROC_CLK_POLICY_FREQ__policy3_freq_R          24
#define IHOST_PROC_CLK_POLICY_FREQ__priv_access_mode        31
#define IHOST_PROC_CLK_POLICY_CTL                           0x1900000c
#define IHOST_PROC_CLK_POLICY_CTL__GO                       0
#define IHOST_PROC_CLK_POLICY_CTL__GO_AC                    1
#define IHOST_PROC_CLK_PLLARMA                              0x19000c00
#define IHOST_PROC_CLK_PLLARMA__pllarm_soft_resetb          0
#define IHOST_PROC_CLK_PLLARMA__pllarm_soft_post_resetb     1
#define IHOST_PROC_CLK_PLLARMA__pllarm_ndiv_int_R           8
#define IHOST_PROC_CLK_PLLARMA__pllarm_pdiv_R               24
#define IHOST_PROC_CLK_PLLARMA__pllarm_lock                 28
#define IHOST_PROC_CLK_PLLARMB                              0x19000c04
#define IHOST_PROC_CLK_PLLARMB__pllarm_ndiv_frac_R          0
#define IHOST_PROC_CLK_PLLARMB__pllarm_ndiv_frac_WIDTH      20
#define IHOST_PROC_CLK_PLLARMC                              0x19000c08
#define IHOST_PROC_CLK_PLLARMCTRL5                          0x19000c20
#define IHOST_PROC_CLK_CORE0_CLKGATE                        0x19000200
#define IHOST_PROC_CLK_CORE1_CLKGATE                        0x19000204
#define IHOST_PROC_CLK_ARM_SWITCH_CLKGATE                   0x19000210
#define IHOST_PROC_CLK_ARM_PERIPH_CLKGATE                   0x19000300
#define IHOST_PROC_CLK_APB0_CLKGATE                         0x19000400

#define IHOST_SCU_INVALIDATE_ALL 0x1902000c
#define IHOST_SCU_CONTROL 0x19020000
#define IHOST_L2C_CACHE_ID 0x19022000

/* SRAB */
#define ChipcommonG_SRAB_CMDSTAT                            0x1800702c
#define ChipcommonG_SRAB_WDH                                0x18007030
#define ChipcommonG_SRAB_WDL                                0x18007034
#define ChipcommonG_SRAB_RDH                                0x18007038
#define ChipcommonG_SRAB_RDL                                0x1800703c
#define ChipcommonG_SRAB_SW_IF                              0x18007040
 
#endif /* __SOCREGS_H */
