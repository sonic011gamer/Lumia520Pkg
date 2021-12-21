/* Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Foundation nor the names of its
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

#include <Library/LKEnvLib.h>
#include <Chipset/clock.h>
#include <Chipset/clock_pll.h>
#include <Chipset/clock_lib2.h>
#include <Platform/clock.h>
#include <Platform/iomap.h>


/* Mux source select values */
#define cxo_source_val    0
#define gpll0_source_val  1
#define cxo_mm_source_val 0
#define mmpll0_mm_source_val 1
#define mmpll1_mm_source_val 2
#define mmpll3_mm_source_val 3
#define gpll0_mm_source_val 5
/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of The Linux Foundation nor
 *     the names of its contributors may be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <Base.h>
#include <Library/LKEnvLib.h>
#include <Chipset/clock.h>
#include <Chipset/clock_pll.h>
#include <Chipset/clock-local.h>
#include <Platform/iomap.h>
#include <Platform/clock.h>

static int xo_clk_enable(struct clk *clk)
{
	/* Assuming pxo already running */
	return 0;
}

static void xo_clk_disable(struct clk *clk)
{
	/* Do nothing */
}

static struct clk_ops clk_ops_xo = {
	.enable = xo_clk_enable,
	.disable = xo_clk_disable,
	.get_rate = fixed_clk_get_rate,
};

static struct fixed_clk pxo_clk = {
	.rate = 27000000,
	.c = {
		.dbg_name = "pxo_clk",
		.ops = &clk_ops_xo,
	},
};

#ifdef DEBUG_CLOCK
static struct fixed_clk cxo_clk = {
	.rate = 19200000,
	.c = {
		.dbg_name = "cxo_clk",
		.ops = &clk_ops_xo,
	},
};
#endif

/*
 * PLL Clocks
 */
struct clk_ops clk_ops_pll_vote = {
	.enable = pll_vote_clk_enable,
	.disable = pll_vote_clk_disable,
	.is_enabled = pll_vote_clk_is_enabled,
	.get_rate = pll_vote_clk_get_rate,
	.get_parent = pll_vote_clk_get_parent,
};

struct clk_ops clk_ops_pll = {
	.enable = pll_clk_enable,
	.disable = pll_clk_disable,
	.get_rate = pll_clk_get_rate,
	.get_parent = pll_clk_get_parent,
};

static struct pll_clk pll2_clk = {
	.rate = 800000000,
	.mode_reg = (void *)MM_PLL1_MODE_REG,
	.parent = &pxo_clk.c,
	.c = {
		.dbg_name = "pll2_clk",
		.ops = &clk_ops_pll,
	},
};

static struct pll_clk pll3_clk = {
	.rate = 1200000000,
	.mode_reg = (void *)BB_PLL3_MODE_REG,
	.parent = &pxo_clk.c,
	.c = {
		.dbg_name = "pll3_clk",
		.ops = &clk_ops_pll,
	},
};

static struct pll_vote_clk pll8_clk = {
	.rate = 384000000,
	.en_reg = (void *)BB_PLL_ENA_SC0_REG,
	.en_mask = BIT(8),
	.status_reg = (void *)BB_PLL8_STATUS_REG,
	.status_mask = BIT(16),
	.parent = &pxo_clk.c,
	.c = {
		.dbg_name = "pll8_clk",
		.ops = &clk_ops_pll_vote,
	},
};

static struct clk_ops soc_clk_ops_8960 = {
	.enable = local_clk_enable,
	.disable = local_clk_disable,
	.set_rate = local_clk_set_rate,
	.get_rate = local_clk_get_rate,
	.is_enabled = local_clk_is_enabled,
	.round_rate = local_clk_round_rate,
	.get_parent = local_clk_get_parent,
};

static struct clk_ops clk_ops_branch = {
	.enable = branch_clk_enable,
	.disable = branch_clk_disable,
	.is_enabled = branch_clk_is_enabled,
	.get_parent = branch_clk_get_parent,
	.set_parent = branch_clk_set_parent,
};

/*
 * Peripheral Clocks
 */
#define CLK_GSBI_UART(i, n, h_r, h_b) \
	struct rcg_clk i##_clk = { \
		.b = { \
			.ctl_reg = (void *)GSBIn_UART_APPS_NS_REG(n), \
			.en_mask = BIT(9), \
			.reset_reg = (void *)GSBIn_RESET_REG(n), \
			.reset_mask = BIT(0), \
			.halt_reg = (void *)h_r, \
			.halt_bit = h_b, \
		}, \
		.ns_reg = (void *)GSBIn_UART_APPS_NS_REG(n), \
		.md_reg = (void *)GSBIn_UART_APPS_MD_REG(n), \
		.root_en_mask = BIT(11), \
		.ns_mask = (BM(31, 16) | BM(6, 0)), \
		.set_rate = set_rate_mnd, \
		.freq_tbl = clk_tbl_gsbi_uart, \
		.current_freq = &local_dummy_freq, \
		.c = { \
			.dbg_name = #i "_clk", \
			.ops = &soc_clk_ops_8960, \
		}, \
	}
#define F_GSBI_UART(f, s, d, m, n) \
	{ \
		.freq_hz = f, \
		.src_clk = &s##_clk.c, \
		.md_val = MD16(m, n), \
		.ns_val = NS(31, 16, n, m, 5, 4, 3, d, 2, 0, s##_to_bb_mux), \
		.mnd_en_mask = BIT(8) * !!(n), \
	}
static struct clk_freq_tbl clk_tbl_gsbi_uart[] = {
	F_GSBI_UART( 1843200, pll8, 1,  3, 625),
	F_GSBI_UART( 3686400, pll8, 1,  6, 625),
	F_GSBI_UART( 7372800, pll8, 1, 12, 625),
	F_GSBI_UART(14745600, pll8, 1, 24, 625),
	F_GSBI_UART(16000000, pll8, 4,  1,   6),
	F_GSBI_UART(24000000, pll8, 4,  1,   4),
	F_GSBI_UART(32000000, pll8, 4,  1,   3),
	F_GSBI_UART(40000000, pll8, 1,  5,  48),
	F_GSBI_UART(46400000, pll8, 1, 29, 240),
	F_GSBI_UART(48000000, pll8, 4,  1,   2),
	F_GSBI_UART(51200000, pll8, 1,  2,  15),
	F_GSBI_UART(56000000, pll8, 1,  7,  48),
	F_GSBI_UART(58982400, pll8, 1, 96, 625),
	F_GSBI_UART(64000000, pll8, 2,  1,   3),
	F_END
};

static CLK_GSBI_UART(gsbi1_uart,   1, CLK_HALT_CFPB_STATEA_REG, 10);
static CLK_GSBI_UART(gsbi2_uart,   2, CLK_HALT_CFPB_STATEA_REG,  6);
static CLK_GSBI_UART(gsbi3_uart,   3, CLK_HALT_CFPB_STATEA_REG,  2);
static CLK_GSBI_UART(gsbi4_uart,   4, CLK_HALT_CFPB_STATEB_REG, 26);
static CLK_GSBI_UART(gsbi5_uart,   5, CLK_HALT_CFPB_STATEB_REG, 22);
static CLK_GSBI_UART(gsbi6_uart,   6, CLK_HALT_CFPB_STATEB_REG, 18);
static CLK_GSBI_UART(gsbi7_uart,   7, CLK_HALT_CFPB_STATEB_REG, 14);
static CLK_GSBI_UART(gsbi8_uart,   8, CLK_HALT_CFPB_STATEB_REG, 10);
static CLK_GSBI_UART(gsbi9_uart,   9, CLK_HALT_CFPB_STATEB_REG,  6);
static CLK_GSBI_UART(gsbi10_uart, 10, CLK_HALT_CFPB_STATEB_REG,  2);
static CLK_GSBI_UART(gsbi11_uart, 11, CLK_HALT_CFPB_STATEC_REG, 17);
static CLK_GSBI_UART(gsbi12_uart, 12, CLK_HALT_CFPB_STATEC_REG, 13);

#define CLK_GSBI_QUP(i, n, h_r, h_b) \
	struct rcg_clk i##_clk = { \
		.b = { \
			.ctl_reg = (void *)GSBIn_QUP_APPS_NS_REG(n), \
			.en_mask = BIT(9), \
			.reset_reg = (void *)GSBIn_RESET_REG(n), \
			.reset_mask = BIT(0), \
			.halt_reg = (void *)h_r, \
			.halt_bit = h_b, \
		}, \
		.ns_reg = (void *)GSBIn_QUP_APPS_NS_REG(n), \
		.md_reg = (void *)GSBIn_QUP_APPS_MD_REG(n), \
		.root_en_mask = BIT(11), \
		.ns_mask = (BM(23, 16) | BM(6, 0)), \
		.set_rate = set_rate_mnd, \
		.freq_tbl = clk_tbl_gsbi_qup, \
		.current_freq = &local_dummy_freq, \
		.c = { \
			.dbg_name = #i "_clk", \
			.ops = &soc_clk_ops_8960, \
		}, \
	}
#define F_GSBI_QUP(f, s, d, m, n) \
	{ \
		.freq_hz = f, \
		.src_clk = &s##_clk.c, \
		.md_val = MD8(16, m, 0, n), \
		.ns_val = NS(23, 16, n, m, 5, 4, 3, d, 2, 0, s##_to_bb_mux), \
		.mnd_en_mask = BIT(8) * !!(n), \
	}
static struct clk_freq_tbl clk_tbl_gsbi_qup[] = {
	F_GSBI_QUP( 1100000, pxo,  1, 2, 49),
	F_GSBI_QUP( 5400000, pxo,  1, 1,  5),
	F_GSBI_QUP(10800000, pxo,  1, 2,  5),
	F_GSBI_QUP(15060000, pll8, 1, 2, 51),
	F_GSBI_QUP(24000000, pll8, 4, 1,  4),
	F_GSBI_QUP(25600000, pll8, 1, 1, 15),
	F_GSBI_QUP(27000000, pxo,  1, 0,  0),
	F_GSBI_QUP(48000000, pll8, 4, 1,  2),
	F_GSBI_QUP(51200000, pll8, 1, 2, 15),
	F_END
};

static CLK_GSBI_QUP(gsbi1_qup,   1, CLK_HALT_CFPB_STATEA_REG,  9);
static CLK_GSBI_QUP(gsbi2_qup,   2, CLK_HALT_CFPB_STATEA_REG,  4);
static CLK_GSBI_QUP(gsbi3_qup,   3, CLK_HALT_CFPB_STATEA_REG,  0);
static CLK_GSBI_QUP(gsbi4_qup,   4, CLK_HALT_CFPB_STATEB_REG, 24);
static CLK_GSBI_QUP(gsbi5_qup,   5, CLK_HALT_CFPB_STATEB_REG, 20);
static CLK_GSBI_QUP(gsbi6_qup,   6, CLK_HALT_CFPB_STATEB_REG, 16);
static CLK_GSBI_QUP(gsbi7_qup,   7, CLK_HALT_CFPB_STATEB_REG, 12);
static CLK_GSBI_QUP(gsbi8_qup,   8, CLK_HALT_CFPB_STATEB_REG,  8);
static CLK_GSBI_QUP(gsbi9_qup,   9, CLK_HALT_CFPB_STATEB_REG,  4);
static CLK_GSBI_QUP(gsbi10_qup, 10, CLK_HALT_CFPB_STATEB_REG,  0);
static CLK_GSBI_QUP(gsbi11_qup, 11, CLK_HALT_CFPB_STATEC_REG, 15);
static CLK_GSBI_QUP(gsbi12_qup, 12, CLK_HALT_CFPB_STATEC_REG, 11);

#define F_USB(f, s, d, m, n) \
	{ \
		.freq_hz = f, \
		.src_clk = &s##_clk.c, \
		.md_val = MD8(16, m, 0, n), \
		.ns_val = NS(23, 16, n, m, 5, 4, 3, d, 2, 0, s##_to_bb_mux), \
		.mnd_en_mask = BIT(8) * !!(n), \
	}
static struct clk_freq_tbl clk_tbl_usb[] = {
	F_USB(60000000, pll8, 1, 5, 32),
	F_END
};

static struct rcg_clk usb_hs1_xcvr_clk = {
	.b = {
		.ctl_reg = (void *)USB_HS1_XCVR_FS_CLK_NS_REG,
		.en_mask = BIT(9),
		.reset_reg = (void *)USB_HS1_RESET_REG,
		.reset_mask = BIT(0),
		.halt_reg = (void *)CLK_HALT_DFAB_STATE_REG,
		.halt_bit = 0,
	},
	.ns_reg = (void *)USB_HS1_XCVR_FS_CLK_NS_REG,
	.md_reg = (void *)USB_HS1_XCVR_FS_CLK_MD_REG,
	.root_en_mask = BIT(11),
	.ns_mask = (BM(23, 16) | BM(6, 0)),
	.set_rate = set_rate_mnd,
	.freq_tbl = clk_tbl_usb,
	.current_freq = &local_dummy_freq,
	.c = {
		.dbg_name = "usb_hs1_xcvr_clk",
		.ops = &soc_clk_ops_8960,
	},
};

#define CLK_SDC(i, n, h_r, h_c, h_b) \
	struct rcg_clk i##_clk = { \
		.b = { \
			.ctl_reg = (void *)SDCn_APPS_CLK_NS_REG(n), \
			.en_mask = BIT(9), \
			.reset_reg = (void *)SDCn_RESET_REG(n), \
			.reset_mask = BIT(0), \
			.halt_reg = (void *)h_r, \
			.halt_check = h_c, \
			.halt_bit = h_b, \
		}, \
		.ns_reg = (void *)SDCn_APPS_CLK_NS_REG(n), \
		.md_reg = (void *)SDCn_APPS_CLK_MD_REG(n), \
		.root_en_mask = BIT(11), \
		.ns_mask = (BM(23, 16) | BM(6, 0)), \
		.set_rate = set_rate_mnd, \
		.freq_tbl = clk_tbl_sdc, \
		.current_freq = &local_dummy_freq, \
		.c = { \
			.dbg_name = #i "_clk", \
			.ops = &soc_clk_ops_8960, \
		}, \
	}
#define F_SDC(f, s, d, m, n) \
	{ \
		.freq_hz = f, \
		.src_clk = &s##_clk.c, \
		.md_val = MD8(16, m, 0, n), \
		.ns_val = NS(23, 16, n, m, 5, 4, 3, d, 2, 0, s##_to_bb_mux), \
		.mnd_en_mask = BIT(8) * !!(n), \
	}
static struct clk_freq_tbl clk_tbl_sdc[] = {
	F_SDC(   144000, pxo,   3, 2, 125),
	F_SDC(   400000, pll8,  4, 1, 240),
	F_SDC( 16000000, pll8,  4, 1,   6),
	F_SDC( 17070000, pll8,  1, 2,  45),
	F_SDC( 20210000, pll8,  1, 1,  19),
	F_SDC( 24000000, pll8,  4, 1,   4),
	F_SDC( 48000000, pll8,  4, 1,   2),
	F_SDC( 64000000, pll8,  3, 1,   2),
	F_SDC( 96000000, pll8,  4, 0,   0),
	F_SDC(192000000, pll8,  2, 0,   0),
	F_END
};

static CLK_SDC(sdc1, 1, CLK_HALT_DFAB_STATE_REG, HALT, 6);
static CLK_SDC(sdc2, 2, CLK_HALT_DFAB_STATE_REG, HALT, 5);
static CLK_SDC(sdc3, 3, CLK_HALT_DFAB_STATE_REG, HALT, 4);
static CLK_SDC(sdc4, 4, CLK_HALT_DFAB_STATE_REG, HALT, 3);
static CLK_SDC(sdc5, 5, CLK_HALT_DFAB_STATE_REG, HALT, 2);

static struct branch_clk ce1_core_clk = {
	.b = {
		.ctl_reg = (void *)CE1_CORE_CLK_CTL_REG,
		.en_mask = BIT(4),
		.halt_reg = (void *)CLK_HALT_CFPB_STATEC_REG,
		.halt_bit = 27,
	},
	.c = {
		.dbg_name = "ce1_core_clk",
		.ops = &clk_ops_branch,
	},
};
static struct branch_clk ce1_p_clk = {
	.b = {
		.ctl_reg = (void *)CE1_HCLK_CTL_REG,
		.en_mask = BIT(4),
		.halt_reg = (void *)CLK_HALT_CFPB_STATEC_REG,
		.halt_bit = 1,
	},
	.c = {
		.dbg_name = "ce1_p_clk",
		.ops = &clk_ops_branch,
	},
};

#define F_CE(f, s, d) \
	{ \
		.freq_hz = f, \
		.src_clk = &s##_clk.c, \
		.ns_val = NS_DIVSRC(6, 3, d, 2, 0, s##_to_bb_mux), \
	}
static struct clk_freq_tbl clk_tbl_ce3[] = {
	F_CE( 48000000, pll8,  8),
	F_CE(100000000, pll3, 12),
	F_END
};

static struct rcg_clk ce3_src_clk = {
	.b = {
		.ctl_reg = (void *)CE3_CLK_SRC_NS_REG,
		.en_mask = 0,
		.halt_check = NOCHECK,
	},
	.ns_reg = (void *)CE3_CLK_SRC_NS_REG,
	.root_en_mask = BIT(7),
	.ns_mask = BM(6, 0),
	.set_rate = set_rate_nop,
	.freq_tbl = clk_tbl_ce3,
	.current_freq = &local_dummy_freq,
	.c = {
		.dbg_name = "ce3_src_clk",
		.ops = &soc_clk_ops_8960,
	},
};

static struct branch_clk ce3_core_clk = {
	.b = {
		.ctl_reg = (void *)CE3_CORE_CLK_CTL_REG,
		.en_mask = BIT(4),
		.halt_reg = (void *)CLK_HALT_GSS_KPSS_MISC_STATE_REG,
		.halt_bit = 5,
	},
	.parent = &ce3_src_clk.c,
	.c = {
		.dbg_name = "ce3_core_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk ce3_p_clk = {
	.b = {
		.ctl_reg = (void *)CE3_HCLK_CTL_REG,
		.en_mask = BIT(4),
		.halt_reg = (void *)CLK_HALT_AFAB_SFAB_STATEB_REG,
		.halt_bit = 16,
	},
	.parent = &ce3_src_clk.c,
	.c = {
		.dbg_name = "ce3_p_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk gsbi1_p_clk = {
	.b = {
		.ctl_reg = (void *)GSBIn_HCLK_CTL_REG(1),
		.en_mask = BIT(4),
		.halt_reg = (void *)CLK_HALT_CFPB_STATEA_REG,
		.halt_bit = 11,
	},
	.c = {
		.dbg_name = "gsbi1_p_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk gsbi2_p_clk = {
	.b = {
		.ctl_reg = (void *)GSBIn_HCLK_CTL_REG(2),
		.en_mask = BIT(4),
		.halt_reg = (void *)CLK_HALT_CFPB_STATEA_REG,
		.halt_bit = 7,
	},
	.c = {
		.dbg_name = "gsbi2_p_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk gsbi3_p_clk = {
	.b = {
		.ctl_reg = (void *)GSBIn_HCLK_CTL_REG(3),
		.en_mask = BIT(4),
		.halt_reg = (void *)CLK_HALT_CFPB_STATEA_REG,
		.halt_bit = 3,
	},
	.c = {
		.dbg_name = "gsbi3_p_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk gsbi4_p_clk = {
	.b = {
		.ctl_reg = (void *)GSBIn_HCLK_CTL_REG(4),
		.en_mask = BIT(4),
		.halt_reg = (void *)CLK_HALT_CFPB_STATEB_REG,
		.halt_bit = 27,
	},
	.c = {
		.dbg_name = "gsbi4_p_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk gsbi5_p_clk = {
	.b = {
		.ctl_reg = (void *)GSBIn_HCLK_CTL_REG(5),
		.en_mask = BIT(4),
		.halt_reg = (void *)CLK_HALT_CFPB_STATEB_REG,
		.halt_bit = 23,
	},
	.c = {
		.dbg_name = "gsbi5_p_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk gsbi6_p_clk = {
	.b = {
		.ctl_reg = (void *)GSBIn_HCLK_CTL_REG(6),
		.en_mask = BIT(4),
		.halt_reg = (void *)CLK_HALT_CFPB_STATEB_REG,
		.halt_bit = 19,
	},
	.c = {
		.dbg_name = "gsbi6_p_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk gsbi7_p_clk = {
	.b = {
		.ctl_reg = (void *)GSBIn_HCLK_CTL_REG(7),
		.en_mask = BIT(4),
		.halt_reg = (void *)CLK_HALT_CFPB_STATEB_REG,
		.halt_bit = 15,
	},
	.c = {
		.dbg_name = "gsbi7_p_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk gsbi8_p_clk = {
	.b = {
		.ctl_reg = (void *)GSBIn_HCLK_CTL_REG(8),
		.en_mask = BIT(4),
		.halt_reg = (void *)CLK_HALT_CFPB_STATEB_REG,
		.halt_bit = 11,
	},
	.c = {
		.dbg_name = "gsbi8_p_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk gsbi9_p_clk = {
	.b = {
		.ctl_reg = (void *)GSBIn_HCLK_CTL_REG(9),
		.en_mask = BIT(4),
		.halt_reg = (void *)CLK_HALT_CFPB_STATEB_REG,
		.halt_bit = 7,
	},
	.c = {
		.dbg_name = "gsbi9_p_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk gsbi10_p_clk = {
	.b = {
		.ctl_reg = (void *)GSBIn_HCLK_CTL_REG(10),
		.en_mask = BIT(4),
		.halt_reg = (void *)CLK_HALT_CFPB_STATEB_REG,
		.halt_bit = 3,
	},
	.c = {
		.dbg_name = "gsbi10_p_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk gsbi11_p_clk = {
	.b = {
		.ctl_reg = (void *)GSBIn_HCLK_CTL_REG(11),
		.en_mask = BIT(4),
		.halt_reg = (void *)CLK_HALT_CFPB_STATEC_REG,
		.halt_bit = 18,
	},
	.c = {
		.dbg_name = "gsbi11_p_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk gsbi12_p_clk = {
	.b = {
		.ctl_reg = (void *)GSBIn_HCLK_CTL_REG(12),
		.en_mask = BIT(4),
		.halt_reg = (void *)CLK_HALT_CFPB_STATEC_REG,
		.halt_bit = 14,
	},
	.c = {
		.dbg_name = "gsbi12_p_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk mdp_axi_clk = {
	.b = {
		.ctl_reg = (void *)MAXI_EN_REG,
		.en_mask = BIT(23),
		.reset_reg = (void *)SW_RESET_AXI_REG,
		.reset_mask = BIT(13),
		.halt_reg = (void *)DBG_BUS_VEC_E_REG,
		.halt_bit = 8,
	},
	.c = {
		.dbg_name = "mdp_axi_clk",
		.ops = &clk_ops_branch,
	},
};

#define F_MDP(f, s, m, n) \
	{ \
		.freq_hz = f, \
		.src_clk = &s##_clk.c, \
		.md_val = MD8(8, m, 0, n), \
		.ns_val = NS_MND_BANKED8(22, 14, n, m, 3, 0, s##_to_mm_mux), \
		.ctl_val = CC_BANKED(9, 6, n), \
		.mnd_en_mask = (BIT(8) | BIT(5)) * !!(n), \
	}
static struct clk_freq_tbl clk_tbl_mdp[] = {
	F_MDP(  9600000, pll8, 1, 40),
	F_MDP( 13710000, pll8, 1, 28),
	F_MDP( 27000000, pxo,  0,  0),
	F_MDP( 29540000, pll8, 1, 13),
	F_MDP( 34910000, pll8, 1, 11),
	F_MDP( 38400000, pll8, 1, 10),
	F_MDP( 59080000, pll8, 2, 13),
	F_MDP( 76800000, pll8, 1,  5),
	F_MDP( 85330000, pll8, 2,  9),
	F_MDP( 96000000, pll8, 1,  4),
	F_MDP(128000000, pll8, 1,  3),
	F_MDP(160000000, pll2, 1,  5),
	F_MDP(177780000, pll2, 2,  9),
	F_MDP(200000000, pll2, 1,  4),
	F_END
};

static struct bank_masks bmnd_info_mdp = {
	.bank_sel_mask =		BIT(11),
	.bank0_mask = {
			.md_reg =		(void *)MDP_MD0_REG,
			.ns_mask =		BM(29, 22) | BM(5, 3),
			.rst_mask =		BIT(31),
			.mnd_en_mask =		BIT(8),
			.mode_mask =		BM(10, 9),
	},
	.bank1_mask = {
			.md_reg =		(void *)MDP_MD1_REG,
			.ns_mask =		BM(21, 14) | BM(2, 0),
			.rst_mask =		BIT(30),
			.mnd_en_mask =		BIT(5),
			.mode_mask =		BM(7, 6),
	},
};

static struct rcg_clk mdp_clk = {
	.b = {
		.ctl_reg = (void *)MDP_CC_REG,
		.en_mask = BIT(0),
		.reset_reg = (void *)SW_RESET_CORE_REG,
		.reset_mask = BIT(21),
		.halt_reg = (void *)DBG_BUS_VEC_C_REG,
		.halt_bit = 10,
	},
	.ns_reg = (void *)MDP_NS_REG,
	.root_en_mask = BIT(2),
	.set_rate = set_rate_mnd_banked,
	.freq_tbl = clk_tbl_mdp,
	.bank_masks = &bmnd_info_mdp,
	.depends = &mdp_axi_clk.c,
	.current_freq = &local_dummy_freq,
	.c = {
		.dbg_name = "mdp_clk",
		.ops = &soc_clk_ops_8960,
	},
};

static struct branch_clk lut_mdp_clk = {
	.b = {
		.ctl_reg = (void *)MDP_LUT_CC_REG,
		.en_mask = BIT(0),
		.halt_reg = (void *)DBG_BUS_VEC_I_REG,
		.halt_bit = 13,
	},
	.parent = &mdp_clk.c,
	.c = {
		.dbg_name = "lut_mdp_clk",
		.ops = &clk_ops_branch,
	},
};

#ifdef DEBUG_CLOCK
struct measure_sel {
	uint32_t test_vector;
	struct clk *clk;
};

static struct measure_sel measure_mux[] = {
	{ TEST_PER_LS(0x13), &sdc1_clk.c },
	{ TEST_PER_LS(0x15), &sdc2_clk.c },
	{ TEST_PER_LS(0x17), &sdc3_clk.c },
	{ TEST_PER_LS(0x19), &sdc4_clk.c },
	{ TEST_PER_LS(0x1B), &sdc5_clk.c },
	{ TEST_PER_LS(0x3D), &gsbi1_p_clk.c },
	{ TEST_PER_LS(0x3E), &gsbi1_uart_clk.c },
	{ TEST_PER_LS(0x3F), &gsbi1_qup_clk.c },
	{ TEST_PER_LS(0x41), &gsbi2_p_clk.c },
	{ TEST_PER_LS(0x42), &gsbi2_uart_clk.c },
	{ TEST_PER_LS(0x44), &gsbi2_qup_clk.c },
	{ TEST_PER_LS(0x45), &gsbi3_p_clk.c },
	{ TEST_PER_LS(0x46), &gsbi3_uart_clk.c },
	{ TEST_PER_LS(0x48), &gsbi3_qup_clk.c },
	{ TEST_PER_LS(0x49), &gsbi4_p_clk.c },
	{ TEST_PER_LS(0x4A), &gsbi4_uart_clk.c },
	{ TEST_PER_LS(0x4C), &gsbi4_qup_clk.c },
	{ TEST_PER_LS(0x4D), &gsbi5_p_clk.c },
	{ TEST_PER_LS(0x4E), &gsbi5_uart_clk.c },
	{ TEST_PER_LS(0x50), &gsbi5_qup_clk.c },
	{ TEST_PER_LS(0x51), &gsbi6_p_clk.c },
	{ TEST_PER_LS(0x52), &gsbi6_uart_clk.c },
	{ TEST_PER_LS(0x54), &gsbi6_qup_clk.c },
	{ TEST_PER_LS(0x55), &gsbi7_p_clk.c },
	{ TEST_PER_LS(0x56), &gsbi7_uart_clk.c },
	{ TEST_PER_LS(0x58), &gsbi7_qup_clk.c },
	{ TEST_PER_LS(0x59), &gsbi8_p_clk.c },
	{ TEST_PER_LS(0x5A), &gsbi8_uart_clk.c },
	{ TEST_PER_LS(0x5C), &gsbi8_qup_clk.c },
	{ TEST_PER_LS(0x5D), &gsbi9_p_clk.c },
	{ TEST_PER_LS(0x5E), &gsbi9_uart_clk.c },
	{ TEST_PER_LS(0x60), &gsbi9_qup_clk.c },
	{ TEST_PER_LS(0x61), &gsbi10_p_clk.c },
	{ TEST_PER_LS(0x62), &gsbi10_uart_clk.c },
	{ TEST_PER_LS(0x64), &gsbi10_qup_clk.c },
	{ TEST_PER_LS(0x65), &gsbi11_p_clk.c },
	{ TEST_PER_LS(0x66), &gsbi11_uart_clk.c },
	{ TEST_PER_LS(0x68), &gsbi11_qup_clk.c },
	{ TEST_PER_LS(0x69), &gsbi12_p_clk.c },
	{ TEST_PER_LS(0x6A), &gsbi12_uart_clk.c },
	{ TEST_PER_LS(0x6C), &gsbi12_qup_clk.c },
	{ TEST_PER_LS(0x85), &usb_hs1_xcvr_clk.c },
	{ TEST_PER_LS(0x92), &ce1_p_clk.c },
	{ TEST_PER_LS(0xA4), &ce1_core_clk.c },
	{ TEST_PER_LS(0x5F), &ce3_p_clk.c },
	{ TEST_PER_LS(0x60), &ce3_core_clk.c },

	{ TEST_MM_HS(0x15), &mdp_axi_clk.c },
	{ TEST_MM_HS(0x1A), &mdp_clk.c },
	{ TEST_MM_HS(0x28), &lut_mdp_clk.c },
};

static struct measure_sel *find_measure_sel(struct clk *clk)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(measure_mux); i++)
		if (measure_mux[i].clk == clk)
			return &measure_mux[i];
	return NULL;
}

static int measure_clk_set_parent(struct clk *c, struct clk *parent)
{
	int ret = 0;
	uint32_t clk_sel;
	struct measure_sel *p;
	struct measure_clk *clk = to_measure_clk(c);

	if (!parent)
		return ERR_INVALID_ARGS;

	p = find_measure_sel(parent);
	if (!p)
		return ERR_INVALID_ARGS;

	/*
	 * Program the test vector, measurement period (sample_ticks)
	 * and scaling multiplier.
	 */
	clk->sample_ticks = 0x10000;
	clk_sel = p->test_vector & TEST_CLK_SEL_MASK;
	clk->multiplier = 1;
	switch (p->test_vector >> TEST_TYPE_SHIFT) {
	case TEST_TYPE_PER_LS:
		writel_relaxed(0x4030D00|BVAL(7, 0, clk_sel), CLK_TEST_REG);
		break;
	case TEST_TYPE_PER_HS:
		writel_relaxed(0x4020000|BVAL(16, 10, clk_sel), CLK_TEST_REG);
		break;
	case TEST_TYPE_MM_LS:
		writel_relaxed(0x4030D97, CLK_TEST_REG);
		writel_relaxed(BVAL(6, 1, clk_sel)|BIT(0), DBG_CFG_REG_LS_REG);
		break;
	case TEST_TYPE_MM_HS:
		writel_relaxed(0x402B800, CLK_TEST_REG);
		writel_relaxed(BVAL(6, 1, clk_sel)|BIT(0), DBG_CFG_REG_HS_REG);
		break;
	default:
		ret = ERR_NOT_SUPPORTED;
	}
	/* Make sure test vector is set before starting measurements. */
	dmb();

	return ret;
}

/* Sample clock for 'ticks' reference clock ticks. */
static uint32_t run_measurement(unsigned ticks)
{
	/* Stop counters and set the XO4 counter start value. */
	writel_relaxed(ticks, RINGOSC_TCXO_CTL_REG);

	/* Wait for timer to become ready. */
	while ((readl_relaxed(RINGOSC_STATUS_REG) & BIT(25)) != 0)
		dmb();

	/* Run measurement and wait for completion. */
	writel_relaxed(BIT(20)|ticks, RINGOSC_TCXO_CTL_REG);
	while ((readl_relaxed(RINGOSC_STATUS_REG) & BIT(25)) == 0)
		dmb();

	/* Stop counters. */
	writel_relaxed(0x0, RINGOSC_TCXO_CTL_REG);

	/* Return measured ticks. */
	return readl_relaxed(RINGOSC_STATUS_REG) & BM(24, 0);
}


/* Perform a hardware rate measurement for a given clock.
   FOR DEBUG USE ONLY: Measurements take ~15 ms! */
static unsigned measure_clk_get_rate(struct clk *c)
{
	uint32_t pdm_reg_backup, ringosc_reg_backup;
	uint64_t raw_count_short, raw_count_full;
	struct measure_clk *clk = to_measure_clk(c);
	unsigned ret;

	ret = clk_enable(&cxo_clk.c);
	if (ret) {
		dprintf(CRITICAL, "CXO clock failed to enable. Can't measure\n");
		return 0;
	}

	/* Enable CXO/4 and RINGOSC branch and root. */
	pdm_reg_backup = readl_relaxed(PDM_CLK_NS_REG);
	ringosc_reg_backup = readl_relaxed(RINGOSC_NS_REG);
	writel_relaxed(0x2898, PDM_CLK_NS_REG);
	writel_relaxed(0xA00, RINGOSC_NS_REG);

	/*
	 * The ring oscillator counter will not reset if the measured clock
	 * is not running.  To detect this, run a short measurement before
	 * the full measurement.  If the raw results of the two are the same
	 * then the clock must be off.
	 */

	/* Run a short measurement. (~1 ms) */
	raw_count_short = run_measurement(0x1000);
	/* Run a full measurement. (~14 ms) */
	raw_count_full = run_measurement(clk->sample_ticks);

	writel_relaxed(ringosc_reg_backup, RINGOSC_NS_REG);
	writel_relaxed(pdm_reg_backup, PDM_CLK_NS_REG);

	/* Return 0 if the clock is off. */
	if (raw_count_full == raw_count_short)
		ret = 0;
	else {
		/* Compute rate in Hz. */
		raw_count_full = ((raw_count_full * 10) + 15) * 4800000;
		raw_count_full /= ((clk->sample_ticks * 10) + 35);
		ret = (raw_count_full * clk->multiplier);
	}

	/* Route dbg_hs_clk to PLLTEST.  300mV single-ended amplitude. */
	writel_relaxed(0x38F8, PLLTEST_PAD_CFG_REG);

	clk_disable(&cxo_clk.c);

	return ret;
}
#else
static int measure_clk_set_parent(struct clk *clk, struct clk *parent)
{
	return ERR_INVALID_ARGS;
}

static unsigned measure_clk_get_rate(struct clk *clk)
{
	return 0;
}
#endif

static struct clk_ops measure_clk_ops = {
	.set_parent = measure_clk_set_parent,
	.get_rate = measure_clk_get_rate,
};

static struct measure_clk measure_clk = {
	.c = {
		.dbg_name = "measure_clk",
		.ops = &measure_clk_ops,
	},
	.multiplier = 1,
};

static struct clk_lookup msm_clocks_8960[] = {
	CLK_LOOKUP("gsbi1_uart_clk",	gsbi1_uart_clk.c),
	CLK_LOOKUP("gsbi2_uart_clk",	gsbi2_uart_clk.c),
	CLK_LOOKUP("gsbi3_uart_clk",	gsbi3_uart_clk.c),
	CLK_LOOKUP("gsbi4_uart_clk",	gsbi4_uart_clk.c),
	CLK_LOOKUP("gsbi5_uart_clk",	gsbi5_uart_clk.c),
	CLK_LOOKUP("gsbi6_uart_clk",	gsbi6_uart_clk.c),
	CLK_LOOKUP("gsbi7_uart_clk",	gsbi7_uart_clk.c),
	CLK_LOOKUP("gsbi8_uart_clk",	gsbi8_uart_clk.c),
	CLK_LOOKUP("gsbi9_uart_clk",	gsbi9_uart_clk.c),
	CLK_LOOKUP("gsbi10_uart_clk",	gsbi10_uart_clk.c),
	CLK_LOOKUP("gsbi11_uart_clk",	gsbi11_uart_clk.c),
	CLK_LOOKUP("gsbi12_uart_clk",	gsbi12_uart_clk.c),
	CLK_LOOKUP("gsbi1_qup_clk",		gsbi1_qup_clk.c),
	CLK_LOOKUP("gsbi2_qup_clk",		gsbi2_qup_clk.c),
	CLK_LOOKUP("gsbi3_qup_clk",		gsbi3_qup_clk.c),
	CLK_LOOKUP("gsbi4_qup_clk",		gsbi4_qup_clk.c),
	CLK_LOOKUP("gsbi5_qup_clk",		gsbi5_qup_clk.c),
	CLK_LOOKUP("gsbi6_qup_clk",		gsbi6_qup_clk.c),
	CLK_LOOKUP("gsbi7_qup_clk",		gsbi7_qup_clk.c),
	CLK_LOOKUP("gsbi8_qup_clk",		gsbi8_qup_clk.c),
	CLK_LOOKUP("gsbi9_qup_clk",		gsbi9_qup_clk.c),
	CLK_LOOKUP("gsbi10_qup_clk",	gsbi10_qup_clk.c),
	CLK_LOOKUP("gsbi11_qup_clk",	gsbi11_qup_clk.c),
	CLK_LOOKUP("gsbi12_qup_clk",	gsbi12_qup_clk.c),
	CLK_LOOKUP("gsbi1_pclk",		gsbi1_p_clk.c),
	CLK_LOOKUP("gsbi2_pclk",		gsbi2_p_clk.c),
	CLK_LOOKUP("gsbi3_pclk",		gsbi3_p_clk.c),
	CLK_LOOKUP("gsbi4_pclk",		gsbi4_p_clk.c),
	CLK_LOOKUP("gsbi5_pclk",		gsbi5_p_clk.c),
	CLK_LOOKUP("gsbi6_pclk",		gsbi6_p_clk.c),
	CLK_LOOKUP("gsbi7_pclk",		gsbi7_p_clk.c),
	CLK_LOOKUP("gsbi8_pclk",		gsbi8_p_clk.c),
	CLK_LOOKUP("gsbi9_pclk",		gsbi9_p_clk.c),
	CLK_LOOKUP("gsbi10_pclk",		gsbi10_p_clk.c),
	CLK_LOOKUP("gsbi11_pclk",		gsbi11_p_clk.c),
	CLK_LOOKUP("gsbi12_pclk",		gsbi12_p_clk.c),
	CLK_LOOKUP("usb_hs_clk",		usb_hs1_xcvr_clk.c),
	CLK_LOOKUP("sdc1_clk",			sdc1_clk.c),
	CLK_LOOKUP("sdc2_clk",			sdc2_clk.c),
	CLK_LOOKUP("sdc3_clk",			sdc3_clk.c),
	CLK_LOOKUP("sdc4_clk",			sdc4_clk.c),
	CLK_LOOKUP("sdc5_clk",			sdc5_clk.c),
	CLK_LOOKUP("mdp_axi_clk",		mdp_axi_clk.c),
	CLK_LOOKUP("mdp_clk",			mdp_clk.c),
	CLK_LOOKUP("lut_mdp",			lut_mdp_clk.c),
	CLK_LOOKUP("ce1_pclk",			ce1_p_clk.c),
	CLK_LOOKUP("ce1_clk",			ce1_core_clk.c),
	CLK_LOOKUP("ce3_src_clk",		ce3_src_clk.c),
	CLK_LOOKUP("ce3_pclk",			ce3_p_clk.c),
	CLK_LOOKUP("ce3_clk",			ce3_core_clk.c),
	CLK_LOOKUP("measure",			measure_clk.c),

};

static int sr_pll_clk_enable(struct clk *clk)
{
	uint32_t mode;
	struct pll_clk *pll = to_pll_clk(clk);

	mode = readl_relaxed(pll->mode_reg);
	/* De-assert active-low PLL reset. */
	mode |= BIT(2);
	writel_relaxed(mode, pll->mode_reg);

	/*
	 * H/W requires a 5us delay between disabling the bypass and
	 * de-asserting the reset. Delay 10us just to be safe.
	 */
	dmb();
	udelay(10);

	/* Disable PLL bypass mode. */
	mode |= BIT(1);
	writel_relaxed(mode, pll->mode_reg);

	/* Wait until PLL is locked. */
	dmb();
	udelay(60);

	/* Enable PLL output. */
	mode |= BIT(0);
	writel_relaxed(mode, pll->mode_reg);

	return 0;
}

static unsigned msm_num_clocks_8960 = ARRAY_SIZE(msm_clocks_8960);

RETURN_STATUS
EFIAPI
LibQcomPlatformClockInit (
  struct clk_lookup **clist,
  unsigned *num
  )
{
	clk_ops_pll.enable = sr_pll_clk_enable;
	*clist = msm_clocks_8960;
	*num = msm_num_clocks_8960;

	return RETURN_SUCCESS;
}


struct clk_freq_tbl rcg_dummy_freq = F_END;


/* Clock Operations */
static struct clk_ops clk_ops_branch =
{
	.enable     = clock_lib2_branch_clk_enable,
	.disable    = clock_lib2_branch_clk_disable,
	.set_rate   = clock_lib2_branch_set_rate,
};

static struct clk_ops clk_ops_rcg_mnd =
{
	.enable     = clock_lib2_rcg_enable,
	.set_rate   = clock_lib2_rcg_set_rate,
};

static struct clk_ops clk_ops_rcg =
{
	.enable     = clock_lib2_rcg_enable,
	.set_rate   = clock_lib2_rcg_set_rate,
};

static struct clk_ops clk_ops_cxo =
{
	.enable     = cxo_clk_enable,
	.disable    = cxo_clk_disable,
};

static struct clk_ops clk_ops_pll_vote =
{
	.enable     = pll_vote_clk_enable,
	.disable    = pll_vote_clk_disable,
	.auto_off   = pll_vote_clk_disable,
	.is_enabled = pll_vote_clk_is_enabled,
};

static struct clk_ops clk_ops_vote =
{
	.enable     = clock_lib2_vote_clk_enable,
	.disable    = clock_lib2_vote_clk_disable,
};

/* Clock Sources */
static struct fixed_clk cxo_clk_src =
{
	.c = {
		.rate     = 19200000,
		.dbg_name = "cxo_clk_src",
		.ops      = &clk_ops_cxo,
	},
};

static struct pll_vote_clk gpll0_clk_src =
{
	.en_reg       = (void *) APCS_GPLL_ENA_VOTE,
	.en_mask      = BIT(0),
	.status_reg   = (void *) GPLL0_STATUS,
	.status_mask  = BIT(17),
	.parent       = &cxo_clk_src.c,

	.c = {
		.rate     = 600000000,
		.dbg_name = "gpll0_clk_src",
		.ops      = &clk_ops_pll_vote,
	},
};

/* SDCC Clocks */
static struct clk_freq_tbl ftbl_gcc_sdcc1_2_apps_clk[] =
{
	F(   144000,    cxo,   16,   3,  25),
	F(   400000,    cxo,   12,   1,   4),
	F( 20000000,  gpll0,  15,   1,   2),
	F( 25000000,  gpll0,  12,   1,   2),
	F( 50000000,  gpll0,  12,   0,   0),
	F(100000000,  gpll0,   6,   0,   0),
	F(200000000,  gpll0,   3,   0,   0),
	F_END
};

static struct rcg_clk sdcc1_apps_clk_src =
{
	.cmd_reg      = (uint32_t *) SDCC1_CMD_RCGR,
	.cfg_reg      = (uint32_t *) SDCC1_CFG_RCGR,
	.m_reg        = (uint32_t *) SDCC1_M,
	.n_reg        = (uint32_t *) SDCC1_N,
	.d_reg        = (uint32_t *) SDCC1_D,

	.set_rate     = clock_lib2_rcg_set_rate_mnd,
	.freq_tbl     = ftbl_gcc_sdcc1_2_apps_clk,
	.current_freq = &rcg_dummy_freq,

	.c = {
		.dbg_name = "sdc1_clk",
		.ops      = &clk_ops_rcg_mnd,
	},
};

static struct branch_clk gcc_sdcc1_apps_clk =
{
	.cbcr_reg     = (uint32_t *) SDCC1_APPS_CBCR,
	.parent       = &sdcc1_apps_clk_src.c,

	.c = {
		.dbg_name = "gcc_sdcc1_apps_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk gcc_sdcc1_ahb_clk =
{
	.cbcr_reg     = (uint32_t *) SDCC1_AHB_CBCR,
	.has_sibling  = 1,

	.c = {
		.dbg_name = "gcc_sdcc1_ahb_clk",
		.ops      = &clk_ops_branch,
	},
};

/* SDCC2 clocks */

static struct rcg_clk sdcc2_apps_clk_src =
{
	.cmd_reg      = (uint32_t *) SDCC2_CMD_RCGR,
	.cfg_reg      = (uint32_t *) SDCC2_CFG_RCGR,
	.m_reg        = (uint32_t *) SDCC2_M,
	.n_reg        = (uint32_t *) SDCC2_N,
	.d_reg        = (uint32_t *) SDCC2_D,

	.set_rate     = clock_lib2_rcg_set_rate_mnd,
	.freq_tbl     = ftbl_gcc_sdcc1_2_apps_clk,
	.current_freq = &rcg_dummy_freq,

	.c = {
		.dbg_name = "sdc2_clk",
		.ops      = &clk_ops_rcg_mnd,
	},
};

static struct branch_clk gcc_sdcc2_apps_clk =
{
	.cbcr_reg     = (uint32_t *) SDCC2_APPS_CBCR,
	.parent       = &sdcc2_apps_clk_src.c,

	.c = {
		.dbg_name = "gcc_sdcc2_apps_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk gcc_sdcc2_ahb_clk =
{
	.cbcr_reg     = (uint32_t *) SDCC2_AHB_CBCR,
	.has_sibling  = 1,

	.c = {
		.dbg_name = "gcc_sdcc2_ahb_clk",
		.ops      = &clk_ops_branch,
	},
};

/* UART Clocks */
static struct clk_freq_tbl ftbl_gcc_blsp1_2_uart1_6_apps_clk[] =
{
	F( 3686400,  gpll0,    1,  96,  15625),
	F( 7372800,  gpll0,    1, 192,  15625),
	F(14745600,  gpll0,    1, 384,  15625),
	F(16000000,  gpll0,    5,   2,     15),
	F(19200000,    cxo,    1,   0,      0),
	F(24000000,  gpll0,    5,   1,      5),
	F(32000000,  gpll0,    1,   4,     75),
	F(40000000,  gpll0,   15,   0,      0),
	F(46400000,  gpll0,    1,  29,    375),
	F(48000000,  gpll0, 12.5,   0,      0),
	F(51200000,  gpll0,    1,  32,    375),
	F(56000000,  gpll0,    1,   7,     75),
	F(58982400,  gpll0,    1, 1536, 15625),
	F(60000000,  gpll0,   10,   0,      0),
	F_END
};


static struct rcg_clk blsp1_uart2_apps_clk_src =
{
	.cmd_reg      = (uint32_t *) BLSP1_UART2_APPS_CMD_RCGR,
	.cfg_reg      = (uint32_t *) BLSP1_UART2_APPS_CFG_RCGR,
	.m_reg        = (uint32_t *) BLSP1_UART2_APPS_M,
	.n_reg        = (uint32_t *) BLSP1_UART2_APPS_N,
	.d_reg        = (uint32_t *) BLSP1_UART2_APPS_D,

	.set_rate     = clock_lib2_rcg_set_rate_mnd,
	.freq_tbl     = ftbl_gcc_blsp1_2_uart1_6_apps_clk,
	.current_freq = &rcg_dummy_freq,

	.c = {
		.dbg_name = "blsp1_uart2_apps_clk",
		.ops      = &clk_ops_rcg_mnd,
	},
};

static struct branch_clk gcc_blsp1_uart2_apps_clk =
{
	.cbcr_reg     = (uint32_t *) BLSP1_UART2_APPS_CBCR,
	.parent       = &blsp1_uart2_apps_clk_src.c,

	.c = {
		.dbg_name = "gcc_blsp1_uart2_apps_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct vote_clk gcc_blsp1_ahb_clk = {
	.cbcr_reg     = (uint32_t *) BLSP1_AHB_CBCR,
	.vote_reg     = (uint32_t *) APCS_CLOCK_BRANCH_ENA_VOTE,
	.en_mask      = BIT(17),

	.c = {
		.dbg_name = "gcc_blsp1_ahb_clk",
		.ops      = &clk_ops_vote,
	},
};

/* USB Clocks */
static struct clk_freq_tbl ftbl_gcc_usb_hs_system_clk[] =
{
	F(75000000,  gpll0,   8,   0,   0),
	F_END
};

static struct rcg_clk usb_hs_system_clk_src =
{
	.cmd_reg      = (uint32_t *) USB_HS_SYSTEM_CMD_RCGR,
	.cfg_reg      = (uint32_t *) USB_HS_SYSTEM_CFG_RCGR,

	.set_rate     = clock_lib2_rcg_set_rate_hid,
	.freq_tbl     = ftbl_gcc_usb_hs_system_clk,
	.current_freq = &rcg_dummy_freq,

	.c = {
		.dbg_name = "usb_hs_system_clk",
		.ops      = &clk_ops_rcg,
	},
};

static struct branch_clk gcc_usb_hs_system_clk =
{
	.cbcr_reg     = (uint32_t *) USB_HS_SYSTEM_CBCR,
	.parent       = &usb_hs_system_clk_src.c,

	.c = {
		.dbg_name = "gcc_usb_hs_system_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk gcc_usb_hs_ahb_clk =
{
	.cbcr_reg     = (uint32_t *) USB_HS_AHB_CBCR,
	.has_sibling  = 1,

	.c = {
		.dbg_name = "gcc_usb_hs_ahb_clk",
		.ops      = &clk_ops_branch,
	},
};

/* Diplay related clock LUT */
static struct clk_freq_tbl ftbl_mmss_axi_clk[] = {
	F_MM(19200000,     cxo,     1,   0,   0),
	F_MM(100000000,  gpll0,     6,   0,   0),
	F_MM(200000000,  gpll0,     3,   0,   0),
	F_END
};

static struct clk_freq_tbl ftbl_mdss_vsync_clk[] = {
	F_MM(19200000,    cxo,   1,   0,   0),
	F_END
};

static struct rcg_clk vsync_clk_src = {
	.cmd_reg  = (uint32_t *) VSYNC_CMD_RCGR,
	.cfg_reg  = (uint32_t *) VSYNC_CFG_RCGR,
	.set_rate = clock_lib2_rcg_set_rate_hid,
	.freq_tbl = ftbl_mdss_vsync_clk,
	.c        = {
		.dbg_name = "vsync_clk_src",
		.ops      = &clk_ops_rcg,
	},
};

static struct rcg_clk axi_clk_src = {
	.cmd_reg  = (uint32_t *) AXI_CMD_RCGR,
	.cfg_reg  = (uint32_t *) AXI_CFG_RCGR,
	.set_rate = clock_lib2_rcg_set_rate_hid,
	.freq_tbl = ftbl_mmss_axi_clk,
	.c        = {
		.dbg_name = "axi_clk_src",
		.ops      = &clk_ops_rcg,
	},
};

static struct branch_clk mdp_ahb_clk = {
	.cbcr_reg    = (uint32_t *) MDP_AHB_CBCR,
	.has_sibling = 1,
	.c           = {
		.dbg_name = "mdp_ahb_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk mdp_axi_clk = {
	.cbcr_reg    = (uint32_t *) MDP_AXI_CBCR,
	.parent      = &axi_clk_src.c,
	.c           = {
		.dbg_name = "mdp_axi_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk mdp_dsi_clk = {
	.cbcr_reg    = (uint32_t *) MDP_DSI_CBCR,
	.c           = {
		.dbg_name = "mdp_dsi_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk mmss_mmssnoc_axi_clk = {
	.cbcr_reg    = (uint32_t *) MMSS_MMSSNOC_AXI_CBCR,
	.parent      = &axi_clk_src.c,
	.c           = {
		.dbg_name = "mmss_mmssnoc_axi_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk mmss_s0_axi_clk = {
	.cbcr_reg    = (uint32_t *) MMSS_S0_AXI_CBCR,
	.parent      = &axi_clk_src.c,
	.c           = {
		.dbg_name = "mmss_s0_axi_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk mdp_vsync_clk = {
	.cbcr_reg    = (uint32_t *) MDP_VSYNC_CBCR,
	.parent      = &vsync_clk_src.c,
	.c           = {
		.dbg_name = "mdp_vsync_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk mdp_lcdc_clk = {
	.cbcr_reg    = (uint32_t *) MDP_LCDC_CBCR,
	.parent      = &axi_clk_src.c,
	.c           = {
		.dbg_name = "mdp_lcdc_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk dsi_clk = {
	.cbcr_reg = (uint32_t *) DSI_CBCR,
	.c = {
		.dbg_name = "dsi_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk dsi_ahb_clk = {
	.cbcr_reg = (uint32_t *) DSI_AHB_CBCR,
	.has_sibling = 1,
	.c = {
		.dbg_name = "dsi_ahb_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk dsi_byte_clk = {
	.cbcr_reg = (uint32_t *) DSI_BYTE_CBCR,
	.c = {
		.dbg_name = "dsi_byte_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk dsi_esc_clk = {
	.cbcr_reg = (uint32_t *) DSI_ESC_CBCR,
	.c = {
		.dbg_name = "dsi_esc_clk",
		.ops = &clk_ops_branch,
	},
};

static struct branch_clk dsi_pclk_clk = {
	.cbcr_reg = (uint32_t *) DSI_PCLK_CBCR,
	.c = {
		.dbg_name = "dsi_pclk_clk",
		.ops = &clk_ops_branch,
	},
};

static struct clk_freq_tbl ftbl_gcc_ce1_clk[] = {
	F( 50000000,  gpll0,  12,   0,   0),
	F(100000000,  gpll0,   6,   0,   0),
	F_END
};

static struct rcg_clk ce1_clk_src = {
	.cmd_reg      = (uint32_t *) GCC_CE1_CMD_RCGR,
	.cfg_reg      = (uint32_t *) GCC_CE1_CFG_RCGR,
	.set_rate     = clock_lib2_rcg_set_rate_hid,
	.freq_tbl     = ftbl_gcc_ce1_clk,
	.current_freq = &rcg_dummy_freq,

	.c = {
		.dbg_name = "ce1_clk_src",
		.ops      = &clk_ops_rcg,
	},
};

static struct vote_clk gcc_ce1_clk = {
	.cbcr_reg      = (uint32_t *) GCC_CE1_CBCR,
	.vote_reg      = (uint32_t *) APCS_CLOCK_BRANCH_ENA_VOTE,
	.en_mask       = BIT(5),

	.c = {
		.dbg_name  = "gcc_ce1_clk",
		.ops       = &clk_ops_vote,
	},
};

static struct vote_clk gcc_ce1_ahb_clk = {
	.cbcr_reg     = (uint32_t *) GCC_CE1_AHB_CBCR,
	.vote_reg     = (uint32_t *) APCS_CLOCK_BRANCH_ENA_VOTE,
	.en_mask      = BIT(3),

	.c = {
		.dbg_name = "gcc_ce1_ahb_clk",
		.ops      = &clk_ops_vote,
	},
};

static struct vote_clk gcc_ce1_axi_clk = {
	.cbcr_reg     = (uint32_t *) GCC_CE1_AXI_CBCR,
	.vote_reg     = (uint32_t *) APCS_CLOCK_BRANCH_ENA_VOTE,
	.en_mask      = BIT(4),

	.c = {
		.dbg_name = "gcc_ce1_axi_clk",
		.ops      = &clk_ops_vote,
	},
};

/* Clock lookup table */
static struct clk_lookup msm_clocks_8612[] =
{
	CLK_LOOKUP("sdc1_iface_clk", gcc_sdcc1_ahb_clk.c),
	CLK_LOOKUP("sdc1_core_clk",  gcc_sdcc1_apps_clk.c),

	CLK_LOOKUP("sdc2_iface_clk", gcc_sdcc2_ahb_clk.c),
	CLK_LOOKUP("sdc2_core_clk",  gcc_sdcc2_apps_clk.c),

	CLK_LOOKUP("uart2_iface_clk", gcc_blsp1_ahb_clk.c),
	CLK_LOOKUP("uart2_core_clk",  gcc_blsp1_uart2_apps_clk.c),

	CLK_LOOKUP("usb_iface_clk",  gcc_usb_hs_ahb_clk.c),
	CLK_LOOKUP("usb_core_clk",   gcc_usb_hs_system_clk.c),

	CLK_LOOKUP("axi_clk_src",          axi_clk_src.c),
	CLK_LOOKUP("mmss_mmssnoc_axi_clk", mmss_mmssnoc_axi_clk.c),
	CLK_LOOKUP("mmss_s0_axi_clk",      mmss_s0_axi_clk.c),
	CLK_LOOKUP("mdp_axi_clk",          mdp_axi_clk.c),
	CLK_LOOKUP("mdp_dsi_clk",          mdp_dsi_clk.c),
	CLK_LOOKUP("mdp_vsync_clk",        mdp_vsync_clk.c),
	CLK_LOOKUP("mdp_lcdc_clk",         mdp_lcdc_clk.c),
	CLK_LOOKUP("mdp_ahb_clk",          mdp_ahb_clk.c),

	CLK_LOOKUP("dsi_clk",              dsi_clk.c),
	CLK_LOOKUP("dsi_ahb_clk",          dsi_ahb_clk.c),
	CLK_LOOKUP("dsi_byte_clk",         dsi_byte_clk.c),
	CLK_LOOKUP("dsi_esc_clk",          dsi_esc_clk.c),
	CLK_LOOKUP("dsi_pclk_clk",         dsi_pclk_clk.c),

	CLK_LOOKUP("ce1_ahb_clk",  gcc_ce1_ahb_clk.c),
	CLK_LOOKUP("ce1_axi_clk",  gcc_ce1_axi_clk.c),
	CLK_LOOKUP("ce1_core_clk", gcc_ce1_clk.c),
	CLK_LOOKUP("ce1_src_clk",  ce1_clk_src.c),
};

RETURN_STATUS
EFIAPI
LibQcomPlatformClockInit (
  struct clk_lookup **clist,
  unsigned *num
  )
{

	*clist = msm_clocks_8612;
	*num = ARRAY_SIZE(msm_clocks_8612);

	return RETURN_SUCCESS;
}