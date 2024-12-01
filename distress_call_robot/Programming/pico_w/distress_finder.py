'''
Algorithm to find sound source.
 1 - Find Amplitude Differences
 2 - Find Time Difference of Arrival
 3 - Triangulation: Combine Amplitude Differences and time difference of Arrival
'''

class DistressCallFinder:
    '''
    Use 3 Microphone Sensors to find orientation of sound source
    '''
    def __init__(self, mic1, mic2, mic3):
        self.mic1 = mic1
        self.mic2 = mic2
        self.mic3 = mic3

    def find_orientation(self) -> int:
        '''
        returns degrees to right/left (positive/negative) to point to sound source
        '''
        # for now just a simple difference between them
        return (self.mic2.avg_sample() - self.mic3.avg_sample())


