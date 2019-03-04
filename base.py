from migen import *

from migen.genlib.io import CRG

from litex.build.generic_platform import *
from litex.build.xilinx import XilinxPlatform

from litesdcard.phy import SDPHY
from litesdcard.clocker import SDClockerS7
from litesdcard.clocker import SDClockerS6
from litesdcard.core import SDCore
from litesdcard.bist import BISTBlockGenerator, BISTBlockChecker
from litex.soc.integration.soc_core import *
from litex.soc.integration.builder import *
from litex.soc.cores import dna, xadc
from litex.soc.cores.spi import SPIMaster
from litex.soc.cores.timer import Timer
from litex.soc.interconnect.csr_eventmanager import *

from ios import Led, RGBLed, Button, Switch
from display import Display

#
# platform
#

_io = [


    ("user_btn", 0, Pins("N17"), IOStandard("LVCMOS33")),
    ("user_btn", 1, Pins("P18"), IOStandard("LVCMOS33")),
    ("user_btn", 2, Pins("P17"), IOStandard("LVCMOS33")),
    ("user_btn", 3, Pins("M17"), IOStandard("LVCMOS33")),
    ("user_btn", 4, Pins("M18"), IOStandard("LVCMOS33")),

    ("clk100", 0, Pins("E3"), IOStandard("LVCMOS33")),

    ("cpu_reset", 0, Pins("C12"), IOStandard("LVCMOS33")),

    ("serial", 0,
        Subsignal("tx", Pins("D4")),
        Subsignal("rx", Pins("C4")),
        IOStandard("LVCMOS33"),
    ),

 
    #("sdcard", 0,
    #     Subsignal("data", Pins("C2 E1 F1 D2"),Misc("PULLUP")),
    #     Subsignal("cmd", Pins("C1"),Misc("PULLUP")),
    #     Subsignal("clk", Pins("B1")),
    #    IOStandard("LVCMOS33"),Misc("SLEW=FAST")
    #),
    ("pantalla_spi", 0,
	Subsignal("cs_n", Pins("G13")),
        Subsignal("clk", Pins("F16")),
        Subsignal("mosi", Pins("G16")),
	Subsignal("miso", Pins("H14")),
        IOStandard("LVCMOS33"),
    ),
    ("pantalla_control",  0, Pins("D14"), IOStandard("LVCMOS33")), #led
    ("pantalla_control",  1, Pins("E16"), IOStandard("LVCMOS33")), #rs
    ("pantalla_control",  2, Pins("F13"), IOStandard("LVCMOS33")), #reset

    ("sdcard_spi", 0,
       Subsignal("cs_n", Pins("F1")),     
       Subsignal("clk", Pins("B1")),#B1
       Subsignal("mosi", Pins("E1")),
       Subsignal("miso", Pins("C2")),#C2
       IOStandard("LVCMOS33"),
    ),

    ("sdcard_v",  0, Pins("C1"), IOStandard("LVCMOS33")), #vdd
    ("sdcard_v",  1, Pins("D2"), IOStandard("LVCMOS33")), #vss - gnd

    #("cs_SD",  0, Pins("F1"), IOStandard("LVCMOS33")), #cs

]



class Platform(XilinxPlatform):
    default_clk_name = "clk100"
    default_clk_period = 10.0

    def __init__(self):
        XilinxPlatform.__init__(self, "xc7a100t-CSG324-1", _io, toolchain="ise")

    def do_finalize(self, fragment):
        XilinxPlatform.do_finalize(self, fragment)


def csr_map_update(csr_map, csr_peripherals):
    csr_map.update(dict((n, v)
        for v, n in enumerate(csr_peripherals, start=max(csr_map.values()) + 1)))


#
# design
#

# create our platform (fpga interface)
platform = Platform()

# create our soc (fpga description)
class BaseSoC(SoCCore):


    # Peripherals CSR declaration
    csr_peripherals = [
        "dna",
        "xadc",
        "user_btn",
	#"sdcard",
	#"sdclk",
        #"sdphy",
        #"sdcore",
        #"sdtimer",
        #"sdemulator",
        #"bist_generator",
        #"bist_checker",
	"pantalla_spi",
	"pantalla_control",
        "sdcard_spi",
	"sdcard_v"
    ]
    csr_map_update(SoCCore.csr_map, csr_peripherals)

    def __init__(self, platform):
        sys_clk_freq = int(100e6)
        # SoC with CPU
        SoCCore.__init__(self, platform,
            cpu_type="lm32",
            clk_freq=100e6,
            ident="CPU Test SoC", ident_version=True,
            integrated_rom_size=0x8000,
            integrated_main_ram_size=16*1024)

        # Clock Reset Generation
        self.submodules.crg = CRG(platform.request("clk100"), ~platform.request("cpu_reset"))

        # FPGA identification
        self.submodules.dna = dna.DNA()

        # FPGA Temperature/Voltage
        self.submodules.xadc = xadc.XADC()

   
        # Buttons
        user_buttons = Cat(*[platform.request("user_btn", i) for i in range(5)])
        self.submodules.buttons = Button(user_buttons)


        #bridge
        #sdcard_pads = platform.request('sdcard')
	

        # sd
        #self.submodules.sdclk = SDClockerS7(1,100e6,sdcard_pads)
        #self.submodules.sdphy = SDPHY(sdcard_pads, platform.device)
        #self.submodules.sdcore = SDCore(self.sdphy)
        #self.submodules.sdtimer = Timer()

        #self.submodules.bist_generator = BISTBlockGenerator(random=True)
        #self.submodules.bist_checker = BISTBlockChecker(random=True)

	#spiLCD
        user_control = Cat(*[platform.request("pantalla_control", i) for i in range(3)])
        self.submodules.pantalla_spi = SPIMaster(platform.request("pantalla_spi"))        
        self.submodules.pantalla_control = Led(user_control)

	#spiSd

        user_control_sd = Cat(*[platform.request("sdcard_v", i) for i in range(2)])
        self.submodules.sdcard_spi = SPIMaster(platform.request("sdcard_spi"))   
        self.submodules.sdcard_v = Led(user_control_sd)     

        interrupt_map = {
            "user_btn" : 4,
        }
        SoCCore.interrupt_map.update(interrupt_map)
        print (SoCCore.interrupt_map)
	




soc = BaseSoC(platform)

#
# build
#
builder = Builder(soc, output_dir="build", csr_csv="test/csr.csv")
builder.build()
