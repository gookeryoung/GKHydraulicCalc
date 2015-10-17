import math

#for i in range(100):
#    print i

def calc_cylinder_volume(d1, l):
    result = math.pi * d1 * d1 * l / 4.0
    return result

def calc_u1(d1, p, t):
    detaP = (16.0 / d1) ** 2 * p
    p1 = (200.0 - detaP) / 100.0
    t1 = (250.0 - t) / 100.0
    u1 = math.exp(0.155 * t1 ** 3 * p1 - 0.494 * t1 ** 2 * p1 + 0.36 * t1 * p1 + 1.135 * t1 + 1.523 * p1 - 4.185)
    return u1

def calc_Q(d2, deta, vl, u1, d1, p):
    detaP = (16.0 / d1) ** 2 * p
    Q = 1E-3 * 2.5 * math.pi * d2 * deta ** 3 * detaP / (12.0 * u1 * vl)
    return Q

def calc_t(v, Q):
    t = v / (Q * 1.0E9)
    return t