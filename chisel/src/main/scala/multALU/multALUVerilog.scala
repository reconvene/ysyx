// See README.md for license details.

package multALU

// _root_ disambiguates from package chisel3.util.circt if user imports chisel3.util._

import _root_.circt.stage.ChiselStage

object multALUVerilog extends App {
  ChiselStage.emitSystemVerilogFile(
    new multALU(4),
    args,
    firtoolOpts = Array("-disable-all-randomization")
  )
}
