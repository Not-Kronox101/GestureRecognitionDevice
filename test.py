from machine import ADC, Pin
import time

# Initialize ADC pins for ADXL335 axes
adc_x = ADC(Pin(26))  # GP26 - X-axis
adc_y = ADC(Pin(27))  # GP27 - Y-axis
adc_z = ADC(Pin(28))  # GP28 - Z-axis

# Main loop: read and print values every 100ms
while True:
    x = adc_x.read_u16()
    y = adc_y.read_u16()
    z = adc_z.read_u16()
    
    # Print as CSV: x,y,z
    print("{},{},{}".format(x, y, z))
    
    time.sleep(0.1)  # 100 ms delay