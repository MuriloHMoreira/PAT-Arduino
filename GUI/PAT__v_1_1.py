import serial
import matplotlib.pyplot as plt
import numpy as np
import re
import time
import os
import argparse
import csv

T1_label = 'T1_Ent'
T2_label = 'T2_Sai'
T3_label = 'T3_Frn'
T4_label = 'T4_Amb'

# cal_T1 = 0
# cal_T2 = 0
# cal_T3 = 0
# cal_T4 = 0

parser = argparse.ArgumentParser(description="Name of the file to save the test data.")
parser.add_argument("filename", nargs='*', type=str, default='', help="Path to the output CSV file")
args = parser.parse_args()

def parse_sensor_data(data_string):
    # Improved regex pattern to capture parameters including underscores
    pattern = r'([\w_\s\[\]/°]+):\s*(-?\d+\.\d+|\d+)'

    # Find all matches
    matches = re.findall(pattern, data_string)

    # Process matches into a dictionary
    data_dict = {}
    for key, value in matches:
        # Remove spaces and brackets
        clean_key = key.strip().replace("[", "").replace("]", "")
        data_dict[clean_key] = float(value) if "." in value else int(value)

    return data_dict

if len(args.filename)>0:
    filename = args.filename[0]
else:
    filename = input("Input Filename: ")
csv_file = filename + ".csv"
# csv_cal_file = filename + "_cal.csv"
print("Running Test: " + filename)
opened_file = open(csv_file, mode="a")
# opened_cal_file = open(csv_cal_file, mode="a")
writer = csv.writer(opened_file)
# writer_cal = csv.writer(opened_cal_file)

# Set up the serial connection (adjust port name)
ser = serial.Serial(port="COM3", baudrate=9600, timeout=1)
time.sleep(2)  # Allow time for connection to establish

# Initialize plot
plt.ion()
fig, ax = plt.subplots()
x_T_1_data, T_1_data, x_T_2_data, T_2_data, x_T_3_data, T_3_data, x_T_4_data, T_4_data = [], [], [], [], [], [], [], []
line_1, = ax.plot(x_T_1_data, T_1_data, 'g-.', label=T1_label)
line_2, = ax.plot(x_T_2_data, T_2_data, 'b--', label=T2_label)
line_3, = ax.plot(x_T_3_data, T_3_data, 'r-' , label=T3_label)
line_4, = ax.plot(x_T_4_data, T_4_data, 'k:' , label=T4_label)
ax.set_xlabel("Time [s]")
ax.set_ylabel("Temperature [°C]")
ax.legend(loc=2)

writer.writerow(["Time", "T_1", "T_2", "T_3", "T_4"])
# writer_cal.writerow(["Time", "T_1", "T_2", "T_3", "T_4"])

# Live update function
def update_plot():
    while True:
        try:
            data_string = ser.readline().decode('utf-8').strip()
            print(data_string)
            if data_string:
                t = None
                data_dict = parse_sensor_data(data_string)
                if 'T_1 °C' in data_dict.keys():
                    t = data_dict['Time s']
                    T_1 = data_dict['T_1 °C']  # Convert received data to float
                    # T_1_cal = T_1*cal_T1
                    x_T_1_data.append(t)
                    T_1_data.append(T_1)
                    line_1.set_xdata(x_T_1_data)
                    line_1.set_ydata(T_1_data)
                else:
                    T_1 = None
                    # T_1_cal = None
                    
                if 'T_2 °C' in data_dict.keys():
                    t = data_dict['Time s']
                    T_2 = data_dict['T_2 °C']  # Convert received data to float
                    # T_2_cal = T_2*cal_T2
                    x_T_2_data.append(t)
                    T_2_data.append(T_2)
                    line_2.set_xdata(x_T_2_data)
                    line_2.set_ydata(T_2_data)
                else:
                    T_2 = None
                    # T_2_cal = None

                if 'T_3 °C' in data_dict.keys():
                    t = data_dict['Time s']
                    T_3 = data_dict['T_3 °C']  # Convert received data to float
                    # T_3_cal = T_3*cal_T3
                    x_T_3_data.append(t)
                    T_3_data.append(T_3)
                    line_3.set_xdata(x_T_3_data)
                    line_3.set_ydata(T_3_data)
                else:
                    T_3 = None
                    # T_3_cal = None

                if 'T_4 °C' in data_dict.keys():
                    t = data_dict['Time s']
                    T_4 = data_dict['T_4 °C']  # Convert received data to float
                    # T_4_cal = T_4*cal_T4
                    x_T_4_data.append(t)
                    T_4_data.append(T_4)
                    line_4.set_xdata(x_T_4_data)
                    line_4.set_ydata(T_4_data)
                else:
                    T_4 = None
                    # T_4_cal = None
                    
                writer.writerow([t, T_1, T_2, T_3, T_4])
                # writer_cal.writerow([t, T_1_cal, T_2_cal, T_3_cal, T_4_cal])
                ax.relim()
                ax.autoscale()
                plt.draw()
                plt.pause(0.1)  # Adjust for smoother updates
        except KeyboardInterrupt:
            break
    ser.close()

# Run the plot update
update_plot()
