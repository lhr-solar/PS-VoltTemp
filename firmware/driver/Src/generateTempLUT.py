import numpy as np

temps_C = np.array([
-40,-35,-30,-25,-20,-15,-10,-5,0,5,10,15,20,25,30,35,40,45,50,55,60,
65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150
])

ratios = np.array([
40.49,28.81,20.72,15.07,11.06,8.198,6.129,4.622,3.515,2.694,
2.080,1.618,1.267,1.0,0.7944,0.6350,0.5108,0.4132,0.3363,
0.2752,0.2263,0.1871,0.1554,0.1297,0.1087,0.09153,0.07738,
0.06567,0.05596,0.04786,0.04108,0.03539,0.03059,0.02652,
0.02307,0.02013,0.01762,0.01546,0.01361
])

R25 = 10000.0
R_fixed = 6490.0
Vref = 3.3
gain = 1.8
ADC_MAX = 4095

counts = []

for ratio in ratios:
    Rth = ratio * R25

    # divider (thermistor on top)
    Vnode = Vref * (R_fixed / (Rth + R_fixed))

    Vadc = gain * Vnode
    Vadc = min(Vadc, Vref)

    adc = (Vadc / Vref) * ADC_MAX
    counts.append(adc)

counts = np.array(counts)

# temperatures must increase for interpolation
temps_sorted = temps_C
counts_sorted = counts

# ADC axis
adc_range = np.arange(0, ADC_MAX + 1)

# invert mapping (ADC -> temperature)
lut_C = np.interp(adc_range, counts_sorted, temps_sorted)

# convert to milliC
lut_mC = (lut_C * 1000).astype(int)

# Example lookup
def adc_to_temp_mC(adc):
    return lut_mC[adc]

print("Example:", adc_to_temp_mC(2000), "m°C")

# ---- Generate C header file ----

header_name = "../Inc/thermistor_lut.h"

with open(header_name, "w") as f:
    f.write("#pragma once\n\n")
    f.write("#include <stdint.h>\n\n")

    f.write("// ADC (0-4095) -> Temperature (milli-Celsius)\n")
    f.write("// Thermistor: Panasonic ERTJ1VR series\n")
    f.write("// Divider: 3.3V -> thermistor -> node -> 6.49k -> GND\n")
    f.write("// Amplifier gain: 1.8\n\n")

    f.write("#define THERM_LUT_SIZE 4096\n\n")
    f.write("static const int32_t thermistor_lut[THERM_LUT_SIZE] = {\n")

    # write values 8 per line for readability
    for i in range(0, len(lut_mC), 8):
        row = lut_mC[i:i+8]
        line = ", ".join(f"{v}" for v in row)
        if i + 8 < len(lut_mC):
            f.write(f"    {line},\n")
        else:
            f.write(f"    {line}\n")

    f.write("};\n\n")

print("Header file generated:", header_name)