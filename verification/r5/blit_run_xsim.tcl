# Run with Vivado 2021.2: vivado -mode batch -source verification/r5/blit_run_xsim.tcl
# Behavioral simulation only. No Ti60 utilization/timing is inferred from AMD.
set here [file normalize [file dirname [info script]]]
set root [file normalize [file join $here ../..]]
set output [file join $here blit_xsim_repro]
create_project -force ti60_portable_checks $output
add_files [file join $root PROJECT/rtl/blit/blit_engine.v]
add_files -fileset sim_1 [file join $here blit_control_tb.sv]
add_files -fileset sim_1 [file join $here blit_alpha_tb.sv]
set_property target_simulator XSim [current_project]
foreach test {blit_control_tb blit_alpha_tb} {
    set_property top $test [get_filesets sim_1]
    set_property xsim.simulate.runtime 250us [get_filesets sim_1]
    update_compile_order -fileset sim_1
    launch_simulation -simset sim_1 -mode behavioral
    close_sim
    set transcript [file join $output ti60_portable_checks.sim sim_1 behav xsim simulate.log]
    set f [open $transcript r]
    set text [read $f]
    close $f
    file copy -force $transcript [file join $here ${test}_repro.log]
    if {![string match *TEST_PASS* $text] || [regexp -nocase {fatal:|error:|watchdog} $text]} {
        error "Functional gate failed for $test; inspect $transcript"
    }
}
close_project
puts "TEST_PASS: all portable XSim contracts"
