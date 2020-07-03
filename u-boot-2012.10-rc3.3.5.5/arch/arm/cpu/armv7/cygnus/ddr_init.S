
 #include "asm/arch/socregs.h"
.global ddr_init
ddr_init:

           LDR     r1,=CRMU_DDR_PHY_AON_CTRL
           LDR     r2,[r1]
           ORR     r2,r2,#0x10
           STR     r2,[r1]
           LDR     r3,=0x0
           LDR     r4,=0x500
delay1:
           ADD     r3 , r3 ,#0x1  
           CMP     r3,r4
           BNE     delay1
           
           LDR     r2,[r1]
           ORR     r2,r2,#0x20
           STR     r2,[r1]
           LDR     r3,=0x0
           LDR     r4,=0x800
delay2:
           ADD     r3 , r3 ,#0x1  
           CMP     r3,r4
           BNE     delay2
           
           LDR     r2,[r1]
           BIC     r2,r2,#0x4
           STR     r2,[r1]
           LDR     r2,[r1]
           BIC     r2,r2,#0x2
           STR     r2,[r1]
           LDR     r2,[r1]
           BIC     r2,r2,#0x1
           STR     r2,[r1]
           
           LDR     r3,=0x0
           LDR     r4,=0xa00
delay3:
           ADD     r3 , r3 ,#0x1  
           CMP     r3,r4
           BNE     delay3
           
           LDR     r2,[r1]
           ORR     r2,r2,#0x40
           STR     r2,[r1]
           LDR     r1,=DDR_S1_IDM_IO_STATUS
loop4:
           LDR     r2,[r1]
           AND     r2,r2,#0x8
           CMP     r2,#0x8
           BNE     loop4
#-------------------------------------------

        LDR     r1, =DDR_S1_IDM_RESET_CONTROL
        LDR     r2, =0x00000000
        STR     r2, [r1]
        LDR     r1, =DDR_S2_IDM_RESET_CONTROL
        STR     r2, [r1]
        LDR     r1, =DDR_S0_IDM_RESET_CONTROL
        STR     r2, [r1]
        
        LDR     r1, =DDR_S1_IDM_IO_CONTROL_DIRECT
        LDR     r2, [r1]
        LDR     r3, =0xf000ffff
        AND     r2, r2, r3
        ORR     r2, r2, #0x01900000
        STR     r2, [r1]

        LDR     r9, DDR_PHY_REGS_REV
        LDR     r1, =0x0
        LDR     r3, =0x00019000
        DSB
        ISB

wait_for_ddr_phy_up:
        CMP     r3, r1
        BEQ     ddr_phy_rdy
        SUB     r3, r3, #0x1
        LDR     r2, =0x0
        LDR     r2, [r9]
        CMP     r2, r1
        BNE     ddr_phy_rdy
        B       wait_for_ddr_phy_up
ddr_phy_rdy:
   
        LDR     r1, =0x1801123c
        LDR     r2, =0x00000070
        STR     r2, [r1]

        LDR     r1, =0x180114b0
        LDR     r2, =0x00000003
        STR     r2, [r1]
       
        LDR     r1, =0x180116b0
        LDR     r2, =0x00000003
        STR     r2, [r1]

        LDR     r1, =0x180118b0
        LDR     r2, =0x00000003
        STR     r2, [r1]


        LDR     r1, =0x18011040
        LDR     r2, =0x06000000
        STR     r2, [r1]

        LDR     r1, =0x180114b0
        LDR     r2, =0x00000003
        STR     r2, [r1]

        LDR     r1, =0x180116b0
        LDR     r2, =0x00000003
        STR     r2, [r1]

        LDR     r1, =0x180118b0
        LDR     r2, =0x00000003
        STR     r2, [r1]

        LDR     r1, =0x18011ab0
        LDR     r2, =0x00000003
        STR     r2, [r1]

        LDR     r1, =0x18011008
        LDR     r2, =0x00cd0012
        STR     r2, [r1]

        LDR     r1, =0x18011018
        LDR     r2, =0x004030a0
        STR     r2, [r1]

        LDR     r1, =0x18011010
        LDR     r2, =0x9c000000
        STR     r2, [r1]

        LDR     r1, =0x18011008
        LDR     r2, =0x00cd0010
        STR     r2, [r1]

        DSB
        ISB

pll_poll:
        LDR     r9, =0x18011004
        LDR     r3,[r9]
        UBFX    r2,r3,#0,#1
        CMP     r2,#1
        BNE     pll_poll
        DSB
        ISB
     
        LDR     r1, =0x18011008
        LDR     r2, =0x00cd0000
        STR     r2, [r1]

        LDR     r1, =0x18011200
        LDR     r2, =0x00001820
        STR     r2, [r1]

        LDR     r1, =0x18011060
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18011060
        LDR     r2, =0x00000019
        STR     r2, [r1]

calib_idle_poll:
        LDR     r9, =0x18011064
        LDR     r3,[r9]
        UBFX    r2,r3,#0,#1
        CMP     r2,#1
        BNE     calib_idle_poll
        DSB
        ISB
     
        LDR     r1, =0x18011060
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180110fc
        LDR     r2, =0x0001004d
        STR     r2, [r1]

        LDR     r1, =0x180110f0
        LDR     r2, =0x0001004d
        STR     r2, [r1]

        LDR     r1, =0x180114a0
        LDR     r2, =0x00010000
        STR     r2, [r1]

        LDR     r1, =0x18011488
        LDR     r2, =0x00010025
        STR     r2, [r1]

        LDR     r1, =0x1801148c
        LDR     r2, =0x00010025
        STR     r2, [r1]

        LDR     r1, =0x180116a0
        LDR     r2, =0x00010000
        STR     r2, [r1]

        LDR     r1, =0x18011688
        LDR     r2, =0x00010025
        STR     r2, [r1]

        LDR     r1, =0x1801168c
        LDR     r2, =0x00010025
        STR     r2, [r1]

        LDR     r1, =0x180118a0
        LDR     r2, =0x00010000
        STR     r2, [r1]

        LDR     r1, =0x18011888
        LDR     r2, =0x00010025
        STR     r2, [r1]

        LDR     r1, =0x1801188c
        LDR     r2, =0x00010025
        STR     r2, [r1]

        LDR     r1, =0x18011aa0
        LDR     r2, =0x00010000
        STR     r2, [r1]

        LDR     r1, =0x18011a88
        LDR     r2, =0x00010025
        STR     r2, [r1]

        LDR     r1, =0x18011a8c
        LDR     r2, =0x00010025
        STR     r2, [r1]

        LDR     r1, =0x18011038
        LDR     r2, =0x00031bff
        STR     r2, [r1]

        LDR     r1, =0x180114cc
        LDR     r2, =0x00031bff
        STR     r2, [r1]

        LDR     r1, =0x180116cc
        LDR     r2, =0x00031bff
        STR     r2, [r1]

        LDR     r1, =0x180118cc
        LDR     r2, =0x00031bff
        STR     r2, [r1]

        LDR     r1, =0x18011acc
        LDR     r2, =0x00031bff
        STR     r2, [r1]

        LDR     r1, =0x180114d0
        LDR     r2, =0x000fffff
        STR     r2, [r1]

        LDR     r1, =0x180116d0
        LDR     r2, =0x000fffff
        STR     r2, [r1]

        LDR     r1, =0x180118d0
        LDR     r2, =0x000fffff
        STR     r2, [r1]

        LDR     r1, =0x18011ad0
        LDR     r2, =0x000fffff
        STR     r2, [r1]

        LDR     r1, =0x1801103c
        LDR     r2, =0x09000000
        STR     r2, [r1]

        LDR     r1, =0x180114d8
        LDR     r2, =0x00000e02
        STR     r2, [r1]

        LDR     r1, =0x180116d8
        LDR     r2, =0x00000e02
        STR     r2, [r1]

        LDR     r1, =0x180118d8
        LDR     r2, =0x00000e02
        STR     r2, [r1]

        LDR     r1, =0x18011ad8
        LDR     r2, =0x00000e02
        STR     r2, [r1]

ddr_cntrl_prog:

    
  LDR       r1, =DDR_DENALI_CTL_00
  LDR       r2, =0x00000600
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_01
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_02
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_03
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_04
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_05
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_06
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_07
  LDR       r2, =0x00000400
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_08
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_09
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_10
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_11
  LDR       r2, =0x00000004
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_12
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_13
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_14
  LDR       r2, =0x01000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_15
  LDR       r2, =0x00000043
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_16
  LDR       r2, =0x000000a7
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_17
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_18
  LDR       r2, =0x02000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_19
  LDR       r2, =0x00050a00
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_20
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_21
  LDR       r2, =0x00050a00
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_22
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_23
  LDR       r2, =0x05040400
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_24
  LDR       r2, =0x0e140004
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_25
  LDR       r2, =0x0f050404
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_26
  LDR       r2, =0x14000405
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_27
  LDR       r2, =0x0504040e
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_28
  LDR       r2, =0x0404040f
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_29
  LDR       r2, =0x005b680c
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_30
  LDR       r2, =0x040c0503
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_31
  LDR       r2, =0x000c0404
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_32
  LDR       r2, =0x03005b68
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_33
  LDR       r2, =0x05000c05
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_34
  LDR       r2, =0x00050505
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_35
  LDR       r2, =0x010c0032
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_36
  LDR       r2, =0x0a140a0a
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_37
  LDR       r2, =0x0100000a
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_38
  LDR       r2, =0x0a0a0101
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_39
  LDR       r2, =0x00050503
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_40
  LDR       r2, =0x00010100
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_41
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_42
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_43
  LDR       r2, =0x00780100
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_44
  LDR       r2, =0x00000a20
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_45
  LDR       r2, =0x00000078
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_46
  LDR       r2, =0x00000a20
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_47
  LDR       r2, =0x00000005
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_48
  LDR       r2, =0x00030003
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_49
  LDR       r2, =0x000a000a
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_50
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_51
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_52
  LDR       r2, =0x02000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_53
  LDR       r2, =0x0200005a
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_54
  LDR       r2, =0x0000005a
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_55
  LDR       r2, =0x05000001
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_56
  LDR       r2, =0x00050505
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_57
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_58
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_59
  LDR       r2, =0x00010000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_60
  LDR       r2, =0x10040010
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_61
  LDR       r2, =0x00000400
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_62
  LDR       r2, =0x00000200
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_63
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_64
  LDR       r2, =0x00000001
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_84
  LDR       r2, =0x00000210
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_85
  LDR       r2, =0x00000046
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_86
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_87
  LDR       r2, =0x00000210
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_88
  LDR       r2, =0x00000046
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_98
  LDR       r2, =0x00000210
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_99
  LDR       r2, =0x00000046
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_100
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_101
  LDR       r2, =0x00000210
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_102
  LDR       r2, =0x00000046
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_103
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_104
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_106
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_107
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_108
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_109
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_110
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_111
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_139
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_140
  LDR       r2, =0x01000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_141
  LDR       r2, =0x00000001
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_142
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_143
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_144
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_145
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_146
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_147
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_148
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_149
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_150
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_152
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_153
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_155
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_156
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_157
  LDR       r2, =0x01000200
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_158
  LDR       r2, =0x02000040
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_159
  LDR       r2, =0x00400100
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_160
  LDR       r2, =0x00000200
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_161
  LDR       r2, =0x00000040
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_162
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_163
  LDR       r2, =0x01000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_164
  LDR       r2, =0x0a020101
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_165
  LDR       r2, =0x0101ffff
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_166
  LDR       r2, =0x01010101
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_167
  LDR       r2, =0x01010101
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_168
  LDR       r2, =0x00000103
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_169
  LDR       r2, =0x00000c03
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_170
  LDR       r2, =0x00000001
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_171
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_172
  LDR       r2, =0x01000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_173
  LDR       r2, =0x00000100
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_174
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_175
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_176
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_177
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_178
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_179
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_180
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_181
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_182
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_183
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_184
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_185
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_186
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_187
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_188
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_189
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_190
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_191
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_192
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_193
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_194
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_195
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_196
  LDR       r2, =0x01010200
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_197
  LDR       r2, =0x00000002
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_198
  LDR       r2, =0x06040400
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_199
  LDR       r2, =0x00000106
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_200
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_201
  LDR       r2, =0x02020002
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_202
  LDR       r2, =0x02020101
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_203
  LDR       r2, =0x02000102
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_209
  LDR       r2, =0x0000280d
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_210
  LDR       r2, =0x00010000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_211
  LDR       r2, =0x00010000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_221
  LDR       r2, =0x01000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_222
  LDR       r2, =0x00000001
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_223
  LDR       r2, =0x00000100
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_224
  LDR       r2, =0x00030300
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_225
  LDR       r2, =0x0556AA00
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_226
  LDR       r2, =0x000aa955
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_227
  LDR       r2, =0x000aa955
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_228
  LDR       r2, =0x000556aa
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_229
  LDR       r2, =0x000556aa
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_230
  LDR       r2, =0x000aa955
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_231
  LDR       r2, =0x000aa955
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_232
  LDR       r2, =0x030556aa
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_233
  LDR       r2, =0x01000001
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_234
  LDR       r2, =0x00010300
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_235
  LDR       r2, =0x00676700
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_236
  LDR       r2, =0x00676700
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_237
  LDR       r2, =0x00676700
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_238
  LDR       r2, =0x00676700
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_239
  LDR       r2, =0x00676700
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_240
  LDR       r2, =0x00676700
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_241
  LDR       r2, =0x00676700
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_242
  LDR       r2, =0x00676700
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_243
  LDR       r2, =0x00676700
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_245
  LDR       r2, =0x01000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_246
  LDR       r2, =0x00000001
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_247
  LDR       r2, =0x00000101
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_248
  LDR       r2, =0x00010100
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_249
  LDR       r2, =0x03000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_250
  LDR       r2, =0x03030303
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_251
  LDR       r2, =0x00030303
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_252
  LDR       r2, =0x02020064
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_253
  LDR       r2, =0x02020202
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_254
  LDR       r2, =0x00010202
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_255
  LDR       r2, =0x01010064
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_256
  LDR       r2, =0x01010101
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_257
  LDR       r2, =0x00020101
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_258
  LDR       r2, =0x00000064
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_259
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_260
  LDR       r2, =0x000d0d00
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_261
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_262
  LDR       r2, =0x00001440
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_263
  LDR       r2, =0x02000200
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_264
  LDR       r2, =0x02000200
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_265
  LDR       r2, =0x00001440
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_266
  LDR       r2, =0x00006540
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_267
  LDR       r2, =0x00000505
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_268
  LDR       r2, =0x00001440
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_269
  LDR       r2, =0x02000200
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_270
  LDR       r2, =0x02000200
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_271
  LDR       r2, =0x00001440
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_272
  LDR       r2, =0x00006540
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_273
  LDR       r2, =0x02020505
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_274
  LDR       r2, =0x000a0100
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_275
  LDR       r2, =0x0000000a
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_276
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_277
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_278
  LDR       r2, =0x00000f0a
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_284
  LDR       r2, =0x00000103
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_285
  LDR       r2, =0x00010003
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_286
  LDR       r2, =0x00000003
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_287
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_288
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_289
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_290
  LDR       r2, =0x04000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_291
  LDR       r2, =0x00040404
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_303
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_304
  LDR       r2, =0x00000000
  STR       r2, [r1]
  LDR       r1, =DDR_DENALI_CTL_00
  LDR       r2, =0x00000601
  STR       r2, [r1]

poll_ddr_ctrl_begin:
        LDR     r1, =0x180102bc
poll_ddr_ctrl:
        LDR     r0, [r1]
        LDR     r2, =0x100
        AND     r0, r2, r0
        CMP     r0, r2
        BNE     poll_ddr_ctrl

        BX      lr

DDR_PHY_CFG_BASE:
	.word  DDR_DENALI_CTL_198

DDR_PHY_REGS_REV:
	.word  DDR_PHY_CONTROL_REGS_REVISION

	.end


