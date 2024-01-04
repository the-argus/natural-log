const std = @import("std");

const zcc = @import("compile_commands");

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const raylib_integration = b.option(
        bool,
        "raylib_integration",
        "whether to link raylib and register the logger with raylib when calling init().",
    ) orelse false;

    var targets = std.ArrayList(*std.Build.Step.Compile).init(b.allocator);
    defer targets.deinit();

    const lib = std.Build.addStaticLibrary(b, .{
        .name = "natural_log",
        .optimize = optimize,
        .target = target,
    });

    lib.addCSourceFile(.{
        .file = .{ .path = "natural_log.cpp" },
        .flags = &.{
            "-std=c++20",
            "-DNATURAL_LOG_NOEXCEPT=noexcept",
            if (raylib_integration) "-DNATURAL_LOG_RAYLIB" else "",
        },
    });

    lib.linkLibCpp();

    lib.defineCMacro("FMT_EXCEPTIONS", "0");
    lib.defineCMacro("FMT_HEADER_ONLY", null);

    if (raylib_integration) {
        // raylib is a private dependency
        const raylib_dep = b.dependency("raylib", .{
            .target = target,
            .optimize = optimize,
        });
        const raylib = raylib_dep.artifact("raylib");
        lib.linkLibrary(raylib);
    }

    lib.installHeader("natural_log.hpp", "natural_log.hpp");

    // Add fmt as dependency
    {
        const fmt = b.dependency("fmt", .{});
        // make path to where the fmt builder installs its headers
        const fmt_include_path = try std.fs.path.join(
            b.allocator,
            &.{ fmt.builder.install_path, "include" },
        );
        // we have to wait for fmt to be installed before we can build
        lib.step.dependOn(fmt.builder.getInstallStep());
        // add the path found earlier to our compilation flags
        lib.addIncludePath(.{ .path = fmt_include_path });
    }

    try targets.append(lib);

    b.installArtifact(lib);

    zcc.createStep(b, "cdb", try targets.toOwnedSlice());
}
