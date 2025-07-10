// See README.md for license details.

package npc

// _root_ disambiguates from package chisel3.util.circt if user imports chisel3.util._

import _root_.circt.stage.ChiselStage

object npcVerilog extends App {
  ChiselStage.emitSystemVerilogFile(
    new npc,
    args,
    firtoolOpts = Array("-disable-all-randomization", "--split-verilog", "-o", "build/sv/npc")
  )
}
