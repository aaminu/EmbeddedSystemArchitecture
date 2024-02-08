#include <stdint.h>

/**
 * @brief Cortex-M MPU starting address in the control block 0xE0000000.
 */
#define MPU_BASE (0xE000ED90)

/**
 * @brief MPU Type Register - offset of 0x00.
 * Contains information about the avalaibility of MPU Capabilities
 */
#define MPU_TYPE (*(volatile uint32_t *)(MPU_BASE + 0x00))

/**
 * @brief MPU Control Register - offset of 0x04.
 * Enables the default and non default mapped regions in the MPU
 * Accessing non mapped regions is forbidden if background mapping isn't enabled
 */
#define MPU_CTRL (*(volatile uint32_t *)(MPU_BASE + 0x04))

/**
 * @brief MPU Region Number Register - offset of 0x08.
 * Select region to configure
 */
#define MPU_RNR (*(volatile uint32_t *)(MPU_BASE + 0x08))

/**
 * @brief MPU Region Base Address Register - offset of 0x0C.
 * Changing the base address of the selected region
 */
#define MPU_RBAR (*(volatile uint32_t *)(MPU_BASE + 0x0c))

/**
 * @brief MPU Region Attribute and Size Register - offset of 0x10.
 * Permision, attributes and size of selected region
 */
#define MPU_RASR (*(volatile uint32_t *)(MPU_BASE + 0x10))

/**
 * RASR Permissions
 * https://developer.arm.com/documentation/dui0552/a/cortex-m3-peripherals/optional-memory-protection-unit/mpu-region-attribute-and-size-register
 */
#define RASR_ENABLED (1)
#define RASR_RW (1 << 24)
#define RASR_RDONLY (5 << 24)
#define RASR_NOACCESS (0 << 24)
#define RASR_SCB (7 << 16)
#define RASR_SB (5 << 16)
#define RASR_NOEXEC (1 << 28)

/*Sizes for Bit 1:5 of RASR:  2^(SIZE+1)*/
#define MPU_SIZE_32B (0x04 << 1)
#define MPU_SIZE_64B (0x05 << 1)
#define MPU_SIZE_128B (0x06 << 1)
#define MPU_SIZE_256B (0x07 << 1)
#define MPU_SIZE_512B (0x08 << 1)
#define MPU_SIZE_1K (0x09 << 1)
#define MPU_SIZE_2K (0x0a << 1)
#define MPU_SIZE_4K (0x0b << 1)
#define MPU_SIZE_8K (0x0c << 1)
#define MPU_SIZE_16K (0x0d << 1)
#define MPU_SIZE_32K (0x0e << 1)
#define MPU_SIZE_64K (0x0f << 1)
#define MPU_SIZE_128K (0x10 << 1)
#define MPU_SIZE_256K (0x11 << 1)
#define MPU_SIZE_512K (0x12 << 1)
#define MPU_SIZE_1M (0x13 << 1)
#define MPU_SIZE_2M (0x14 << 1)
#define MPU_SIZE_4M (0x15 << 1)
#define MPU_SIZE_8M (0x16 << 1)
#define MPU_SIZE_16M (0x17 << 1)
#define MPU_SIZE_32M (0x18 << 1)
#define MPU_SIZE_64M (0x19 << 1)
#define MPU_SIZE_128M (0x1a << 1)
#define MPU_SIZE_256M (0x1b << 1)
#define MPU_SIZE_512M (0x1c << 1)
#define MPU_SIZE_1G (0x1d << 1)
#define MPU_SIZE_2G (0x1e << 1)
#define MPU_SIZE_4G (0x1f << 1)

/**
 * @brief System Control Space
 */
#define SCB_BASE (0xE000ED00)

/**
 * @brief System Handler Control and State Register
 */
#define SHCSR (*(volatile uint32_t *)(SCB_BASE + 0x24))

#define GUARD_SIZE (0x400)

extern uint32_t _END_STACK;
extern uint32_t _END_HEAP;
extern uint32_t _FLASH;
extern uint32_t _SRAM;
extern uint32_t STACK_SIZE;

/******************************STATIC**********************************/
static void mpu_set_region(int region, uint32_t start, uint32_t attr)
{
    MPU_RNR = region;
    MPU_RBAR = start;
    MPU_RNR = region;
    MPU_RASR = attr;
}

/******************************GLOBAL*********************************/
int mpu_enable(void)
{
    volatile uint32_t type;
    volatile uint32_t start;
    volatile uint32_t attr;

    type = MPU_TYPE;
    if (type == 0)
    {
        /*No Support for MPU*/
        return -1;
    }

    /*Disabling MPU before change*/
    MPU_CTRL = 0;

    /*Set Flash as Read-Only, Executable*/
    start = (uint32_t)&_FLASH;
    attr = RASR_ENABLED | MPU_SIZE_512K | RASR_SCB | RASR_RDONLY;
    mpu_set_region(0, start, attr);

    /*Set RAM as read_write, not executable*/
    start = (uint32_t)&_SRAM;
    attr = RASR_ENABLED | MPU_SIZE_128K | RASR_SCB | RASR_RW | RASR_NOEXEC;
    mpu_set_region(1, start, attr);

    /*1KB Guard between Heap and STACK*/
    start = (uint32_t)(&_END_HEAP) + GUARD_SIZE;
    attr = RASR_ENABLED | MPU_SIZE_1K | RASR_SCB | RASR_NOACCESS | RASR_NOEXEC;
    mpu_set_region(2, start, attr); // Priority over region 1

    /*System Registers*/
    start = 0xE0000000;
    attr = RASR_ENABLED | MPU_SIZE_256M | RASR_SB | RASR_RW | RASR_NOEXEC;
    mpu_set_region(3, start, attr);

    /*Enable the Memory Fault Exception*/
    SHCSR |= (1 << 16);

    /* Enable the MPU, no background region */
    MPU_CTRL = 1;
    return 0;
}
