import numpy as np
from scipy.optimize import curve_fit

# --- Parameters ---
filename = 'heater.txt'  # your text file
delimiter = '\t'       # or ',' for CSV

# --- Load data, skipping first row, using columns 1 (x) and 2 (y) ---
data = np.loadtxt(filename, delimiter=delimiter, skiprows=1, usecols=(0,1))
x_data = data[:,0]
y_data = data[:,1]

# --- Define the function ---
def func(x, a, b, c):
    return (a / c) * np.log(1 + np.exp(c * (x - b)))

# --- Initial guesses ---
scale_factor = 1024/3.3  # if x is scaled
a_guess = 1.0
b_guess = 1.0 * scale_factor
c_guess = 1.0 / scale_factor
initial_guess = [a_guess, b_guess, c_guess]

# --- Fit the curve ---
popt, pcov = curve_fit(func, x_data, y_data, p0=initial_guess)
a_fit, b_fit, c_fit = popt
print(f"Fitted parameters: a={a_fit}, b={b_fit}, c={c_fit}")

# --- Define inverse function ---
def inverse_func(y, a, b, c):
    return b + (1/c) * np.log(np.exp((c/a)*y) - 1)

# --- Generate lookup table ---
lookup_table = [int(round(inverse_func(y, a_fit, b_fit, c_fit)*scale_factor)) for y in y_data]

# --- Output the lookup table ---
print("Lookup table for inverse function:")
print(lookup_table)

# Optional: save to file
np.savetxt("inverse_lookup_table.txt", lookup_table, fmt='%d')
