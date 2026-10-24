// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vhdmi_dualvideo_tb.h for the primary calling header

#include "Vhdmi_dualvideo_tb__pch.h"

void Vhdmi_dualvideo_tb___024root___ctor_var_reset(Vhdmi_dualvideo_tb___024root* vlSelf);

Vhdmi_dualvideo_tb___024root::Vhdmi_dualvideo_tb___024root(Vhdmi_dualvideo_tb__Syms* symsp, const char* namep)
    : __VdlySched{*symsp->_vm_contextp__}
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vhdmi_dualvideo_tb___024root___ctor_var_reset(this);
}

void Vhdmi_dualvideo_tb___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vhdmi_dualvideo_tb___024root::~Vhdmi_dualvideo_tb___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
