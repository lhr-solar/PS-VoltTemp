import pandas as pd
import numpy as np
import os

def generate_thermistor_lut(csv_path, output_path):
    # Get the directory where the script itself is located
    script_dir = os.path.dirname(os.path.abspath(__file__))
    # Join that directory with your filename
    csv = os.path.join(script_dir, csv_path)
    # Load the dataset
    df = pd.read_csv(csv)
    
    # Extract temperature and ADC count columns
    # We use 'ADC Counts' as the independent variable (x) 
    # and 'Temperature (°C)' as the dependent variable (y)
    x_points = df['ADC Counts'].values
    y_points = df['Temperature (°C)'].values

    # Sort data by ADC counts to ensure proper linear interpolation
    sorted_indices = np.argsort(x_points)
    x_points = x_points[sorted_indices]
    y_points = y_points[sorted_indices]

    # Target range for a 12-bit ADC (0 to 4095)
    adc_range = np.arange(4096)

    # Perform linear interpolation
    # np.interp uses the first/last y-values for x-values out of bounds
    temp_interpolated = np.interp(adc_range, x_points, y_points)

    # Convert to millicelsius and round to integers
    temp_milli_c = np.round(temp_interpolated * 1000).astype(int)

    # Prepare C header file content
    header_content = [
        "#pragma once",
        "",
        "#include <stdint.h>",
        "",
        "/**",
        " * @brief Look-up table for thermistor ADC counts to temperature.",
        " * Index: 12-bit ADC Count (0 - 4095)",
        " * Value: Temperature in millicelsius (m°C)",
        " */",
        "static const int32_t thermistor_lut[4096] = {"
    ]

    # Add array entries formatted for readability (10 per line)
    for i in range(0, len(temp_milli_c), 10):
        row = temp_milli_c[i:i+10]
        row_str = "    " + ", ".join(f"{val:7d}" for val in row)
        if i + 10 < len(temp_milli_c):
            row_str += ","
        header_content.append(row_str)

    header_content.append("};")
    
    # Get the directory where the script itself is located
    script_dir = os.path.dirname(os.path.abspath(__file__))
    # Join that directory with your filename
    output = os.path.join(script_dir, output_path)

    # Ensure directory exists and write file
    os.makedirs(os.path.dirname(output), exist_ok=True)
    with open(output, 'w') as f:
        f.write("\n".join(header_content))

    print(f"Successfully generated {output}")

if __name__ == "__main__":
    csv_filename = 'ERTJ1VR.csv'
    output_filename = '../thermistor_lut.h'
    generate_thermistor_lut(csv_filename, output_filename)
