from migen import *
from migen.genlib.cdc import MultiReg

from litex.soc.interconnect.csr import *
from litex.soc.interconnect.csr_eventmanager import *


class btnintrupt(Module, AutoCSR):
    def __init__(self, signal):
        self._in = CSRStatus(len(signal))
        self.specials += MultiReg(signal, self._in.status)

        self.submodules.ev = EventManager()
        self.ev.cero = EventSourceProcess()
        self.ev.uno = EventSourceProcess()
        self.ev.dos = EventSourceProcess()
        self.ev.tres = EventSourceProcess()
        self.ev.cuatro = EventSourceProcess()
        self.ev.cinco = EventSourceProcess()
        self.ev.seis = EventSourceProcess()
        self.ev.siete = EventSourceProcess()
    #    self.ev.finalize()


        self.comb +=[self.ev.cero.trigger.eq(signal[1]),
            self.ev.uno.trigger.eq(signal [0]),
            self.ev.dos.trigger.eq(signal[2]),
            self.ev.tres.trigger.eq(signal[3]),
            self.ev.cuatro.trigger.eq(signal[4]),
            self.ev.cinco.trigger.eq(signal[5]),
            self.ev.seis.trigger.eq(signal[6]),
            self.ev.siete.trigger.eq(signal[7])
        ]
