:orphan:

============================== ============================== ============================== ============================== ============================== ==============================
Object id                      Property                       Length (byte)                  Decimals                       Unit                           Type                          
------------------------------ ------------------------------ ------------------------------ ------------------------------ ------------------------------ ------------------------------
0x00                           packet_id                      1                                                                                            numeric                       
0x01                           battery                        1                                                             %                              numeric                       
0x02                           temperature                    2                              2                              °C                             numeric                       
0x03                           humidity                       2                              2                              %                              numeric                       
0x04                           pressure                       3                              2                              hPa                            numeric                       
0x05                           illuminance                    3                              2                              lux                            numeric                       
0x06                           mass_kg                        2                              2                              kg                             numeric                       
0x07                           mass_lb                        2                              2                              lb                             numeric                       
0x08                           dewpoint                       2                              2                              °C                             numeric                       
0x09                           count                          1                                                                                            numeric                       
0x0a                           energy                         3                              3                              kWh                            numeric                       
0x0b                           power                          3                              2                              W                              numeric                       
0x0c                           voltage                        2                              3                              V                              numeric                       
0x0d                           pm2_5                          2                                                             ug/m3                          numeric                       
0x0e                           pm10                           2                                                             ug/m3                          numeric                       
0x0f                           generic_boolean                1                                                                                            binary                        
0x10                           power                          1                                                                                            binary                        
0x11                           opening                        1                                                                                            binary                        
0x12                           co2                            2                                                             ppm                            numeric                       
0x13                           tvoc                           2                                                             ug/m3                          numeric                       
0x14                           moisture                       2                              2                              %                              numeric                       
0x15                           battery                        1                                                                                            binary                        
0x16                           battery_charging               1                                                                                            binary                        
0x17                           carbon_monoxide                1                                                                                            binary                        
0x18                           cold                           1                                                                                            binary                        
0x19                           connectivity                   1                                                                                            binary                        
0x1a                           door                           1                                                                                            binary                        
0x1b                           garage_door                    1                                                                                            binary                        
0x1c                           gas                            1                                                                                            binary                        
0x1d                           heat                           1                                                                                            binary                        
0x1e                           light                          1                                                                                            binary                        
0x1f                           lock                           1                                                                                            binary                        
0x20                           moisture                       1                                                                                            binary                        
0x21                           motion                         1                                                                                            binary                        
0x22                           moving                         1                                                                                            binary                        
0x23                           occupancy                      1                                                                                            binary                        
0x24                           plug                           1                                                                                            binary                        
0x25                           presence                       1                                                                                            binary                        
0x26                           problem                        1                                                                                            binary                        
0x27                           running                        1                                                                                            binary                        
0x28                           safety                         1                                                                                            binary                        
0x29                           smoke                          1                                                                                            binary                        
0x2a                           sound                          1                                                                                            binary                        
0x2b                           tamper                         1                                                                                            binary                        
0x2c                           vibration                      1                                                                                            binary                        
0x2d                           window                         1                                                                                            binary                        
0x2e                           humidity_coarse                1                                                             %                              numeric                       
0x2f                           moisture_coarse                1                                                             %                              numeric                       
0x3a                           button_none                    1                                                                                            event                         
0x3a                           button_click                   1                                                                                            event                         
0x3a                           button_double_click            1                                                                                            event                         
0x3a                           button_triple_click            1                                                                                            event                         
0x3a                           button_long_click              1                                                                                            event                         
0x3a                           button_long_double_click       1                                                                                            event                         
0x3a                           button_long_triple_click       1                                                                                            event                         
0x3c                           dimmer_none                    2                                                                                            event                         
0x3c                           dimmer_rotate_left             2                                                             steps                          event                         
0x3c                           dimmer_rotate_right            2                                                             steps                          event                         
0x3d                           count_2                        2                                                                                            numeric                       
0x3e                           count_4                        4                                                                                            numeric                       
0x3f                           rotation                       2                              1                              °                              numeric                       
0x40                           distance_mm                    2                                                             mm                             numeric                       
0x41                           distance_m                     2                              1                              m                              numeric                       
0x42                           duration                       3                              3                              s                              numeric                       
0x43                           current                        2                              3                              A                              numeric                       
0x44                           speed                          2                              2                              m/s                            numeric                       
0x45                           temperature_coarse             2                              1                              °C                             numeric                       
0x46                           uv_index                       1                              1                                                             numeric                       
0x47                           volume                         2                              1                              L                              numeric                       
0x48                           volume_mL                      2                                                             mL                             numeric                       
0x49                           volume_flow_rate               2                              3                              m3/hr                          numeric                       
0x4a                           voltage_coarse                 2                              1                              V                              numeric                       
0x4b                           gas                            3                              3                              m3                             numeric                       
0x4c                           gas_4                          4                              3                              m3                             numeric                       
0x4d                           energy_4                       4                              3                              kWh                            numeric                       
0x4e                           volume_precise                 4                              3                              L                              numeric                       
0x4f                           water                          4                              3                              L                              numeric                       
0x50                           timestamp                      4                                                                                            numeric                       
0x51                           acceleration                   2                              3                              m/s²                           numeric                       
0x52                           gyroscope                      2                              3                              °/s                            numeric                       
============================== ============================== ============================== ============================== ============================== ==============================
