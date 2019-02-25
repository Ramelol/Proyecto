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

from ios import Led, RGBLed, Button, Switch
from display import Display

#
# platform
#

_io = [
    ("user_led",  0, Pins("H17"), IOStandard("LVCMOS33")),
    ("user_led",  1, Pins("K15"), IOStandard("LVCMOS33")),
    ("user_led",  2, Pins("J13"), IOStandard("LVCMOS33")),
    ("user_led",  3, Pins("N14"), IOStandard("LVCMOS33")),
    ("user_led",  4, Pins("R18"), IOStandard("LVCMOS33")),
    ("user_led",  5, Pins("V17"), IOStandard("LVCMOS33")),
    ("user_led",  6, Pins("U17"), IOStandard("LVCMOS33")),
    ("user_led",  7, Pins("U16"), IOStandard("LVCMOS33")),
    ("user_led",  8, Pins("V16"), IOStandard("LVCMOS33")),
    ("user_led",  9, Pins("T15"), IOStandard("LVCMOS33")),
    ("user_led", 10, Pins("U14"), IOStandard("LVCMOS33")),
    ("user_led", 11, Pins("T16"), IOStandard("LVCMOS33")),
    ("user_led", 12, Pins("V15"), IOStandard("LVCMOS33")),
    ("user_led", 13, Pins("V14"), IOStandard("LVCMOS33")),
    #("user_led", 14, Pins("V12"), IOStandard("LVCMOS33")),
    #("user_led", 15, Pins("V11"), IOStandard("LVCMOS33")),

    ("user_sw",  0, Pins("J15"), IOStandard("LVCMOS33")),
    ("user_sw",  1, Pins("L16"), IOStandard("LVCMOS33")),
    ("user_sw",  2, Pins("M13"), IOStandard("LVCMOS33")),
    ("user_sw",  3, Pins("R15"), IOStandard("LVCMOS33")),
    ("user_sw",  4, Pins("R17"), IOStandard("LVCMOS33")),
    ("user_sw",  5, Pins("T18"), IOStandard("LVCMOS33")),
    ("user_sw",  6, Pins("U18"), IOStandard("LVCMOS33")),
    ("user_sw",  7, Pins("R13"), IOStandard("LVCMOS33")),
    ("user_sw",  8, Pins("T8"), IOStandard("LVCMOS33")),
    ("user_sw",  9, Pins("U8"), IOStandard("LVCMOS33")),
    ("user_sw", 10, Pins("R16"), IOStandard("LVCMOS33")),
    ("user_sw", 11, Pins("T13"), IOStandard("LVCMOS33")),
    ("user_sw", 12, Pins("H6"), IOStandard("LVCMOS33")),
    ("user_sw", 13, Pins("U12"), IOStandard("LVCMOS33")),
    ("user_sw", 14, Pins("U11"), IOStandard("LVCMOS33")),
    ("user_sw", 15, Pins("V10"), IOStandard("LVCMOS33")),

    ("user_btn", 0, Pins("N17"), IOStandard("LVCMOS33")),
    ("user_btn", 1, Pins("P18"), IOStandard("LVCMOS33")),
    ("user_btn", 2, Pins("P17"), IOStandard("LVCMOS33")),
    ("user_btn", 3, Pins("M17"), IOStandard("LVCMOS33")),
    ("user_btn", 4, Pins("M18"), IOStandard("LVCMOS33")),

    ("user_rgb_led", 0,
        Subsignal("r", Pins("N16")),
        Subsignal("g", Pins("R11")),
        Subsignal("b", Pins("G14")),
        IOStandard("LVCMOS33"),
    ),

    ("display_cs_n",  0, Pins("J17 J18 J14 P14 K2 U13 T9 T14"), IOStandard("LVCMOS33")),
    ("display_abcdefg",  0, Pins("T10 R10 K16 K13 P15 T11 L18 H15"), IOStandard("LVCMOS33")),

    ("clk100", 0, Pins("E3"), IOStandard("LVCMOS33")),

    ("cpu_reset", 0, Pins("C12"), IOStandard("LVCMOS33")),

    ("serial", 0,
        Subsignal("tx", Pins("D4")),
        Subsignal("rx", Pins("C4")),
        IOStandard("LVCMOS33"),
    ),

    ("adxl362_spi", 0,
        Subsignal("cs_n", Pins("D15")),
        Subsignal("clk", Pins("F15")),
        Subsignal("mosi", Pins("F14")),
        Subsignal("miso", Pins("E15")),
        IOStandard("LVCMOS33")
    ),

#    ("sdcard", 0,
     #   Subsignal("data", Pins("C2 E1 F1 D2"),Misc("PULLUP")),
     #   Subsignal("cmd", Pins("C1"),Misc("PULLUP")),
        #Subsignal("clk", Pins("B1")),
     #  Subsignal("clk", Pins("B1")),
     #   IOStandard("LVCMOS33"),Misc("SLEW=FAST")
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
       Subsignal("clk", Pins("B1")),
       Subsignal("mosi", Pins("E1")),
       Subsignal("miso", Pins("C2")),
       IOStandard("LVCMOS33"),
    ),

    ("sdcard_v",  0, Pins("C1"), IOStandard("LVCMOS33")), #vdd
    ("sdcard_v",  1, Pins("D2"), IOStandard("LVCMOS33")), #vss - gnd

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
        "rgbled",
        "leds",
        "switches",
        "buttons",
        #"adxl362",
        "display",
	#"sdcard",
	"sdclk",
        "sdphy",
        "sdcore",
        "sdtimer",
        "sdemulator",
        "bist_generator",
        "bist_checker",
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

        # Led
        #user_leds = Cat(*[platform.request("user_led", i) for i in range(16)])
        #self.submodules.leds = Led(user_leds)

        # Switches
        user_switches = Cat(*[platform.request("user_sw", i) for i in range(16)])
        self.submodules.switches = Switch(user_switches)

        # Buttons
        user_buttons = Cat(*[platform.request("user_btn", i) for i in range(5)])
        self.submodules.buttons = Button(user_buttons)

        # RGB Led
        self.submodules.rgbled  = RGBLed(platform.request("user_rgb_led",  0))
        
        # Accelerometer
        #self.submodules.adxl362 = SPIMaster(platform.request("adxl362_spi"))

        # Display
        self.submodules.display = Display(sys_clk_freq)
        self.comb += [
            platform.request("display_cs_n").eq(~self.display.cs),
            platform.request("display_abcdefg").eq(~self.display.abcdefg)
		]
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
	




soc = BaseSoC(platform)

#
# build
#
builder = Builder(soc, output_dir="build", csr_csv="test/csr.csv")
builder.build()
