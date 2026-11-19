"""
=========================================================
        VANGUARD - CALIBRATION TOOL
        Baseline-Ratio Version
=========================================================

Purpose:
    Collect RGB readings from the Arduino UNO,
    calculate yellowness, compare measurements
    with a baseline, and suggest thresholds.

Arduino:
    UNO

Sensor:
    TCS3200

Expected Serial Output:
    RED
    GREEN
    BLUE
    YELLOWNESS

Python package required:
    pyserial

Install:
    pip install pyserial

Usage:
    python calibration_tool.py

IMPORTANT:
    This is a prototype calibration tool.
    It does NOT provide clinically validated
    jaundice thresholds.
=========================================================
"""

import serial
import time
import re
import statistics


# ======================================================
# CONFIGURATION
# ======================================================

SERIAL_PORT = "COM6"

BAUD_RATE = 9600

NUMBER_OF_SAMPLES = 5

# Number of calibration pairs
NUMBER_OF_PAIRS = 5


# ======================================================
# DATA STORAGE
# ======================================================

baseline_values = []

measurement_values = []


# ======================================================
# SERIAL CONNECTION
# ======================================================

def connect_arduino():

    print()
    print("=" * 55)
    print("        VANGUARD CALIBRATION TOOL")
    print("=" * 55)

    print()
    print("Connecting to Arduino...")
    print("Port:", SERIAL_PORT)
    print("Baud:", BAUD_RATE)

    try:

        ser = serial.Serial(
            SERIAL_PORT,
            BAUD_RATE,
            timeout=2
        )

        # Arduino usually resets when serial opens
        time.sleep(2)

        print()
        print("Arduino connected successfully.")

        return ser

    except serial.SerialException as e:

        print()
        print("ERROR: Could not connect to Arduino.")
        print()

        print("Check:")
        print("1. Arduino UNO is connected")
        print("2. Correct COM port is selected")
        print("3. Arduino Serial Monitor is closed")
        print("4. USB cable is connected")

        print()
        print("Original error:")
        print(e)

        return None


# ======================================================
# EXTRACT VALUE FROM SERIAL LINE
# ======================================================

def extract_value(line, name):

    pattern = rf"{name}\s*:\s*(-?\d+(?:\.\d+)?)"

    match = re.search(pattern, line)

    if match:

        return float(match.group(1))

    return None


# ======================================================
# READ ONE SENSOR READING
# ======================================================

def read_sensor(ser):

    red = None
    green = None
    blue = None
    yellowness = None

    start_time = time.time()

    while time.time() - start_time < 10:

        try:

            line = ser.readline().decode(
                "utf-8",
                errors="ignore"
            ).strip()

        except Exception:
            continue

        if not line:
            continue

        print("Arduino:", line)

        r = extract_value(line, "RED")

        g = extract_value(line, "GREEN")

        b = extract_value(line, "BLUE")

        y = extract_value(line, "YELLOWNESS")

        if r is not None:
            red = r

        if g is not None:
            green = g

        if b is not None:
            blue = b

        if y is not None:
            yellowness = y

        if (
            red is not None
            and green is not None
            and blue is not None
            and yellowness is not None
        ):

            return {
                "red": red,
                "green": green,
                "blue": blue,
                "yellowness": yellowness
            }

    print()
    print("ERROR: Sensor reading timeout.")

    return None


# ======================================================
# TAKE MULTIPLE READINGS
# ======================================================

def collect_average(ser, label):

    print()
    print("=" * 55)
    print(label)
    print("=" * 55)

    print()
    print("Place the sensor correctly.")
    print("Keep the probe steady.")
    print()

    input("Press ENTER to start measurement...")

    readings = []

    print()
    print("Collecting readings...")

    for i in range(NUMBER_OF_SAMPLES):

        print()
        print(
            f"Sample {i + 1}/{NUMBER_OF_SAMPLES}"
        )

        reading = read_sensor(ser)

        if reading is not None:

            readings.append(reading)

        time.sleep(0.5)

    if len(readings) == 0:

        print()
        print("No valid readings collected.")

        return None

    # Average RGB values
    avg_red = statistics.mean(
        r["red"] for r in readings
    )

    avg_green = statistics.mean(
        r["green"] for r in readings
    )

    avg_blue = statistics.mean(
        r["blue"] for r in readings
    )

    # Calculate yellowness again
    if avg_red != 0:

        avg_yellow = (
            avg_green - avg_blue
        ) / avg_red

    else:

        avg_yellow = 0

    result = {

        "red": avg_red,

        "green": avg_green,

        "blue": avg_blue,

        "yellowness": avg_yellow
    }

    print()
    print("-" * 55)

    print("AVERAGE RESULT")

    print("-" * 55)

    print(
        f"Red        : {avg_red:.4f}"
    )

    print(
        f"Green      : {avg_green:.4f}"
    )

    print(
        f"Blue       : {avg_blue:.4f}"
    )

    print(
        f"Yellowness : {avg_yellow:.4f}"
    )

    print("-" * 55)

    return result


# ======================================================
# CALCULATE BASELINE RATIO
# ======================================================

def calculate_baseline_ratio(
        baseline,
        measurement):

    baseline_yellow = baseline["yellowness"]

    measurement_yellow = (
        measurement["yellowness"]
    )

    if baseline_yellow == 0:

        return measurement_yellow

    ratio = (
        measurement_yellow /
        baseline_yellow
    )

    return ratio


# ======================================================
# MAIN CALIBRATION
# ======================================================

def main():

    ser = connect_arduino()

    if ser is None:

        return

    print()
    print("=" * 55)
    print("CALIBRATION PROCEDURE")
    print("=" * 55)

    print()
    print("The tool will collect:")
    print()
    print("1. BASELINE")
    print("2. MEASUREMENT")
    print()
    print(
        "Then it will calculate the baseline-adjusted ratio."
    )

    print()

    try:

        for pair in range(
            NUMBER_OF_PAIRS
        ):

            print()
            print()
            print("=" * 55)

            print(
                f"CALIBRATION PAIR "
                f"{pair + 1}/{NUMBER_OF_PAIRS}"
            )

            print("=" * 55)

            # ------------------------------------------
            # BASELINE
            # ------------------------------------------

            baseline = collect_average(
                ser,
                "BASELINE READING"
            )

            if baseline is None:

                continue

            # ------------------------------------------
            # MEASUREMENT
            # ------------------------------------------

            measurement = collect_average(
                ser,
                "MEASUREMENT READING"
            )

            if measurement is None:

                continue

            # ------------------------------------------
            # CALCULATE RATIO
            # ------------------------------------------

            ratio = calculate_baseline_ratio(
                baseline,
                measurement
            )

            baseline_values.append(
                baseline
            )

            measurement_values.append(
                measurement
            )

            print()
            print("=" * 55)

            print("CALIBRATION PAIR RESULT")

            print("=" * 55)

            print(
                f"Baseline yellowness   : "
                f"{baseline['yellowness']:.4f}"
            )

            print(
                f"Measurement yellowness : "
                f"{measurement['yellowness']:.4f}"
            )

            print(
                f"Baseline ratio        : "
                f"{ratio:.4f}"
            )

            print("=" * 55)

    except KeyboardInterrupt:

        print()
        print()
        print("Calibration interrupted by user.")

    finally:

        ser.close()

        print()
        print("Arduino connection closed.")

    # ==================================================
    # GENERATE SUMMARY
    # ==================================================

    generate_summary()


# ======================================================
# SUMMARY
# ======================================================

def generate_summary():

    print()
    print()
    print("=" * 60)
    print("             VANGUARD CALIBRATION SUMMARY")
    print("=" * 60)

    if len(measurement_values) == 0:

        print()
        print("No calibration data available.")

        return

    ratios = []

    for baseline, measurement in zip(
        baseline_values,
        measurement_values
    ):

        ratio = calculate_baseline_ratio(
            baseline,
            measurement
        )

        ratios.append(ratio)

    # -----------------------------------------------
    # Display all ratios
    # -----------------------------------------------

    print()
    print("CALIBRATION RATIOS")
    print("-" * 60)

    for i, value in enumerate(
        ratios
    ):

        print(
            f"Sample {i + 1}: "
            f"{value:.4f}"
        )

    # -----------------------------------------------
    # Statistics
    # -----------------------------------------------

    minimum = min(ratios)

    maximum = max(ratios)

    average = statistics.mean(
        ratios
    )

    print()
    print("-" * 60)

    print(
        f"Minimum : {minimum:.4f}"
    )

    print(
        f"Maximum : {maximum:.4f}"
    )

    print(
        f"Average : {average:.4f}"
    )

    print("-" * 60)

    # =================================================
    # SUGGESTED THRESHOLDS
    # =================================================

    print()
    print("SUGGESTED STARTING THRESHOLDS")
    print("-" * 60)

    low_threshold = (
        minimum +
        (average - minimum) * 0.5
    )

    high_threshold = (
        average +
        (maximum - average) * 0.5
    )

    print()
    print(
        f"THRESH_LOW  = {low_threshold:.4f}"
    )

    print(
        f"THRESH_HIGH = {high_threshold:.4f}"
    )

    print()
    print("=" * 60)

    print("COPY THESE VALUES INTO YOUR")
    print("FINAL VANGUARD ARDUINO CODE")
    print("=" * 60)

    print()
    print(
        f"const float THRESH_LOW = "
        f"{low_threshold:.4f};"
    )

    print(
        f"const float THRESH_HIGH = "
        f"{high_threshold:.4f};"
    )

    print()
    print("IMPORTANT:")
    print(
        "These are prototype calibration values only."
    )

    print(
        "They are NOT clinically validated "
        "jaundice thresholds."
    )

    print()
    print("=" * 60)


# ======================================================
# PROGRAM START
# ======================================================

if __name__ == "__main__":

    main()
