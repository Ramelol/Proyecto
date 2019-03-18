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

from ios import Led, RGBLed, Button, Switch, Buttoninter
from btn_itrupt import*
#
# platform
#

_io = [

    ("user_btn", 0, Pins("M18"), IOStandard("LVCMOS33")),#h16
    ("user_btn", 1, Pins("M17"), IOStandard("LVCMOS33")),
    ("user_btn", 2, Pins("H14"), IOStandard("LVCMOS33")),
    ("user_btn", 3, Pins("P17"), IOStandard("LVCMOS33")),
    ("user_btn", 4, Pins("F16"), IOStandard("LVCMOS33")),
    ("user_btn", 5, Pins("D14"), IOStandard("LVCMOS33")),
    ("user_btn", 6, Pins("N17"), IOStandard("LVCMOS33")),
    ("user_btn", 7, Pins("F13"), IOStandard("LVCMOS33")),

    ("clk100", 0, Pins("E3"), IOStandard("LVCMOS33")),

    ("cpu_reset", 0, Pins("C12"), IOStandard("LVCMOS33")),

    ("serial", 0,
        Subsignal("tx", Pins("D4")),
        Subsignal("rx", Pins("C4")),
        IOStandard("LVCMOS33"),
    ),

    ("pantalla_spi", 0,
	Subsignal("cs_n", Pins("G17")),
        Subsignal("clk", Pins("D17")),
        Subsignal("mosi", Pins("C17")),
	Subsignal("miso", Pins("K1")),
        IOStandard("LVCMOS33"),
    ),
    ("pantalla_control",  0, Pins("E17"), IOStandard("LVCMOS33")), #led
    ("pantalla_control",  1, Pins("D18"), IOStandard("LVCMOS33")), #rs
    ("pantalla_control",  2, Pins("E18"), IOStandard("LVCMOS33")), #reset

    ("led_GB",  0, Pins("F18"), IOStandard("LVCMOS33")), #ledAzul

    ("sdcard_spi", 0,
       Subsignal("cs_n", Pins("H17")),     
       Subsignal("clk", Pins("B1")),#B1
       Subsignal("mosi", Pins("C1")),#C1
       Subsignal("miso", Pins("C2")),#C2
       IOStandard("LVCMOS33"),
    ),

    ("sdcard_v",  0, Pins("E2"), IOStandard("LVCMOS33")), #vdd
    ("sdcard_v",  1, Pins("D2"), IOStandard("LVCMOS33")), #cs

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
        "user_btn",
	"pantalla_spi",
	"pantalla_control",
	"led_GB",
        "sdcard_spi",
	"sdcard_v"
    ]
    csr_map_update(SoCCore.csr_map, csr_peripherals)


    def __init__(self, platform):
        sys_clk_freq = int(100e6)
        # SoC with CPU
        interrupt_map = {
            "buttons" : 4,
        }
        SoCCore.interrupt_map.update(interrupt_map)
        SoCCore.__init__(self, platform,
            cpu_type="lm32",
            clk_freq=100e6,
            ident="CPU Test SoC", ident_version=True,
            integrated_rom_size=0x8000,
            integrated_main_ram_size=32*1024)

        # Clock Reset Generation
        self.submodules.crg = CRG(platform.request("clk100"), ~platform.request("cpu_reset"))

        # FPGA identification
        self.submodules.dna = dna.DNA()

        # Buttons
        user_buttons = Cat(*[platform.request("user_btn", i) for i in range(8)])
        self.submodules.buttons = btnintrupt(user_buttons)

	#LedAzul
        user_led = Cat(*[platform.request("led_GB", i) for i in range(1)])
        self.submodules.led_GB = Led(user_led)
	#spiLCD
        user_control = Cat(*[platform.request("pantalla_control", i) for i in range(3)])
        self.submodules.pantalla_spi = SPIMaster(platform.request("pantalla_spi"))        
        self.submodules.pantalla_control = Led(user_control)

	#spiSd

        user_control_sd = Cat(*[platform.request("sdcard_v", i) for i in range(1)])
        self.submodules.sdcard_spi = SPIMaster(platform.request("sdcard_spi"))   
        self.submodules.sdcard_v = Led(user_control_sd)     


        print (SoCCore.interrupt_map)

  
 
  

soc = BaseSoC(platform)

#
# build
#
builder = Builder(soc, output_dir="build", csr_csv="test/csr.csv")
builder.build()
