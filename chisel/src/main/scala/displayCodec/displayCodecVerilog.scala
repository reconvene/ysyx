// See README.md for license details.

package displayCodec

// _root_ disambiguates from package chisel3.util.circt if user imports chisel3.util._

import _root_.circt.stage.ChiselStage

object displayCodecVerilog extends App {
  ChiselStage.emitSystemVerilogFile(
    new displayCodecTop,
    args,
    firtoolOpts = Array("-disable-all-randomization")
  )
}
