#include <common.h>
#include <asm/io.h>
#include <asm/arch/iproc_regs.h>
#if (defined(CONFIG_NORTHSTAR) || defined(CONFIG_NS_PLUS))
        #define RESET_BASE_ADDR CRU_RESET_OFFSET
#elif defined(CONFIG_CYGNUS)
	#define CRMU_MAIL_BOX1	0x03024028
#else
        #define RESET_BASE_ADDR DMU_CRU_RESET
#endif
/*
 * Reset the cpu by setting software reset request bit
 */
void reset_cpu(ulong ignored)
{
#if (defined(CONFIG_NORTHSTAR) || defined(CONFIG_NS_PLUS))
        *(unsigned int *)(RESET_BASE_ADDR) = 0x1;
#elif defined(CONFIG_CYGNUS)
	writel(0xffffffff, CRMU_MAIL_BOX1);
#else
        *(unsigned int *)(RESET_BASE_ADDR) = 0; /* Reset all */
#endif
        while (1) {
                // Never reached
        }
}

