// See README.md for license details.

package singleALU

// _root_ disambiguates from package chisel3.util.circt if user imports chisel3.util._

import _root_.circt.stage.ChiselStage

object singleALUVerilog extends App {
  ChiselStage.emitSystemVerilogFile(
    new singleALUTop,
    args,
    firtoolOpts = Array("-disable-all-randomization")
  )
}
