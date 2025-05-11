# Before running this script, please install python-osc package:
# pip3 install python-osc

import time
import math
from pythonosc import udp_client

# Create OSC client
client = udp_client.SimpleUDPClient("127.0.0.1", 8000)

# Animation duration (seconds)
ANIMATION_DURATION = 10.0

def get_rgb_values(current_time):
    """Calculate RGB gradient values"""
    phase_r = current_time * 2 * math.pi / ANIMATION_DURATION
    phase_g = phase_r + 2 * math.pi / 3  # 120 degrees phase difference
    phase_b = phase_g + 2 * math.pi / 3  # 240 degrees phase difference
    
    r = (math.sin(phase_r) + 1) / 2
    g = (math.sin(phase_g) + 1) / 2
    b = (math.sin(phase_b) + 1) / 2
    
    return r, g, b

def get_white_flash(current_time):
    """Calculate white light flash value"""
    # Flash every 2 seconds
    flash_period = 2.0
    flash_duration = 0.1  # Flash duration
    
    # Calculate time within current period
    time_in_period = current_time % flash_period
    
    # Return 1.0 if within flash duration, otherwise 0.0
    return 1.0 if time_in_period < flash_duration else 0.0

def main():
    start_time = time.time()
    
    try:
        while True:
            current_time = (time.time() - start_time) % ANIMATION_DURATION
            
            # Get RGB and white light values
            r, g, b = get_rgb_values(current_time)
            w = get_white_flash(current_time)
            
            # Send OSC messages
            client.send_message("/red", r)
            client.send_message("/green", g)
            client.send_message("/blue", b)
            client.send_message("/white", w)
            
            # Control refresh rate
            time.sleep(0.02)  # 50Hz refresh rate
            
    except KeyboardInterrupt:
        # Turn off all lights
        for color in ["/red", "/green", "/blue", "/white"]:
            client.send_message(color, 0)
        print("\nAnimation stopped")

if __name__ == "__main__":
    print("Starting light animation, press Ctrl+C to stop...")
    main()