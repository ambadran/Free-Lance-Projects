"""
This script will generate arctan LUT given a fraction numerator `a` and denumerator `b`

The arctan is calculate the pitch or roll given Ax, Ay, Az

  pitch = ARCTAN_A_B[accel_values[0]+ACCEL_SCALE][accel_values[2]];
  roll = ARCTAN_A_B[accel_values[1]+ACCEL_SCALE][accel_values[2]];

The acceleration values has range -1g to 1g. it's normally a float value. but for our case
to remove the usage of floating points, we'll convert it to fixed point by multiplying by
ACCEL_SCALE. In this case the ACCEL_SCALE=10. so the range is -10 to 10 with +1 integer increments
This means that we have 20*20 possible combinations but the actual number is 20*10 because sign division rules. the last thing is that we can't have -ve number as index so we'll add the ACCEL_SCALE to the numerator value to make any negatives have +ve index.
"""
from math import atan2, degrees
import pprint

possible_a = tuple(range(-10, 11))
possible_b = tuple(range(1, 11))

seta = set()
seta_len = len(seta)
lista = []
dicta = {}
for a in possible_a:
    for b in possible_b:
        ans = round(degrees(atan2(a, b)), 3)

        if ans in seta:
            print("Matched before: ", end='')
        else:
            seta.add(ans)

        lista.append(ans)

        dicta[f"atan({a}/{b})"] = ans

        print(f"atan2({a}/{b}) = {ans}")

print(len(possible_a), len(possible_b))
print(len(lista))
print(len(seta))
print(len(dicta))
# print(lista)
pprint.pp(dicta)


