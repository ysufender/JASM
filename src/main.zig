const std = @import("std");
const jasm = @import("core/jasm.zig");

pub fn main(init: std.process.Init) void {
    return jasm.jasmmain(&init);
}
