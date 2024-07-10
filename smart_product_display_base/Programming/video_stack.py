'''
Implements a Stack to deal with video showing
'''
import os
import time

class Stack:
    """
    singly linkedlist with LIFO settings
    """ 
    def __init__(self):
        self._container = []

    @property
    def peak(self):
        if self._container:
            return self._container[-1]
        else:
            return None

    def push(self, value):
        self._container.append(value)

    def pop(self):
        return self._container.pop()

    @property
    def empty(self):
        return not self._container

    def __repr__(self):
        return repr(self._container)

class VideoStack:
    '''
    Stack that plays and stop videos as elements are pushed/popped
    '''
    VIDEOS = ['./videos/default.mp4',
              './videos/video1.mp4',
              './videos/video2.mp4',
              './videos/video3.mp4',
              './videos/video4.mp4']
    def __init__(self):

        # Creating Underlying Data Structures
        self._stack = Stack()
        self._set = set()

        # Default video should always be playing if no other videos are triggered
        self._stack.push(0)
        self._set.add(0)

        # Disabling screen saver
        self.stop_screen_saver()

        # Starts playing the Default video
        self.currently_playing = 0
        self.play_video(0)

    def stop_screen_saver(self):
        '''
        calls the xset terminal command
        '''
        os.system("xset dpms force on")
        os.system("xset -dpms")
        os.system("xset s off")
        os.system("xset s noblank")

    def stop_video(self):
        '''
        Stops video playing
        '''
        os.system("pkill vlc")
        time.sleep(0.5)
        #TODO: halt until vlc is completely stopped

    def play_video(self, ind):
        '''
        Starts video playing
        '''
        if ind > (len(self.VIDEOS)-1):
            raise ValueError("Trying to play a video out of index")

        os.system(f"cvlc --loop --no-osd {self.VIDEOS[ind]} &")
        time.sleep(2)
        #TODO: halt until vlc is playing the video

    def push(self, ind: int):
        '''
        kills whatever video was playing and plays the new video
        '''
        self._stack.push(ind)
        self.stop_video()
        self.play_video(ind)

    def pop(self):
        '''
        kills whatever video was playing and plays the new video
        '''
        self._stack.pop()
        self.stop_video()
        if not self._stack.empty:
            self.play_video(self._stack.peak)
        
    def process(self, p_states: list[bool]):
        '''
        Decides what video should be playing now
        '''
        # inserting True at 0 index since Default Video should always be there
        # Also makes the ind value correct
        p_states.insert(0, True)

        # inverting all the states as we want to show when product not in place
        for ind, state in enumerate(p_states):
            p_states[ind] = not state

        # Update the Data Structures
        for ind, state in enumerate(p_states):

            # Checking if something new happenned
            if not ((ind in self._set) == state):

                # Prev values not equal new state
                if state:
                    # New ON Trigger
                    self._stack.push(ind)
                    self._set.add(ind)

                else:
                    # New OFF Trigger
                    # Check if latest remove is the stack peak
                    if ind == self._stack.peak:
                        # ind to be removed is the stack peak value
                        # Removing Top value of Stack
                        self._stack.pop()

                        # MUST CHECK IF BOTTOM VALUES NEED REMOVING
                        # BOTTOM VALUES COULD HAVE BEEN REMOVED WHEN
                        # THE PEAK IND WAS STILL OUT
                        # Keeps removing until a video supposed to play is there
                        while self._stack.peak not in self._set:
                            self._stack.pop()

                    self._set.remove(ind)

        # Playing the stack peak ind video :D (if not already playing)
        if self._stack.peak != self.currently_playing:
            self.stop_video()
            self.play_video(self._stack.peak)
            self.currently_playing = self._stack.peak

