import numpy as np

# Original LUT values
orig = [
    0, 14, 25, 38, 53, 73, 97, 126,
    161, 205, 258, 322, 401, 495, 610, 748,
    913, 1111, 1345, 1621, 1944, 2319, 2752, 3247,
    3808, 4437, 5137, 5910, 6754, 7671, 8657, 9711,
    10830, 12012, 13253, 14549, 15898, 17297, 18741, 20228,
    21756, 23320, 24920, 26552, 28214, 29904, 31621, 33362,
    35126, 36912, 38717, 40541, 42383, 44242, 46116, 48004,
    49906, 51822, 53749, 55688, 57637, 59597, 61567, 63546,
    65533, 65535
]

# Original "x" positions = evenly spaced indices
x_old = np.linspace(0, 1, len(orig))   # normalized domain
y_old = np.array(orig)

# New x positions for 64 evenly spaced samples
x_new = np.linspace(0, 1, 64)

# Linear interpolation
y_new = np.interp(x_new, x_old, y_old).astype(int)

# Print as C code
print("uint16_t heater_circuit_LUT[64] = {")
for i, v in enumerate(y_new):
    print(f"\t{v},", end="\n" if (i+1) % 8 == 0 else " ")
print("};")