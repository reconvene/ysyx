// See README.md for license details.

package codec

// _root_ disambiguates from package chisel3.util.circt if user imports chisel3.util._

import _root_.circt.stage.ChiselStage

object codecVerilog extends App {
  ChiselStage.emitSystemVerilogFile(
    new codecTop(4),
    args,
    firtoolOpts = Array("-disable-all-randomization")
  )
}
