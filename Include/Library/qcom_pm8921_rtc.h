/*
 * Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 * Copyright (c) 2011-2014, Xiaomi Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __DEV_PM8921_H
#define __DEV_PM8921_H

#include <Library/qcom_pm8921_leds.h>

enum 
{
	lvs_start = 1,
	lvs_1 = lvs_start,
	lvs_2,
	lvs_3,
	lvs_4,
	lvs_5,
	lvs_6,
	lvs_7,
	lvs_end = lvs_7,
};

enum 
{
	mpp_start = 1,
	mpp_1 = mpp_start,
	mpp_2,
	mpp_3,
	mpp_4,
	mpp_5,
	mpp_6,
	mpp_7,
	mpp_8,
	mpp_9,
	mpp_10,
	mpp_11,
	mpp_12,
	mpp_end = mpp_12,
};

#define PM_GPIO_DIR_OUT         0x01
#define PM_GPIO_DIR_IN          0x02
#define PM_GPIO_DIR_BOTH        (PM_GPIO_DIR_OUT | PM_GPIO_DIR_IN)

/* output_buffer */
#define PM_GPIO_OUT_BUF_OPEN_DRAIN  1
#define PM_GPIO_OUT_BUF_CMOS        0

#define PM_GPIO_PULL_UP_30      0
#define PM_GPIO_PULL_UP_1_5     1
#define PM_GPIO_PULL_UP_31_5    2
/* 1.5uA + 30uA boost */
#define PM_GPIO_PULL_UP_1_5_30  3
#define PM_GPIO_PULL_DN         4
#define PM_GPIO_PULL_NO         5

#define PM_GPIO_STRENGTH_NO     0
#define PM_GPIO_STRENGTH_HIGH   1
#define PM_GPIO_STRENGTH_MED    2
#define PM_GPIO_STRENGTH_LOW    3

#define PM_GPIO_FUNC_NORMAL     0
#define PM_GPIO_FUNC_PAIRED     1
#define PM_GPIO_FUNC_1          2
#define PM_GPIO_FUNC_2          3

/* LDO define values */
#define LDO_P_MASK (1 << 7)

#define LDO_2      (2)
#define LDO_8      (8  | LDO_P_MASK)
#define LDO_11     (11 | LDO_P_MASK)
#define LDO_23     (23 | LDO_P_MASK)
#define LDO_29     (29 | LDO_P_MASK)

enum
{
	LDO_VOLTAGE_1_2V = 0,
	LDO_VOLTAGE_1_8V = 1,
	LDO_VOLTAGE_3_0V = 2,
	LDO_VOLTAGE_ENTRIES
};

#define PM_GPIO(_x)                    ((_x) - 1)
#define PM_IRQ_BLOCK(_x)               (_x)


#define PM_IRQ_BLOCK_GPIO_START        PM_IRQ_BLOCK(24)

#define PM_GPIO_BLOCK_ID(gpio)         (PM_IRQ_BLOCK_GPIO_START + (gpio)/8)
#define PM_GPIO_ID_TO_BIT_MASK(gpio)   (1 << ((gpio)%8))
#define PM_PWRKEY_BLOCK_ID		6
#define PM_PWRKEY_PRESS_BIT		(1 << 3)

typedef int (*pm8921_read_func)(UINT8 *data, UINT32 length, UINT32 addr);
typedef int (*pm8921_write_func)(UINT8 *data, UINT32 length, UINT32 addr);

typedef struct
{
	UINT32 initialized;

	pm8921_read_func	read;
	pm8921_write_func	write;

} pm8921_dev_t;


struct pm8921_gpio 
{
	int direction;
	int output_buffer;
	int output_value;
	int pull;
	int vin_sel;
	int out_strength;
	int function;
	int inv_int_pol;
	int disable_pin;
};

void pm8921_init(pm8921_dev_t *);
int  pm8921_gpio_config(int gpio, struct pm8921_gpio *param);
void pm8921_boot_done(void);
int  pm8921_ldo_set_voltage(UINT32 ldo_id, UINT32 voltage);
int  pm8921_config_reset_pwr_off(unsigned reset);
int  pm8921_gpio_get(UINT8 gpio, UINT8 *status);
int  pm8921_pwrkey_status(UINT8 *status);
int  pm8921_config_led_current(enum pm8921_leds led_num,UINT8 current,enum led_mode sink,int enable);
int  pm8921_config_drv_keypad(unsigned int drv_flash_sel,unsigned int flash_logic,unsigned int flash_ensel);
int  pm8921_low_voltage_switch_enable(UINT8 lvs_id);
int  pm8921_mpp_set_digital_output(UINT8 mpp_id);
int  pm8921_rtc_alarm_disable(void);
#endif
/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2014, The Linux Foundation. All rights reserved.
 * Copyright (c) 2011-2014, Xiaomi Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __PLATFORM_MSM_SHARED_SMEM_H
#define __PLATFORM_MSM_SHARED_SMEM_H


#define SMEM_V7_SMEM_MAX_PMIC_DEVICES   1
#define SMEM_V8_SMEM_MAX_PMIC_DEVICES   3
#define SMEM_MAX_PMIC_DEVICES           SMEM_V8_SMEM_MAX_PMIC_DEVICES

#define SMEM_RAM_PTABLE_VERSION_OFFSET  8

#define RAM_PART_NAME_LENGTH            16
#define RAM_NUM_PART_ENTRIES            32

#define _SMEM_RAM_PTABLE_MAGIC_1        0x9DA5E0A8
#define _SMEM_RAM_PTABLE_MAGIC_2        0xAF9EC4E2

#define SMEM_TARGET_INFO_IDENTIFIER     0x49494953

#define SMEM_NUM_SMD_STREAM_CHANNELS        64

enum smem_ram_ptable_version
{
	SMEM_RAM_PTABLE_VERSION_0,
	SMEM_RAM_PTABLE_VERSION_1,
};

struct smem_proc_comm {
	unsigned command;
	unsigned status;
	unsigned data1;
	unsigned data2;
};

struct smem_heap_info {
	unsigned initialized;
	unsigned free_offset;
	unsigned heap_remaining;
	unsigned reserved;
};

struct smem_alloc_info {
	unsigned allocated;
	unsigned offset;
	unsigned size;
	unsigned base_ext;
};

struct smem_board_info_v2 {
	unsigned format;
	unsigned msm_id;
	unsigned msm_version;
	char build_id[32];
	unsigned raw_msm_id;
	unsigned raw_msm_version;
};

struct smem_addr_info
{
	UINT32 identifier; /* Shared memory magic number */
	UINT32 size; /* Shared memory Size */
	UINT32 phy_addr; /* Shared memory Addr */
};

typedef enum
{
   PMIC_IS_PM6610,
   PMIC_IS_PM6620,
   PMIC_IS_PM6640,
   PMIC_IS_PM6650,
   PMIC_IS_PM7500,
   PMIC_IS_PANORAMIX,
   PMIC_IS_PM6652,
   PMIC_IS_PM6653,
   PMIC_IS_PM6658,
   PMIC_IS_EPIC,
   PMIC_IS_HAN,
   PMIC_IS_KIP,
   PMIC_IS_WOOKIE,
   PMIC_IS_PM8058,
   PMIC_IS_PM8028,
   PMIC_IS_PM8901,
   PMIC_IS_PM8027 ,
   PMIC_IS_ISL_9519,
   PMIC_IS_PM8921,
   PMIC_IS_PM8018,
   PMIC_IS_PM8015,
   PMIC_IS_PM8014,
   PMIC_IS_PM8821,
   PMIC_IS_PM8038,
   PMIC_IS_PM8922,
   PMIC_IS_PM8917,
   PMIC_IS_INVALID = 0x7fffffff,
} pm_model_type_afly;

typedef enum
{
	PMIC_IS_UNKNOWN   = 0,
	PMIC_IS_PM8941    = 1,
	PMIC_IS_PM8841    = 2,
	PMIC_IS_PM8019    = 3,
	PMIC_IS_PM8026    = 4,
	PMIC_IS_PM8110    = 5,
} pm_model_type_bfly;

struct smem_board_info_v3 {
	unsigned format;
	unsigned msm_id;
	unsigned msm_version;
	char build_id[32];
	unsigned raw_msm_id;
	unsigned raw_msm_version;
	unsigned hw_platform;
};

struct smem_board_info_v4 {
	struct smem_board_info_v3 board_info_v3;
	unsigned platform_version;
	unsigned buffer_align;	//Need for 8 bytes alignment while reading from shared memory.
};

struct smem_board_info_v5 {
	struct smem_board_info_v3 board_info_v3;
	unsigned platform_version;
	unsigned fused_chip;
};

struct smem_board_info_v6 {
	struct smem_board_info_v3 board_info_v3;
	unsigned platform_version;
	unsigned fused_chip;
	unsigned platform_subtype;
	unsigned buffer_align;	//Need for 8 bytes alignment while reading from shared memory.
};

struct smem_board_info_v7 {
	struct smem_board_info_v3 board_info_v3;
	unsigned platform_version;
	unsigned fused_chip;
	unsigned platform_subtype;
	unsigned pmic_type;
	unsigned pmic_version;
	unsigned buffer_align;	//Need for 8 bytes alignment while reading from shared memory.
};

struct smem_pmic_info {
	unsigned pmic_type;
	unsigned pmic_version;
};

struct smem_board_info_v8 {
	struct smem_board_info_v3 board_info_v3;
	unsigned platform_version;
	unsigned fused_chip;
	unsigned platform_subtype;
	struct smem_pmic_info pmic_info[SMEM_V8_SMEM_MAX_PMIC_DEVICES];
	/*
	 * Need for 8 bytes alignment
	 * while reading from shared memory
	 */
	unsigned buffer_align;
};

typedef struct {
	unsigned key_len;
	unsigned iv_len;
	unsigned char key[32];
	unsigned char iv[32];
} boot_symmetric_key_info;

typedef struct {
	unsigned int update_status;
	unsigned int bl_error_code;
} boot_ssd_status;



/* Dummy structure to keep it for other targets */
typedef struct {
	UINT32 boot_flags;
	boot_ssd_status status;
} boot_info_for_apps;

/* chip information */
enum {
	UNKNOWN = 0,
	MDM9200 = 57,
	MDM9600 = 58,
	MSM8260 = 70,
	MSM8660 = 71,
	APQ8060 = 86,
	MSM8960 = 87,
	MSM7225A = 88,
	MSM7625A = 89,
	MSM7227A = 90,
	MSM7627A = 91,
	ESM7227A = 92,
	ESM7225A = 96,
	ESM7627A = 97,
	MSM7225AA = 98,
	MSM7625AA = 99,
	ESM7225AA = 100,
	MSM7227AA = 101,
	MSM7627AA = 102,
	ESM7227AA = 103,
	APQ8064 = 109,
	MSM8930 = 116,
	MSM8630 = 117,
	MSM8230 = 118,
	APQ8030 = 119,
	MSM8627 = 120,
	MSM8227 = 121,
	MSM8660A = 122,
	MSM8260A = 123,
	APQ8060A = 124,
	MSM8974 = 126,
	MSM8225 = 127,
	MSM8625 = 129,
	MPQ8064 = 130,
	MSM7225AB = 131,
	MSM7625AB = 132,
	ESM7225AB = 133,
	MDM9625   = 134,
	MSM7125A  = 135,
	MSM7127A  = 136,
	MSM8125A  = 137,
	MSM8960AB = 138,
	APQ8060AB = 139,
	MSM8260AB = 140,
	MSM8660AB = 141,
	MSM8930AA = 142,
	MSM8630AA = 143,
	MSM8230AA = 144,
	MSM8626   = 145,
	MSM8610   = 147,
	MDM9225   = 149,
	MDM9225M  = 150,
	MDM9625M  = 152,
	APQ8064AB = 153, /* aka V2 PRIME */
	MSM8930AB = 154,
	MSM8630AB = 155,
	MSM8230AB = 156,
	APQ8030AB = 157,
	MSM8226   = 158,
	MSM8826   = 159,
	APQ8030AA = 160,
	MSM8110   = 161,
	MSM8210   = 162,
	MSM8810   = 163,
	MSM8212   = 164,
	MSM8612   = 165,
	MSM8812   = 166,
	MSM8125   = 167,
	MDM9310   = 171,
	APQ8064AA = 172, /* aka V2 SLOW_PRIME */
	APQ8084   = 178,
	MSM8130   = 179,
	MSM8130AA = 180,
	MSM8130AB = 181,
	MSM8627AA = 182,
	MSM8227AA = 183,
	APQ8074   = 184,
	MSM8274   = 185,
	MSM8674   = 186,
	MDM9635   = 187,
	FSM9900   = 188,
	FSM9905   = 189,
	FSM9955   = 190,
	FSM9950   = 191,
	FSM9915   = 192,
	FSM9910   = 193,
	MSM8974AC = 194,
	MSM8126   = 198,
	APQ8026   = 199,
	MSM8926   = 200,
	MSM8326   = 205,
	MSM8916   = 206,
	MSM8994   = 207,
	APQ8074AA  = 208,
	APQ8074AB  = 209,
	APQ8074AC  = 210,
	MSM8274AA  = 211,
	MSM8274AB  = 212,
	MSM8274AC  = 213,
	MSM8674AA  = 214,
	MSM8674AB  = 215,
	MSM8674AC  = 216,
	MSM8974AA  = 217,
	MSM8974AB  = 218,
	APQ8028  = 219,
	MSM8128  = 220,
	MSM8228  = 221,
	MSM8528  = 222,
	MSM8628  = 223,
	MSM8928  = 224,
	MSM8510  = 225,
	MSM8512  = 226,
	MSM8936  = 233,
	MSMZIRC  = 234,
	MSM8939  = 239,
	APQ8036  = 240,
	APQ8039  = 241,
	MSM8236  = 242,
	MSM8636  = 243,
	MSM8909  = 245,
	APQ8016  = 247,
	MSM8216  = 248,
	MSM8116  = 249,
	MSM8616  = 250,
	APQ8094  = 253,
	FSM9008  = 254,
	FSM9010  = 255,
	FSM9016  = 256,
	FSM9055  = 257,
};

enum platform {
	HW_PLATFORM_UNKNOWN = 0,
	HW_PLATFORM_SURF = 1,
	HW_PLATFORM_FFA = 2,
	HW_PLATFORM_FLUID = 3,
	HW_PLATFORM_SVLTE = 4,
	HW_PLATFORM_OEM = 5,
	HW_PLATFORM_QT = 6,
	HW_PLATFORM_MTP_MDM = 7,
	HW_PLATFORM_MTP = 8,
	HW_PLATFORM_LIQUID = 9,
	HW_PLATFORM_DRAGON = 10,
	HW_PLATFORM_QRD = 11,
	HW_PLATFORM_HRD = 13,
	HW_PLATFORM_DTV = 14,
	HW_PLATFORM_RUMI   = 15,
	HW_PLATFORM_VIRTIO = 16,
	HW_PLATFORM_BTS = 19,
	HW_PLATFORM_RCM = 21,
	HW_PLATFORM_DMA = 22,
	HW_PLATFORM_STP = 23,
	HW_PLATFORM_SBC = 24,
	HW_PLATFORM_32BITS = 0x7FFFFFFF,
};

enum platform_subtype {
	HW_PLATFORM_SUBTYPE_UNKNOWN = 0,
	HW_PLATFORM_SUBTYPE_MDM = 1,
	HW_PLATFORM_SUBTYPE_8974PRO_PM8084 = 1,
	HW_PLATFORM_SUBTYPE_CSFB = 1,
	HW_PLATFORM_SUBTYPE_SVLTE1 = 2,
	HW_PLATFORM_SUBTYPE_SVLTE2A = 3,
	HW_PLATFORM_SUBTYPE_SGLTE = 6,
	HW_PLATFORM_SUBTYPE_DSDA = 7,
	HW_PLATFORM_SUBTYPE_DSDA2 = 8,
	HW_PLATFORM_SUBTYPE_SGLTE2 = 9,
	HW_PLATFORM_SUBTYPE_32BITS = 0x7FFFFFFF
};

typedef enum {
	SMEM_SPINLOCK_ARRAY = 7,
	SMEM_AARM_PARTITION_TABLE = 9,
	SMEM_CHANNEL_ALLOC_TBL = 13,
	SMEM_SMD_BASE_ID = 14,

	SMEM_APPS_BOOT_MODE = 106,

	SMEM_BOARD_INFO_LOCATION = 137,

	SMEM_SMD_FIFO_BASE_ID = 338,

	SMEM_USABLE_RAM_PARTITION_TABLE = 402,

	SMEM_POWER_ON_STATUS_INFO = 403,

	SMEM_RLOCK_AREA = 404,

	SMEM_BOOT_INFO_FOR_APPS = 418,

	SMEM_FIRST_VALID_TYPE = SMEM_SPINLOCK_ARRAY,
	SMEM_LAST_VALID_TYPE = SMEM_BOOT_INFO_FOR_APPS,

	SMEM_MAX_SIZE = SMEM_BOOT_INFO_FOR_APPS + 1,
} smem_mem_type_t;

/* Note: buf MUST be 4byte aligned, and max_len MUST be a multiple of 4. */
unsigned smem_read_alloc_entry(smem_mem_type_t type, void *buf, int max_len);

/* SMEM RAM Partition */
enum {
	DEFAULT_ATTRB = ~0x0,
	READ_ONLY = 0x0,
	READWRITE,
};

enum {
	DEFAULT_CATEGORY = ~0x0,
	SMI = 0x0,
	EBI1,
	EBI2,
	QDSP6,
	IRAM,
	IMEM,
	EBI0_CS0,
	EBI0_CS1,
	EBI1_CS0,
	EBI1_CS1,
	SDRAM = 0xE,
};

enum {
	DEFAULT_DOMAIN = 0x0,
	APPS_DOMAIN,
	MODEM_DOMAIN,
	SHARED_DOMAIN,
};

enum {
	SYS_MEMORY = 1,		/* system memory */
	BOOT_REGION_MEMORY1,	/* boot loader memory 1 */
	BOOT_REGION_MEMORY2,	/* boot loader memory 2,reserved */
	APPSBL_MEMORY,		/* apps boot loader memory */
	APPS_MEMORY,		/* apps  usage memory */
};

struct smem {
	struct smem_proc_comm proc_comm[4];
	unsigned version_info[32];
	struct smem_heap_info heap_info;
	struct smem_alloc_info alloc_info[SMEM_MAX_SIZE];
};

struct smem_ram_ptn {
	char name[16];
	UINT32 start;
	UINT32 size;
	UINT32 attr;          /* RAM Partition attribute: READ_ONLY, READWRITE etc.*/
	UINT32 category;      /* RAM Partition category: EBI0, EBI1, IRAM, IMEM */
	UINT32 domain;        /* RAM Partition domain: APPS, MODEM, APPS & MODEM (SHARED) etc. */
	UINT32 type;          /* RAM Partition type: system, bootloader, appsboot, apps etc. */
	UINT32 num_partitions;/* Number of memory partitions */
	UINT32 reserved3;
	UINT32 reserved4;
	UINT32 reserved5;
} __attribute__ ((__packed__));

struct smem_ram_ptn_v1 {
	char name[RAM_PART_NAME_LENGTH];
	UINT64 start;
	UINT64 size;
	UINT32 attr;          /* RAM Partition attribute: READ_ONLY, READWRITE etc.*/
	UINT32 category;      /* RAM Partition category: EBI0, EBI1, IRAM, IMEM */
	UINT32 domain;        /* RAM Partition domain: APPS, MODEM, APPS & MODEM (SHARED) etc. */
	UINT32 type;          /* RAM Partition type: system, bootloader, appsboot, apps etc. */
	UINT32 num_partitions;/* Number of memory partitions */
	UINT32 reserved3;
	UINT32 reserved4;     /* Reserved for future use */
	UINT32 reserved5;     /* Reserved for future use */
} __attribute__ ((__packed__));

struct smem_ram_ptable {
	unsigned magic[2];
	unsigned version;
	unsigned reserved1;
	unsigned len;
	struct smem_ram_ptn parts[32];
	unsigned buf;
} __attribute__ ((__packed__));

struct smem_ram_ptable_hdr
{
	UINT32 magic[2];
	UINT32 version;
	UINT32 reserved1;
	UINT32 len;
} __attribute__ ((__packed__));

struct smem_ram_ptable_v1 {
	struct smem_ram_ptable_hdr hdr;
	UINT32 reserved2;     /* Added for 8 bytes alignment of header */
	struct smem_ram_ptn_v1 parts[RAM_NUM_PART_ENTRIES];
} __attribute__ ((__packed__));

/* Power on reason/status info */
#define PWR_ON_EVENT_RTC_ALARM 0x2
#define PWR_ON_EVENT_USB_CHG   0x20
#define PWR_ON_EVENT_WALL_CHG  0x40

#define SMEM_PTABLE_MAX_PARTS_V3  16
#define SMEM_PTABLE_MAX_PARTS_V4  32
#define SMEM_PTABLE_MAX_PARTS     SMEM_PTABLE_MAX_PARTS_V4

#define SMEM_PTABLE_HDR_LEN    (4*sizeof(unsigned))

struct smem_ptn {
	char name[16];
	unsigned start;
	unsigned size;
	unsigned attr;
} __attribute__ ((__packed__));


struct smem_ptable {
#define _SMEM_PTABLE_MAGIC_1 0x55ee73aa
#define _SMEM_PTABLE_MAGIC_2 0xe35ebddb
	unsigned magic[2];
	unsigned version;
	unsigned len;
	struct smem_ptn parts[SMEM_PTABLE_MAX_PARTS];
} __attribute__ ((__packed__));

typedef struct smem_ram_ptable_v1 ram_partition_table;
typedef struct smem_ram_ptn_v1 ram_partition;

unsigned smem_read_alloc_entry_offset(smem_mem_type_t type, void *buf, int len, int offset);
int smem_ram_ptable_init(struct smem_ram_ptable *smem_ram_ptable);
int smem_ram_ptable_init_v1(void); /* Used on platforms that use ram ptable v1 */
void smem_get_ram_ptable_entry(ram_partition*, UINT32 entry);
UINT32 smem_get_ram_ptable_version(void);
UINT32 smem_get_ram_ptable_len(void);
void* smem_get_alloc_entry(smem_mem_type_t type, UINT32* size);
#endif				/* __PLATFORM_MSM_SHARED_SMEM_H */
