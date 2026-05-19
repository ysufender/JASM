const std = @import("std");

pub const ParserConfig = struct {
    prefix: []const u8 = "--",
    aliasPrefix: []const u8 = "-",
};

pub const ParserEntry = struct {
    /// Type
    type,

    /// Name
    []const u8,

    /// Binding
    []const u8,

    /// Description
    []const u8,
};

pub fn Parser(comptime options: []const ParserEntry) type {
}
