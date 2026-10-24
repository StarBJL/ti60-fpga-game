// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vhdmi_dualvideo_tb.h for the primary calling header

#include "Vhdmi_dualvideo_tb__pch.h"

VlCoroutine Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__0(Vhdmi_dualvideo_tb___024root* vlSelf);
VlCoroutine Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__1(Vhdmi_dualvideo_tb___024root* vlSelf);
VlCoroutine Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__2(Vhdmi_dualvideo_tb___024root* vlSelf);
VlCoroutine Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__3(Vhdmi_dualvideo_tb___024root* vlSelf);
VlCoroutine Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__4(Vhdmi_dualvideo_tb___024root* vlSelf);

void Vhdmi_dualvideo_tb___024root___eval_initial(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_initial\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__1(vlSelf);
    Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__2(vlSelf);
    Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__3(vlSelf);
    Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__4(vlSelf);
}

void Vhdmi_dualvideo_tb___024root___eval_sample(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_sample\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vhdmi_dualvideo_tb___024root___eval_ico(Vhdmi_dualvideo_tb___024root* vlSelf, CData/*0:0*/ firstIteration) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_ico\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VicoTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VicoTriggered[0U]) 
                                     | (IData)((IData)(firstIteration)));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vhdmi_dualvideo_tb___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
    }
#endif
    return (0U);
}

void Vhdmi_dualvideo_tb___024root___timing_ready(Vhdmi_dualvideo_tb___024root* vlSelf);
void Vhdmi_dualvideo_tb___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in);
#ifdef VL_DEBUG
VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
bool Vhdmi_dualvideo_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);
void Vhdmi_dualvideo_tb___024root___timing_resume(Vhdmi_dualvideo_tb___024root* vlSelf);

bool Vhdmi_dualvideo_tb___024root___eval_act(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_act\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    {
        // Inlined CFunc: _eval_triggers_vec__act
        CData/*0:0*/ __Vinline_0__eval_triggers_vec__act___Vtrigprevexpr_h8b77f5eb__0;
        __Vinline_0__eval_triggers_vec__act___Vtrigprevexpr_h8b77f5eb__0 = 0;
        CData/*0:0*/ __Vinline_0__eval_triggers_vec__act___Vtrigprevexpr_h98362db4__0;
        __Vinline_0__eval_triggers_vec__act___Vtrigprevexpr_h98362db4__0 = 0;
        CData/*0:0*/ __Vinline_0__eval_triggers_vec__act___Vtrigprevexpr_h7d65cb35__0;
        __Vinline_0__eval_triggers_vec__act___Vtrigprevexpr_h7d65cb35__0 = 0;
        __Vinline_0__eval_triggers_vec__act___Vtrigprevexpr_h8b77f5eb__0 
            = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__frame_en) 
                & (0x0168U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt))) 
               & (0x0190U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt)));
        __Vinline_0__eval_triggers_vec__act___Vtrigprevexpr_h98362db4__0 
            = ((0x021cU == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt)) 
               & (0x01f4U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt)));
        __Vinline_0__eval_triggers_vec__act___Vtrigprevexpr_h7d65cb35__0 
            = ((2U == vlSelfRef.hdmi_dualvideo_tb__DOT__mode_changes) 
               & (3U == vlSelfRef.hdmi_dualvideo_tb__DOT__frames));
        vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                        (((__Vinline_0__eval_triggers_vec__act___Vtrigprevexpr_h7d65cb35__0 
                                                           != (IData)(vlSelfRef.__Vtrigprevexpr_h7d65cb35__1)) 
                                                          << 8U) 
                                                         | ((((((__Vinline_0__eval_triggers_vec__act___Vtrigprevexpr_h98362db4__0 
                                                                 != (IData)(vlSelfRef.__Vtrigprevexpr_h98362db4__1)) 
                                                                << 3U) 
                                                               | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080) 
                                                                   != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080__0)) 
                                                                  << 2U)) 
                                                              | (((__Vinline_0__eval_triggers_vec__act___Vtrigprevexpr_h8b77f5eb__0 
                                                                   != (IData)(vlSelfRef.__Vtrigprevexpr_h8b77f5eb__1)) 
                                                                  << 1U) 
                                                                 | vlSelfRef.__VdlySched.awaitingCurrentTime())) 
                                                             << 4U) 
                                                            | (((((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__pix_rst_n)) 
                                                                  & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__pix_rst_n__0)) 
                                                                 << 3U) 
                                                                | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__pix_clk) 
                                                                    & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__pix_clk__0))) 
                                                                   << 2U)) 
                                                               | ((((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__clk)) 
                                                                    & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__clk__0)) 
                                                                   << 1U) 
                                                                  | ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__clk) 
                                                                     & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__clk__0)))))))));
        vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__clk__0 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__clk;
        vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__pix_clk__0 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__pix_clk;
        vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__pix_rst_n__0 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__pix_rst_n;
        vlSelfRef.__Vtrigprevexpr_h8b77f5eb__1 = __Vinline_0__eval_triggers_vec__act___Vtrigprevexpr_h8b77f5eb__0;
        vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080__0 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080;
        vlSelfRef.__Vtrigprevexpr_h98362db4__1 = __Vinline_0__eval_triggers_vec__act___Vtrigprevexpr_h98362db4__0;
        vlSelfRef.__Vtrigprevexpr_h7d65cb35__1 = __Vinline_0__eval_triggers_vec__act___Vtrigprevexpr_h7d65cb35__0;
    }
    Vhdmi_dualvideo_tb___024root___timing_ready(vlSelf);
    Vhdmi_dualvideo_tb___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VactTriggered, vlSelfRef.__VactTriggeredAcc);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vhdmi_dualvideo_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vhdmi_dualvideo_tb___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vhdmi_dualvideo_tb___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        vlSelfRef.__VactTriggeredAcc.fill(0ULL);
        Vhdmi_dualvideo_tb___024root___timing_resume(vlSelf);
        {
            // Inlined CFunc: _eval_body__act
            if ((0x00000000000001e2ULL & vlSelfRef.__VactTriggered[0U])) {
                {
                    // Inlined CFunc: _act_comb__TOP__0
                    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__dma_rst 
                        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__rst) 
                           | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rst));
                    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__apb_wr 
                        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_psel) 
                           & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_penable) 
                              & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwrite)));
                    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fifo_wrst 
                        = (1U & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_s) 
                                  >> 1U) | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__dma_rst)));
                    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__aud_apb_push 
                        = (IData)(((0x0030U == (0x00fcU 
                                                & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr))) 
                                   & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__apb_wr)));
                    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wdo 
                        = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wgray) 
                            != ((0x00000180U & ((~ 
                                                 ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray_w2) 
                                                  >> 7U)) 
                                                << 7U)) 
                                | (0x0000007fU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray_w2)))) 
                           & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__aud_apb_push));
                    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wbin_n 
                        = (0x000001ffU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wbin) 
                                          + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wdo)));
                }
            }
        }
    }
    return (__VactExecute);
}

bool Vhdmi_dualvideo_tb___024root___eval_inact(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_inact\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VinactExecute;
    // Body
    __VinactExecute = vlSelfRef.__VdlySched.awaitingZeroDelay();
    if (__VinactExecute) {
        VL_FATAL_MT("hdmi_dualvideo_tb.sv", 2, "", "ZERODLY: Design Verilated with '--no-sched-zero-delay', but #0 delay executed at runtime");
    }
    return (__VinactExecute);
}

void Vhdmi_dualvideo_tb___024root___eval_body__nba(Vhdmi_dualvideo_tb___024root* vlSelf);
void Vhdmi_dualvideo_tb___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out);

bool Vhdmi_dualvideo_tb___024root___eval_nba(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_nba\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vhdmi_dualvideo_tb___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vhdmi_dualvideo_tb___024root___eval_body__nba(vlSelf);
        Vhdmi_dualvideo_tb___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

bool Vhdmi_dualvideo_tb___024root___eval_obs(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_obs\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    return (0U);
}

bool Vhdmi_dualvideo_tb___024root___eval_react(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_react\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    return (0U);
}

void Vhdmi_dualvideo_tb___024root___eval_postponed(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_postponed\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VlCoroutine Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__0(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    co_await vlSelfRef.__VdlySched.delay(0x00000000000002dbULL, 
                                         nullptr, "hdmi_dualvideo_tb.sv", 
                                         6);
    while (true) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001a43ULL, 
                                             nullptr, 
                                             "hdmi_dualvideo_tb.sv", 
                                             6);
        vlSelfRef.hdmi_dualvideo_tb__DOT__pix_clk = 
            (1U & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__pix_clk)));
    }
    co_return;
}

void Vhdmi_dualvideo_tb___024root____VbeforeTrig_h32447ec5__0(Vhdmi_dualvideo_tb___024root* vlSelf, const char* __VeventDescription);
void Vhdmi_dualvideo_tb___024root____VbeforeTrig_h1b3f35c3__0(Vhdmi_dualvideo_tb___024root* vlSelf, const char* __VeventDescription);
void Vhdmi_dualvideo_tb___024root____VbeforeTrig_h85c17222__0(Vhdmi_dualvideo_tb___024root* vlSelf, const char* __VeventDescription);
void Vhdmi_dualvideo_tb___024root____VbeforeTrig_h10016e0a__0(Vhdmi_dualvideo_tb___024root* vlSelf, const char* __VeventDescription);
void Vhdmi_dualvideo_tb___024root____VbeforeTrig_he9510f89__0(Vhdmi_dualvideo_tb___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__1(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    SData/*15:0*/ hdmi_dualvideo_tb__DOT__apb_write__Vstatic__address;
    hdmi_dualvideo_tb__DOT__apb_write__Vstatic__address = 0;
    IData/*31:0*/ hdmi_dualvideo_tb__DOT__apb_write__Vstatic__value;
    hdmi_dualvideo_tb__DOT__apb_write__Vstatic__value = 0;
    IData/*31:0*/ hdmi_dualvideo_tb__DOT__unnamedblk1_1__DOT____Vrepeat0;
    hdmi_dualvideo_tb__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ hdmi_dualvideo_tb__DOT__unnamedblk1_2__DOT____Vrepeat1;
    hdmi_dualvideo_tb__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    SData/*15:0*/ __Vtask_hdmi_dualvideo_tb__DOT__apb_write__8__address;
    __Vtask_hdmi_dualvideo_tb__DOT__apb_write__8__address = 0;
    IData/*31:0*/ __Vtask_hdmi_dualvideo_tb__DOT__apb_write__8__value;
    __Vtask_hdmi_dualvideo_tb__DOT__apb_write__8__value = 0;
    SData/*15:0*/ __Vtask_hdmi_dualvideo_tb__DOT__apb_write__9__address;
    __Vtask_hdmi_dualvideo_tb__DOT__apb_write__9__address = 0;
    IData/*31:0*/ __Vtask_hdmi_dualvideo_tb__DOT__apb_write__9__value;
    __Vtask_hdmi_dualvideo_tb__DOT__apb_write__9__value = 0;
    SData/*15:0*/ __Vtask_hdmi_dualvideo_tb__DOT__apb_write__10__address;
    __Vtask_hdmi_dualvideo_tb__DOT__apb_write__10__address = 0;
    IData/*31:0*/ __Vtask_hdmi_dualvideo_tb__DOT__apb_write__10__value;
    __Vtask_hdmi_dualvideo_tb__DOT__apb_write__10__value = 0;
    // Body
    hdmi_dualvideo_tb__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000014U;
    while (VL_LTS_III(32, 0U, hdmi_dualvideo_tb__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vhdmi_dualvideo_tb___024root____VbeforeTrig_h32447ec5__0(vlSelf, 
                                                                 "@(negedge hdmi_dualvideo_tb.clk)");
        co_await vlSelfRef.__VtrigSched_h32447ec5__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge hdmi_dualvideo_tb.clk)", 
                                                             "hdmi_dualvideo_tb.sv", 
                                                             139);
        hdmi_dualvideo_tb__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (hdmi_dualvideo_tb__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    vlSelfRef.hdmi_dualvideo_tb__DOT__rst = 0U;
    vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rst = 0U;
    vlSelfRef.hdmi_dualvideo_tb__DOT__pix_rst_n = 1U;
    __Vtask_hdmi_dualvideo_tb__DOT__apb_write__8__value = 1U;
    __Vtask_hdmi_dualvideo_tb__DOT__apb_write__8__address = 0U;
    hdmi_dualvideo_tb__DOT__apb_write__Vstatic__address 
        = __Vtask_hdmi_dualvideo_tb__DOT__apb_write__8__address;
    hdmi_dualvideo_tb__DOT__apb_write__Vstatic__value 
        = __Vtask_hdmi_dualvideo_tb__DOT__apb_write__8__value;
    Vhdmi_dualvideo_tb___024root____VbeforeTrig_h32447ec5__0(vlSelf, 
                                                             "@(negedge hdmi_dualvideo_tb.clk)");
    co_await vlSelfRef.__VtrigSched_h32447ec5__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge hdmi_dualvideo_tb.clk)", 
                                                         "hdmi_dualvideo_tb.sv", 
                                                         37);
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr = hdmi_dualvideo_tb__DOT__apb_write__Vstatic__address;
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwdata = hdmi_dualvideo_tb__DOT__apb_write__Vstatic__value;
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_psel = 1U;
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwrite = 1U;
    Vhdmi_dualvideo_tb___024root____VbeforeTrig_h32447ec5__0(vlSelf, 
                                                             "@(negedge hdmi_dualvideo_tb.clk)");
    co_await vlSelfRef.__VtrigSched_h32447ec5__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge hdmi_dualvideo_tb.clk)", 
                                                         "hdmi_dualvideo_tb.sv", 
                                                         38);
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_penable = 1U;
    Vhdmi_dualvideo_tb___024root____VbeforeTrig_h32447ec5__0(vlSelf, 
                                                             "@(negedge hdmi_dualvideo_tb.clk)");
    co_await vlSelfRef.__VtrigSched_h32447ec5__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge hdmi_dualvideo_tb.clk)", 
                                                         "hdmi_dualvideo_tb.sv", 
                                                         39);
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_psel = 0U;
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_penable = 0U;
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwrite = 0U;
    while ((1U & (~ (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__frame_en) 
                      & (0x0168U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt))) 
                     & (0x0190U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt)))))) {
        Vhdmi_dualvideo_tb___024root____VbeforeTrig_h1b3f35c3__0(vlSelf, 
                                                                 "@( ((hdmi_dualvideo_tb.dut.frame_en & (12'h168 == hdmi_dualvideo_tb.dut.v_cnt)) & (12'h190 == hdmi_dualvideo_tb.dut.h_cnt)))");
        co_await vlSelfRef.__VtrigSched_h1b3f35c3__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( ((hdmi_dualvideo_tb.dut.frame_en & (12'h168 == hdmi_dualvideo_tb.dut.v_cnt)) & (12'h190 == hdmi_dualvideo_tb.dut.h_cnt)))", 
                                                             "hdmi_dualvideo_tb.sv", 
                                                             142);
    }
    __Vtask_hdmi_dualvideo_tb__DOT__apb_write__9__value = 1U;
    __Vtask_hdmi_dualvideo_tb__DOT__apb_write__9__address = 0x0050U;
    hdmi_dualvideo_tb__DOT__apb_write__Vstatic__address 
        = __Vtask_hdmi_dualvideo_tb__DOT__apb_write__9__address;
    hdmi_dualvideo_tb__DOT__apb_write__Vstatic__value 
        = __Vtask_hdmi_dualvideo_tb__DOT__apb_write__9__value;
    Vhdmi_dualvideo_tb___024root____VbeforeTrig_h32447ec5__0(vlSelf, 
                                                             "@(negedge hdmi_dualvideo_tb.clk)");
    co_await vlSelfRef.__VtrigSched_h32447ec5__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge hdmi_dualvideo_tb.clk)", 
                                                         "hdmi_dualvideo_tb.sv", 
                                                         37);
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr = hdmi_dualvideo_tb__DOT__apb_write__Vstatic__address;
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwdata = hdmi_dualvideo_tb__DOT__apb_write__Vstatic__value;
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_psel = 1U;
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwrite = 1U;
    Vhdmi_dualvideo_tb___024root____VbeforeTrig_h32447ec5__0(vlSelf, 
                                                             "@(negedge hdmi_dualvideo_tb.clk)");
    co_await vlSelfRef.__VtrigSched_h32447ec5__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge hdmi_dualvideo_tb.clk)", 
                                                         "hdmi_dualvideo_tb.sv", 
                                                         38);
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_penable = 1U;
    Vhdmi_dualvideo_tb___024root____VbeforeTrig_h32447ec5__0(vlSelf, 
                                                             "@(negedge hdmi_dualvideo_tb.clk)");
    co_await vlSelfRef.__VtrigSched_h32447ec5__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge hdmi_dualvideo_tb.clk)", 
                                                         "hdmi_dualvideo_tb.sv", 
                                                         39);
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_psel = 0U;
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_penable = 0U;
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwrite = 0U;
    while ((1U & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)))) {
        Vhdmi_dualvideo_tb___024root____VbeforeTrig_h85c17222__0(vlSelf, 
                                                                 "@( hdmi_dualvideo_tb.dut.mode_1080)");
        co_await vlSelfRef.__VtrigSched_h85c17222__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( hdmi_dualvideo_tb.dut.mode_1080)", 
                                                             "hdmi_dualvideo_tb.sv", 
                                                             143);
    }
    while ((1U & (~ ((0x021cU == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt)) 
                     & (0x01f4U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt)))))) {
        Vhdmi_dualvideo_tb___024root____VbeforeTrig_h10016e0a__0(vlSelf, 
                                                                 "@( ((12'h21c == hdmi_dualvideo_tb.dut.v_cnt) & (12'h1f4 == hdmi_dualvideo_tb.dut.h_cnt)))");
        co_await vlSelfRef.__VtrigSched_h10016e0a__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( ((12'h21c == hdmi_dualvideo_tb.dut.v_cnt) & (12'h1f4 == hdmi_dualvideo_tb.dut.h_cnt)))", 
                                                             "hdmi_dualvideo_tb.sv", 
                                                             144);
    }
    __Vtask_hdmi_dualvideo_tb__DOT__apb_write__10__value = 0U;
    __Vtask_hdmi_dualvideo_tb__DOT__apb_write__10__address = 0x0050U;
    hdmi_dualvideo_tb__DOT__apb_write__Vstatic__address 
        = __Vtask_hdmi_dualvideo_tb__DOT__apb_write__10__address;
    hdmi_dualvideo_tb__DOT__apb_write__Vstatic__value 
        = __Vtask_hdmi_dualvideo_tb__DOT__apb_write__10__value;
    Vhdmi_dualvideo_tb___024root____VbeforeTrig_h32447ec5__0(vlSelf, 
                                                             "@(negedge hdmi_dualvideo_tb.clk)");
    co_await vlSelfRef.__VtrigSched_h32447ec5__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge hdmi_dualvideo_tb.clk)", 
                                                         "hdmi_dualvideo_tb.sv", 
                                                         37);
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr = hdmi_dualvideo_tb__DOT__apb_write__Vstatic__address;
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwdata = hdmi_dualvideo_tb__DOT__apb_write__Vstatic__value;
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_psel = 1U;
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwrite = 1U;
    Vhdmi_dualvideo_tb___024root____VbeforeTrig_h32447ec5__0(vlSelf, 
                                                             "@(negedge hdmi_dualvideo_tb.clk)");
    co_await vlSelfRef.__VtrigSched_h32447ec5__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge hdmi_dualvideo_tb.clk)", 
                                                         "hdmi_dualvideo_tb.sv", 
                                                         38);
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_penable = 1U;
    Vhdmi_dualvideo_tb___024root____VbeforeTrig_h32447ec5__0(vlSelf, 
                                                             "@(negedge hdmi_dualvideo_tb.clk)");
    co_await vlSelfRef.__VtrigSched_h32447ec5__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge hdmi_dualvideo_tb.clk)", 
                                                         "hdmi_dualvideo_tb.sv", 
                                                         39);
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_psel = 0U;
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_penable = 0U;
    vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwrite = 0U;
    while ((1U & (~ ((2U == vlSelfRef.hdmi_dualvideo_tb__DOT__mode_changes) 
                     & (3U == vlSelfRef.hdmi_dualvideo_tb__DOT__frames))))) {
        Vhdmi_dualvideo_tb___024root____VbeforeTrig_he9510f89__0(vlSelf, 
                                                                 "@( ((32'sh2 == hdmi_dualvideo_tb.mode_changes) & (32'sh3 == hdmi_dualvideo_tb.frames)))");
        co_await vlSelfRef.__VtrigSched_he9510f89__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( ((32'sh2 == hdmi_dualvideo_tb.mode_changes) & (32'sh3 == hdmi_dualvideo_tb.frames)))", 
                                                             "hdmi_dualvideo_tb.sv", 
                                                             145);
    }
    hdmi_dualvideo_tb__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0x0000000aU;
    while (VL_LTS_III(32, 0U, hdmi_dualvideo_tb__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vhdmi_dualvideo_tb___024root____VbeforeTrig_h32447ec5__0(vlSelf, 
                                                                 "@(negedge hdmi_dualvideo_tb.clk)");
        co_await vlSelfRef.__VtrigSched_h32447ec5__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge hdmi_dualvideo_tb.clk)", 
                                                             "hdmi_dualvideo_tb.sv", 
                                                             146);
        hdmi_dualvideo_tb__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (hdmi_dualvideo_tb__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    if (VL_UNLIKELY((((((0U != vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_cnt) 
                        | (0U == vlSelfRef.hdmi_dualvideo_tb__DOT__avi720)) 
                       | (0U == vlSelfRef.hdmi_dualvideo_tb__DOT__avi1080)) 
                      | (0x003bc400U != vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_pixels))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: hdmi_dualvideo_tb.sv:147: Assertion failed in %m: UF/AVI/TMDS coverage %0d\n",4, 'M',vlSymsp->name(),"hdmi_dualvideo_tb", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '~',32,vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_pixels);
        VL_STOP_MT("hdmi_dualvideo_tb.sv", 147, "", false);
    }
    VL_WRITEF_NX("TEST_PASS: 720->1080->720 asynchronous clocks, 3 full canvases, nearest pixels, AVI VIC/checksum, %0d decoded TMDS pixels; AR=%0d R=%0d\n",3
                 , '~',32,vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_pixels
                 , '~',32,vlSelfRef.hdmi_dualvideo_tb__DOT__ar_count
                 , '~',32,vlSelfRef.hdmi_dualvideo_tb__DOT__r_count);
    VL_FINISH_MT("hdmi_dualvideo_tb.sv", 149, "");
    co_return;
}

VlCoroutine Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__2(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__2\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    co_await vlSelfRef.__VdlySched.delay(0x000000174876e800ULL, 
                                         nullptr, "hdmi_dualvideo_tb.sv", 
                                         151);
    VL_WRITEF_NX("[%0t] %%Fatal: hdmi_dualvideo_tb.sv:151: Assertion failed in %m: watchdog\n",3, 'M',vlSymsp->name(),"hdmi_dualvideo_tb", 'T',-9
                 , '#',64,VL_TIME_UNITED_Q(1000));
    VL_STOP_MT("hdmi_dualvideo_tb.sv", 151, "", false);
    co_return;
}

VlCoroutine Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__3(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__3\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x00000000000009c4ULL, 
                                             nullptr, 
                                             "hdmi_dualvideo_tb.sv", 
                                             5);
        vlSelfRef.hdmi_dualvideo_tb__DOT__clk = (1U 
                                                 & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__clk)));
    }
    co_return;
}

void Vhdmi_dualvideo_tb___024root____VbeforeTrig_he72dfe88__0(Vhdmi_dualvideo_tb___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__4(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_initial__TOP__Vtiming__4\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __Vfunc_hdmi_dualvideo_tb__DOT__pixel__3__x;
    __Vfunc_hdmi_dualvideo_tb__DOT__pixel__3__x = 0;
    IData/*31:0*/ __Vfunc_hdmi_dualvideo_tb__DOT__pixel__3__y;
    __Vfunc_hdmi_dualvideo_tb__DOT__pixel__3__y = 0;
    IData/*23:0*/ __Vfunc_hdmi_dualvideo_tb__DOT__rgb__4__Vfuncout;
    __Vfunc_hdmi_dualvideo_tb__DOT__rgb__4__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_hdmi_dualvideo_tb__DOT__rgb__4__p;
    __Vfunc_hdmi_dualvideo_tb__DOT__rgb__4__p = 0;
    SData/*9:0*/ __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__sym;
    __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__sym = 0;
    CData/*7:0*/ __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__q;
    __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__q = 0;
    CData/*7:0*/ __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d;
    __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d = 0;
    SData/*9:0*/ __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__sym;
    __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__sym = 0;
    CData/*7:0*/ __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__q;
    __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__q = 0;
    CData/*7:0*/ __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d;
    __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d = 0;
    SData/*9:0*/ __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__sym;
    __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__sym = 0;
    CData/*7:0*/ __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__q;
    __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__q = 0;
    CData/*7:0*/ __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d;
    __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d = 0;
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        Vhdmi_dualvideo_tb___024root____VbeforeTrig_he72dfe88__0(vlSelf, 
                                                                 "@(posedge hdmi_dualvideo_tb.pix_clk)");
        co_await vlSelfRef.__VtrigSched_he72dfe88__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge hdmi_dualvideo_tb.pix_clk)", 
                                                             "hdmi_dualvideo_tb.sv", 
                                                             98);
        vlSelfRef.hdmi_dualvideo_tb__DOT__old_x = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h1;
        vlSelfRef.hdmi_dualvideo_tb__DOT__old_y = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v1;
        vlSelfRef.hdmi_dualvideo_tb__DOT__check_pixel 
            = ((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                 >> 3U) & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__de1)) 
               & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__frame_en));
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080) {
            vlSelfRef.hdmi_dualvideo_tb__DOT__sx = 
                VL_DIVS_III(32, VL_MULS_III(32, (IData)(2U), vlSelfRef.hdmi_dualvideo_tb__DOT__old_x), (IData)(3U));
            vlSelfRef.hdmi_dualvideo_tb__DOT__sy = 
                VL_DIVS_III(32, VL_MULS_III(32, (IData)(2U), vlSelfRef.hdmi_dualvideo_tb__DOT__old_y), (IData)(3U));
        } else {
            vlSelfRef.hdmi_dualvideo_tb__DOT__sx = vlSelfRef.hdmi_dualvideo_tb__DOT__old_x;
            vlSelfRef.hdmi_dualvideo_tb__DOT__sy = vlSelfRef.hdmi_dualvideo_tb__DOT__old_y;
        }
        __Vfunc_hdmi_dualvideo_tb__DOT__pixel__3__y 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__sy;
        __Vfunc_hdmi_dualvideo_tb__DOT__pixel__3__x 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__sx;
        vlSelfRef.hdmi_dualvideo_tb__DOT____VlemCall_0__pixel 
            = (0x0000ffffU & (0x1357U ^ (((IData)(0x0000000dU) 
                                          * __Vfunc_hdmi_dualvideo_tb__DOT__pixel__3__x) 
                                         ^ ((IData)(0x000000f1U) 
                                            * __Vfunc_hdmi_dualvideo_tb__DOT__pixel__3__y))));
        __Vfunc_hdmi_dualvideo_tb__DOT__rgb__4__p = vlSelfRef.hdmi_dualvideo_tb__DOT____VlemCall_0__pixel;
        __Vfunc_hdmi_dualvideo_tb__DOT__rgb__4__Vfuncout 
            = ((((0x000000f8U & ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__rgb__4__p) 
                                 >> 8U)) | (7U & ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__rgb__4__p) 
                                                  >> 0x0dU))) 
                << 0x00000010U) | ((((0x000000fcU & 
                                      ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__rgb__4__p) 
                                       >> 3U)) | (3U 
                                                  & ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__rgb__4__p) 
                                                     >> 9U))) 
                                    << 8U) | ((0x000000f8U 
                                               & ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__rgb__4__p) 
                                                  << 3U)) 
                                              | (7U 
                                                 & ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__rgb__4__p) 
                                                    >> 2U)))));
        vlSelfRef.hdmi_dualvideo_tb__DOT__expected_rgb 
            = __Vfunc_hdmi_dualvideo_tb__DOT__rgb__4__Vfuncout;
        if ((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
              >> 3U) & (2U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__lty)))) {
            if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080) {
                vlSelfRef.hdmi_dualvideo_tb__DOT__avi1080 
                    = ((IData)(1U) + vlSelfRef.hdmi_dualvideo_tb__DOT__avi1080);
            } else {
                vlSelfRef.hdmi_dualvideo_tb__DOT__avi720 
                    = ((IData)(1U) + vlSelfRef.hdmi_dualvideo_tb__DOT__avi720);
            }
            vlSelfRef.hdmi_dualvideo_tb__DOT__sum = 
                ((IData)(0x00000091U) + (0x000000ffU 
                                         & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__l_s0)));
            vlSelfRef.hdmi_dualvideo_tb__DOT__sum = 
                (vlSelfRef.hdmi_dualvideo_tb__DOT__sum 
                 + (0x000000ffU & (IData)((vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__l_s0 
                                           >> 8U))));
            vlSelfRef.hdmi_dualvideo_tb__DOT__sum = 
                (vlSelfRef.hdmi_dualvideo_tb__DOT__sum 
                 + (0x000000ffU & (IData)((vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__l_s0 
                                           >> 0x00000010U))));
            vlSelfRef.hdmi_dualvideo_tb__DOT__sum = 
                (vlSelfRef.hdmi_dualvideo_tb__DOT__sum 
                 + (0x000000ffU & (IData)((vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__l_s0 
                                           >> 0x00000018U))));
            vlSelfRef.hdmi_dualvideo_tb__DOT__sum = 
                (vlSelfRef.hdmi_dualvideo_tb__DOT__sum 
                 + (0x000000ffU & (IData)((vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__l_s0 
                                           >> 0x00000020U))));
            vlSelfRef.hdmi_dualvideo_tb__DOT__sum = 
                (vlSelfRef.hdmi_dualvideo_tb__DOT__sum 
                 + (0x000000ffU & (IData)((vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__l_s0 
                                           >> 0x00000028U))));
            vlSelfRef.hdmi_dualvideo_tb__DOT__sum = 
                (vlSelfRef.hdmi_dualvideo_tb__DOT__sum 
                 + (0x000000ffU & (IData)((vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__l_s0 
                                           >> 0x00000030U))));
            if (VL_UNLIKELY(((0U != (0x000000ffU & vlSelfRef.hdmi_dualvideo_tb__DOT__sum))))) {
                VL_WRITEF_NX("[%0t] %%Fatal: hdmi_dualvideo_tb.sv:107: Assertion failed in %m: AVI checksum\n",3, 'M',vlSymsp->name(),"hdmi_dualvideo_tb", 'T',-9
                             , '#',64,VL_TIME_UNITED_Q(1000));
                VL_STOP_MT("hdmi_dualvideo_tb.sv", 107, "", false);
            }
            if (VL_UNLIKELY((((0x000000ffU & (IData)(
                                                     (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__l_s0 
                                                      >> 0x20U))) 
                              != ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                   ? 0x00000022U : 4U))))) {
                VL_WRITEF_NX("[%0t] %%Fatal: hdmi_dualvideo_tb.sv:108: Assertion failed in %m: AVI VIC\n",3, 'M',vlSymsp->name(),"hdmi_dualvideo_tb", 'T',-9
                             , '#',64,VL_TIME_UNITED_Q(1000));
                VL_STOP_MT("hdmi_dualvideo_tb.sv", 108, "", false);
            }
        }
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "hdmi_dualvideo_tb.sv", 
                                             111);
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_check2) {
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__sym 
                = vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_d2;
            vlSelfRef.hdmi_dualvideo_tb__DOT____VlemCall_1__tmds_decode = 0;
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__q = 0;
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d = 0;
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__q 
                = (0x000000ffU & ((0x00000200U & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__sym))
                                   ? (~ (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__sym))
                                   : (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__sym)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d 
                = ((0xfeU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d)) 
                   | (1U & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__q)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d 
                = ((0xfdU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d)) 
                   | (2U & (((((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__q) 
                               >> 1U) ^ (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__q)) 
                             ^ (~ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__sym) 
                                   >> 8U))) << 1U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d 
                = ((0xfbU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d)) 
                   | (4U & ((~ (VL_REDXOR_4((6U & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__q))) 
                                ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__sym) 
                                   >> 8U))) << 2U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d 
                = ((0xf7U & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d)) 
                   | (8U & ((~ (VL_REDXOR_4((0x0cU 
                                             & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__q))) 
                                ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__sym) 
                                   >> 8U))) << 3U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d 
                = ((0xefU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d)) 
                   | (0x00000010U & ((~ (VL_REDXOR_8(
                                                     (0x18U 
                                                      & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__q))) 
                                         ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__sym) 
                                            >> 8U))) 
                                     << 4U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d 
                = ((0xdfU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d)) 
                   | (0x00000020U & ((~ (VL_REDXOR_8(
                                                     (0x30U 
                                                      & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__q))) 
                                         ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__sym) 
                                            >> 8U))) 
                                     << 5U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d 
                = ((0xbfU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d)) 
                   | (0x00000040U & ((~ (VL_REDXOR_8(
                                                     (0x60U 
                                                      & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__q))) 
                                         ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__sym) 
                                            >> 8U))) 
                                     << 6U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d 
                = ((0x7fU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d)) 
                   | (0x00000080U & ((~ (VL_REDXOR_8(
                                                     (0xc0U 
                                                      & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__q))) 
                                         ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__sym) 
                                            >> 8U))) 
                                     << 7U)));
            vlSelfRef.hdmi_dualvideo_tb__DOT____VlemCall_1__tmds_decode 
                = __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__5__d;
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__sym 
                = vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_d1;
            vlSelfRef.hdmi_dualvideo_tb__DOT____VlemCall_2__tmds_decode = 0;
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__q = 0;
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d = 0;
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__q 
                = (0x000000ffU & ((0x00000200U & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__sym))
                                   ? (~ (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__sym))
                                   : (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__sym)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d 
                = ((0xfeU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d)) 
                   | (1U & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__q)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d 
                = ((0xfdU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d)) 
                   | (2U & (((((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__q) 
                               >> 1U) ^ (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__q)) 
                             ^ (~ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__sym) 
                                   >> 8U))) << 1U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d 
                = ((0xfbU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d)) 
                   | (4U & ((~ (VL_REDXOR_4((6U & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__q))) 
                                ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__sym) 
                                   >> 8U))) << 2U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d 
                = ((0xf7U & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d)) 
                   | (8U & ((~ (VL_REDXOR_4((0x0cU 
                                             & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__q))) 
                                ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__sym) 
                                   >> 8U))) << 3U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d 
                = ((0xefU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d)) 
                   | (0x00000010U & ((~ (VL_REDXOR_8(
                                                     (0x18U 
                                                      & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__q))) 
                                         ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__sym) 
                                            >> 8U))) 
                                     << 4U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d 
                = ((0xdfU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d)) 
                   | (0x00000020U & ((~ (VL_REDXOR_8(
                                                     (0x30U 
                                                      & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__q))) 
                                         ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__sym) 
                                            >> 8U))) 
                                     << 5U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d 
                = ((0xbfU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d)) 
                   | (0x00000040U & ((~ (VL_REDXOR_8(
                                                     (0x60U 
                                                      & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__q))) 
                                         ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__sym) 
                                            >> 8U))) 
                                     << 6U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d 
                = ((0x7fU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d)) 
                   | (0x00000080U & ((~ (VL_REDXOR_8(
                                                     (0xc0U 
                                                      & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__q))) 
                                         ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__sym) 
                                            >> 8U))) 
                                     << 7U)));
            vlSelfRef.hdmi_dualvideo_tb__DOT____VlemCall_2__tmds_decode 
                = __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__6__d;
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__sym 
                = vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_d0;
            vlSelfRef.hdmi_dualvideo_tb__DOT____VlemCall_3__tmds_decode = 0;
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__q = 0;
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d = 0;
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__q 
                = (0x000000ffU & ((0x00000200U & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__sym))
                                   ? (~ (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__sym))
                                   : (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__sym)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d 
                = ((0xfeU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d)) 
                   | (1U & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__q)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d 
                = ((0xfdU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d)) 
                   | (2U & (((((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__q) 
                               >> 1U) ^ (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__q)) 
                             ^ (~ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__sym) 
                                   >> 8U))) << 1U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d 
                = ((0xfbU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d)) 
                   | (4U & ((~ (VL_REDXOR_4((6U & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__q))) 
                                ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__sym) 
                                   >> 8U))) << 2U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d 
                = ((0xf7U & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d)) 
                   | (8U & ((~ (VL_REDXOR_4((0x0cU 
                                             & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__q))) 
                                ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__sym) 
                                   >> 8U))) << 3U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d 
                = ((0xefU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d)) 
                   | (0x00000010U & ((~ (VL_REDXOR_8(
                                                     (0x18U 
                                                      & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__q))) 
                                         ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__sym) 
                                            >> 8U))) 
                                     << 4U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d 
                = ((0xdfU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d)) 
                   | (0x00000020U & ((~ (VL_REDXOR_8(
                                                     (0x30U 
                                                      & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__q))) 
                                         ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__sym) 
                                            >> 8U))) 
                                     << 5U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d 
                = ((0xbfU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d)) 
                   | (0x00000040U & ((~ (VL_REDXOR_8(
                                                     (0x60U 
                                                      & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__q))) 
                                         ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__sym) 
                                            >> 8U))) 
                                     << 6U)));
            __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d 
                = ((0x7fU & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d)) 
                   | (0x00000080U & ((~ (VL_REDXOR_8(
                                                     (0xc0U 
                                                      & (IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__q))) 
                                         ^ ((IData)(__Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__sym) 
                                            >> 8U))) 
                                     << 7U)));
            vlSelfRef.hdmi_dualvideo_tb__DOT____VlemCall_3__tmds_decode 
                = __Vfunc_hdmi_dualvideo_tb__DOT__tmds_decode__7__d;
            if (VL_UNLIKELY((((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT____VlemCall_1__tmds_decode) 
                                << 0x00000010U) | (
                                                   ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT____VlemCall_2__tmds_decode) 
                                                    << 8U) 
                                                   | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT____VlemCall_3__tmds_decode))) 
                              != vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_expect2)))) {
                VL_WRITEF_NX("[%0t] %%Fatal: hdmi_dualvideo_tb.sv:114: Assertion failed in %m: TMDS video decode differs from expected source pixel\n",3, 'M',vlSymsp->name(),"hdmi_dualvideo_tb", 'T',-9
                             , '#',64,VL_TIME_UNITED_Q(1000));
                VL_STOP_MT("hdmi_dualvideo_tb.sv", 114, "", false);
            }
            vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_pixels 
                = ((IData)(1U) + vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_pixels);
        }
        vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_expect2 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_expect1;
        vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_expect1 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_expect0;
        vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_expect0 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__expected_rgb;
        vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_check2 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_check1;
        vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_check1 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_check0;
        vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_check0 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__check_pixel;
        if (VL_UNLIKELY(((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                           >> 3U) & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__old_mode) 
                                     != (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)))))) {
            if (VL_UNLIKELY(((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__de0) 
                               | (0U != (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt))) 
                              | ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt) 
                                 != ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                      ? 0x043cU : 0x02d5U)))))) {
                VL_WRITEF_NX("[%0t] %%Fatal: hdmi_dualvideo_tb.sv:120: Assertion failed in %m: mode changed outside vsync boundary\n",3, 'M',vlSymsp->name(),"hdmi_dualvideo_tb", 'T',-9
                             , '#',64,VL_TIME_UNITED_Q(1000));
                VL_STOP_MT("hdmi_dualvideo_tb.sv", 120, "", false);
            }
            vlSelfRef.hdmi_dualvideo_tb__DOT__mode_changes 
                = ((IData)(1U) + vlSelfRef.hdmi_dualvideo_tb__DOT__mode_changes);
            VL_WRITEF_NX("MODE_COMMIT %0d at %0t\n",3, 'T',-9
                         , '#',1,vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080
                         , '#',64,VL_TIME_UNITED_Q(1000));
        }
        vlSelfRef.hdmi_dualvideo_tb__DOT__old_mode 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080;
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__check_pixel) {
            if (VL_UNLIKELY((vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf1))) {
                VL_WRITEF_NX("[%0t] %%Fatal: hdmi_dualvideo_tb.sv:126: Assertion failed in %m: unexpected video underrun\n",3, 'M',vlSymsp->name(),"hdmi_dualvideo_tb", 'T',-9
                             , '#',64,VL_TIME_UNITED_Q(1000));
                VL_STOP_MT("hdmi_dualvideo_tb.sv", 126, "", false);
            }
            if (VL_UNLIKELY((((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__r2) 
                                << 0x00000010U) | (
                                                   ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g2) 
                                                    << 8U) 
                                                   | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__b2))) 
                              != vlSelfRef.hdmi_dualvideo_tb__DOT__expected_rgb)))) {
                VL_WRITEF_NX("[%0t] %%Fatal: hdmi_dualvideo_tb.sv:128: Assertion failed in %m: pixel mode=%0d x=%0d y=%0d source=%0d,%0d got=%h expected=%h\n",10, 'M',vlSymsp->name(),"hdmi_dualvideo_tb", 'T',-9
                             , '#',64,VL_TIME_UNITED_Q(1000)
                             , '#',1,(IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                             , '~',32,vlSelfRef.hdmi_dualvideo_tb__DOT__old_x
                             , '~',32,vlSelfRef.hdmi_dualvideo_tb__DOT__old_y
                             , '~',32,vlSelfRef.hdmi_dualvideo_tb__DOT__sx
                             , '~',32,vlSelfRef.hdmi_dualvideo_tb__DOT__sy
                             , '#',24,(((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__r2) 
                                        << 0x00000010U) 
                                       | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g2) 
                                           << 8U) | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__b2)))
                             , '#',24,vlSelfRef.hdmi_dualvideo_tb__DOT__expected_rgb);
                VL_STOP_MT("hdmi_dualvideo_tb.sv", 128, "", false);
            }
            vlSelfRef.hdmi_dualvideo_tb__DOT__pixels 
                = ((IData)(1U) + vlSelfRef.hdmi_dualvideo_tb__DOT__pixels);
            if (VL_UNLIKELY((((vlSelfRef.hdmi_dualvideo_tb__DOT__old_x 
                               == ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                    ? 0x0000077fU : 0x000004ffU)) 
                              & (vlSelfRef.hdmi_dualvideo_tb__DOT__old_y 
                                 == ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                      ? 0x00000437U
                                      : 0x000002cfU)))))) {
                vlSelfRef.hdmi_dualvideo_tb__DOT__expected_count 
                    = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                        ? 0x001fa400U : 0x000e1000U);
                if (VL_UNLIKELY(((vlSelfRef.hdmi_dualvideo_tb__DOT__pixels 
                                  != vlSelfRef.hdmi_dualvideo_tb__DOT__expected_count)))) {
                    VL_WRITEF_NX("[%0t] %%Fatal: hdmi_dualvideo_tb.sv:132: Assertion failed in %m: frame pixel count %0d expected %0d\n",5, 'M',vlSymsp->name(),"hdmi_dualvideo_tb", 'T',-9
                                 , '#',64,VL_TIME_UNITED_Q(1000)
                                 , '~',32,vlSelfRef.hdmi_dualvideo_tb__DOT__pixels
                                 , '~',32,vlSelfRef.hdmi_dualvideo_tb__DOT__expected_count);
                    VL_STOP_MT("hdmi_dualvideo_tb.sv", 132, "", false);
                }
                vlSelfRef.hdmi_dualvideo_tb__DOT__frames 
                    = ((IData)(1U) + vlSelfRef.hdmi_dualvideo_tb__DOT__frames);
                vlSelfRef.hdmi_dualvideo_tb__DOT__pixels = 0U;
                VL_WRITEF_NX("FRAME_PASS %0d mode=%0d pixels=%0d\n",3
                             , '~',32,vlSelfRef.hdmi_dualvideo_tb__DOT__frames
                             , '#',1,(IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                             , '~',32,vlSelfRef.hdmi_dualvideo_tb__DOT__expected_count);
            }
        }
    }
    co_return;
}

bool Vhdmi_dualvideo_tb___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___trigger_anySet__ico\n"); );
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

bool Vhdmi_dualvideo_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___trigger_anySet__act\n"); );
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

void Vhdmi_dualvideo_tb___024root___nba_sequent__TOP__0(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___nba_sequent__TOP__0\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*2:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__vs_s;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__vs_s = 0;
    IData/*31:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__uf_cnt;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__uf_cnt = 0;
    IData/*31:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__fb_cur;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__fb_cur = 0;
    IData/*31:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__present_cnt;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__present_cnt = 0;
    IData/*31:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__repeat_cnt;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__repeat_cnt = 0;
    IData/*31:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__active_scan_cnt;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__active_scan_cnt = 0;
    SData/*12:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__outstanding;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__outstanding = 0;
    CData/*0:0*/ __Vdly__hdmi_dualvideo_tb__DOT__axi_arvalid;
    __Vdly__hdmi_dualvideo_tb__DOT__axi_arvalid = 0;
    SData/*12:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__bursts_left;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__bursts_left = 0;
    IData/*31:0*/ __Vdly__hdmi_dualvideo_tb__DOT__axi_araddr;
    __Vdly__hdmi_dualvideo_tb__DOT__axi_araddr = 0;
    // Body
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__uf_cnt 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_cnt;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__outstanding 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__outstanding;
    vlSelfRef.__VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__mem__v0 = 0U;
    __Vdly__hdmi_dualvideo_tb__DOT__axi_araddr = vlSelfRef.hdmi_dualvideo_tb__DOT__axi_araddr;
    vlSelfRef.__VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__mem__v0 = 0U;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__vs_s 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_s;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__fb_cur 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fb_cur;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__present_cnt 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__present_cnt;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__repeat_cnt 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__repeat_cnt;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__active_scan_cnt 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__active_scan_cnt;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__bursts_left 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__bursts_left;
    __Vdly__hdmi_dualvideo_tb__DOT__axi_arvalid = vlSelfRef.hdmi_dualvideo_tb__DOT__axi_arvalid;
    if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__dma_rst) {
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__outstanding = 0U;
        __Vdly__hdmi_dualvideo_tb__DOT__axi_araddr = 0x01000000U;
        __Vdly__hdmi_dualvideo_tb__DOT__axi_arvalid = 0U;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__bursts_left = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__drop_mode = 1U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__start_pending = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__running = 0U;
    } else {
        if ((2U == (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ar_hs) 
                     << 1U) | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rvalid)))) {
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__outstanding 
                = (0x00001fffU & ((IData)(0x0040U) 
                                  + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__outstanding)));
        } else if ((1U == (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ar_hs) 
                            << 1U) | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rvalid)))) {
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__outstanding 
                = (0x00001fffU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__outstanding) 
                                  - (IData)(1U)));
        } else if ((3U == (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ar_hs) 
                            << 1U) | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rvalid)))) {
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__outstanding 
                = (0x00001fffU & ((IData)(0x003fU) 
                                  + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__outstanding)));
        }
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ar_hs) {
            __Vdly__hdmi_dualvideo_tb__DOT__axi_araddr 
                = ((IData)(0x00000100U) + vlSelfRef.hdmi_dualvideo_tb__DOT__axi_araddr);
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__bursts_left 
                = (0x00001fffU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__bursts_left) 
                                  - (IData)(1U)));
            __Vdly__hdmi_dualvideo_tb__DOT__axi_arvalid = 0U;
        }
        if ((1U & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_rise)))) {
            if ((1U & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_fall)))) {
                if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__start) {
                    __Vdly__hdmi_dualvideo_tb__DOT__axi_araddr 
                        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fb_cur;
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__bursts_left = 0x1c20U;
                } else if ((((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__running) 
                               & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_arvalid))) 
                              & (~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_s) 
                                    >> 1U))) & (0U 
                                                != (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__bursts_left))) 
                            & (0x0800U >= (0x00003fffU 
                                           & ((IData)(0x0040U) 
                                              + ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__outstanding) 
                                                 + 
                                                 (0x00000fffU 
                                                  & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wbin) 
                                                     - 
                                                     ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2) 
                                                      ^ 
                                                      (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2) 
                                                        >> 1U) 
                                                       ^ 
                                                       (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2) 
                                                         >> 2U) 
                                                        ^ 
                                                        (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2) 
                                                          >> 3U) 
                                                         ^ 
                                                         (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2) 
                                                           >> 4U) 
                                                          ^ 
                                                          (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2) 
                                                            >> 5U) 
                                                           ^ 
                                                           (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2) 
                                                             >> 6U) 
                                                            ^ 
                                                            (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2) 
                                                              >> 7U) 
                                                             ^ 
                                                             (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2) 
                                                               >> 8U) 
                                                              ^ 
                                                              (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2) 
                                                                >> 9U) 
                                                               ^ 
                                                               (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2) 
                                                                 >> 0x0000000aU) 
                                                                ^ 
                                                                ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2) 
                                                                 >> 0x0000000bU)))))))))))))))))))) {
                    __Vdly__hdmi_dualvideo_tb__DOT__axi_arvalid = 1U;
                }
            }
        }
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_rise) {
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__drop_mode = 1U;
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__start_pending = 0U;
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__running = 0U;
        } else {
            if ((1U & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_fall)))) {
                if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__start) {
                    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__drop_mode = 0U;
                    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__running = 1U;
                }
            }
            if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_fall) {
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__start_pending 
                    = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ctrl_en) 
                       & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ctrl_test)));
            } else if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__start) {
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__start_pending = 0U;
            }
        }
    }
    if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wdo) {
        vlSelfRef.__VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__mem__v0 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rdata;
        vlSelfRef.__VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__mem__v0 
            = (0x000007ffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wbin));
        vlSelfRef.__VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__mem__v0 = 1U;
    }
    if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wdo) {
        vlSelfRef.__VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__mem__v0 
            = (vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwdata 
               & (- (IData)((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__aud_apb_push))));
        vlSelfRef.__VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__mem__v0 
            = (0x000000ffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wbin));
        vlSelfRef.__VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__mem__v0 = 1U;
    }
    if (vlSelfRef.hdmi_dualvideo_tb__DOT__rst) {
        vlSelfRef.hdmi_dualvideo_tb__DOT__wp = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__rp = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__beat = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__syscycle = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__ar_count = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__r_count = 0U;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__uf_cnt = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_s = 0U;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__vs_s = 0U;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__fb_cur = 0x01000000U;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__present_cnt = 0U;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__repeat_cnt = 0U;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__active_scan_cnt = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray_w2 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wbin = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fb_pending = 0x01000000U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray_w1 = 0U;
    } else {
        vlSelfRef.hdmi_dualvideo_tb__DOT__syscycle 
            = ((IData)(1U) + vlSelfRef.hdmi_dualvideo_tb__DOT__syscycle);
        if (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_arvalid) 
             & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_arready))) {
            vlSelfRef.hdmi_dualvideo_tb__DOT__ar_count 
                = ((IData)(1U) + vlSelfRef.hdmi_dualvideo_tb__DOT__ar_count);
            if (VL_UNLIKELY((((0x01000000U > vlSelfRef.hdmi_dualvideo_tb__DOT__axi_araddr) 
                              | (0x011c2000U < ((IData)(0x00000100U) 
                                                + vlSelfRef.hdmi_dualvideo_tb__DOT__axi_araddr)))))) {
                VL_WRITEF_NX("[%0t] %%Fatal: hdmi_dualvideo_tb.sv:72: Assertion failed in %m: DMA exceeds fixed720 canvas: %h\n",4, 'M',vlSymsp->name(),"hdmi_dualvideo_tb", 'T',-9
                             , '#',64,VL_TIME_UNITED_Q(1000)
                             , '#',32,vlSelfRef.hdmi_dualvideo_tb__DOT__axi_araddr);
                VL_STOP_MT("hdmi_dualvideo_tb.sv", 72, "", false);
            }
            vlSelfRef.hdmi_dualvideo_tb__DOT__addresses[(0x0000ffffU 
                                                         & vlSelfRef.hdmi_dualvideo_tb__DOT__wp)] 
                = vlSelfRef.hdmi_dualvideo_tb__DOT__axi_araddr;
            if (VL_UNLIKELY(((0U != (0x000000ffU & 
                                     (vlSelfRef.hdmi_dualvideo_tb__DOT__axi_araddr 
                                      - (IData)(0x01000000U))))))) {
                VL_WRITEF_NX("[%0t] %%Fatal: hdmi_dualvideo_tb.sv:73: Assertion failed in %m: unaligned burst\n",3, 'M',vlSymsp->name(),"hdmi_dualvideo_tb", 'T',-9
                             , '#',64,VL_TIME_UNITED_Q(1000));
                VL_STOP_MT("hdmi_dualvideo_tb.sv", 73, "", false);
            }
            vlSelfRef.hdmi_dualvideo_tb__DOT__due[(0x0000ffffU 
                                                   & vlSelfRef.hdmi_dualvideo_tb__DOT__wp)] 
                = ((IData)(2U) + (vlSelfRef.hdmi_dualvideo_tb__DOT__syscycle 
                                  + (0x0000000fU & 
                                     (vlSelfRef.hdmi_dualvideo_tb__DOT__rng 
                                      >> 4U))));
            vlSelfRef.hdmi_dualvideo_tb__DOT__wp = 
                ((IData)(1U) + vlSelfRef.hdmi_dualvideo_tb__DOT__wp);
        }
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rvalid) {
            vlSelfRef.hdmi_dualvideo_tb__DOT__r_count 
                = ((IData)(1U) + vlSelfRef.hdmi_dualvideo_tb__DOT__r_count);
            if (VL_UNLIKELY((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rlast) 
                              != (0x0000003fU == vlSelfRef.hdmi_dualvideo_tb__DOT__beat))))) {
                VL_WRITEF_NX("[%0t] %%Fatal: hdmi_dualvideo_tb.sv:77: Assertion failed in %m: RLAST\n",3, 'M',vlSymsp->name(),"hdmi_dualvideo_tb", 'T',-9
                             , '#',64,VL_TIME_UNITED_Q(1000));
                VL_STOP_MT("hdmi_dualvideo_tb.sv", 77, "", false);
            }
            if ((0x0000003fU == vlSelfRef.hdmi_dualvideo_tb__DOT__beat)) {
                vlSelfRef.hdmi_dualvideo_tb__DOT__rp 
                    = ((IData)(1U) + vlSelfRef.hdmi_dualvideo_tb__DOT__rp);
                vlSelfRef.hdmi_dualvideo_tb__DOT__beat = 0U;
            } else {
                vlSelfRef.hdmi_dualvideo_tb__DOT__beat 
                    = ((IData)(1U) + vlSelfRef.hdmi_dualvideo_tb__DOT__beat);
            }
        }
        if ((1U & VL_REDXOR_32((3U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_s) 
                                      >> 1U))))) {
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__uf_cnt 
                = ((IData)(1U) + vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_cnt);
        }
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_s 
            = ((6U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_s) 
                      << 1U)) | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_tog));
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__vs_s 
            = ((6U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_s) 
                      << 1U)) | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_zone));
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_fall) {
            if (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ctrl_en) 
                 & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ctrl_test)))) {
                __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__active_scan_cnt 
                    = ((IData)(1U) + vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__active_scan_cnt);
                if ((vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fb_pending 
                     != vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fb_cur)) {
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__present_cnt 
                        = ((IData)(1U) + vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__present_cnt);
                } else {
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__repeat_cnt 
                        = ((IData)(1U) + vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__repeat_cnt);
                }
            }
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__fb_cur 
                = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fb_pending;
        }
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray_w2 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray_w1;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wbin 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wbin_n;
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__apb_wr) {
            if ((1U & (~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr) 
                          >> 7U)))) {
                if ((1U & (~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr) 
                              >> 6U)))) {
                    if ((1U & (~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr) 
                                  >> 5U)))) {
                        if ((1U & (~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr) 
                                      >> 4U)))) {
                            if ((1U & (~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr) 
                                          >> 3U)))) {
                                if ((4U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr))) {
                                    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fb_pending 
                                        = (0xffffff00U 
                                           & vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwdata);
                                }
                            }
                        }
                    }
                }
            }
        }
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray_w1 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray;
    }
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_cnt 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__uf_cnt;
    vlSelfRef.hdmi_dualvideo_tb__DOT__axi_araddr = __Vdly__hdmi_dualvideo_tb__DOT__axi_araddr;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__present_cnt 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__present_cnt;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__repeat_cnt 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__repeat_cnt;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__active_scan_cnt 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__active_scan_cnt;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fb_cur 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__fb_cur;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__bursts_left 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__bursts_left;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__outstanding 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__outstanding;
    vlSelfRef.hdmi_dualvideo_tb__DOT__axi_arvalid = __Vdly__hdmi_dualvideo_tb__DOT__axi_arvalid;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_s 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__vs_s;
    if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fifo_wrst) {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wbin = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w1 = 0U;
    } else {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wbin 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wbin_n;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w1;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w1 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray;
    }
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_rise 
        = (IData)((2U == (6U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_s))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_fall 
        = (IData)((4U == (6U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_s))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__start 
        = ((~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_s) 
               >> 1U)) & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__start_pending) 
                          & ((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_arvalid)) 
                             & (0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__outstanding)))));
}

extern const VlUnpacked<CData/*1:0*/, 64> Vhdmi_dualvideo_tb__ConstPool__TABLE_h83b4be6e_0;
extern const VlUnpacked<CData/*1:0*/, 64> Vhdmi_dualvideo_tb__ConstPool__TABLE_hea843932_0;
extern const VlUnpacked<CData/*1:0*/, 64> Vhdmi_dualvideo_tb__ConstPool__TABLE_hefdb3507_0;
extern const VlWide<64>/*2047:0*/ Vhdmi_dualvideo_tb__ConstPool__CONST_h54f42bc6_0;
extern const VlUnpacked<CData/*3:0*/, 512> Vhdmi_dualvideo_tb__ConstPool__TABLE_h9c3d9630_0;
extern const VlUnpacked<CData/*3:0*/, 512> Vhdmi_dualvideo_tb__ConstPool__TABLE_h66e20c3f_0;
extern const VlUnpacked<SData/*8:0*/, 512> Vhdmi_dualvideo_tb__ConstPool__TABLE_h41587afb_0;
extern const VlWide<16>/*511:0*/ Vhdmi_dualvideo_tb__ConstPool__CONST_hbdb02f09_0;
extern const VlWide<8>/*255:0*/ Vhdmi_dualvideo_tb__ConstPool__CONST_ha457a04f_0;
extern const VlUnpacked<CData/*1:0*/, 64> Vhdmi_dualvideo_tb__ConstPool__TABLE_hbd0c7d12_0;

void Vhdmi_dualvideo_tb___024root___nba_sequent__TOP__1(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___nba_sequent__TOP__1\n"); );
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
    SData/*8:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    SData/*8:0*/ __Vtableidx2;
    __Vtableidx2 = 0;
    SData/*8:0*/ __Vtableidx3;
    __Vtableidx3 = 0;
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
    CData/*1:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__en_p_s;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__en_p_s = 0;
    CData/*1:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__test_p_s;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__test_p_s = 0;
    CData/*1:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__mode_req_p;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__mode_req_p = 0;
    CData/*0:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080 = 0;
    SData/*11:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt = 0;
    SData/*11:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt = 0;
    CData/*1:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_x;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_x = 0;
    SData/*9:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_word;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_word = 0;
    CData/*1:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_y;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_y = 0;
    IData/*23:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acc;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acc = 0;
    CData/*7:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx = 0;
    IData/*19:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_cnt;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_cnt = 0;
    CData/*7:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0 = 0;
    QData/*55:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2 = 0;
    QData/*55:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q0;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q0 = 0;
    CData/*7:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__eh;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__eh = 0;
    QData/*55:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3 = 0;
    CData/*4:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pc;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pc = 0;
    CData/*7:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1 = 0;
    IData/*23:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__hsr;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__hsr = 0;
    CData/*7:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2 = 0;
    CData/*1:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__slot;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__slot = 0;
    QData/*55:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1 = 0;
    CData/*7:0*/ __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3 = 0;
    SData/*15:0*/ __VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache0__v0;
    __VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache0__v0 = 0;
    SData/*8:0*/ __VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache0__v0;
    __VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache0__v0 = 0;
    CData/*0:0*/ __VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache0__v0;
    __VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache0__v0 = 0;
    SData/*15:0*/ __VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache1__v0;
    __VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache1__v0 = 0;
    SData/*8:0*/ __VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache1__v0;
    __VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache1__v0 = 0;
    CData/*0:0*/ __VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache1__v0;
    __VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache1__v0 = 0;
    SData/*15:0*/ __VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache2__v0;
    __VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache2__v0 = 0;
    SData/*8:0*/ __VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache2__v0;
    __VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache2__v0 = 0;
    CData/*0:0*/ __VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache2__v0;
    __VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache2__v0 = 0;
    // Body
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__en_p_s 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__en_p_s;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__test_p_s 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__test_p_s;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acc 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acc;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__mode_req_p 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_req_p;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__slot 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__slot;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pc 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pc;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__eh 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__eh;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q0 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q0;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_word 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_word;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_x 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_x;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_y 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_y;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_cnt 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_cnt;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080;
    __VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache0__v0 = 0U;
    __VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache1__v0 = 0U;
    __VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache2__v0 = 0U;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__hsr 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__hsr;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1;
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__en_p_s 
        = ((2U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__en_p_s) 
                  << 1U)) | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ctrl_en));
    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__test_p_s 
        = ((2U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__test_p_s) 
                  << 1U)) | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ctrl_test));
    if ((IData)(((0U == (0x0600U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_waddr))) 
                 & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__cache_write_d) 
                    | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_high_d))))) {
        __VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache0__v0 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_wdata;
        __VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache0__v0 
            = (0x000001ffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_waddr));
        __VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache0__v0 = 1U;
    }
    if ((IData)(((0x0200U == (0x0600U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_waddr))) 
                 & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__cache_write_d) 
                    | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_high_d))))) {
        __VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache1__v0 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_wdata;
        __VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache1__v0 
            = (0x000001ffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_waddr));
        __VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache1__v0 = 1U;
    }
    if ((IData)(((0x0400U == (0x0600U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_waddr))) 
                 & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__cache_write_d) 
                    | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_high_d))))) {
        __VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache2__v0 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_wdata;
        __VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache2__v0 
            = (0x000001ffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_waddr));
        __VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache2__v0 = 1U;
    }
    if (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__de0) 
         & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__repeat_line))) {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_read_bank 
            = (3U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_raddr) 
                     >> 9U));
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_pixel1 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache1
            [(0x000001ffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_raddr))];
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_pixel0 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache0
            [(0x000001ffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_raddr))];
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_pixel2 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache2
            [(0x000001ffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_raddr))];
    }
    if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fifo_ren) {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fifo_rdata 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__mem
            [(0x000007ffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rbin))];
    }
    if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT____Vcellinp__u_fifo__rrst) {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wgray_r2 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rbin = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wgray_r1 = 0U;
    } else {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wgray_r2 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wgray_r1;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray 
            = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rbin_n) 
               ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rbin_n) 
                  >> 1U));
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rbin 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rbin_n;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wgray_r1 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wgray;
    }
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__repeat_d 
        = ((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                  >> 3U)) && (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__repeat_line));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ren_d 
        = ((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                  >> 3U)) && (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__afifo_ren));
    if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__afifo_ren) {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__afifo_rdata 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__mem
            [(0x000000ffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rbin))];
    }
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__hodd1 
        = ((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                  >> 3U)) && (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                     ? (2U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_x))
                                     : (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n1d 
        = (0x0000000fU & ((((((((1U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__r2)) 
                                + (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__r2) 
                                         >> 1U))) + 
                               (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__r2) 
                                      >> 2U))) + (1U 
                                                  & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__r2) 
                                                     >> 3U))) 
                             + (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__r2) 
                                      >> 4U))) + (1U 
                                                  & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__r2) 
                                                     >> 5U))) 
                           + (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__r2) 
                                    >> 6U))) + (1U 
                                                & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__r2) 
                                                   >> 7U))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n1d 
        = (0x0000000fU & ((((((((1U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g2)) 
                                + (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g2) 
                                         >> 1U))) + 
                               (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g2) 
                                      >> 2U))) + (1U 
                                                  & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g2) 
                                                     >> 3U))) 
                             + (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g2) 
                                      >> 4U))) + (1U 
                                                  & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g2) 
                                                     >> 5U))) 
                           + (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g2) 
                                    >> 6U))) + (1U 
                                                & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g2) 
                                                   >> 7U))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n1d 
        = (0x0000000fU & ((((((((1U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__b2)) 
                                + (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__b2) 
                                         >> 1U))) + 
                               (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__b2) 
                                      >> 2U))) + (1U 
                                                  & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__b2) 
                                                     >> 3U))) 
                             + (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__b2) 
                                      >> 4U))) + (1U 
                                                  & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__b2) 
                                                     >> 5U))) 
                           + (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__b2) 
                                    >> 6U))) + (1U 
                                                & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__b2) 
                                                   >> 7U))));
    if ((8U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr))) {
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap) {
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx 
                = ((0xbfU == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx))
                    ? 0U : (0x000000ffU & ((IData)(1U) 
                                           + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx))));
        }
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_tone) {
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tone_ph 
                = ((0x2fU == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tone_ph))
                    ? 0U : (0x0000003fU & ((IData)(1U) 
                                           + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tone_ph))));
        }
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__mode_req_p 
            = ((2U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_req_p) 
                      << 1U)) | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_req));
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__latch) {
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__slot = 0U;
        } else if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__load0) {
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__slot 
                = (3U & ((IData)(1U) + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__slot)));
        }
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__slot 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__slot;
        if ((1U & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__latch)))) {
            if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__load0) {
                __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pc = 0U;
                __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__eh = 0U;
                __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q0 
                    = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__l_s0;
                __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0 = 0U;
                __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1 = 0U;
                __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2 = 0U;
                __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3 = 0U;
            } else if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__idat0) {
                __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pc 
                    = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pc)));
                __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__eh 
                    = ((0x18U > (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pc))
                        ? ((0x83U & (- (IData)((1U 
                                                & (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__hsr 
                                                   ^ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__eh)))))) 
                           ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__eh) 
                              >> 1U)) : (0x0000007fU 
                                         & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__eh) 
                                            >> 1U)));
                if ((0x1cU > (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pc))) {
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q0 
                        = (0x003fffffffffffffULL & 
                           (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q0 
                            >> 2U));
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0 
                        = ((0x83U & (- (IData)((1U 
                                                & ((IData)(
                                                           (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q0 
                                                            >> 1U)) 
                                                   ^ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0a)))))) 
                           ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0a) 
                              >> 1U));
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1 
                        = ((0x83U & (- (IData)((1U 
                                                & ((IData)(
                                                           (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1 
                                                            >> 1U)) 
                                                   ^ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1a)))))) 
                           ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1a) 
                              >> 1U));
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2 
                        = ((0x83U & (- (IData)((1U 
                                                & ((IData)(
                                                           (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2 
                                                            >> 1U)) 
                                                   ^ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2a)))))) 
                           ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2a) 
                              >> 1U));
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3 
                        = ((0x83U & (- (IData)((1U 
                                                & ((IData)(
                                                           (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3 
                                                            >> 1U)) 
                                                   ^ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3a)))))) 
                           ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3a) 
                              >> 1U));
                } else {
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0 
                        = (0x0000003fU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0) 
                                          >> 2U));
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1 
                        = (0x0000003fU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1) 
                                          >> 2U));
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2 
                        = (0x0000003fU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2) 
                                          >> 2U));
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3 
                        = (0x0000003fU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3) 
                                          >> 2U));
                }
            }
        }
        if (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
             >= ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                  ? 0x0780U : 0x0500U))) {
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_x = 0U;
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_word = 0U;
        } else if ((2U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_x))) {
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_word 
                = (0x000003ffU & ((IData)(1U) + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_word)));
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_x = 0U;
        } else {
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_x 
                = (3U & ((IData)(1U) + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_x)));
        }
        if (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt) 
             >= ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                  ? 0x0438U : 0x02d0U))) {
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_y = 0U;
        } else if (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                    == (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                          ? 0x0898U : 0x0672U) - (IData)(1U)))) {
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_y 
                = ((2U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_y))
                    ? 0U : (3U & ((IData)(1U) + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_y))));
        }
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_y 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_y;
        if (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
             == ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                  ? 0x0780U : 0x0500U))) {
            if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_line) {
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_tog 
                    = (1U & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_tog)));
            }
        }
        if ((((0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt)) 
              & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt) 
                 == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__out_vss))) 
             & ((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_req_p) 
                       >> 1U)) != (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)))) {
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080 
                = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_req_p) 
                         >> 1U));
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt = 0U;
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt 
                = ((2U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_req_p))
                    ? 0x0000043cU : 0x000002d5U);
        } else if (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                    == ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__out_htot) 
                        - (IData)(1U)))) {
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt 
                = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt) 
                    == (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                          ? 0x0465U : 0x02eeU) - (IData)(1U)))
                    ? 0U : (0x00000fffU & ((IData)(1U) 
                                           + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt))));
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt = 0U;
        } else {
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt 
                = (0x00000fffU & ((IData)(1U) + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt)));
        }
    } else {
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tone_ph = 0U;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__mode_req_p = 0U;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__slot = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__slot 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__slot;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pc = 0U;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_x = 0U;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_word = 0U;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_y = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_y 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_y;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_tog = 0U;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt = 0U;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt = 0U;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080 = 0U;
    }
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_req_p 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__mode_req_p;
    if (__VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache1__v0) {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache1[__VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache1__v0] 
            = __VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache1__v0;
    }
    if (__VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache0__v0) {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache0[__VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache0__v0] 
            = __VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache0__v0;
    }
    if (__VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache2__v0) {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache2[__VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache2__v0] 
            = __VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__line_cache2__v0;
    }
    if ((8U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr))) {
        if ((1U & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__latch)))) {
            if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__load0) {
                if ((0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__lty))) {
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__hsr 
                        = (2U | ((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kbb) 
                                   & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kpres)) 
                                  << 0x00000014U) | 
                                 ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kpres) 
                                  << 8U)));
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2 
                        = ((((QData)((IData)(((2U & 
                                               ((VL_REDXOR_16(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kr2) 
                                                 << 1U) 
                                                ^ (0x7ffffffeU 
                                                   & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb) 
                                                      >> 1U)))) 
                                              | (1U 
                                                 & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb) 
                                                     >> 2U) 
                                                    ^ 
                                                    VL_REDXOR_16(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kl2)))))) 
                             << 0x00000036U) | (((QData)((IData)(
                                                                 (1U 
                                                                  & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb) 
                                                                     >> 2U)))) 
                                                 << 0x00000035U) 
                                                | (((QData)((IData)(
                                                                    (1U 
                                                                     & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb) 
                                                                        >> 2U)))) 
                                                    << 0x00000034U) 
                                                   | (((QData)((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kr2)) 
                                                       << 0x00000020U) 
                                                      | (QData)((IData)(
                                                                        ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kl2) 
                                                                         << 8U))))))) 
                           & (- (QData)((IData)((1U 
                                                 & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kpres) 
                                                    >> 2U))))));
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3 
                        = ((((QData)((IData)(((2U & 
                                               ((VL_REDXOR_16(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kr3) 
                                                 << 1U) 
                                                ^ (0x3ffffffeU 
                                                   & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb) 
                                                      >> 2U)))) 
                                              | (1U 
                                                 & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb) 
                                                     >> 3U) 
                                                    ^ 
                                                    VL_REDXOR_16(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kl3)))))) 
                             << 0x00000036U) | (((QData)((IData)(
                                                                 (1U 
                                                                  & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb) 
                                                                     >> 3U)))) 
                                                 << 0x00000035U) 
                                                | (((QData)((IData)(
                                                                    (1U 
                                                                     & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb) 
                                                                        >> 3U)))) 
                                                    << 0x00000034U) 
                                                   | (((QData)((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kr3)) 
                                                       << 0x00000020U) 
                                                      | (QData)((IData)(
                                                                        ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kl3) 
                                                                         << 8U))))))) 
                           & (- (QData)((IData)((1U 
                                                 & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kpres) 
                                                    >> 3U))))));
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1 
                        = ((((QData)((IData)(((2U & 
                                               ((VL_REDXOR_16(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kr1) 
                                                 << 1U) 
                                                ^ (0xfffffffeU 
                                                   & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb)))) 
                                              | (1U 
                                                 & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb) 
                                                     >> 1U) 
                                                    ^ 
                                                    VL_REDXOR_16(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kl1)))))) 
                             << 0x00000036U) | (((QData)((IData)(
                                                                 (1U 
                                                                  & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb) 
                                                                     >> 1U)))) 
                                                 << 0x00000035U) 
                                                | (((QData)((IData)(
                                                                    (1U 
                                                                     & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb) 
                                                                        >> 1U)))) 
                                                    << 0x00000034U) 
                                                   | (((QData)((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kr1)) 
                                                       << 0x00000020U) 
                                                      | (QData)((IData)(
                                                                        ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kl1) 
                                                                         << 8U))))))) 
                           & (- (QData)((IData)((1U 
                                                 & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kpres) 
                                                    >> 1U))))));
                } else {
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__hsr 
                        = ((1U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__lty))
                            ? 1U : ((2U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__lty))
                                     ? 0x000d0282U : 0x000a0184U));
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2 
                        = (0x0000180087220100ULL & 
                           (- (QData)((IData)((1U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__lty))))));
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3 
                        = (0x0000180087220100ULL & 
                           (- (QData)((IData)((1U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__lty))))));
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1 
                        = (0x0000180087220100ULL & 
                           (- (QData)((IData)((1U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__lty))))));
                }
            } else if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__idat0) {
                if ((0x18U > (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pc))) {
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__hsr 
                        = (0x007fffffU & (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__hsr 
                                          >> 1U));
                }
                if ((0x1cU > (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pc))) {
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2 
                        = (0x003fffffffffffffULL & 
                           (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2 
                            >> 2U));
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3 
                        = (0x003fffffffffffffULL & 
                           (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3 
                            >> 2U));
                    __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1 
                        = (0x003fffffffffffffULL & 
                           (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1 
                            >> 2U));
                }
            }
        }
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__cnt 
            = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__vid_b)
                ? (0x0000001fU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision2)
                                   ? ((0x00000100U 
                                       & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__q_m_reg))
                                       ? (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__cnt) 
                                           + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n1q_m)) 
                                          - (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n0q_m))
                                       : (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__cnt) 
                                           + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n0q_m)) 
                                          - (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n1q_m)))
                                   : ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision3)
                                       ? (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__cnt) 
                                           + (2U & 
                                              ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__q_m_reg) 
                                               >> 7U))) 
                                          + ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n0q_m) 
                                             - (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n1q_m)))
                                       : (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__cnt) 
                                           - (2U & 
                                              ((~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__q_m_reg) 
                                                   >> 8U)) 
                                               << 1U))) 
                                          + ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n1q_m) 
                                             - (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n0q_m))))))
                : 0U);
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__cnt 
            = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__vid_b)
                ? (0x0000001fU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision2)
                                   ? ((0x00000100U 
                                       & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__q_m_reg))
                                       ? (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__cnt) 
                                           + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n1q_m)) 
                                          - (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n0q_m))
                                       : (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__cnt) 
                                           + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n0q_m)) 
                                          - (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n1q_m)))
                                   : ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision3)
                                       ? (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__cnt) 
                                           + (2U & 
                                              ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__q_m_reg) 
                                               >> 7U))) 
                                          + ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n0q_m) 
                                             - (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n1q_m)))
                                       : (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__cnt) 
                                           - (2U & 
                                              ((~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__q_m_reg) 
                                                   >> 8U)) 
                                               << 1U))) 
                                          + ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n1q_m) 
                                             - (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n0q_m))))))
                : 0U);
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__cnt 
            = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__vid_b)
                ? (0x0000001fU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision2)
                                   ? ((0x00000100U 
                                       & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__q_m_reg))
                                       ? (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__cnt) 
                                           + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n1q_m)) 
                                          - (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n0q_m))
                                       : (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__cnt) 
                                           + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n0q_m)) 
                                          - (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n1q_m)))
                                   : ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision3)
                                       ? (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__cnt) 
                                           + (2U & 
                                              ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__q_m_reg) 
                                               >> 7U))) 
                                          + ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n0q_m) 
                                             - (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n1q_m)))
                                       : (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__cnt) 
                                           - (2U & 
                                              ((~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__q_m_reg) 
                                                   >> 8U)) 
                                               << 1U))) 
                                          + ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n1q_m) 
                                             - (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n0q_m))))))
                : 0U);
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__source_req) {
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__cache_addr_d 
                = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_word;
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__cache_bad_d 
                = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf0;
        }
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_word 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_word;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v1 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt;
        if ((0x00912b80U <= vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acc)) {
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acc 
                = (0x00ffffffU & ((IData)(0x006ed480U) 
                                  + vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acc));
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tick = 1U;
        } else {
            __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acc 
                = (0x00ffffffU & ((IData)(0x00001800U) 
                                  + vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acc));
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tick = 0U;
        }
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acc 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acc;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_x 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_x;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h1 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt;
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__latch) {
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ty1 
                = Vhdmi_dualvideo_tb__ConstPool__TABLE_h83b4be6e_0
                [vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0];
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ty0 
                = Vhdmi_dualvideo_tb__ConstPool__TABLE_hea843932_0
                [vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0];
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ty2 
                = Vhdmi_dualvideo_tb__ConstPool__TABLE_hefdb3507_0
                [vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0];
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__lhdmi 
                = (1U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__actl_p2));
        }
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wgray_r2 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wgray_r1;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_cnt 
            = ((0x00012286U == vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_cnt)
                ? 0U : (0x000fffffU & ((IData)(1U) 
                                       + vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_cnt)));
        if ((0U == vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_cnt)) {
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_due = 1U;
        } else if ((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__latch) 
                     & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__s_acr)) 
                    & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__actl_p2))) {
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_due = 0U;
        }
    } else {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__cnt = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__cnt = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__cnt = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__cache_addr_d = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_word 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_word;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__cache_bad_d = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v1 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acc = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tick = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acc 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acc;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_x 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__scale_x;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h1 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ty1 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ty0 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ty2 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wgray_r2 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__lhdmi = 0U;
        __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_cnt = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_due = 0U;
    }
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_cnt 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_cnt;
    hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tone_s 
        = (0x0000ffffU & Vhdmi_dualvideo_tb__ConstPool__CONST_h54f42bc6_0
           [(0x07ffffffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tone_ph))]);
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_high_d 
        = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
            >> 3U) & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__cache_write_d));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__lty 
        = ((0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__slot))
            ? (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ty0)
            : ((1U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__slot))
                ? (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ty1)
                : (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ty2)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_raddr 
        = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_word) 
            << 1U) | (2U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__scale_x)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__actl_p2 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__actl_p1;
    if ((8U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr))) {
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__latch) {
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kr0 
                = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pr0;
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kl0 
                = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pl0;
        }
        vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_d1 = 
            ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__vid_b)
              ? ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision2)
                  ? ((0x00000200U & ((~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__q_m_reg) 
                                         >> 8U)) << 9U)) 
                     | ((0x00000100U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__q_m_reg)) 
                        | (0x000000ffU & ((0x00000100U 
                                           & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__q_m_reg))
                                           ? (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__q_m_reg)
                                           : (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__q_m_reg))))))
                  : ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision3)
                      ? (0x00000200U | ((0x00000100U 
                                         & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__q_m_reg)) 
                                        | (0x000000ffU 
                                           & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__q_m_reg)))))
                      : (0x000001ffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__q_m_reg))))
              : (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__sym_b));
    } else {
        vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_d1 = 0U;
    }
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__vid_b 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__vid_a;
    __Vtableidx2 = (((((4U & ((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision1)) 
                              << 2U)) | ((2U & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision1)
                                                  ? 
                                                 (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16))
                                                  : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16)) 
                                                << 1U)) 
                                         | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm6))) 
                      << 6U) | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm5) 
                                 << 5U) | ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm4) 
                                           << 4U))) 
                    | ((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm3) 
                         << 3U) | ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm2) 
                                   << 2U)) | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm1) 
                                               << 1U) 
                                              | (1U 
                                                 & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q)))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n1q_m 
        = Vhdmi_dualvideo_tb__ConstPool__TABLE_h9c3d9630_0
        [__Vtableidx2];
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n0q_m 
        = Vhdmi_dualvideo_tb__ConstPool__TABLE_h66e20c3f_0
        [__Vtableidx2];
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__q_m_reg 
        = Vhdmi_dualvideo_tb__ConstPool__TABLE_h41587afb_0
        [__Vtableidx2];
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__sym_b 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__sym_a;
    vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_d2 = ((8U 
                                                  & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr))
                                                  ? 
                                                 ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__vid_b)
                                                   ? 
                                                  ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision2)
                                                    ? 
                                                   ((0x00000200U 
                                                     & ((~ 
                                                         ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__q_m_reg) 
                                                          >> 8U)) 
                                                        << 9U)) 
                                                    | ((0x00000100U 
                                                        & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__q_m_reg)) 
                                                       | (0x000000ffU 
                                                          & ((0x00000100U 
                                                              & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__q_m_reg))
                                                              ? (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__q_m_reg)
                                                              : 
                                                             (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__q_m_reg))))))
                                                    : 
                                                   ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision3)
                                                     ? 
                                                    (0x00000200U 
                                                     | ((0x00000100U 
                                                         & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__q_m_reg)) 
                                                        | (0x000000ffU 
                                                           & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__q_m_reg)))))
                                                     : 
                                                    (0x000001ffU 
                                                     & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__q_m_reg))))
                                                   : (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__sym_b))
                                                  : 0U);
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__vid_b 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__vid_a;
    __Vtableidx1 = (((((4U & ((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision1)) 
                              << 2U)) | ((2U & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision1)
                                                  ? 
                                                 (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_10))
                                                  : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_10)) 
                                                << 1U)) 
                                         | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm6))) 
                      << 6U) | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm5) 
                                 << 5U) | ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm4) 
                                           << 4U))) 
                    | ((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm3) 
                         << 3U) | ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm2) 
                                   << 2U)) | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm1) 
                                               << 1U) 
                                              | (1U 
                                                 & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q)))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n1q_m 
        = Vhdmi_dualvideo_tb__ConstPool__TABLE_h9c3d9630_0
        [__Vtableidx1];
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n0q_m 
        = Vhdmi_dualvideo_tb__ConstPool__TABLE_h66e20c3f_0
        [__Vtableidx1];
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__q_m_reg 
        = Vhdmi_dualvideo_tb__ConstPool__TABLE_h41587afb_0
        [__Vtableidx1];
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__sym_b 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__sym_a;
    vlSelfRef.hdmi_dualvideo_tb__DOT__tmds_d0 = ((8U 
                                                  & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr))
                                                  ? 
                                                 ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__vid_b)
                                                   ? 
                                                  ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision2)
                                                    ? 
                                                   ((0x00000200U 
                                                     & ((~ 
                                                         ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__q_m_reg) 
                                                          >> 8U)) 
                                                        << 9U)) 
                                                    | ((0x00000100U 
                                                        & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__q_m_reg)) 
                                                       | (0x000000ffU 
                                                          & ((0x00000100U 
                                                              & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__q_m_reg))
                                                              ? (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__q_m_reg)
                                                              : 
                                                             (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__q_m_reg))))))
                                                    : 
                                                   ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision3)
                                                     ? 
                                                    (0x00000200U 
                                                     | ((0x00000100U 
                                                         & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__q_m_reg)) 
                                                        | (0x000000ffU 
                                                           & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__q_m_reg)))))
                                                     : 
                                                    (0x000001ffU 
                                                     & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__q_m_reg))))
                                                   : (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__sym_b))
                                                  : 0U);
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__vid_b 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__vid_a;
    __Vtableidx3 = (((((4U & ((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision1)) 
                              << 2U)) | ((2U & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision1)
                                                  ? 
                                                 (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_22))
                                                  : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_22)) 
                                                << 1U)) 
                                         | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm6))) 
                      << 6U) | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm5) 
                                 << 5U) | ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm4) 
                                           << 4U))) 
                    | ((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm3) 
                         << 3U) | ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm2) 
                                   << 2U)) | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm1) 
                                               << 1U) 
                                              | (1U 
                                                 & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q)))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n1q_m 
        = Vhdmi_dualvideo_tb__ConstPool__TABLE_h9c3d9630_0
        [__Vtableidx3];
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n0q_m 
        = Vhdmi_dualvideo_tb__ConstPool__TABLE_h66e20c3f_0
        [__Vtableidx3];
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__q_m_reg 
        = Vhdmi_dualvideo_tb__ConstPool__TABLE_h41587afb_0
        [__Vtableidx3];
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__sym_b 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__sym_a;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray 
        = ((8U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr))
            ? ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rbin_n) 
               ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rbin_n) 
                  >> 1U)) : 0U);
    hdmi_dualvideo_tb__DOT__dut__DOT__afifo_rempty 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray) 
           == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wgray_r2));
    hdmi_dualvideo_tb__DOT__dut__DOT__fifo_rempty = 
        ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray) 
         == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wgray_r2));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__cache_write_d 
        = ((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                  >> 3U)) && ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__source_req) 
                              & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__s_acr 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__acr_due) 
           & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__actl_p2) 
              >> 1U));
    __VdfgRegularize_h6e95ff9d_0_3 = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tick) 
                                      & (2U == (6U 
                                                & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__actl_p2))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_tone 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__tick) 
           & (6U == (6U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__actl_p2))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__actl_p1 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__aud_ctrl;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__vid_a 
        = (1U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m1_2));
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
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g2;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__sym_a 
        = (0x000003ffU & ((0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m1_2))
                           ? ((0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__c1_2))
                               ? 0x0354U : ((1U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__c1_2))
                                             ? 0x00abU
                                             : ((2U 
                                                 == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__c1_2))
                                                 ? 0x0154U
                                                 : 0x02abU)))
                           : ((2U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m1_2))
                               ? Vhdmi_dualvideo_tb__ConstPool__CONST_hbdb02f09_0
                              [(0x07ffffffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__t1_2))]
                               : (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__f1_2))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__vid_a 
        = (1U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m2_2));
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
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__r2;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__sym_a 
        = (0x000003ffU & ((0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m2_2))
                           ? ((0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__c2_2))
                               ? 0x0354U : ((1U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__c2_2))
                                             ? 0x00abU
                                             : ((2U 
                                                 == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__c2_2))
                                                 ? 0x0154U
                                                 : 0x02abU)))
                           : ((2U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m2_2))
                               ? Vhdmi_dualvideo_tb__ConstPool__CONST_hbdb02f09_0
                              [(0x07ffffffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__t2_2))]
                               : (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__f2_2))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__vid_a 
        = (1U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m0_2));
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
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__b2;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__sym_a 
        = (0x000003ffU & ((0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m0_2))
                           ? ((0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__c0_2))
                               ? 0x0354U : ((1U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__c0_2))
                                             ? 0x00abU
                                             : ((2U 
                                                 == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__c0_2))
                                                 ? 0x0154U
                                                 : 0x02abU)))
                           : ((2U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m0_2))
                               ? Vhdmi_dualvideo_tb__ConstPool__CONST_hbdb02f09_0
                              [(0x07ffffffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__t0_2))]
                               : (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__f0_2))));
    if ((8U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr))) {
        if (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
             == ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                  ? 0x0780U : 0x0500U))) {
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_line = 0U;
        } else if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf0) {
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_line = 1U;
        }
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__latch) {
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kbb 
                = ((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__bb3) 
                     << 3U) | ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__bb2) 
                               << 2U)) | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__bb1) 
                                           << 1U) | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__bb0)));
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kr2 
                = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pr2;
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kl2 
                = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pl2;
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kr3 
                = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pr3;
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kl3 
                = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pl3;
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kr1 
                = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pr1;
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kl1 
                = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pl1;
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcb 
                = ((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cb3) 
                     << 3U) | ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cb2) 
                               << 2U)) | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cb1) 
                                           << 1U) | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cb0)));
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcnt 
                = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pcnt;
        }
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rbin 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rbin_n;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wgray_r1 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wgray;
        if (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__latch) 
             | ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap) 
                & (4U != (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pcnt))))) {
            if ((0U == ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__latch)
                         ? 0U : (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pcnt)))) {
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pl0 
                    = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_l;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pr0 
                    = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_r;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cb0 
                    = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_c;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__bb0 
                    = (0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx));
            } else if ((1U == ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__latch)
                                ? 0U : (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pcnt)))) {
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pl1 
                    = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_l;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pr1 
                    = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_r;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cb1 
                    = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_c;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__bb1 
                    = (0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx));
            } else if ((2U == ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__latch)
                                ? 0U : (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pcnt)))) {
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pl2 
                    = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_l;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pr2 
                    = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_r;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cb2 
                    = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_c;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__bb2 
                    = (0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx));
            } else {
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pl3 
                    = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_l;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pr3 
                    = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_r;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cb3 
                    = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_c;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__bb3 
                    = (0U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx));
            }
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pcnt 
                = (7U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__latch)
                          ? ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap)
                              ? 1U : 0U) : ((IData)(1U) 
                                            + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pcnt))));
        }
    } else {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_line = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rbin = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wgray_r1 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pcnt = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcnt = 0U;
    }
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__afifo_ren 
        = ((~ (IData)(hdmi_dualvideo_tb__DOT__dut__DOT__afifo_rempty)) 
           & (IData)(__VdfgRegularize_h6e95ff9d_0_3));
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
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_tone) 
           | ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ren_d) 
              | ((IData)(hdmi_dualvideo_tb__DOT__dut__DOT__afifo_rempty) 
                 & (IData)(__VdfgRegularize_h6e95ff9d_0_3))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_waddr 
        = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__cache_addr_d) 
            << 1U) | (1U & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__cache_write_d))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g_scale__DOT__cache_wdata 
        = (0x0000ffffU & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__cache_write_d)
                            ? vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fifo_rdata
                            : (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fifo_rdata 
                               >> 0x00000010U)) & (- (IData)(
                                                             (1U 
                                                              & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__cache_bad_d)))))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__cap_c 
        = ((2U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx)) 
           | ((0x19U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx)) 
              | (0x21U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__fidx))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kpres 
        = (0x0000000fU & (((1U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcnt))
                            ? 1U : ((2U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcnt))
                                     ? 3U : (7U | (- (IData)(
                                                             (3U 
                                                              != (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcnt))))))) 
                          & (- (IData)((0U != (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__kcnt))))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rbin_n 
        = (0x000001ffU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rbin) 
                          + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__afifo_ren)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision1 
        = ((4U < (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n1d)) 
           | ((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q)) 
              & (4U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__n1d))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__f1_2 = 0x0133U;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision1 
        = ((4U < (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n1d)) 
           | ((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q)) 
              & (4U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__n1d))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision1 
        = ((4U < (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n1d)) 
           | ((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q)) 
              & (4U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__n1d))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__f0_2 = 0x02ccU;
    if ((8U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr))) {
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__de1) {
            if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__frame_en) {
                if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf1) {
                    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g2 
                        = (0x000000ffU & 0U);
                    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__r2 
                        = (0x000000ffU & 0U);
                    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__b2 
                        = (0x000000ffU & 0U);
                } else {
                    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g2 
                        = (0x000000ffU & ((0x000000fcU 
                                           & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__pix565) 
                                              >> 3U)) 
                                          | (3U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__pix565) 
                                                   >> 9U))));
                    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__r2 
                        = (0x000000ffU & ((0x000000f8U 
                                           & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__pix565) 
                                              >> 8U)) 
                                          | (7U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__pix565) 
                                                   >> 0x0dU))));
                    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__b2 
                        = (0x000000ffU & ((0x000000f8U 
                                           & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__pix565) 
                                              << 3U)) 
                                          | (7U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__pix565) 
                                                   >> 2U))));
                }
            } else {
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g2 
                    = (0x000000ffU & (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__bar_rgb 
                                      >> 8U));
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__r2 
                    = (0x000000ffU & (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__bar_rgb 
                                      >> 0x10U));
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__b2 
                    = (0x000000ffU & vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__bar_rgb);
            }
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m1_2 = 1U;
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m2_2 = 1U;
        } else {
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g2 = 0U;
            if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vgb1) {
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m1_2 = 3U;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m2_2 = 3U;
            } else if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__igb1) {
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m1_2 = 3U;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m2_2 = 3U;
            } else if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__idat1) {
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m1_2 = 2U;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m2_2 = 2U;
            } else {
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m1_2 = 0U;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m2_2 = 0U;
            }
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__r2 = 0U;
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__b2 = 0U;
        }
        if ((0x1cU > (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pc))) {
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__t1_2 
                = ((((2U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3) 
                            << 1U)) | (1U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2))) 
                    << 2U) | ((2U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1) 
                                     << 1U)) | (1U 
                                                & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q0))));
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__t2_2 
                = ((((2U & ((IData)((vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3 
                                     >> 1U)) << 1U)) 
                     | (1U & (IData)((vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2 
                                      >> 1U)))) << 2U) 
                   | ((2U & ((IData)((vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1 
                                      >> 1U)) << 1U)) 
                      | (1U & (IData)((vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q0 
                                       >> 1U)))));
        } else {
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__t1_2 
                = ((((2U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3) 
                            << 1U)) | (1U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2))) 
                    << 2U) | ((2U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1) 
                                     << 1U)) | (1U 
                                                & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0))));
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__t2_2 
                = ((((2U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3)) 
                     | (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2) 
                              >> 1U))) << 2U) | ((2U 
                                                  & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1)) 
                                                 | (1U 
                                                    & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0) 
                                                       >> 1U))));
        }
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__c1_2 
            = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vpre1) 
               | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ipre1));
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q0 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q0;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__f2_2 
            = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vgb1)
                ? 0x02ccU : 0x0133U);
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__c2_2 
            = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ipre1;
    } else {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__g2 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__t1_2 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__c1_2 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m1_2 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__r2 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__t2_2 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q0 
            = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q0;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__f2_2 = 0x02ccU;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__c2_2 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m2_2 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__b2 = 0U;
    }
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
    if ((8U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr))) {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__c0_2 
            = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs1) 
                << 1U) | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__hs1));
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m0_2 
            = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__de1)
                ? 1U : ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vgb1)
                         ? 3U : ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__igb1)
                                  ? 2U : ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__idat1)
                                           ? 2U : 0U))));
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__t0_2 
            = ((((((~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__igb1) 
                       | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ifirst1))) 
                   & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__idat1)) 
                  | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__igb1)) 
                 << 3U) | (4U & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__igb1) 
                                  | ((0x18U > (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pc))
                                      ? vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__hsr
                                      : (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__eh))) 
                                 << 2U))) | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs1) 
                                              << 1U) 
                                             | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__hs1)));
    } else {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__c0_2 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__m0_2 = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__t0_2 = 0U;
    }
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__eh 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__eh;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__hsr 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__hsr;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pc 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__pc;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm1 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision1)
                  ? (~ VL_REDXOR_32((3U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q))))
                  : VL_REDXOR_32((3U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q)))));
    __VdfgRegularize_h6e95ff9d_0_11 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm1) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q) 
                                                >> 2U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vpre1 
        = ((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                  >> 3U)) && ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4) 
                              & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                                  >= ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                       ? 0x088eU : 0x0668U)) 
                                 & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                                    < ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                        ? 0x0896U : 0x0670U)))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm1 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision1)
                  ? (~ VL_REDXOR_32((3U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q))))
                  : VL_REDXOR_32((3U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q)))));
    __VdfgRegularize_h6e95ff9d_0_5 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm1) 
                                            ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q) 
                                               >> 2U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3a 
        = ((0x83U & (- (IData)((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q3) 
                                      ^ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3)))))) 
           ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e3) 
              >> 1U));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2a 
        = ((0x83U & (- (IData)((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q2) 
                                      ^ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2)))))) 
           ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e2) 
              >> 1U));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1a 
        = ((0x83U & (- (IData)((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q1) 
                                      ^ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1)))))) 
           ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e1) 
              >> 1U));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0a 
        = ((0x83U & (- (IData)((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__q0) 
                                      ^ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0)))))) 
           ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__e0) 
              >> 1U));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ipre1 
        = ((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                  >> 3U)) && ((0U != (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ln)) 
                              & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                                  >= ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                       ? 0x0784U : 0x0504U)) 
                                 & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                                    < ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                        ? 0x078cU : 0x050cU)))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm1 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision1)
                  ? (~ VL_REDXOR_32((3U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q))))
                  : VL_REDXOR_32((3U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q)))));
    __VdfgRegularize_h6e95ff9d_0_17 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm1) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q) 
                                                >> 2U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf1 
        = ((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                  >> 3U)) && ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__word_start)
                               ? (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf0)
                               : (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_word)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__de1 
        = ((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                  >> 3U)) && (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__de0));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vgb1 
        = ((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                  >> 3U)) && (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                               >= ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                    ? 0x0896U : 0x0670U)) 
                              & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs1 
        = ((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                  >> 3U)) && (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs0));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__idat1 
        = ((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                  >> 3U)) && (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__idat0));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__hs1 
        = ((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                  >> 3U)) && (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                               >= ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                    ? 0x07d8U : 0x056eU)) 
                              & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                                 < ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                     ? 0x0804U : 0x0596U))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ifirst1 
        = ((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                  >> 3U)) && ((0U != (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ln)) 
                              & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                                 == ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                      ? 0x078eU : 0x050eU))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__igb1 
        = ((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                  >> 3U)) && ((0U != (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ln)) 
                              & ((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                                   >= ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                        ? 0x078cU : 0x050cU)) 
                                  & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                                     < ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                         ? 0x078eU : 0x050eU))) 
                                 | (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                                     >= (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__iend)) 
                                    & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                                       < (0x00000fffU 
                                          & ((IData)(2U) 
                                             + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__iend))))))));
    if ((8U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr))) {
        if (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_zone) 
             & (~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs0)))) {
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__frame_en 
                = (IData)((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__en_p_s) 
                            >> 1U) & (~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__test_p_s) 
                                         >> 1U))));
        }
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__word_start) {
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_word 
                = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf0;
        }
        if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__latch) {
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ln 
                = Vhdmi_dualvideo_tb__ConstPool__TABLE_hbd0c7d12_0
                [vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0];
        }
    } else {
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__frame_en = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf_word = 0U;
        vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ln = 0U;
    }
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__en_p_s 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__en_p_s;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__test_p_s 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__test_p_s;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080 
        = __Vdly__hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080;
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm2 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_11))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_11)));
    __VdfgRegularize_h6e95ff9d_0_12 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm2) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q) 
                                                >> 3U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm2 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_5))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_5)));
    __VdfgRegularize_h6e95ff9d_0_6 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm2) 
                                            ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q) 
                                               >> 3U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm2 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_17))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_17)));
    __VdfgRegularize_h6e95ff9d_0_18 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm2) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q) 
                                                >> 3U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_zone 
        = ((1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                  >> 3U)) && (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs0));
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
    hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__s_avi 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt) 
           == ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                ? 0x0438U : 0x02d0U));
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
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs0 
        = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt) 
            >= ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                 ? 0x043cU : 0x02d5U)) & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt) 
                                          < ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                              ? 0x0441U
                                              : 0x02daU)));
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
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__latch 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
           == ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                ? 0x0778U : 0x04f8U));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm3 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_12))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_12)));
    __VdfgRegularize_h6e95ff9d_0_13 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm3) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q) 
                                                >> 4U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm3 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_6))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_6)));
    __VdfgRegularize_h6e95ff9d_0_7 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm3) 
                                            ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q) 
                                               >> 4U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm3 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_18))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_18)));
    __VdfgRegularize_h6e95ff9d_0_19 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm3) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q) 
                                                >> 4U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__source_req 
        = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__de0) 
           & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__word_start) 
              & ((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__repeat_line)) 
                 & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__frame_en))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__iend 
        = (0x00000fffU & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ln) 
                           << 5U) + ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                      ? 0x078eU : 0x050eU)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__uf0 
        = ((IData)(hdmi_dualvideo_tb__DOT__dut__DOT__fifo_rempty) 
           & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__source_req));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fifo_ren 
        = ((~ (IData)(hdmi_dualvideo_tb__DOT__dut__DOT__fifo_rempty)) 
           & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__source_req));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__idat0 
        = ((0U != (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__ln)) 
           & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
               >= ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                    ? 0x078eU : 0x050eU)) & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                                             < (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__iend))));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm4 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_13))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_13)));
    __VdfgRegularize_h6e95ff9d_0_14 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm4) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q) 
                                                >> 5U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm4 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_7))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_7)));
    __VdfgRegularize_h6e95ff9d_0_8 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm4) 
                                            ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q) 
                                               >> 5U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm4 
        = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__decision1)
                  ? (~ (IData)(__VdfgRegularize_h6e95ff9d_0_19))
                  : (IData)(__VdfgRegularize_h6e95ff9d_0_19)));
    __VdfgRegularize_h6e95ff9d_0_20 = (1U & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm4) 
                                             ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q) 
                                                >> 5U)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rbin_n 
        = (0x00000fffU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rbin) 
                          + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fifo_ren)));
    vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__load0 
        = ((0U == (0x0000001fU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt) 
                                  - ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080)
                                      ? 0x078eU : 0x050eU)))) 
           & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_island__DOT__idat0));
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
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16 = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__qm6) 
                                                 ^ 
                                                 ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds1__DOT__din_q) 
                                                  >> 7U));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_10 = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__qm6) 
                                                 ^ 
                                                 ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds2__DOT__din_q) 
                                                  >> 7U));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_22 = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__qm6) 
                                                 ^ 
                                                 ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_tmds0__DOT__din_q) 
                                                  >> 7U));
}

void Vhdmi_dualvideo_tb___024root___eval_body__nba(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_body__nba\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vhdmi_dualvideo_tb___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((4ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vhdmi_dualvideo_tb___024root___nba_sequent__TOP__1(vlSelf);
    }
    if ((0x000000000000000cULL & vlSelfRef.__VnbaTriggered[0U])) {
        {
            // Inlined CFunc: _nba_sequent__TOP__2
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr 
                = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__pix_rst_n)
                    ? (1U | (0x0000000eU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                                            << 1U)))
                    : 0U);
        }
    }
    if ((2ULL & vlSelfRef.__VnbaTriggered[0U])) {
        {
            // Inlined CFunc: _nba_sequent__TOP__3
            IData/*31:0*/ __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0__Vfuncout;
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0__Vfuncout = 0;
            IData/*31:0*/ __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0__addr;
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0__addr = 0;
            SData/*15:0*/ __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0____VlefCall_1__pixel;
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0____VlefCall_1__pixel = 0;
            SData/*15:0*/ __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0____VlefCall_0__pixel;
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0____VlefCall_0__pixel = 0;
            IData/*31:0*/ __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0__p;
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0__p = 0;
            SData/*15:0*/ __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__1__Vfuncout;
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__1__Vfuncout = 0;
            IData/*31:0*/ __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__1__x;
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__1__x = 0;
            IData/*31:0*/ __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__1__y;
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__1__y = 0;
            SData/*15:0*/ __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__2__Vfuncout;
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__2__Vfuncout = 0;
            IData/*31:0*/ __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__2__x;
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__2__x = 0;
            IData/*31:0*/ __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__2__y;
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__2__y = 0;
            vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rlast 
                = (0x0000003fU == vlSelfRef.hdmi_dualvideo_tb__DOT__beat);
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0__addr 
                = (vlSelfRef.hdmi_dualvideo_tb__DOT__addresses
                   [(0x0000ffffU & vlSelfRef.hdmi_dualvideo_tb__DOT__rp)] 
                   + (vlSelfRef.hdmi_dualvideo_tb__DOT__beat 
                      << 2U));
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0__p 
                = ((__Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0__addr 
                    - (IData)(0x01000000U)) >> 1U);
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__1__y 
                = VL_DIVS_III(32, ((IData)(1U) + __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0__p), (IData)(0x00000500U));
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__1__x 
                = VL_MODDIVS_III(32, ((IData)(1U) + __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0__p), (IData)(0x00000500U));
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__1__Vfuncout 
                = (0x0000ffffU & (0x1357U ^ (((IData)(0x0000000dU) 
                                              * __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__1__x) 
                                             ^ ((IData)(0x000000f1U) 
                                                * __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__1__y))));
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0____VlefCall_0__pixel 
                = __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__1__Vfuncout;
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__2__y 
                = VL_DIVS_III(32, __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0__p, (IData)(0x00000500U));
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__2__x 
                = VL_MODDIVS_III(32, __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0__p, (IData)(0x00000500U));
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__2__Vfuncout 
                = (0x0000ffffU & (0x1357U ^ (((IData)(0x0000000dU) 
                                              * __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__2__x) 
                                             ^ ((IData)(0x000000f1U) 
                                                * __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__2__y))));
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0____VlefCall_1__pixel 
                = __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__pixel__2__Vfuncout;
            __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0__Vfuncout 
                = (((IData)(__Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0____VlefCall_0__pixel) 
                    << 0x00000010U) | __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0____VlefCall_1__pixel);
            vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rdata 
                = __Vinline_0__nba_sequent__TOP__3___Vfunc_hdmi_dualvideo_tb__DOT__memory_word__0__Vfuncout;
            vlSelfRef.hdmi_dualvideo_tb__DOT__rng = 
                ((vlSelfRef.hdmi_dualvideo_tb__DOT__rng 
                  << 1U) | (1U & VL_REDXOR_32((0x80200003U 
                                               & vlSelfRef.hdmi_dualvideo_tb__DOT__rng))));
            vlSelfRef.hdmi_dualvideo_tb__DOT__axi_arready 
                = ((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__rst)) 
                   & (0U != (7U & vlSelfRef.hdmi_dualvideo_tb__DOT__rng)));
            vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rvalid 
                = ((((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__rst)) 
                     & VL_LTS_III(32, vlSelfRef.hdmi_dualvideo_tb__DOT__rp, vlSelfRef.hdmi_dualvideo_tb__DOT__wp)) 
                    & VL_GTES_III(32, vlSelfRef.hdmi_dualvideo_tb__DOT__syscycle, vlSelfRef.hdmi_dualvideo_tb__DOT__due
                                  [(0x0000ffffU & vlSelfRef.hdmi_dualvideo_tb__DOT__rp)])) 
                   & (0U != (7U & (vlSelfRef.hdmi_dualvideo_tb__DOT__rng 
                                   >> 4U))));
        }
    }
    if ((0x00000000000001e2ULL & vlSelfRef.__VnbaTriggered[0U])) {
        {
            // Inlined CFunc: _nba_comb__TOP__0
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__dma_rst 
                = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__rst) 
                   | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rst));
        }
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        {
            // Inlined CFunc: _nba_sequent__TOP__4
            if (vlSelfRef.__VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__mem__v0) {
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__mem[vlSelfRef.__VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__mem__v0] 
                    = vlSelfRef.__VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__mem__v0;
            }
            if (vlSelfRef.__VdlySet__hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__mem__v0) {
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__mem[vlSelfRef.__VdlyDim0__hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__mem__v0] 
                    = vlSelfRef.__VdlyVal__hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__mem__v0;
            }
            if (vlSelfRef.hdmi_dualvideo_tb__DOT__rst) {
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ctrl_en = 0U;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ctrl_test = 0U;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_req = 0U;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wgray = 0U;
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__aud_ctrl = 0x33U;
            } else {
                if (vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__apb_wr) {
                    if ((1U & (~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr) 
                                  >> 7U)))) {
                        if ((1U & (~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr) 
                                      >> 6U)))) {
                            if ((1U & (~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr) 
                                          >> 5U)))) {
                                if ((1U & (~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr) 
                                              >> 4U)))) {
                                    if ((1U & (~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr) 
                                                  >> 3U)))) {
                                        if ((1U & (~ 
                                                   ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr) 
                                                    >> 2U)))) {
                                            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ctrl_en 
                                                = (1U 
                                                   & vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwdata);
                                            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ctrl_test 
                                                = (1U 
                                                   & (vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwdata 
                                                      >> 1U));
                                        }
                                    }
                                }
                            }
                            if ((0x00000020U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr))) {
                                if ((1U & (~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr) 
                                              >> 4U)))) {
                                    if ((1U & (~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr) 
                                                  >> 3U)))) {
                                        if ((1U & (~ 
                                                   ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr) 
                                                    >> 2U)))) {
                                            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__aud_ctrl 
                                                = (0x0000003fU 
                                                   & vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwdata);
                                        }
                                    }
                                }
                            }
                        }
                        if ((0x00000040U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr))) {
                            if ((1U & (~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr) 
                                          >> 5U)))) {
                                if ((0x00000010U & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr))) {
                                    if ((1U & (~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr) 
                                                  >> 3U)))) {
                                        if ((1U & (~ 
                                                   ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr) 
                                                    >> 2U)))) {
                                            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_req 
                                                = (1U 
                                                   & vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwdata);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wgray 
                    = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wbin_n) 
                       ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wbin_n) 
                          >> 1U));
            }
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wgray 
                = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fifo_wrst)
                    ? 0U : ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wbin_n) 
                            ^ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wbin_n) 
                               >> 1U)));
        }
    }
    if ((0x000000000000000cULL & vlSelfRef.__VnbaTriggered[0U])) {
        {
            // Inlined CFunc: _nba_comb__TOP__1
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT____Vcellinp__u_fifo__rrst 
                = (1U & ((~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_pix_rst__DOT__sr) 
                             >> 3U)) | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_zone)));
        }
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        {
            // Inlined CFunc: _nba_comb__TOP__2
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__ar_hs 
                = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_arvalid) 
                   & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_arready));
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wdo 
                = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wgray) 
                    != ((0x00000c00U & ((~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2) 
                                            >> 0x0000000aU)) 
                                        << 0x0000000aU)) 
                        | (0x000003ffU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__rgray_w2)))) 
                   & ((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__drop_mode)) 
                      & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__axi_rvalid)));
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wbin_n 
                = (0x00000fffU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wbin) 
                                  + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_fifo__DOT__wdo)));
        }
    }
    if ((0x00000000000001e3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        {
            // Inlined CFunc: _nba_comb__TOP__3
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__fifo_wrst 
                = (1U & (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__vs_s) 
                          >> 1U) | (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__dma_rst)));
        }
    }
    if ((0x00000000000001e2ULL & vlSelfRef.__VnbaTriggered[0U])) {
        {
            // Inlined CFunc: _nba_comb__TOP__4
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__apb_wr 
                = ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_psel) 
                   & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_penable) 
                      & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_pwrite)));
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__aud_apb_push 
                = (IData)(((0x0030U == (0x00fcU & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__apb_paddr))) 
                           & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__apb_wr)));
        }
    }
    if ((0x00000000000001e3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        {
            // Inlined CFunc: _nba_comb__TOP__5
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wdo 
                = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wgray) 
                    != ((0x00000180U & ((~ ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray_w2) 
                                            >> 7U)) 
                                        << 7U)) | (0x0000007fU 
                                                   & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__rgray_w2)))) 
                   & (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__aud_apb_push));
            vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wbin_n 
                = (0x000001ffU & ((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wbin) 
                                  + (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__u_aud_fifo__DOT__wdo)));
        }
    }
}

void Vhdmi_dualvideo_tb___024root___timing_ready(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___timing_ready\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((2ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h32447ec5__0.ready("@(negedge hdmi_dualvideo_tb.clk)");
    }
    if ((0x0000000000000020ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h1b3f35c3__0.ready("@( ((hdmi_dualvideo_tb.dut.frame_en & (12'h168 == hdmi_dualvideo_tb.dut.v_cnt)) & (12'h190 == hdmi_dualvideo_tb.dut.h_cnt)))");
    }
    if ((0x0000000000000040ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h85c17222__0.ready("@( hdmi_dualvideo_tb.dut.mode_1080)");
    }
    if ((0x0000000000000080ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h10016e0a__0.ready("@( ((12'h21c == hdmi_dualvideo_tb.dut.v_cnt) & (12'h1f4 == hdmi_dualvideo_tb.dut.h_cnt)))");
    }
    if ((0x0000000000000100ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_he9510f89__0.ready("@( ((32'sh2 == hdmi_dualvideo_tb.mode_changes) & (32'sh3 == hdmi_dualvideo_tb.frames)))");
    }
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_he72dfe88__0.ready("@(posedge hdmi_dualvideo_tb.pix_clk)");
    }
}

void Vhdmi_dualvideo_tb___024root___timing_resume(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___timing_resume\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VtrigSched_h32447ec5__0.moveToResumeQueue(
                                                          "@(negedge hdmi_dualvideo_tb.clk)");
    vlSelfRef.__VtrigSched_h1b3f35c3__0.moveToResumeQueue(
                                                          "@( ((hdmi_dualvideo_tb.dut.frame_en & (12'h168 == hdmi_dualvideo_tb.dut.v_cnt)) & (12'h190 == hdmi_dualvideo_tb.dut.h_cnt)))");
    vlSelfRef.__VtrigSched_h85c17222__0.moveToResumeQueue(
                                                          "@( hdmi_dualvideo_tb.dut.mode_1080)");
    vlSelfRef.__VtrigSched_h10016e0a__0.moveToResumeQueue(
                                                          "@( ((12'h21c == hdmi_dualvideo_tb.dut.v_cnt) & (12'h1f4 == hdmi_dualvideo_tb.dut.h_cnt)))");
    vlSelfRef.__VtrigSched_he9510f89__0.moveToResumeQueue(
                                                          "@( ((32'sh2 == hdmi_dualvideo_tb.mode_changes) & (32'sh3 == hdmi_dualvideo_tb.frames)))");
    vlSelfRef.__VtrigSched_he72dfe88__0.moveToResumeQueue(
                                                          "@(posedge hdmi_dualvideo_tb.pix_clk)");
    vlSelfRef.__VtrigSched_h32447ec5__0.resume("@(negedge hdmi_dualvideo_tb.clk)");
    vlSelfRef.__VtrigSched_h1b3f35c3__0.resume("@( ((hdmi_dualvideo_tb.dut.frame_en & (12'h168 == hdmi_dualvideo_tb.dut.v_cnt)) & (12'h190 == hdmi_dualvideo_tb.dut.h_cnt)))");
    vlSelfRef.__VtrigSched_h85c17222__0.resume("@( hdmi_dualvideo_tb.dut.mode_1080)");
    vlSelfRef.__VtrigSched_h10016e0a__0.resume("@( ((12'h21c == hdmi_dualvideo_tb.dut.v_cnt) & (12'h1f4 == hdmi_dualvideo_tb.dut.h_cnt)))");
    vlSelfRef.__VtrigSched_he9510f89__0.resume("@( ((32'sh2 == hdmi_dualvideo_tb.mode_changes) & (32'sh3 == hdmi_dualvideo_tb.frames)))");
    vlSelfRef.__VtrigSched_he72dfe88__0.resume("@(posedge hdmi_dualvideo_tb.pix_clk)");
    if ((0x0000000000000010ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vhdmi_dualvideo_tb___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___trigger_orInto__act_vec_vec\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((0U >= n));
}

void Vhdmi_dualvideo_tb___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

void Vhdmi_dualvideo_tb___024root____VbeforeTrig_h32447ec5__0(Vhdmi_dualvideo_tb___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root____VbeforeTrig_h32447ec5__0\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)((((~ (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__clk)) 
                                   & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__clk__0)) 
                                  << 1U)));
    vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__clk__0 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__clk;
    if ((2ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h32447ec5__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h32447ec5__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h32447ec5__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h32447ec5__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h32447ec5__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h32447ec5__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h32447ec5__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h32447ec5__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h32447ec5__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h32447ec5__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h32447ec5__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vhdmi_dualvideo_tb___024root____VbeforeTrig_h1b3f35c3__0(Vhdmi_dualvideo_tb___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root____VbeforeTrig_h1b3f35c3__0\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    CData/*0:0*/ __Vtrigprevexpr_h8b77f5eb__0;
    __Vtrigprevexpr_h8b77f5eb__0 = 0;
    // Body
    __Vtrigprevexpr_h8b77f5eb__0 = (((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__frame_en) 
                                     & (0x0168U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt))) 
                                    & (0x0190U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt)));
    __VTmp[0U] = (QData)((IData)((((IData)(__Vtrigprevexpr_h8b77f5eb__0) 
                                   != (IData)(vlSelfRef.__Vtrigprevexpr_h8b77f5eb__1)) 
                                  << 5U)));
    vlSelfRef.__Vtrigprevexpr_h8b77f5eb__1 = __Vtrigprevexpr_h8b77f5eb__0;
    if ((0x0000000000000020ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h1b3f35c3__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vhdmi_dualvideo_tb___024root____VbeforeTrig_h85c17222__0(Vhdmi_dualvideo_tb___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root____VbeforeTrig_h85c17222__0\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080) 
                                   != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080__0)) 
                                  << 6U)));
    vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080__0 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__mode_1080;
    if ((0x0000000000000040ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h85c17222__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vhdmi_dualvideo_tb___024root____VbeforeTrig_h10016e0a__0(Vhdmi_dualvideo_tb___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root____VbeforeTrig_h10016e0a__0\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    CData/*0:0*/ __Vtrigprevexpr_h98362db4__0;
    __Vtrigprevexpr_h98362db4__0 = 0;
    // Body
    __Vtrigprevexpr_h98362db4__0 = ((0x021cU == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__v_cnt)) 
                                    & (0x01f4U == (IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__dut__DOT__h_cnt)));
    __VTmp[0U] = (QData)((IData)((((IData)(__Vtrigprevexpr_h98362db4__0) 
                                   != (IData)(vlSelfRef.__Vtrigprevexpr_h98362db4__1)) 
                                  << 7U)));
    vlSelfRef.__Vtrigprevexpr_h98362db4__1 = __Vtrigprevexpr_h98362db4__0;
    if ((0x0000000000000080ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h10016e0a__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vhdmi_dualvideo_tb___024root____VbeforeTrig_he9510f89__0(Vhdmi_dualvideo_tb___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root____VbeforeTrig_he9510f89__0\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    CData/*0:0*/ __Vtrigprevexpr_h7d65cb35__0;
    __Vtrigprevexpr_h7d65cb35__0 = 0;
    // Body
    __Vtrigprevexpr_h7d65cb35__0 = ((2U == vlSelfRef.hdmi_dualvideo_tb__DOT__mode_changes) 
                                    & (3U == vlSelfRef.hdmi_dualvideo_tb__DOT__frames));
    __VTmp[0U] = (QData)((IData)((((IData)(__Vtrigprevexpr_h7d65cb35__0) 
                                   != (IData)(vlSelfRef.__Vtrigprevexpr_h7d65cb35__1)) 
                                  << 8U)));
    vlSelfRef.__Vtrigprevexpr_h7d65cb35__1 = __Vtrigprevexpr_h7d65cb35__0;
    if ((0x0000000000000100ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_he9510f89__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vhdmi_dualvideo_tb___024root____VbeforeTrig_he72dfe88__0(Vhdmi_dualvideo_tb___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root____VbeforeTrig_he72dfe88__0\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)((((IData)(vlSelfRef.hdmi_dualvideo_tb__DOT__pix_clk) 
                                   & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__pix_clk__0))) 
                                  << 2U)));
    vlSelfRef.__Vtrigprevexpr___TOP__hdmi_dualvideo_tb__DOT__pix_clk__0 
        = vlSelfRef.hdmi_dualvideo_tb__DOT__pix_clk;
    if ((4ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_he72dfe88__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

#ifdef VL_DEBUG
void Vhdmi_dualvideo_tb___024root___eval_debug_assertions(Vhdmi_dualvideo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhdmi_dualvideo_tb___024root___eval_debug_assertions\n"); );
    Vhdmi_dualvideo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
