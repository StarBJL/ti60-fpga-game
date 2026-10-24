// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vhdmi_dualvideo_tb.h for the primary calling header

#include "Vhdmi_dualvideo_tb__pch.h"

void Vhdmi_dualvideo_tb___024root___timing_ready(Vhdmi_dualvideo_tb___024root* vlSelf);

VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___eval_static(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_static\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    {
        // Inlined CFunc: _eval_static__TOP
        vlSelfRef.hdmi_dualvideo_tb__DOT__clk = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__pix_clk = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__rst = 1U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rst = 1U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__pix_rst_n = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__apb_psel = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__apb_penable = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwrite = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwdata = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__axi_arready = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rvalid = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rlast = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rdata = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__wp = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__rp = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__beat = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__syscycle = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__ar_count = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__r_count = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__rng = 0x5a937213U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__pixels = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__frames = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__mode_changes = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__avi720 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__avi1080 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__old_mode = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_expect0 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_expect1 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_expect2 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_check0 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_check1 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_check2 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_pixels = 0U;
    }
    vlSelfRef.__VactTriggered[0U] = (0x0000000000000020ULL 
                                     | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (0x0000000000000040ULL 
                                     | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (0x0000000000000080ULL 
                                     | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (0x0000000000000100ULL 
                                     | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__clk__0 = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__pix_clk__0 = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__pix_rst_n__0 = 0U;
    vlSelfRef.__Vtrigprevexpr_h8b77f5eb__1 = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__frame_en) 
                                               & (0x0168U 
                                                  == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt))) 
                                              & (0x0190U 
                                                 == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt)));
    vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080__0 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080;
    vlSelfRef.__Vtrigprevexpr_h98362db4__1 = ((0x021cU 
                                               == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt)) 
                                              & (0x01f4U 
                                                 == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt)));
    vlSelfRef.__Vtrigprevexpr_h7d65cb35__1 = 0U;
    Vhdmi_dualvideo_tb___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vhdmi_dualvideo_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);
VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___stl_sequent__TOP__0(Vhdmi_dualvideo_tb___024root* vlSelf);

VL_ATTR_COLD bool Vhdmi_dualvideo_tb___024root___eval_stl(Vhdmi_dualvideo_tb___024root* vlSelf, CData/*0:0*/ firstIteration) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_stl\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(firstIteration)));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vhdmi_dualvideo_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vhdmi_dualvideo_tb___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        {
            // Inlined CFunc: _eval_body__stl
            if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
                Vhdmi_dualvideo_tb___024root___stl_sequent__TOP__0(vlSelf);
            }
        }
    }
    return (__VstlExecute);
}

VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___eval_dump_triggers__stl(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_dump_triggers__stl\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
#ifdef VL_DEBUG
    Vhdmi_dualvideo_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___eval_dump_triggers__ico(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_dump_triggers__ico\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
#ifdef VL_DEBUG
    Vhdmi_dualvideo_tb___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
#endif
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___eval_dump_triggers__act(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_dump_triggers__act\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
#ifdef VL_DEBUG
    Vhdmi_dualvideo_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
}

VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___eval_dump_triggers__nba(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_dump_triggers__nba\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
#ifdef VL_DEBUG
    Vhdmi_dualvideo_tb___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
}

VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___eval_dump_triggers__obs(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_dump_triggers__obs\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___eval_dump_triggers__react(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_dump_triggers__react\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___eval_final(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_final\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vhdmi_dualvideo_tb___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vhdmi_dualvideo_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___trigger_anySet__stl\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

extern const VlWide<8>/*255:0*/ Vhdmi_dualvideo_tb__ConstPool__CONST_ha457a04f_0;
extern const VlWide<64>/*2047:0*/ Vhdmi_dualvideo_tb__ConstPool__CONST_h54f42bc6_0;

VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___stl_sequent__TOP__0(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___stl_sequent__TOP__0\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ hdmi_dualvideo_tb__DOT__dut__DOT__fifo_rempty;
    hdmi_dualvideo_tb__DOT__dut__DOT__fifo_rempty = 0;
    CData/*0:0*/ hdmi_dualvideo_tb__DOT__dut__DOT__afifo_rempty;
    hdmi_dualvideo_tb__DOT__dut__DOT__afifo_rempty = 0;
    SData/*15:0*/ hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tone_s;
    hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tone_s = 0;
    CData/*0:0*/ hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__s_avi;
    hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__s_avi = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_3;
    __VdfgRegularize_h6e95ff9d_0_3 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_5;
    __VdfgRegularize_h6e95ff9d_0_5 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_6;
    __VdfgRegularize_h6e95ff9d_0_6 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_7;
    __VdfgRegularize_h6e95ff9d_0_7 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_8;
    __VdfgRegularize_h6e95ff9d_0_8 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_9;
    __VdfgRegularize_h6e95ff9d_0_9 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_11;
    __VdfgRegularize_h6e95ff9d_0_11 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_12;
    __VdfgRegularize_h6e95ff9d_0_12 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_13;
    __VdfgRegularize_h6e95ff9d_0_13 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_14;
    __VdfgRegularize_h6e95ff9d_0_14 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_15;
    __VdfgRegularize_h6e95ff9d_0_15 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_17;
    __VdfgRegularize_h6e95ff9d_0_17 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_18;
    __VdfgRegularize_h6e95ff9d_0_18 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_19;
    __VdfgRegularize_h6e95ff9d_0_19 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_20;
    __VdfgRegularize_h6e95ff9d_0_20 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_21;
    __VdfgRegularize_h6e95ff9d_0_21 = 0;
    // Body
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_rise 
        = (IData)((2U == (6U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_s))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_fall 
        = (IData)((4U == (6U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_s))));
    if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080) {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__out_htot = 0x0898U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__out_vss = 0x043cU;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__word_start 
            = (1U & (0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_x)));
    } else {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__out_htot = 0x0672U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__out_vss = 0x02d5U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__word_start 
            = (1U & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt)));
    }
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_c 
        = ((2U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx)) 
           | ((0x19U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx)) 
              | (0x21U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ar_hs 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_arvalid) 
           & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_arready));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT____Vcellinp__u_fifo__rrst 
        = (1U & ((~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                     >> 3U)) | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_zone)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs0 
        = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt) 
            >= ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                 ? 0x043cU : 0x02d5U)) & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt) 
                                          < ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                              ? 0x0441U
                                              : 0x02daU)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_raddr 
        = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_word) 
            << 1U) | (2U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_x)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__latch 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
           == ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                ? 0x0778U : 0x04f8U));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_waddr 
        = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__cache_addr_d) 
            << 1U) | (1U & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__cache_write_d))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0a 
        = ((0x83U & (- (IData)((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q0) 
                                      ^ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0)))))) 
           ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0) 
              >> 1U));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1a 
        = ((0x83U & (- (IData)((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1) 
                                      ^ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1)))))) 
           ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1) 
              >> 1U));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2a 
        = ((0x83U & (- (IData)((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2) 
                                      ^ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2)))))) 
           ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2) 
              >> 1U));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3a 
        = ((0x83U & (- (IData)((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3) 
                                      ^ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3)))))) 
           ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3) 
              >> 1U));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_wdata 
        = (0x0000ffffU & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__cache_write_d)
                            ? vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fifo_rdata
                            : (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fifo_rdata 
                               >> 0x00000010U)) & (- (IData)(
                                                             (1U 
                                                              & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__cache_bad_d)))))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__bar_rgb 
        = (0x00ffffffU & ((0x0eU == (0x0000000fU & 
                                     ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v1) 
                                      >> 5U))) ? ((0x00ff0000U 
                                                   & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h1) 
                                                      << 0x0000000eU)) 
                                                  | ((0x0000ff00U 
                                                      & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h1) 
                                                         << 6U)) 
                                                     | (0x000000ffU 
                                                        & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h1) 
                                                           >> 2U))))
                           : Vhdmi_dualvideo_tb__ConstPool__CONST_ha457a04f_0
                          [(7U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h1) 
                                  >> 7U))]));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision2 
        = ((0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__cnt)) 
           | ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n1q_m) 
              == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n0q_m)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision3 
        = (((~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__cnt) 
                >> 4U)) & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n1q_m) 
                           > (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n0q_m))) 
           | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__cnt) 
               >> 4U) & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n0q_m) 
                         > (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n1q_m))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision2 
        = ((0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__cnt)) 
           | ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n1q_m) 
              == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n0q_m)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision3 
        = (((~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__cnt) 
                >> 4U)) & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n1q_m) 
                           > (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n0q_m))) 
           | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__cnt) 
               >> 4U) & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n0q_m) 
                         > (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n1q_m))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision2 
        = ((0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__cnt)) 
           | ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n1q_m) 
              == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n0q_m)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision3 
        = (((~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__cnt) 
                >> 4U)) & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n1q_m) 
                           > (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n0q_m))) 
           | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__cnt) 
               >> 4U) & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n0q_m) 
                         > (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n1q_m))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4 = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__lhdmi) 
                                                & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt) 
                                                    < 
                                                    ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                                      ? 0x0437U
                                                      : 0x02cfU)) 
                                                   | ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt) 
                                                      == 
                                                      ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                                        ? 0x0464U
                                                        : 0x02edU))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__start 
        = ((~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_s) 
               >> 1U)) & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__start_pending) 
                          & ((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_arvalid)) 
                             & (0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__outstanding)))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__dma_rst 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__rst) 
           | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rst));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__pix565 
        = (0x0000ffffU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__repeat_d)
                           ? ((0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_read_bank))
                               ? (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_pixel0)
                               : ((1U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_read_bank))
                                   ? (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_pixel1)
                                   : (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_pixel2)))
                           : ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__hodd1)
                               ? (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fifo_rdata 
                                  >> 0x00000010U) : vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fifo_rdata)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wdo 
        = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wgray) 
            != ((0x00000c00U & ((~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2) 
                                    >> 0x0000000aU)) 
                                << 0x0000000aU)) | 
                (0x000003ffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2)))) 
           & ((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__drop_mode)) 
              & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rvalid)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kpres 
        = (0x0000000fU & (((1U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcnt))
                            ? 1U : ((2U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcnt))
                                     ? 3U : (7U | (- (IData)(
                                                             (3U 
                                                              != (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcnt))))))) 
                          & (- (IData)((0U != (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcnt))))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__s_acr 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_due) 
           & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__actl_p2) 
              >> 1U));
    hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__s_avi 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt) 
           == ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                ? 0x0438U : 0x02d0U));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__lty 
        = ((0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__slot))
            ? (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ty0)
            : ((1U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__slot))
                ? (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ty1)
                : (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ty2)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__iend 
        = (0x00000fffU & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ln) 
                           << 5U) + ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                      ? 0x078eU : 0x050eU)));
    hdmi_dualvideo_tb__DOT__dut__DOT__fifo_rempty = 
        ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray) 
         == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wgray_r2));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__apb_wr 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_psel) 
           & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_penable) 
              & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwrite)));
    hdmi_dualvideo_tb__DOT__dut__DOT__afifo_rempty 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray) 
           == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wgray_r2));
    hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tone_s 
        = (0x0000ffffU & Vhdmi_dualvideo_tb__ConstPool__CONST_h54f42bc6_0
           [(0x07ffffffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tone_ph))]);
    __VdfgRegularize_h6e95ff9d_0_3 = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tick) 
                                      & (2U == (6U 
                                                & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__actl_p2))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__repeat_line 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080) 
           & (1U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_y)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__de0 
        = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
            < ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                ? 0x0780U : 0x0500U)) & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt) 
                                         < ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                             ? 0x0438U
                                             : 0x02d0U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_tone 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tick) 
           & (6U == (6U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__actl_p2))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision1 
        = ((4U < (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n1d)) 
           | ((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q)) 
              & (4U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n1d))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision1 
        = ((4U < (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n1d)) 
           | ((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q)) 
              & (4U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n1d))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision1 
        = ((4U < (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n1d)) 
           | ((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q)) 
              & (4U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n1d))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fifo_wrst 
        = (1U & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_s) 
                  >> 1U) | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__dma_rst)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wbin_n 
        = (0x00000fffU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wbin) 
                          + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wdo)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0 = ((((4U 
                                                   & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__actl_p2) 
                                                      << 2U)) 
                                                  | (3U 
                                                     & (2U 
                                                        | (- (IData)(
                                                                     (1U 
                                                                      & (~ (IData)(hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__s_avi)))))))) 
                                                 << 3U) 
                                                | ((((IData)(hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__s_avi) 
                                                     | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__actl_p2) 
                                                         >> 1U) 
                                                        & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt) 
                                                           == 
                                                           ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                                             ? 0x0439U
                                                             : 0x02d1U)))) 
                                                    << 2U) 
                                                   | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__s_acr) 
                                                       << 1U) 
                                                      | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__actl_p2) 
                                                          >> 1U) 
                                                         & (0U 
                                                            != (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pcnt))))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__l_s0 
        = ((0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__lty))
            ? ((((QData)((IData)(((2U & ((VL_REDXOR_16(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kr0) 
                                          ^ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb)) 
                                         << 1U)) | 
                                  (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb) 
                                         ^ VL_REDXOR_16(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kl0)))))) 
                 << 0x00000036U) | (((QData)((IData)(
                                                     (1U 
                                                      & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb)))) 
                                     << 0x00000035U) 
                                    | (((QData)((IData)(
                                                        (1U 
                                                         & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb)))) 
                                        << 0x00000034U) 
                                       | (((QData)((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kr0)) 
                                           << 0x00000020U) 
                                          | (QData)((IData)(
                                                            ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kl0) 
                                                             << 8U))))))) 
               & (- (QData)((IData)((1U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kpres))))))
            : ((1U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__lty))
                ? 0x0000180087220100ULL : ((2U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__lty))
                                            ? (((QData)((IData)(
                                                                ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                                                  ? 0x22U
                                                                  : 4U))) 
                                                << 0x00000020U) 
                                               | (QData)((IData)(
                                                                 (0x00280000U 
                                                                  | (0x000000ffU 
                                                                     & (- 
                                                                        ((IData)(0xb9U) 
                                                                         + 
                                                                         ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                                                           ? 0x22U
                                                                           : 4U))))))))
                                            : 0x0000000000000170ULL)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__idat0 
        = ((0U != (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ln)) 
           & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
               >= ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                    ? 0x078eU : 0x050eU)) & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                                             < (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__iend))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__aud_apb_push 
        = (IData)(((0x0030U == (0x00fcU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr))) 
                   & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__apb_wr)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__afifo_ren 
        = ((~ (IData)(hdmi_dualvideo_tb__DOT__dut__DOT__afifo_rempty)) 
           & (IData)(__VdfgRegularize_h6e95ff9d_0_3));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__source_req 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__de0) 
           & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__word_start) 
              & ((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__repeat_line)) 
                 & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__frame_en))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_tone) 
           | ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ren_d) 
              | ((IData)(hdmi_dualvideo_tb__DOT__dut__DOT__afifo_rempty) 
                 & (IData)(__VdfgRegularize_h6e95ff9d_0_3))));
    if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ren_d) {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_l 
            = (0x0000ffffU & vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__afifo_rdata);
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_r 
            = (0x0000ffffU & (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__afifo_rdata 
                              >> 0x00000010U));
    } else {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_l 
            = (0x0000ffffU & ((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__actl_p2) 
                                >> 4U) & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_tone))
                               ? (IData)(hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tone_s)
                               : 0U));
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_r 
            = (0x0000ffffU & ((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__actl_p2) 
                                >> 5U) & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_tone))
                               ? (IData)(hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tone_s)
                               : 0U));
    }
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__load0 
        = ((0U == (0x0000001fU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                                  - ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                      ? 0x078eU : 0x050eU)))) 
           & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__idat0));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wdo 
        = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wgray) 
            != ((0x00000180U & ((~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray_w2) 
                                    >> 7U)) << 7U)) 
                | (0x0000007fU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray_w2)))) 
           & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__aud_apb_push));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rbin_n 
        = (0x000001ffU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rbin) 
                          + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__afifo_ren)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf0 
        = ((IData)(hdmi_dualvideo_tb__DOT__dut__DOT__fifo_rempty) 
           & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__source_req));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fifo_ren 
        = ((~ (IData)(hdmi_dualvideo_tb__DOT__dut__DOT__fifo_rempty)) 
           & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__source_req));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm1 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision1)
                  ? (~ VL_REDXOR_32((3U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q))))
                  : VL_REDXOR_32((3U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q)))));
    __VdfgRegularize_h6e95ff9d_0_5 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm1) 
                                            ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q) 
                                               >> 2U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm1 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision1)
                  ? (~ VL_REDXOR_32((3U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q))))
                  : VL_REDXOR_32((3U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q)))));
    __VdfgRegularize_h6e95ff9d_0_11 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm1) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q) 
                                                >> 2U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm1 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision1)
                  ? (~ VL_REDXOR_32((3U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q))))
                  : VL_REDXOR_32((3U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q)))));
    __VdfgRegularize_h6e95ff9d_0_17 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm1) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q) 
                                                >> 2U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wbin_n 
        = (0x000001ffU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wbin) 
                          + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wdo)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rbin_n 
        = (0x00000fffU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rbin) 
                          + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fifo_ren)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm2 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_5))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_5)));
    __VdfgRegularize_h6e95ff9d_0_6 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm2) 
                                            ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q) 
                                               >> 3U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm2 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_11))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_11)));
    __VdfgRegularize_h6e95ff9d_0_12 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm2) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q) 
                                                >> 3U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm2 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_17))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_17)));
    __VdfgRegularize_h6e95ff9d_0_18 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm2) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q) 
                                                >> 3U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm3 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_6))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_6)));
    __VdfgRegularize_h6e95ff9d_0_7 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm3) 
                                            ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q) 
                                               >> 4U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm3 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_12))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_12)));
    __VdfgRegularize_h6e95ff9d_0_13 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm3) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q) 
                                                >> 4U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm3 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_18))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_18)));
    __VdfgRegularize_h6e95ff9d_0_19 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm3) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q) 
                                                >> 4U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm4 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_7))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_7)));
    __VdfgRegularize_h6e95ff9d_0_8 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm4) 
                                            ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q) 
                                               >> 5U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm4 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_13))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_13)));
    __VdfgRegularize_h6e95ff9d_0_14 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm4) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q) 
                                                >> 5U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm4 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_19))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_19)));
    __VdfgRegularize_h6e95ff9d_0_20 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm4) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q) 
                                                >> 5U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm5 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_8))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_8)));
    __VdfgRegularize_h6e95ff9d_0_9 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm5) 
                                            ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q) 
                                               >> 6U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm6 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_9))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_9)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm5 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_14))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_14)));
    __VdfgRegularize_h6e95ff9d_0_15 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm5) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q) 
                                                >> 6U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm6 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_15))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_15)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm5 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_20))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_20)));
    __VdfgRegularize_h6e95ff9d_0_21 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm5) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q) 
                                                >> 6U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm6 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_21))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_21)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_10 = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm6) 
                                                 ^ 
                                                 ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q) 
                                                  >> 7U));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16 = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm6) 
                                                 ^ 
                                                 ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q) 
                                                  >> 7U));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_22 = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm6) 
                                                 ^ 
                                                 ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q) 
                                                  >> 7U));
}

bool Vhdmi_dualvideo_tb___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___dump_triggers__ico\n"); );
    // Body
    if ((1U & (~ (IData)(Vhdmi_dualvideo_tb___024root___trigger_anySet__ico(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'ico' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

bool Vhdmi_dualvideo_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vhdmi_dualvideo_tb___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge hdmi_dualvideo_tb.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge hdmi_dualvideo_tb.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @(posedge hdmi_dualvideo_tb.pix_clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @(negedge hdmi_dualvideo_tb.pix_rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 4U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 4 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
    if ((1U & (IData)((triggers[0U] >> 5U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 5 is active: @( ((hdmi_dualvideo_tb.dut.frame_en & (12'h168 == hdmi_dualvideo_tb.dut.v_cnt)) & (12'h190 == hdmi_dualvideo_tb.dut.h_cnt)))\n");
    }
    if ((1U & (IData)((triggers[0U] >> 6U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 6 is active: @( hdmi_dualvideo_tb.dut.mode_1080)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 7U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 7 is active: @( ((12'h21c == hdmi_dualvideo_tb.dut.v_cnt) & (12'h1f4 == hdmi_dualvideo_tb.dut.h_cnt)))\n");
    }
    if ((1U & (IData)((triggers[0U] >> 8U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 8 is active: @( ((32'sh2 == hdmi_dualvideo_tb.mode_changes) & (32'sh3 == hdmi_dualvideo_tb.frames)))\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___ctor_var_reset(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___ctor_var_reset\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->hdmi_dualvideo_tb__DOT__axi_arvalid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10351729693932430931ull);
    vlSelf->hdmi_dualvideo_tb__DOT__axi_araddr = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 11620281036613470687ull);
    vlSelf->hdmi_dualvideo_tb__DOT__tmds_d0 = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 16125060484699202783ull);
    vlSelf->hdmi_dualvideo_tb__DOT__tmds_d1 = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 12771976736304250925ull);
    vlSelf->hdmi_dualvideo_tb__DOT__tmds_d2 = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 11191700908577637036ull);
    for (int __Vi0 = 0; __Vi0 < 65536; ++__Vi0) {
        vlSelf->hdmi_dualvideo_tb__DOT__addresses[__Vi0] = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 17275429810836860412ull);
    }
    for (int __Vi0 = 0; __Vi0 < 65536; ++__Vi0) {
        vlSelf->hdmi_dualvideo_tb__DOT__due[__Vi0] = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 14742041402726004913ull);
    }
    vlSelf->hdmi_dualvideo_tb__DOT__old_x = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 8907501363077192468ull);
    vlSelf->hdmi_dualvideo_tb__DOT__old_y = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 9164654013632488087ull);
    vlSelf->hdmi_dualvideo_tb__DOT__sx = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 8300928499164266924ull);
    vlSelf->hdmi_dualvideo_tb__DOT__sy = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 4532974382961854614ull);
    vlSelf->hdmi_dualvideo_tb__DOT__expected_count = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 6954543325654230084ull);
    vlSelf->hdmi_dualvideo_tb__DOT__check_pixel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18428478661788867832ull);
    vlSelf->hdmi_dualvideo_tb__DOT__expected_rgb = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 3575662155424462266ull);
    vlSelf->hdmi_dualvideo_tb__DOT__sum = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 2508199041002759736ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__ctrl_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16806751521465930184ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__ctrl_test = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6048206206061649424ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__aud_ctrl = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 10425002136920323502ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__en_p_s = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 2865053668141159677ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__test_p_s = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 6381874189401448921ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__actl_p1 = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 285336338982343888ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__actl_p2 = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 16425918445987861054ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__mode_req = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15144778600928690569ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__mode_req_p = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 14621820319656637073ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15378033585139713269ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__out_htot = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 15567577356966847260ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__out_vss = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 13978824788204201234ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 16421856624820540575ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 11432858106513313029ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__de0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 175456574183221466ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__vs0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10939979394240906188ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__de1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4896357151054194310ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__hs1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5938219517770871190ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__vs1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17338553894918020483ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__hodd1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 507468947858577566ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__uf1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3086818833096696974ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__h1 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 2309835339446293204ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__v1 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 10893950438066981239ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__vs_zone = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4760986474745380151ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__frame_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7727854041427202251ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__fifo_rdata = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 8520597623072228713ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__scale_x = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 8494623647229292692ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__scale_y = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 3937416862178186300ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__scale_word = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 15148716590647545273ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__repeat_line = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9071789565244237558ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__word_start = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8860220597326310563ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__source_req = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14837043103681725523ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__fifo_ren = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7772565640593477614ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__uf0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16303702166179816817ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__cache_write_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13167130335168581895ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__cache_bad_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13841788613831823450ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__repeat_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5409096907436564714ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__uf_word = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14797098027781911361ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__cache_addr_d = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 18259080594322193715ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__uf_line = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2418966676893235024ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__uf_tog = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5270368060521457892ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__pix565 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 1858152698631327370ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__bar_rgb = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 2821478990986983328ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__vpre1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17714730949034910881ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__vgb1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1094795020921926966ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__ipre1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2869569943652344511ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__igb1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15232815276996079086ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__idat1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4460172587109553277ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__ifirst1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8961656329159539583ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__afifo_ren = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1160914996447106816ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__afifo_rdata = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 1343233307566443678ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__r2 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12700705790676066987ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__g2 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17450389975943565061ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__b2 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2278821326444867124ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__m0_2 = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 5873962086466502789ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__m1_2 = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 3265511420851943486ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__m2_2 = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 10182957522456855415ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__c0_2 = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 5437379345063794548ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__c1_2 = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 10973788861974211666ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__c2_2 = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 315585597988474791ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__t0_2 = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 9831022826196925621ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__t1_2 = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12807348126621749514ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__t2_2 = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 16757245821507892370ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__f0_2 = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 574599658643147118ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__f1_2 = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 15534858851328835781ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__f2_2 = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 14111721899566179028ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__vs_s = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 17792256569106146118ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__uf_s = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 9114313090776175469ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__vs_rise = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2290912356285000821ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__vs_fall = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8343627824459935743ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__fb_pending = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 5721503209771495223ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__fb_cur = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 12821683937620326625ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__uf_cnt = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 16232314061822054730ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__present_cnt = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 10208945204666451421ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__repeat_cnt = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 5733973307096338503ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__active_scan_cnt = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 13935456828107202923ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__apb_wr = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15097906588438961991ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__aud_apb_push = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7822577785148141753ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__dma_rst = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3213470094093634163ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__outstanding = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 6052205182083187824ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__bursts_left = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 5374595368978256633ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__running = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5328291540764880166ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__drop_mode = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7779838449297393915ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__start_pending = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18184918975312099871ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__ar_hs = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6125720299504675378ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__start = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3641803981618137373ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__fifo_wrst = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7100796496098133183ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT____Vcellinp__u_fifo__rrst = 0;
    for (int __Vi0 = 0; __Vi0 < 512; ++__Vi0) {
        vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache0[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 16939158702684022118ull);
    }
    for (int __Vi0 = 0; __Vi0 < 512; ++__Vi0) {
        vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache1[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 13870452169444375684ull);
    }
    for (int __Vi0 = 0; __Vi0 < 512; ++__Vi0) {
        vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache2[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 6542412433235833551ull);
    }
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_pixel0 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 1202702422074270541ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_pixel1 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 1875789770717094316ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_pixel2 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 3338468708570773960ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_read_bank = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 5065687740456242620ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_high_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2540145672145745964ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_raddr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 10536732621346067363ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_waddr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 7557095924471959034ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_wdata = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 16664023782776386353ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n1d = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 3308323236824772698ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8498335328907999245ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__vid_a = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15371909103948508605ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__vid_b = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1846537515974380477ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__sym_a = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 6817738697678080255ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__sym_b = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 6102042029210941100ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11822905887884909285ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5199852849061660903ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6956119567277565402ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm3 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8748998052803718441ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm4 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2034748934712750095ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm5 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6606940790562491963ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm6 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4514343155686271355ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n1q_m = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 5515776611205169268ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n0q_m = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2449502702528677959ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__q_m_reg = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 8639717785049297524ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__cnt = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 805115078666775045ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2063322837051318913ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision3 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7890649157943579909ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n1d = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 5067586361641044716ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11167840909238272106ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__vid_a = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6437208136506136762ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__vid_b = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17746143505415579552ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__sym_a = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 15898977830356526617ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__sym_b = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 17833004369362870206ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4757815014619104118ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7377047380277064193ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17947948969208337854ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm3 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7484015561954649754ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm4 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11678851807109070126ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm5 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12748789383619317155ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm6 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1739354935103774020ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n1q_m = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 16694337299862838091ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n0q_m = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 8513509002700316149ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__q_m_reg = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 11793952526568906699ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__cnt = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 7900730956317010109ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15504617175953498636ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision3 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13508123155939895248ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n1d = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 17433453851911264811ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13980996514922433202ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__vid_a = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14227588483759151219ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__vid_b = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12151873668905843233ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__sym_a = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 14999705667233903502ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__sym_b = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 6504999920177048045ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3349901737939948487ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11459301864930411790ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10302707159356598373ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm3 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11993383701958983638ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm4 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2591350631819664418ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm5 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14811937351030981983ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm6 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8645645152891681981ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n1q_m = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 9283632737885133915ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n0q_m = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 692993105543758729ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__q_m_reg = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 11578425653377036784ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__cnt = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 12960813215166024833ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15268395094441474683ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision3 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10957347061475819361ull);
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 5730717437150593292ull);
    }
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wbin = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 12278772275282348506ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wgray = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 7497050739520324291ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rbin = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 1945367282401216581ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 11529818776395368280ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray_w1 = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 8109099336126158673ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray_w2 = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 3542742876367518216ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wgray_r1 = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 13886294083896151123ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wgray_r2 = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 12744678779237454044ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wdo = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13974318623013039749ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wbin_n = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 6388866046420806474ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rbin_n = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 16554534716963867016ull);
    for (int __Vi0 = 0; __Vi0 < 2048; ++__Vi0) {
        vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 8722720965701526155ull);
    }
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wbin = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 12130169004006591870ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wgray = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 8795751449048924151ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rbin = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 17151771200994059874ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 7507713604874461132ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w1 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 17748721159424396230ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 14481294929053642034ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wgray_r1 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 6894338365300457098ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wgray_r2 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 6906025998039661195ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wdo = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4729340035179308967ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wbin_n = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 51025973974236876ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rbin_n = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 7322632453229808466ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acc = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 10013755155691119388ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tick = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8889629321639066799ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_cnt = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 10030319796460355297ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_due = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14604421872636292055ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tone_ph = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 6661955468319025051ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ren_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7776456900186123456ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_tone = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14370831711461054622ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5891877615958552905ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_l = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 12560065701138285606ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_r = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11142321886333198805ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3086529401349494023ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_c = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18021031985031236670ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pl0 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 3268447385071010409ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pl1 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 2174663933216610262ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pl2 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 8989119587025441323ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pl3 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 9129463699068224999ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pr0 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11302030946140689347ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pr1 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 1619891811006386459ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pr2 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 8769154188029253336ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pr3 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 7263170486808297376ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cb0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16888392540041572702ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cb1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18155572988056847319ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cb2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16184308836452280035ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cb3 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6047561775582806504ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__bb0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12704469853813733952ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__bb1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12327230828316915295ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__bb2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12908145477628084909ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__bb3 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7834211270995815410ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pcnt = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 10927046370262590035ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kl0 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11733862400101803797ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kl1 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11135468632824791172ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kl2 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10196295861508291719ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kl3 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 18325142377549238954ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kr0 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 14517050966822271890ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kr1 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 267771084253494932ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kr2 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 675529454603006325ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kr3 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 3739654776654905486ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2899336268535468530ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kbb = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 9715164574430640979ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcnt = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 8726564684329522696ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__latch = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 352119627441477677ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__s_acr = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17277102265229086655ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ln = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 4090903178558960149ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ty0 = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 1440081938419261495ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ty1 = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 14668211864894960075ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ty2 = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 13927569202256648788ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__lhdmi = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3577974341628290994ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__slot = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 563411831812434890ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__iend = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 8915343372874639130ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__idat0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 742153990027546082ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__load0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5804029259673624418ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__lty = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 5151353608934048394ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kpres = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 15339482497836094825ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__l_s0 = VL_SCOPED_RAND_RESET_Q(56, __VscopeHash, 651790477029547748ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pc = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 2731491576382865257ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__hsr = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 4423332071802975721ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__eh = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3175401362666027276ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q0 = VL_SCOPED_RAND_RESET_Q(56, __VscopeHash, 471366454079730779ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1 = VL_SCOPED_RAND_RESET_Q(56, __VscopeHash, 2201658768979514350ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2 = VL_SCOPED_RAND_RESET_Q(56, __VscopeHash, 13184911039640864734ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3 = VL_SCOPED_RAND_RESET_Q(56, __VscopeHash, 9292647516460514562ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5581154422498743865ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14641573632562226116ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18445386958636188159ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18149416046422187696ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3891681777247645772ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8603787334945382434ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 727516321956614017ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10396522161922592659ull);
    vlSelf->hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 11328239027397023791ull);
    vlSelf->__VdfgRegularize_h6e95ff9d_0_0 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_4 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_10 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_16 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_22 = 0;
    vlSelf->__VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__mem__v0 = 0;
    vlSelf->__VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__mem__v0 = 0;
    vlSelf->__VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__mem__v0 = 0;
    vlSelf->__VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__mem__v0 = 0;
    vlSelf->__VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__mem__v0 = 0;
    vlSelf->__VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__mem__v0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VicoTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__pix_clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__pix_rst_n__0 = 0;
    vlSelf->__Vtrigprevexpr_h8b77f5eb__1 = 0;
    vlSelf->__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080__0 = 0;
    vlSelf->__Vtrigprevexpr_h98362db4__1 = 0;
    vlSelf->__Vtrigprevexpr_h7d65cb35__1 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
