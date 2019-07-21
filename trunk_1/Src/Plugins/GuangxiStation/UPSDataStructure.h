#pragma once

struct UPSSystemAnalogDataStructure
{
    FLOAT m1_input_voltage_ab;
    FLOAT m1_input_voltage_bc;
    FLOAT m1_input_voltage_ca;
    FLOAT m1_output_voltage_a;
    FLOAT m1_output_voltage_b;
    FLOAT m1_output_voltage_c;
    FLOAT m1_output_electricity_a;
    FLOAT m1_output_electricity_b;
    FLOAT m1_output_electricity_c;
    FLOAT m1_direct_current_voltage;
    FLOAT m1_output_frequency;
};

struct UPSSelfDefAnalogDataStructureOne
{
    FLOAT m1_input_voltage_ab;
    FLOAT m1_input_voltage_bc;
    FLOAT m1_input_voltage_ca;
    FLOAT m1_input_electricity_a;
    FLOAT m1_input_electricity_b;
    FLOAT m1_input_electricity_c;
    FLOAT m1_input_frequency;
    FLOAT m1_all_input_power_divisor;
    FLOAT m1_bypass_voltage_a;
    FLOAT m1_bypass_voltage_b;
    FLOAT m1_bypass_voltage_c;
    FLOAT m1_bypass_frequency;
    FLOAT m1_battery_current;
};


struct UPSSelfDefAnalogDataStructureTwo
{
    FLOAT m1_output_power_divisor_a;
    FLOAT m1_output_power_divisor_b;
    FLOAT m1_output_power_divisor_c;
    FLOAT m1_output_peak_ratio_a;
    FLOAT m1_output_peak_ratio_b;
    FLOAT m1_output_peak_ratio_c;
    FLOAT m1_output_active_power_a;
    FLOAT m1_output_active_power_b;
    FLOAT m1_output_active_power_c;
    FLOAT m1_output_apparent_power_a;
    FLOAT m1_output_apparent_power_b;
    FLOAT m1_output_apparent_power_c;
    FLOAT m1_output_load_percentage_a;
    FLOAT m1_output_load_percentage_b;
    FLOAT m1_output_load_percentage_c;
 
};

struct UPSSelfDefAnalogDataStructureThree
{
    FLOAT m1_output_reactive_power_a;
    FLOAT m1_output_reactive_power_b;
    FLOAT m1_output_reactive_power_c;
    FLOAT m1_sys_output_active_power_a;
    FLOAT m1_sys_output_active_power_b;
    FLOAT m1_sys_output_active_power_c;
    FLOAT m1_sys_output_apparent_power_a;
    FLOAT m1_sys_output_apparent_power_b;
    FLOAT m1_sys_output_apparent_power_c;
    FLOAT m1_sys_output_reactive_power_a;
    FLOAT m1_sys_output_reactive_power_b;
    FLOAT m1_sys_output_reactive_power_c;
    FLOAT m1_attery_backup_time;
    FLOAT m1_attery_tempereture;
    FLOAT m1_enviro_tempereture;
    FLOAT m1_attery_old_ratio;
    
};


struct UPSSwitchInputState
{
    UCHAR m2_power_supply_mode;
    UCHAR m2_attery_self_checking;
    UCHAR m2_evenfloat_charge;
    UCHAR m2_shutdown;
    UCHAR m2_ups_supplytype;
    UCHAR m2_generator_access;
    UCHAR m2_entry_switch;
    UCHAR m2_maintain_bypass_switch;
    UCHAR m2_bypass_shutdown;
    UCHAR m2_output_switch;
    UCHAR m2_weaver_system_supply;
    UCHAR m2_rotary_switch;
    UCHAR m2_rejector;
};

struct UPSAlarm
{
    UCHAR m3_inverter_sync;
    UCHAR m3_main_road_voltage_abnormal;
    UCHAR m3_rectifier_blockade;
    UCHAR m3_abnormal_output_voltage;
    UCHAR m3_bypass_situation;
    UCHAR m3_total_battery_voltage_state;
    UCHAR m3_main_road_frequency_abnormal;
    UCHAR m3_main_road_fuse_broken;
    UCHAR m3_main_path_reversed;
    UCHAR m3_main_phase_alarm;
    UCHAR m3_auxiliary_power1_poweroff;
    UCHAR m3_auxiliary_power2_poweroff;
    UCHAR m3_rectify_current_limiting;
    UCHAR m3_soft_start_failure;
    UCHAR m3_rectify_over_temp;
    UCHAR m3_input_filter_alarm;
    UCHAR m3_filter_flow;
    UCHAR m3_filter_contactor_alarm;
    UCHAR m3_rectify_drives_cable_alarm;
    UCHAR m3_rectify_commun_alarm;
    UCHAR m3_inverter_overheats;
    UCHAR m3_fan_alarm;
    UCHAR m3_inverter_thyristor_alarm;
    UCHAR m3_bypass_thyristor_alarm;
    UCHAR m3_user_opre_error;
    UCHAR m3_single_output_overload;
    UCHAR m3_parallel_overloaded;
    UCHAR m3_single_overload_timeout;
    UCHAR m3_bypass_abnormal_shutdown;
    UCHAR m3_output_over_voltage;
    UCHAR m3_inverter_overflows;
    UCHAR m3_bypass_phase_reversed;
    UCHAR m3_load_shock_bypass;
    UCHAR m3_bypass_switch_times_limit;
    UCHAR m3_weaver_flow_alarm;
    UCHAR m3_bus_abnormal_shutdown;
    UCHAR m3_neighbor_machine_request_bypass;
    UCHAR m3_weaver_plate__alarm;
    UCHAR m3_parallel_connection_alarm;
    UCHAR m3_parallel_machine_communi_alarm;
    UCHAR m3_bypass_out_alarm;
    UCHAR m3_lbs_activate_alarm;
    UCHAR m3_bypass_induces_over_temperature;
    UCHAR m3_static_switch_over_temperature;
    UCHAR m3_bypass_reverse_irrigation_alarm;
    UCHAR m3_inverter_drives_cable_alarm;
    UCHAR m3_contravariant_communi_alarm;
    UCHAR m3_parallel_sys_battery_alarm;
    UCHAR m3_emergency_switch;
    UCHAR m3_env_excess_temperature;
    UCHAR m3_storage_battery_life;
    UCHAR m3_battery_excess_temperature;
    UCHAR m3_battery_ground_alarm;
    UCHAR m3_battery_fuse;
    UCHAR m3_bcb_access_situation;
    UCHAR m3_output_fuse_broken;
    UCHAR m3_capacitance_over_voltage;
    UCHAR m3_bus_over_voltage;
    UCHAR m3_busbar_short_circuit_alarm;
    UCHAR m3_input_electricity_unevenness;
    UCHAR m3_output_capacitance_maintain;
    UCHAR m3_filter_frequency;
};


