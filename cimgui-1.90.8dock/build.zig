const std = @import("std");

pub fn compile(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
) *std.Build.Step.Compile {
    const imgui = b.addStaticLibrary(.{
        .name = "imgui",
        .target = target,
        .optimize = optimize,
    });
    imgui.linkLibC();
    imgui.linkLibCpp();

    imgui.addCSourceFiles(.{
        .root = b.path("cimgui-1.90.8dock"),
        .files = &.{
            "imgui/imgui.cpp",
            "imgui/imgui_demo.cpp",
            "imgui/imgui_draw.cpp",
            "imgui/imgui_tables.cpp",
            "imgui/imgui_widgets.cpp",
            "cimgui.cpp",
        },
    });

    return imgui;
}
