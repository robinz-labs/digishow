class DGSSignal:
    def __init__(self, signal_type = 0, channel = 0, a_value = 0, a_range = 0, b_value = 0):
        self.channel = channel
        self.type    = signal_type
        self.a_value = a_value
        self.a_range = a_range
        self.b_value = b_value
    
    @classmethod
    def analog(cls, channel, a_value, a_range = 0xffff):
        return cls(ord('A'), channel, a_value, a_range)

    @classmethod
    def binary(cls, channel, b_value = 0):
        return cls(ord('B'), channel, 0, 0, b_value)    

    @classmethod
    def note(cls, channel, a_value = 0, b_value = 0):
        return cls(ord('N'), channel, a_value, 127, b_value)    

    def from_message(self, message):
        items = message.split(',')
        if len(items)!=6 or items[0]!='dgss' :
            return False

        self.channel = int(items[1])
        self.type    = int(items[2])
        self.a_value = int(items[3])
        self.a_range = int(items[4])
        self.b_value = int(items[5])
        return True

    def to_message(self):
        return 'dgss,' + \
            str(self.channel) + ',' + \
            str(self.type) + ',' + \
            str(self.a_value) + ',' + \
            str(self.a_range) + ',' + \
            str(self.b_value)
