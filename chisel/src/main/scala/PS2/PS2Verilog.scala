// See README.md for license details.

package PS2

// _root_ disambiguates from package chisel3.util.circt if user imports chisel3.util._

import _root_.circt.stage.ChiselStage

object PS2Verilog extends App {
  ChiselStage.emitSystemVerilogFile(
    new ps2Display,
    args,
    firtoolOpts = Array("-disable-all-randomization")
  )
}
