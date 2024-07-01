from random import getrandbits

class GPS:
    def __init__(self):
        pass

    @property
    def value(self) -> tuple[float, float]:
        x = getrandbits(16)
        y = getrandbits(16)
        return (round((x/10000+37), 3), round((y/10000-1223), 3))

    @property
    def str_value(self) -> str:
        return f"{self.value[0],self.value[1]}"

