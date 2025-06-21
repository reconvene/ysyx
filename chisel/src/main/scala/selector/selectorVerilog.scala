// See README.md for license details.

package selector

// _root_ disambiguates from package chisel3.util.circt if user imports chisel3.util._

import _root_.circt.stage.ChiselStage

object selectorVerilog extends App {
  ChiselStage.emitSystemVerilogFile(
    new selectorTop,
    args,
    firtoolOpts = Array("-disable-all-randomization")
  )
}
