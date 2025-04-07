#!/usr/bin/env python3
import numpy as np
import numba as nb

#@nb.njit
def calcHSVINT(R, G, B):
    E = 65535
    bitShift = 16

    # Step 1 find the min, max and mid of RGB
    temp_list = np.array([R, G, B], dtype=np.int64)
    m = min(R, G, B)
    M = max(R, G, B)
    c = np.median(temp_list)

    # Step 2 Set V equal to M
    V = M

    # Step 3 calculate the difference between M and m, if its 0, set S to 0, and H to -1 (It is undefined in this case)
    d = int(M - m)
    if V == 0:
        S = 0
        H = -1
        return H, S, V

    # Step 4 calculate S using d and V
    S = int(((d << bitShift) - 1) // V)

    # Step 5 find the selector index based on which color is the Min/Max, special case is needed if two are the same
    if M == R and m == B:
        I = 0
    elif M == G and m == B:
        I = 1
    elif M == G and m == R:
        I = 2
    elif M == B and m == R:
        I = 3
    elif M == B and m == G:
        I = 4
    elif M == R and m == G:
        I = 5

    # Step 6 calculate F using c, m and d, check if I is 1,3,5 and set F to its inverse
    F = int(((int(c - m) << 16) // d) + 1)
    if I % 2 != 0:
        F = E - F

    # Step 7 calculate H using E, I and F
    H = (E * I) + F

    return H, S, V

def RGB_2_HSV(RGB):
    ''' Converts an integer RGB tuple (value range from 0 to 255) to an HSV tuple '''

    # Unpack the tuple for readability
    R, G, B = RGB

    # Compute the H value by finding the maximum of the RGB values
    RGB_Max = max(RGB)
    RGB_Min = min(RGB)

    # Compute the value
    V = RGB_Max;
    if V == 0:
        H = S = 0
        return (H,S,V)


    # Compute the saturation value
    S = 255 * (RGB_Max - RGB_Min) // V

    if S == 0:
        H = 0
        return (H, S, V)

    # Compute the Hue
    if RGB_Max == R:
        H = 0 + 43*(G - B)//(RGB_Max - RGB_Min)
    elif RGB_Max == G:
        H = 85 + 43*(B - R)//(RGB_Max - RGB_Min)
    else: # RGB_MAX == B
        H = 171 + 43*(R - G)//(RGB_Max - RGB_Min)

    return (H, S, V)

if __name__ == "__main__":
    print(RGB_2_HSV((255,0,0)))
    print(RGB_2_HSV((0,255,0)))
    print(RGB_2_HSV((0,0,255)))
    print(RGB_2_HSV((200,200,2)))
    print(RGB_2_HSV((255,255,255)))
    print(RGB_2_HSV((128,128,128)))
    print(RGB_2_HSV((255,0,255)))
