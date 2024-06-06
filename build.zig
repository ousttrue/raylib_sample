const std = @import("std");
const raylib_build = @import("raylib/build.zig");

const Program = struct {
    name: []const u8,
    path: []const u8,
    includes: []const []const u8 = &.{},
    source: ?[]const u8 = null,
};

// example.addIncludePath(b.path());
const examples = [_]Program{
    // core
    .{
        .name = "core_world_screen",
        .path = "examples/core/core_world_screen.zig",
    },
    .{
        .name = "core_3d_camera_split_screen",
        .path = "examples/core/core_3d_camera_split_screen.zig",
    },
    .{
        .name = "core_3d_camera_first_person",
        .path = "examples/core/core_3d_camera_first_person.zig",
    },
    .{
        .name = "orbitcamera",
        .path = "examples/core/orbitcamera.zig",
    },
    .{
        .name = "blender_fly_camera",
        .path = "examples/core/blender_fly_camera.zig",
        .includes = &.{
            "examples/core",
        },
    },
    // models
    .{
        .name = "models_geometric_shapes",
        .path = "examples/models/models_geometric_shapes.zig",
    },
    // shapes
    .{
        .name = "shapes_lines_bezier",
        .path = "examples/shapes/shapes_lines_bezier.zig",
    },
    .{
        .name = "shapes_draw_ring",
        .path = "examples/shapes/shapes_basic_shapes.zig",
    },
    .{
        .name = "shapes_draw_ring",
        .path = "examples/shapes/shapes_draw_ring.zig",
        .includes = &.{
            "raygui",
        },
    },
    // text
    .{
        .name = "text_raylib_fonts",
        .path = "examples/text/text_raylib_fonts.zig",
    },
    // others
    .{
        .name = "rlgl_standalone",
        .path = "examples/others/rlgl_standalone.zig",
        .includes = &.{
            "raylib/external/glfw/include",
        },
    },
    .{
        .name = "raygizmo",
        .path = "examples/others/raygizmo.zig",
        .includes = &.{"raygizmo"},
    },
    // .{
    //     .name = "bvhview",
    //     .path = "examples/others/bvhview.zig",
    // },
    // shaders
    .{
        .name = "shaders_texture_tiling",
        .path = "examples/shaders/shaders_texture_tiling.zig",
    },
    // raygui
    .{
        .name = "floating_window",
        .path = "examples/raygui/floating_window.zig",
        .includes = &.{"raygui"},
    },
    .{
        .name = "controls_test_suite",
        .path = "examples/raygui/controls_test_suite/controls_test_suite.zig",
        .includes = &.{"raygui"},
    },
    .{
        .name = "portable_window",
        .path = "examples/raygui/portable_window/portable_window.zig",
        .includes = &.{"raygui"},
    },
    .{
        .name = "timeline",
        .path = "examples//raygui/timeline/timeline.zig",
        .includes = &.{
            "examples/raygui/timeline",
            "raygui",
        },
        .source =
        \\#include "raylib.h"
        \\#define RAYGUI_IMPLEMENTATION
        \\#include "raygui.h"
        \\#define _TIMELINE_IMPL_
        \\#include "timeline.h"
        //
        ,
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

        example.linkLibC();
        example.linkLibrary(raylib_compile);

        // for @cImclude
        example.addIncludePath(b.path("raylib"));
        for (ex.includes) |include| {
            example.addIncludePath(b.path(include));
        }

        // add c source
        if (ex.source) |src| {
            var gen_step = b.addWriteFiles();
            example.step.dependOn(&gen_step.step);
            const timeline_c_path = gen_step.add("tmp.c", src);
            example.addCSourceFile(.{ .file = timeline_c_path });
        }
    }
}
