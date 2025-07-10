// 结束模块
module finishSim (
    input finishStatus,
    input clock,
    input reset
);
    always @(posedge clock) begin
        if(finishStatus) begin
            $display("End the simulation!");
            $finish;
        end
    end
endmodule