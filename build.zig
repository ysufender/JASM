const std = @import("std");

const resourcePath = "res/";

const targets = [_]std.Target.Query{
    .{},
    .{ .os_tag = .windows, .cpu_arch = .x86_64 },
    .{ .os_tag = .linux,   .cpu_arch = .x86_64 },
};

const version = std.SemanticVersion{
    .major = 0,
    .minor = 1,
    .patch = 0,
};

pub fn build(b: *std.Build) void {
    const config = configureBuild(b);
    addTargets(b, .Debug, config);
    addTargets(b, .ReleaseFast, config);
    addTargets(b, .ReleaseSafe, config);
    addTargets(b, .ReleaseSmall, config);
}

fn addTargets(b: *std.Build, optimize: std.builtin.OptimizeMode, config: BuildConfig) void {
    var seen = std.StringHashMap(void).init(b.allocator);
    defer seen.deinit();

    const toLower = std.ascii.allocLowerString;
    const print = std.fmt.allocPrint;

    for (targets) |query| {

        const target   = b.resolveTargetQuery(query);

        const targetName = print(b.allocator, "{s}-{s}-{s}", .{
            toLower(b.allocator, @tagName(optimize)) catch unreachable,
            @tagName(target.result.os.tag),
            @tagName(target.result.cpu.arch),
        }) catch unreachable;

        if (seen.contains(targetName)) continue;
        seen.putNoClobber(targetName, {}) catch unreachable;

        const versionString = print(b.allocator, "v{d}.{d}.{d}", .{
            version.major,
            version.minor,
            version.patch,
        }) catch unreachable;

        const opts = b.addOptions();
        opts.addOption(bool, "isDebug", optimize == .Debug);
        opts.addOption([]const u8, "version", versionString);
        opts.addOption(bool, "toolchainMode", config.toolchain);
        opts.addOption(bool, "pipelineSupport", config.pipeline);
        opts.addOption([]const u8, "JASM_ENDL", "__JASM__ENDL__");
        opts.addOption([]const u8, "JASM_EOF", "__JASM__EOF__");

        const exe = b.addExecutable(.{
            .name = "jasm",
            .version = version,
            .root_module = b.createModule(
                if (optimize == .Debug) .{
                    .root_source_file = b.path(
                        if (config.toolchain) "src/root.zig"
                        else "src/main.zig"
                    ),
                    .target = target,
                    .optimize = optimize,
                    .link_libc  = target.result.os.tag == .windows,
                    .error_tracing = true,
                    .omit_frame_pointer = false,
                }
                else .{
                    .root_source_file = b.path(
                        if (config.toolchain) "src/root.zig"
                        else "src/main.zig"
                    ),
                    .target = target,
                    .optimize = optimize,
                    .link_libc  = target.result.os.tag == .windows,
                    .strip = true,
                }
            ),
        });
        exe.root_module.addEmbedPath(b.path(resourcePath));
        exe.root_module.addOptions("config", opts);

        const install = b.addInstallArtifact(exe, .{
            .dest_dir = .{ .override = .{ .custom = b.pathJoin(&.{
                if (config.toolchain) "lib" else "",
                targetName,
                versionString,
            }) } }
        });

        const step = b.step(targetName, print(
            b.allocator,
            "Build for {s}",
            .{targetName},
        ) catch unreachable);
        step.dependOn(&install.step);
        step.dependOn(b.getInstallStep());
    }
}

fn configureBuild(b: *std.Build) BuildConfig {
    const toolchainMode = b.option(bool, "ToolchainMode", "Enable toolchain mode.") orelse false;
    const pipelineSupport = b.option(bool, "PipelineSupport", "Enable pipeline support.") orelse false;

    return .{
        .toolchain = toolchainMode,
        .pipeline = pipelineSupport,
    };
}

const BuildConfig = struct {
    toolchain: bool,
    pipeline: bool,
};
