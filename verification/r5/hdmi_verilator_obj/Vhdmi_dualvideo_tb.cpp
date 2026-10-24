// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vhdmi_dualvideo_tb__pch.h"

//============================================================
// Constructors

Vhdmi_dualvideo_tb::Vhdmi_dualvideo_tb(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vhdmi_dualvideo_tb__Syms(contextp(), _vcname__, this)}
    , m_evalLoop{*this, /*convergeLimit:*/ 10000}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vhdmi_dualvideo_tb::Vhdmi_dualvideo_tb(const char* _vcname__)
    : Vhdmi_dualvideo_tb(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vhdmi_dualvideo_tb::~Vhdmi_dualvideo_tb() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vhdmi_dualvideo_tb___024root___eval_debug_assertions(Vhdmi_dualvideo_tb___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___eval_static(Vhdmi_dualvideo_tb___024root* vlSelf);
void Vhdmi_dualvideo_tb___024root___eval_initial(Vhdmi_dualvideo_tb___024root* vlSelf);
VL_ATTR_COLD bool Vhdmi_dualvideo_tb___024root___eval_stl(Vhdmi_dualvideo_tb___024root* vlSelf, CData/*0:0*/ firstIteration);
void Vhdmi_dualvideo_tb___024root___eval_sample(Vhdmi_dualvideo_tb___024root* vlSelf);
bool Vhdmi_dualvideo_tb___024root___eval_ico(Vhdmi_dualvideo_tb___024root* vlSelf, CData/*0:0*/ firstIteration);
bool Vhdmi_dualvideo_tb___024root___eval_act(Vhdmi_dualvideo_tb___024root* vlSelf);
bool Vhdmi_dualvideo_tb___024root___eval_inact(Vhdmi_dualvideo_tb___024root* vlSelf);
bool Vhdmi_dualvideo_tb___024root___eval_nba(Vhdmi_dualvideo_tb___024root* vlSelf);
bool Vhdmi_dualvideo_tb___024root___eval_obs(Vhdmi_dualvideo_tb___024root* vlSelf);
bool Vhdmi_dualvideo_tb___024root___eval_react(Vhdmi_dualvideo_tb___024root* vlSelf);
void Vhdmi_dualvideo_tb___024root___eval_postponed(Vhdmi_dualvideo_tb___024root* vlSelf);
VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___eval_final(Vhdmi_dualvideo_tb___024root* vlSelf);
VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___eval_dump_triggers__stl(Vhdmi_dualvideo_tb___024root* vlSelf);
VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___eval_dump_triggers__ico(Vhdmi_dualvideo_tb___024root* vlSelf);
VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___eval_dump_triggers__act(Vhdmi_dualvideo_tb___024root* vlSelf);
VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___eval_dump_triggers__nba(Vhdmi_dualvideo_tb___024root* vlSelf);
VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___eval_dump_triggers__obs(Vhdmi_dualvideo_tb___024root* vlSelf);
VL_ATTR_COLD void Vhdmi_dualvideo_tb___024root___eval_dump_triggers__react(Vhdmi_dualvideo_tb___024root* vlSelf);

void Vhdmi_dualvideo_tb::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vhdmi_dualvideo_tb::eval_step\n"); );
    m_evalLoop.eval();
}

void Vhdmi_dualvideo_tb::evalBegin() {
#ifdef VL_DEBUG
    // Debug assertions
    Vhdmi_dualvideo_tb___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
}

void Vhdmi_dualvideo_tb::evalEnd() {
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

void Vhdmi_dualvideo_tb::evalStatic() {
    Vhdmi_dualvideo_tb___024root___eval_static(&(vlSymsp->TOP));
}

void Vhdmi_dualvideo_tb::evalInitial() {
    Vhdmi_dualvideo_tb___024root___eval_initial(&(vlSymsp->TOP));
}

bool Vhdmi_dualvideo_tb::evalStl(bool firstIteration) {
    return Vhdmi_dualvideo_tb___024root___eval_stl(&(vlSymsp->TOP), firstIteration);
}

void Vhdmi_dualvideo_tb::evalSample() {
    Vhdmi_dualvideo_tb___024root___eval_sample(&(vlSymsp->TOP));
}

bool Vhdmi_dualvideo_tb::evalIco(bool firstIteration) {
    return Vhdmi_dualvideo_tb___024root___eval_ico(&(vlSymsp->TOP), firstIteration);
}

bool Vhdmi_dualvideo_tb::evalAct() {
    return Vhdmi_dualvideo_tb___024root___eval_act(&(vlSymsp->TOP));
}

bool Vhdmi_dualvideo_tb::evalInact() {
    return Vhdmi_dualvideo_tb___024root___eval_inact(&(vlSymsp->TOP));
}

bool Vhdmi_dualvideo_tb::evalNba() {
    return Vhdmi_dualvideo_tb___024root___eval_nba(&(vlSymsp->TOP));
}

bool Vhdmi_dualvideo_tb::evalObs() {
    return Vhdmi_dualvideo_tb___024root___eval_obs(&(vlSymsp->TOP));
}

bool Vhdmi_dualvideo_tb::evalReact() {
    return Vhdmi_dualvideo_tb___024root___eval_react(&(vlSymsp->TOP));
}

void Vhdmi_dualvideo_tb::evalPostponed() {
    Vhdmi_dualvideo_tb___024root___eval_postponed(&(vlSymsp->TOP));
}

void Vhdmi_dualvideo_tb::evalFinal() {
    Vhdmi_dualvideo_tb___024root___eval_final(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vhdmi_dualvideo_tb::dumpTriggersStl() {
    Vhdmi_dualvideo_tb___024root___eval_dump_triggers__stl(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vhdmi_dualvideo_tb::dumpTriggersIco() {
    Vhdmi_dualvideo_tb___024root___eval_dump_triggers__ico(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vhdmi_dualvideo_tb::dumpTriggersAct() {
    Vhdmi_dualvideo_tb___024root___eval_dump_triggers__act(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vhdmi_dualvideo_tb::dumpTriggersNba() {
    Vhdmi_dualvideo_tb___024root___eval_dump_triggers__nba(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vhdmi_dualvideo_tb::dumpTriggersObs() {
    Vhdmi_dualvideo_tb___024root___eval_dump_triggers__obs(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vhdmi_dualvideo_tb::dumpTriggersReact() {
    Vhdmi_dualvideo_tb___024root___eval_dump_triggers__react(&(vlSymsp->TOP));
}

//============================================================
// Events and timing
bool Vhdmi_dualvideo_tb::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty() && !contextp()->gotFinish(); }

uint64_t Vhdmi_dualvideo_tb::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vhdmi_dualvideo_tb::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

VL_ATTR_COLD void Vhdmi_dualvideo_tb::final() {
    contextp()->executingFinal(true);
    evalFinal();
    contextp()->executingFinal(false);
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vhdmi_dualvideo_tb::hierName() const { return vlSymsp->name(); }
const char* Vhdmi_dualvideo_tb::modelName() const { return "Vhdmi_dualvideo_tb"; }
unsigned Vhdmi_dualvideo_tb::threads() const { return 1; }
void Vhdmi_dualvideo_tb::prepareClone() const { contextp()->prepareClone(); }
void Vhdmi_dualvideo_tb::atClone() const {
    contextp()->threadPoolpOnClone();
}
