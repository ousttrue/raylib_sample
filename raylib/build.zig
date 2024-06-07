const std = @import("std");
const builtin = @import("builtin");

// for zig-0.13
pub fn compile(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    options: Options,
) *std.Build.Step.Compile {
    const raylib_flags = &[_][]const u8{
        "-std=gnu99",
        "-D_GNU_SOURCE",
        "-DGL_SILENCE_DEPRECATION=199309L",
    };

    const raylib = b.addStaticLibrary(.{
        .name = "raylib",
        .target = target,
        .optimize = optimize,
    });
    raylib.linkLibC();

    // No GLFW required on PLATFORM_DRM
    if (!options.platform_drm) {
        raylib.addIncludePath(.{ .cwd_relative = srcdir ++ "/external/glfw/include" });
    }

    raylib.addCSourceFiles(.{
        .root = .{ .cwd_relative = srcdir },
        .files = &.{
            "rcore.c",
            "utils.c",
        },
        .flags = raylib_flags,
    });

    if (options.raudio) {
        raylib.addCSourceFiles(.{
            .root = .{ .cwd_relative = srcdir },
            .files = &.{
                "raudio.c",
            },
            .flags = raylib_flags,
        });
    }
    if (options.rmodels) {
        raylib.addCSourceFiles(.{
            .root = .{ .cwd_relative = srcdir },
            .files = &.{
                "rmodels.c",
            },
            .flags = &[_][]const u8{
                "-fno-sanitize=undefined", // https://github.com/raysan5/raylib/issues/1891
            } ++ raylib_flags,
        });
    }
    if (options.rshapes) {
        raylib.addCSourceFiles(.{
            .root = .{ .cwd_relative = srcdir },
            .files = &.{
                "rshapes.c",
            },
            .flags = raylib_flags,
        });
    }
    if (options.rtext) {
        raylib.addCSourceFiles(.{
            .root = .{ .cwd_relative = srcdir },
            .files = &.{
                "rtext.c",
            },
            .flags = raylib_flags,
        });
    }
    if (options.rtextures) {
        raylib.addCSourceFiles(.{
            .root = .{ .cwd_relative = srcdir },
            .files = &.{
                "rtextures.c",
            },
            .flags = raylib_flags,
        });
    }

    var gen_step = b.addWriteFiles();
    raylib.step.dependOn(&gen_step.step);

    if (options.raygui) {
        const raygui_c_path = gen_step.add("raygui.c", "#define RAYGUI_IMPLEMENTATION\n#include \"raygui.h\"\n");
        raylib.addCSourceFile(.{ .file = raygui_c_path, .flags = raylib_flags });
        raylib.addIncludePath(.{ .cwd_relative = srcdir });
        raylib.addIncludePath(.{ .cwd_relative = srcdir ++ "/../raygui" });
    }

    switch (target.result.os.tag) {
        .windows => {
            raylib.addCSourceFiles(.{
                .root = .{ .cwd_relative = srcdir },
                .files = &.{
                    "rglfw.c",
                },
                .flags = raylib_flags,
            });
            raylib.linkSystemLibrary("winmm");
            raylib.linkSystemLibrary("gdi32");
            raylib.linkSystemLibrary("opengl32");
            raylib.addIncludePath(.{ .cwd_relative = "external/glfw/deps/mingw" });

            raylib.defineCMacro("PLATFORM_DESKTOP", null);
        },
        .linux => {
            if (!options.platform_drm) {
                raylib.addCSourceFiles(.{
                    .root = .{ .cwd_relative = srcdir },
                    .files = &.{
                        "rglfw.c",
                    },
                    .flags = raylib_flags,
                });
                raylib.linkSystemLibrary("GL");
                raylib.linkSystemLibrary("rt");
                raylib.linkSystemLibrary("dl");
                raylib.linkSystemLibrary("m");
                raylib.linkSystemLibrary("X11");
                raylib.addLibraryPath(.{ .cwd_relative = "/usr/lib" });
                raylib.addIncludePath(.{ .cwd_relative = "/usr/include" });

                raylib.defineCMacro("PLATFORM_DESKTOP", null);
            } else {
                raylib.linkSystemLibrary("GLESv2");
                raylib.linkSystemLibrary("EGL");
                raylib.linkSystemLibrary("drm");
                raylib.linkSystemLibrary("gbm");
                raylib.linkSystemLibrary("pthread");
                raylib.linkSystemLibrary("rt");
                raylib.linkSystemLibrary("m");
                raylib.linkSystemLibrary("dl");
                raylib.addIncludePath(.{ .cwd_relative = "/usr/include/libdrm" });

                raylib.defineCMacro("PLATFORM_DRM", null);
                raylib.defineCMacro("GRAPHICS_API_OPENGL_ES2", null);
                raylib.defineCMacro("EGL_NO_X11", null);
                raylib.defineCMacro("DEFAULT_BATCH_BUFFER_ELEMENT", "2048");
            }
        },
        .freebsd, .openbsd, .netbsd, .dragonfly => {
            raylib.addCSourceFiles(.{
                .root = .{ .cwd_relative = srcdir },
                .files = &.{
                    "rglfw.c",
                },
                .flags = raylib_flags,
            });
            raylib.linkSystemLibrary("GL");
            raylib.linkSystemLibrary("rt");
            raylib.linkSystemLibrary("dl");
            raylib.linkSystemLibrary("m");
            raylib.linkSystemLibrary("X11");
            raylib.linkSystemLibrary("Xrandr");
            raylib.linkSystemLibrary("Xinerama");
            raylib.linkSystemLibrary("Xi");
            raylib.linkSystemLibrary("Xxf86vm");
            raylib.linkSystemLibrary("Xcursor");

            raylib.defineCMacro("PLATFORM_DESKTOP", null);
        },
        .macos => {
            // On macos rglfw.c include Objective-C files.
            const raylib_flags_extra_macos = &[_][]const u8{
                "-ObjC",
            };
            raylib.addCSourceFiles(.{
                .root = .{ .cwd_relative = srcdir },
                .files = &.{
                    "rglfw.c",
                },
                .flags = raylib_flags ++ raylib_flags_extra_macos,
            });
            raylib.linkFramework("Foundation");
            raylib.linkFramework("CoreServices");
            raylib.linkFramework("CoreGraphics");
            raylib.linkFramework("AppKit");
            raylib.linkFramework("IOKit");

            raylib.defineCMacro("PLATFORM_DESKTOP", null);
        },
        .emscripten => {
            raylib.defineCMacro("PLATFORM_WEB", null);
            raylib.defineCMacro("GRAPHICS_API_OPENGL_ES2", null);

            if (b.sysroot == null) {
                @panic("Pass '--sysroot \"$EMSDK/upstream/emscripten\"'");
            }

            const cache_include = std.fs.path.join(b.allocator, &.{ b.sysroot.?, "cache", "sysroot", "include" }) catch @panic("Out of memory");
            defer b.allocator.free(cache_include);

            var dir = std.fs.openDirAbsolute(cache_include, std.fs.Dir.OpenDirOptions{ .access_sub_paths = true, .no_follow = true }) catch @panic("No emscripten cache. Generate it!");
            dir.close();

            raylib.addIncludePath(.{ .cwd_relative = cache_include });
        },
        else => {
            @panic("Unsupported OS");
        },
    }

    return raylib;
}

pub const Options = struct {
    raudio: bool = true,
    rmodels: bool = true,
    rshapes: bool = true,
    rtext: bool = true,
    rtextures: bool = true,
    raygui: bool = false,
    platform_drm: bool = false,
};

const srcdir = struct {
    fn getSrcDir() []const u8 {
        return std.fs.path.dirname(@src().file).?;
    }
}.getSrcDir();
