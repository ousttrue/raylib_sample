const std = @import("std");
const raylib_build = @import("raylib/build.zig");

const Program = struct {
    name: []const u8,
    path: []const u8,
    includes: []const []const u8 = &.{},
};

const examples = [_]Program{
    .{
        .name = "core_3d_camera_first_person",
        .path = "examples/core/core_3d_camera_first_person.zig",
    },
    .{
        .name = "rlgl_standalone",
        .path = "examples/others/rlgl_standalone.zig",
    },
    .{
        .name = "shapes_draw_ring",
        .path = "examples/shapes/shapes_draw_ring.zig",
    },
    // macro define not work
    // .{
    //     .name = "property_list",
    //     .path = "examples/raygui/property_list/property_list.zig",
    //     .includes = &[_][]const u8{"examples/raygui/property_list"},
    // },
};

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const raylib_compile = raylib_build.addRaylib(b, target, optimize, .{
        .raygui = true,
    });

    {
        const exe = b.addExecutable(.{
            .name = "raylib_sample",
            .root_source_file = b.path("src/main.zig"),
            .target = target,
            .optimize = optimize,
        });
        b.installArtifact(exe);
        exe.addIncludePath(b.path("raylib"));
        exe.linkLibC();
        exe.linkLibrary(raylib_compile);
    }

    {
        const exe = b.addExecutable(.{
            .name = "raygui_sample",
            .root_source_file = b.path("src/raygui_main.zig"),
            .target = target,
            .optimize = optimize,
        });
        b.installArtifact(exe);
        exe.addIncludePath(b.path("raylib"));
        exe.addIncludePath(b.path("raygui"));
        exe.linkLibC();
        exe.linkLibrary(raylib_compile);
    }

    // const run_cmd = b.addRunArtifact(exe);
    // run_cmd.step.dependOn(b.getInstallStep());
    // if (b.args) |args| {
    //     run_cmd.addArgs(args);
    // }
    // const run_step = b.step("run", "Run the app");
    // run_step.dependOn(&run_cmd.step);
    //
    // const exe_unit_tests = b.addTest(.{
    //     .root_source_file = b.path("src/main.zig"),
    //     .target = target,
    //     .optimize = optimize,
    // });
    // const run_exe_unit_tests = b.addRunArtifact(exe_unit_tests);
    // const test_step = b.step("test", "Run unit tests");
    // test_step.dependOn(&run_exe_unit_tests.step);

    //
    // examples
    //
    // https://github.com/Not-Nik/raylib-zig/blob/devel/build.zig
    for (examples) |ex| {
        const example = b.addExecutable(.{
            .name = ex.name,
            .root_source_file = b.path(ex.path),
            .target = target,
            .optimize = optimize,
        });
        b.installArtifact(example);
        example.addIncludePath(b.path("raylib"));
        example.addIncludePath(b.path("raylib/external/glfw/include"));
        example.addIncludePath(b.path("raygui"));

        for (ex.includes) |include| {
            example.addIncludePath(b.path(include));
        }

        example.linkLibC();
        example.linkLibrary(raylib_compile);
    }
}
