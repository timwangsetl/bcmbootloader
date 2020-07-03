/*
 * $Copyright Open Broadcom Corporation$
 */


#include <common.h>
#include <post.h>
#include <net.h>


#if CONFIG_POST & CONFIG_SYS_POST_RGMII
DECLARE_GLOBAL_DATA_PTR;

extern  IPaddr_t    NetPingIP;

static char localIPName[16]="ipaddr";
static char serverIPName[16]="serverip";

#define XMK_STR(x)	#x
#define MK_STR(x)	XMK_STR(x)

static  const char serverIP[] = {MK_STR(CONFIG_SERVERIP)};

extern int NetRxPacketLen;

#if defined(CONFIG_CYGNUS)
#include "../../drivers/net/ethHw_data.h"
extern bcm_eth_t g_eth_data;
extern int srab_wreg(void *robo, uint8 page, uint8 reg, void *val, int len);
extern int srab_rreg(void *robo, uint8 page, uint8 reg, void *val, int len);
extern void chip_phy_wr(bcm_eth_t *eth_data, uint ext, uint phyaddr, uint reg, uint16_t v);
extern uint16_t chip_phy_rd(bcm_eth_t *eth_data, uint ext, uint phyaddr, uint reg);
extern int ethHw_Init(void);
extern void chip_phy_init(bcm_eth_t *eth_data, int ext, uint phyaddr);
extern void iproc_rgmii_iomux(int op);


static void control_tx(int port, int en)
{
	uint8_t tmp = 0; 
	
	if (port == 2) {
		port = 5;
	}
	srab_rreg(g_eth_data.bcmgmac.robo, 0, port, &tmp, 1);
	
	if (!en) {
		tmp |= (1<<1);					
	} else {
		tmp &= ~(1<<1);	
	}
	
  	srab_wreg(g_eth_data.bcmgmac.robo, 0, port, &tmp, 1);
  	
  	/*srab_rreg(g_eth_data.bcmgmac.robo, 0, port, &tmp, 1);
	printf("control: %x\n", tmp);*/
  	
}
static void control_rx(int port, int en)
{
	uint8_t tmp = 0;
	
	if (port == 2) {
		port = 5;
	}	
	srab_rreg(g_eth_data.bcmgmac.robo, 0, port, &tmp, 1);
	/*printf("control: %x\n", tmp); */
	
	if (!en) {
		tmp |= (1<<0);		
	} else {
		tmp &= ~(1<<0);
	}
	 	
  	srab_wreg(g_eth_data.bcmgmac.robo, 0, port, &tmp, 1);  	
  	/*srab_rreg(g_eth_data.bcmgmac.robo, 0, port, &tmp, 1);
	printf("control: %x\n", tmp);*/
  	
}

static void set_phy_loopback(int port, int en)
{
	uint16_t val;
	uint phyaddr;
	uint ext = 0; 
	uint reg =  0;
	
	if (port == 0) {
		phyaddr = 0;
	} else if (port == 1) {
		phyaddr = 1;
	} else {
		phyaddr = 0x10;
		ext = 1;
	}
	val = chip_phy_rd(&g_eth_data, ext, phyaddr, reg);
	//post_log("phy(%x) ctrl reg:0x%4x\n", phyaddr, (int)val);
	if (en) {
		val |= (1 << 14);
	} else {
		val &= ~(1 << 14);
	}
      
	chip_phy_wr(&g_eth_data, ext, phyaddr, reg, val);
	//val = chip_phy_rd(&g_eth_data, ext, phyaddr, reg);
	//post_log("phy(%x) ctrl reg:0x%4x\n", phyaddr, (int)val);
	
}
extern void DhcpRequest(void);
extern int post_get_board_diags_type(void);
static int bc_lb_check(void)
{
	    net_init();
        eth_halt();
		eth_set_current();
		eth_init(gd->bd);
		do {
		    NetRxPacketLen = 0;
		    eth_rx(); 
		} while (NetRxPacketLen != 0);
        DhcpRequest();
        udelay(1000);
        NetRxPacketLen = 0;
        eth_rx();     
        printf("NetRxPacketLen = %d\n",NetRxPacketLen);  
        if (NetRxPacketLen < 64) {
        	return -1;
        } 
	    return 0;
}


void p2_phy_reset(void)
{
	uint32_t val;
	
	val = chip_phy_rd(&g_eth_data, 1, 0x10, 0);
	//post_log("phy(%x) ctrl reg:0x%4x\n", phyaddr, (int)val);	
	val |= (1 << 15);
	chip_phy_wr(&g_eth_data, 1, 0x10, 0, val);
	
	mdelay(1000);
	
	/*val = chip_phy_rd(&g_eth_data, 1, 0x10, 0);
	post_log("phy(%x) ctrl reg:0x%4x\n", 0x10, (int)val);		
	val &= ~(1 << 15);
	chip_phy_wr(&g_eth_data, 1, 0x10, 0, val);*/
	
	chip_phy_init(&g_eth_data, 1, 0x10);	
}


static int link_status(int port)
{
	uint16	val;
	int ext = 0;
	uint phyaddr = port;	
	
	if (port >= 2) {
	    phyaddr = 0x10;
	    ext =1;
	}
	val = chip_phy_rd(&g_eth_data, ext, phyaddr, 1);
    if ((val & 0x4) == 0x4) {
    	post_log("link is ok for port %d\n", port);
	    return 1;
	} else 
	    return 0;
}

static void wait_link(int ms, int port)
{
	int i;
	for (i = 0; i< ms; i++) 
	{
		udelay(1000);
		if (link_status(port)) {
			return;
		}					
	}
}

static void set_phy2_lane(int op)
{
	uint	reg;
	uint16	val;
	int ext = 1;
	uint phyaddr = 0x10;
	
	reg = 0x17;
	val = 0x0F09;
	chip_phy_wr(&g_eth_data, ext, phyaddr, reg, val);
						
	reg = 0x15;	
	if (op == 0)
	    val = 0x11b;
	else 
		val = 0x11e;
						
	chip_phy_wr(&g_eth_data, ext, phyaddr, reg, val);	
}



#if 0

static void set_phy0_lane(void)
{
	uint	reg;
	uint16	val;
	int ext = 0;
	uint phyaddr = 0;
	
	reg = 0x17;
	val = 0x0F09;
	chip_phy_wr(&g_eth_data, ext, phyaddr, reg, val);
						
	reg = 0x15;	
	val = 0x0000;
						
	chip_phy_wr(&g_eth_data, ext, phyaddr, reg, val);	
}

static void set_phy1_lane(void)
{
	uint	reg;
	uint16	val;
	int ext = 0;
	uint phyaddr = 1;
	
	reg = 0x17;
	val = 0x0F09;
	chip_phy_wr(&g_eth_data, ext, phyaddr, reg, val);
						
	reg = 0x15;	
	val = 0x7000;
						
	chip_phy_wr(&g_eth_data, ext, phyaddr, reg, val);	
}

void p0_phy_reset(void)
{
	uint32_t val;
	
	val = chip_phy_rd(&g_eth_data, 0, 0, 0);
	//post_log("phy(%x) ctrl reg:0x%4x\n", phyaddr, (int)val);	
	val |= (1 << 15);
	chip_phy_wr(&g_eth_data, 0, 0, 0, val);
	
	mdelay(1000);
	
	/*val = chip_phy_rd(&g_eth_data, 1, 0x10, 0);
	post_log("phy(%x) ctrl reg:0x%4x\n", 0x10, (int)val);		
	val &= ~(1 << 15);
	chip_phy_wr(&g_eth_data, 1, 0x10, 0, val);*/
}

void p1_phy_reset(void)
{
	uint32_t val;
	
	val = chip_phy_rd(&g_eth_data, 0, 1, 0);
	//post_log("phy(%x) ctrl reg:0x%4x\n", phyaddr, (int)val);	
	val |= (1 << 15);
	chip_phy_wr(&g_eth_data, 0, 1, 0, val);
	
	mdelay(1000);
	
	/*val = chip_phy_rd(&g_eth_data, 1, 0x10, 0);
	post_log("phy(%x) ctrl reg:0x%4x\n", 0x10, (int)val);		
	val &= ~(1 << 15);
	chip_phy_wr(&g_eth_data, 1, 0x10, 0, val);*/
		
}

static void phy_stat(void)
{
	int port;
	int phyaddr;
	int ext;
	uint32 val;
	int reg;
	
	for (port = 0; port < 2; port++)
	{
		phyaddr = port;
		ext = 0;
		if (port == 2) {
			ext = 1;
			phyaddr = 0x10;
		}
	   reg =0;
	   val = chip_phy_rd(&g_eth_data, ext, phyaddr, reg);
	   post_log("phy(%x) reg 0x%x val:0x%4x\n", phyaddr, reg, (int)val);	
	
	   reg = 1;
       val = chip_phy_rd(&g_eth_data, ext, phyaddr, reg);
       post_log("phy(%x) reg 0x%x val:0x%4x\n", phyaddr, reg, (int)val);
       
       
       chip_phy_wr(&g_eth_data, ext, phyaddr, 0x17, 0xF00); 
	   printf("PHY %d count: %d\n", phyaddr, chip_phy_rd(&g_eth_data, ext, phyaddr , 0x15));
	    
	   
	   printf("PHY %d ext status: 0x%x\n", phyaddr, chip_phy_rd(&g_eth_data, ext, phyaddr , 0x11));
	}
	
	
}
static void mac_stat(void)
{
  		int tmp;
  		short tmp1;
  		 		
  	    //srab_wreg(g_eth_data.bcmgmac.robo, 0, port, &tmp1, 1); 
        //srab_rreg(g_eth_data.bcmgmac.robo, 0xe4, 0x02, &tmp1, 2);
        //tmp1 &= ~(1 << 3);
        //tmp1 |= (1 << 2);
        //srab_wreg(g_eth_data.bcmgmac.robo, 0xe4, 0x02, &tmp1, 2);
        //udelay(10000);
        //srab_rreg(g_eth_data.bcmgmac.robo, 0xe4, 0x0a, &tmp1, 2);
  		//printf("port 1 state: %x \n", tmp1);
  		
  		
  		srab_rreg(g_eth_data.bcmgmac.robo, 0x20, 0x10, &tmp, 4);
  		printf("port 0 tx pack: %d \n", tmp);
  		srab_rreg(g_eth_data.bcmgmac.robo, 0x20, 0x9c, &tmp, 4);
  		printf("port 0 rx pack: %d \n", tmp);
  		
  		srab_rreg(g_eth_data.bcmgmac.robo, 0x21, 0x10, &tmp, 4);
  		printf("port 1 tx pack: %d \n", tmp);
  		srab_rreg(g_eth_data.bcmgmac.robo, 0x21, 0x9c, &tmp, 4);
  		printf("port 1 rx pack: %d \n", tmp); 
  		 		
        //srab_wreg(g_eth_data.bcmgmac.robo, 0, port, &tmp1, 1); 
        /*srab_rreg(g_eth_data.bcmgmac.robo, 0xe4, 0x0a, &tmp1, 2);
        tmp1 &= 0x0F;
        srab_wreg(g_eth_data.bcmgmac.robo, 0xe4, 0x0a, &tmp1, 2);
        udelay(10000);*/
        //srab_rreg(g_eth_data.bcmgmac.robo, 0xe4, 0x0a, &tmp1, 2);
  		//printf("port 2 state: %x \n", tmp1); 		
        
 		srab_rreg(g_eth_data.bcmgmac.robo, 0x25, 0x10, &tmp, 4);
  		printf("port 2 tx pack: %d \n", tmp);
  		srab_rreg(g_eth_data.bcmgmac.robo, 0x25, 0x9c, &tmp, 4);
  		printf("port 2 rx pack: %d \n", tmp);
  		  		
  		
  		srab_rreg(g_eth_data.bcmgmac.robo, 0x25, 0x80, &tmp, 4);
  		printf("port 2 RxAlignmentErr: %d \n", tmp);
  		srab_rreg(g_eth_data.bcmgmac.robo, 0x25, 0x84, &tmp, 4);
  		printf("port 2 RxFCSErr: %d \n", tmp);
  				
}


static void link_test(void)
{	
	int i;
	uint8 pos0,pos1,pos2,pos3;
	uint	reg;
	uint16	val,rd0;
	int ext = 0;
	uint phyaddr = 1;
	

	for (pos3=0; pos3< 4; pos3++)
	{		
		for (pos2=0; pos2< 4; pos2++)
		{
			if (pos2 == pos3) continue;
			for (pos1=0; pos1< 4; pos1++)
			{
				if ((pos1 == pos2) || (pos1 == pos3)) continue;
			    for (pos0=0; pos0< 4; pos0++)
				{
					if ((pos0 ==pos1) || (pos0==pos2) || (pos0==pos3)) continue;
	
					reg = 0x17;
					val = 0x0F09;
				    chip_phy_wr(&g_eth_data, ext, phyaddr, reg, val);
						
						reg = 0x15;	
						//val = 0x1100;
						val  = 0x0100;
						val |= (pos3 << 6) | (pos2 << 4) | (pos1 << 2) | pos0; 
						rd0 = chip_phy_rd(&g_eth_data, ext, phyaddr, reg);
						chip_phy_wr(&g_eth_data, ext, phyaddr, reg, val);
						printf("write Val: 0x%x to F09 with val:%x\n", val, rd0);
					    mdelay(1000*8*3);
					 phy_stat();	
					
					
			   }
	      }
	  }
   }
	
}
#endif
#endif

int rgmii_post_test (int flags)
{
    int status=0;
	char *local_ip, *server_ip;
#if defined(CONFIG_CYGNUS)
	int port;
	char ch = 0;
	int test_no;
	int port_num = 3;
	int max_delay_ms =  8000;
#endif

#if (defined(CONFIG_NS_PLUS))

	if ( post_get_board_diags_type() & HR_ER_BOARDS )
	{
		printf("\nThis diag is not supported on this platform\n ");
		return 2;
	}
#endif
    post_log("\nRGMII diags starts!");

	/* get port5 config */
    local_ip = getenv(localIPName);
	if (local_ip == NULL)
	{
		post_log("\nLocal board ip address not defined");
		post_log("\nAdd ipaddr in the env\n");
		return -1;
	}
	server_ip = getenv(serverIPName);
	if (server_ip == NULL)
	{
		post_log("\nserver ip address not defined");
		post_log("\nadd serverip in the env\n");
		return -1;
	}
#if !defined(CONFIG_CYGNUS)

    if ( (flags & POST_AUTO) !=  POST_AUTO )
    {
        post_log("\nmake sure the host pc connected is configured to IP: %s",server_ip );
        post_getConfirmation("\nReady? (Y/N)");
        post_log("\nStrap pins are set to A1:FC  A2:00  A3:07  A4:0f  A5:28  A6:f6  A7:fff");
        post_getConfirmation("\nReady? (Y/N)");
    }
    else
    {
        post_set_strappins("A2 0%A3 7%A4 f%A5 38%A6 f6%");
    }	
#endif

#if  defined(CONFIG_CYGNUS)
    post_log("\n");
    if (post_get_board_diags_type() == BCM958300K) {
    	port_num = 3;
    	iproc_rgmii_iomux(1);
    	p2_phy_reset();
    } else if (post_get_board_diags_type() == BCM911360K) {
    	port_num = 2;
    	max_delay_ms =  12000;
    }else {
    	port_num = 1;
    }    

   	for (port = 0; port < port_num; port++)
	{
	    control_tx(port, 0);
	    control_rx(port, 0);
	}


    for (port = 0; port < port_num; port++)
	{
	    control_tx(port , 1);
	    control_rx(port , 1);
		set_phy_loopback(port, 1);
		if (bc_lb_check() < 0) {
	    	//status = -1;
	    	post_log("Internal PHY loopback test for port %d failed \n", port);
	    } else {
	    	post_log("Internal PHY loopback test for port %d passed \n", port);
	    }
	    control_tx(port , 0);
	    control_rx(port , 0);
	    set_phy_loopback(port, 0); 
#if 0
	    mac_stat(); 
	    phy_stat();    
#endif
	}
	
	for (port = 0; port < port_num; port++)
	{
	       control_tx(port, 1);
	       control_rx(port, 1);
	}	
    
    if ( (flags & POST_AUTO) !=  POST_AUTO )
	{
		
        post_log("\nwill you select outer loopback test (Y/N): ");
	    do 
	    {
	        ch = (char)serial_getc();
	    } while ((ch !='y') && (ch !='Y') && (ch !='N') && (ch !='n'));
        
              
        if (!((ch == 'y') || ( ch =='Y'))) {
	        post_log("\nmake sure the host pc connected is configured to IP: %s",server_ip);
	        post_getConfirmation("\nReady? (Y/N)");
	    }
    }
    
    post_log("\n");
    if ((ch == 'y') || ( ch =='Y')) { 
    	post_log ("1: port 0->port 1\n");
    	post_log ("2: port 0->port 2\n");
    	post_log ("3: port 1->port 0\n");
    	post_log ("4: port 1->port 2\n");
    	post_log ("5: port 2->port 0\n");
    	post_log ("6: port 2->port 1\n");
    	
    	test_no = post_getUserInput ("\nEnter Test Case Number : ");
   	    
   	    for (port = 0; port < port_num; port++)
	    {
	       control_tx(port, 0);
	       control_rx(port, 0);
	    }
   	    
   	    post_log("\n");   	    
   	    switch (test_no) {
   	    	case 1:
   	    		control_tx(0, 1);
   	    		control_rx(1, 1);
   	    		wait_link(max_delay_ms, 0);
   	    		wait_link(max_delay_ms, 1);
   	    		break;
   	    	case 2:
   	    		control_tx(0, 1);
   	    		control_rx(2, 1);
   	    		wait_link(max_delay_ms, 0);
   	    		wait_link(max_delay_ms, 2);
   	    		break;   	    		
   	    	case 3:
   	    		control_tx(1, 1);
   	    		control_rx(0, 1);
   	    		wait_link(max_delay_ms, 0);
   	    		wait_link(max_delay_ms, 1);
   	    		break;
   	    	case 4:
   	    		control_tx(1, 1);
   	    		control_rx(2, 1);
   	    		set_phy2_lane(1);
   	    		wait_link(max_delay_ms, 1);
   	    		wait_link(max_delay_ms, 2);
   	    		break;
   	    	case 5:
   	    		control_tx(2, 1);
   	    		control_rx(0, 1);
   	    		wait_link(max_delay_ms, 2);
   	    		wait_link(max_delay_ms, 0);
   	    		break;
   	    	case 6:
   	    		control_tx(2, 1);
   	    		control_rx(1, 1);
   	    		set_phy2_lane(1);
   	    		wait_link(max_delay_ms, 2);
   	    		wait_link(max_delay_ms, 1);
   	    		break;
   	    	default: 
   	    		status = -1;
	            break;
   	    }
	    
        if (bc_lb_check() < 0) {
        	status = -1;
        	if ((test_no >= 1) && (test_no <= 6))
        		post_log ("Outer loopback test case %d failed\n", test_no);
        } else {
        	post_log ("Outer loopback test case %d passed\n", test_no);
        }
        
   	    for (port = 0; port < port_num; port++)
	    {
	        control_tx(port, 1);
	        control_rx(port, 1);
	    }
#if 0	
        mac_stat();    
	    phy_stat();	 
	    //link_test();   
#endif
        if ((test_no == 4) || (test_no == 6)) {
	    	set_phy2_lane(0);    	
	    }
    } else {
#endif
    	NetPingIP = string_to_ip(server_ip);
    
	    if (NetLoop(PING) < 0)
	    {
	        post_log("Testfailed; host %s is not alive.\n", server_ip);
	        status = -1;
	    } else  {
	        post_log("host %s is alive, test passed \n", server_ip);
	    }
#if defined(CONFIG_CYGNUS)
    }
#endif   
    
    return status;
}
#endif /* CONFIG_POST & CONFIG_SYS_POST_RGMII */
