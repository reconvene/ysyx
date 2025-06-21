// See README.md for license details.

package streamingLight

// _root_ disambiguates from package chisel3.util.circt if user imports chisel3.util._

import _root_.circt.stage.ChiselStage

object streamingLightVerilog extends App {
  ChiselStage.emitSystemVerilogFile(
    new streamingLightTop(8),
    args,
    firtoolOpts = Array("-disable-all-randomization")
  )
}
