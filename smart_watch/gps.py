from random import getrandbits

class GPS:
    def __init__(self):
        pass

    @property
    def value(self) -> tuple[float, float]:
        x = getrandbits(16)
        y = getrandbits(16)
        return (x/10000+20, y/10000+30)
