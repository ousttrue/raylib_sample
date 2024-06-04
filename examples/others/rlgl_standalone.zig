// *******************************************************************************************
//
//    raylib [rlgl] example - Using rlgl module as standalone module
//
//    rlgl library is an abstraction layer for multiple OpenGL versions (1.1, 2.1, 3.3 Core, ES 2.0)
//    that provides a pseudo-OpenGL 1.1 immediate-mode style API (rlVertex, rlTranslate, rlRotate...)
//
//    WARNING: This example is intended only for PLATFORM_DESKTOP and OpenGL 3.3 Core profile.
//        It could work on other platforms if redesigned for those platforms (out-of-scope)
//
//    DEPENDENCIES:
//        glfw3     - Windows and context initialization library
//        rlgl.h    - OpenGL abstraction layer to OpenGL 1.1, 3.3 or ES2
//        glad.h    - OpenGL extensions initialization library (required by rlgl)
//        raymath.h - 3D math library
//
//    WINDOWS COMPILATION:
//        gcc -o rlgl_standalone.exe rlgl_standalone.c -s -Iexternal\include -I..\..\src  \
//            -L. -Lexternal\lib -lglfw3 -lopengl32 -lgdi32 -Wall -std=c99 -DGRAPHICS_API_OPENGL_33
//
//    APPLE COMPILATION:
//        gcc -o rlgl_standalone rlgl_standalone.c -I../../src -Iexternal/include -Lexternal/lib \
//            -lglfw3 -framework CoreVideo -framework OpenGL -framework IOKit -framework Cocoa
//            -Wno-deprecated-declarations -std=c99 -DGRAPHICS_API_OPENGL_33
//
//
//    LICENSE: zlib/libpng
//
//    This example is licensed under an unmodified zlib/libpng license, which is an OSI-certified,
//    BSD-like license that allows static linking with closed source software:
//
//    Copyright (c) 2014-2023 Ramon Santamaria (@raysan5)
//
//    This software is provided "as-is", without any express or implied warranty. In no event
//    will the authors be held liable for any damages arising from the use of this software.
//
//    Permission is granted to anyone to use this software for any purpose, including commercial
//    applications, and to alter it and redistribute it freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not claim that you
//      wrote the original software. If you use this software in a product, an acknowledgment
//      in the product documentation would be appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not be misrepresented
//      as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
// *******************************************************************************************

// // NOTE: rlgl can be configured just re-defining the following values:
// //#define RL_DEFAULT_BATCH_BUFFER_ELEMENTS   8192    // Default internal render batch elements limits
// //#define RL_DEFAULT_BATCH_BUFFERS              1    // Default number of batch buffers (multi-buffering)
// //#define RL_DEFAULT_BATCH_DRAWCALLS          256    // Default number of batch draw calls (by state changes: mode, texture)
// //#define RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS    4    // Maximum number of textures units that can be activated on batch drawing (SetShaderValueTexture())
// //#define RL_MAX_MATRIX_STACK_SIZE             32    // Maximum size of internal Matrix stack
// //#define RL_MAX_SHADER_LOCATIONS              32    // Maximum number of shader locations supported
// //#define RL_CULL_DISTANCE_NEAR              0.01    // Default projection matrix near cull distance
// //#define RL_CULL_DISTANCE_FAR             1000.0    // Default projection matrix far cull distance

const std = @import("std");
const rlgl = @cImport({
    // #define RLGL_IMPLEMENTATION
    @cInclude("rlgl.h"); // OpenGL abstraction layer to OpenGL 1.1, 3.3+ or ES2

    // #define RAYMATH_STATIC_INLINE
    @cInclude("raymath.h"); // Vector2, Vector3, Quaternion and Matrix functionality

    @cInclude("GLFW/glfw3.h"); // Windows/Context and inputs management

    // #include <stdio.h>              // Required for: printf()
});

const RED = Color{ .r = 230, .g = 41, .b = 55, .a = 255 }; // Red
const RAYWHITE = Color{ .r = 245, .g = 245, .b = 245, .a = 255 }; // My own White (raylib logo)
const DARKGRAY = Color{ .r = 80, .g = 80, .b = 80, .a = 255 }; // Dark Gray

//----------------------------------------------------------------------------------
// Structures Definition
//----------------------------------------------------------------------------------
// Color, 4 components, R8G8B8A8 (32bit)
const Color = struct {
    r: u8, // Color red value
    g: u8, // Color green value
    b: u8, // Color blue value
    a: u8, // Color alpha value
};

// Camera type, defines a camera position/orientation in 3d space
const Camera = struct {
    position: rlgl.Vector3, // Camera position
    target: rlgl.Vector3, // Camera target it looks-at
    up: rlgl.Vector3, // Camera up vector (rotation over its axis)
    fovy: f32, // Camera field-of-view apperture in Y (degrees) in perspective, used as near plane width in orthographic
    // projection: c_int, // Camera projection: CAMERA_PERSPECTIVE or CAMERA_ORTHOGRAPHIC
};

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
pub fn main() void {
    // Initialization
    //--------------------------------------------------------------------------------------
    const screenWidth = 800;
    const screenHeight = 450;

    // GLFW3 Initialization + OpenGL 3.3 Context + Extensions
    //--------------------------------------------------------
    _ = rlgl.glfwSetErrorCallback(ErrorCallback);

    if (rlgl.glfwInit() == 0) {
        _ = std.c.printf("GLFW3: Can not initialize GLFW\n");
        std.c.exit(1);
    } else {
        _ = std.c.printf("GLFW3: GLFW initialized successfully\n");
    }
    defer rlgl.glfwTerminate(); // Free GLFW3 resources

    rlgl.glfwWindowHint(rlgl.GLFW_SAMPLES, 4);
    rlgl.glfwWindowHint(rlgl.GLFW_DEPTH_BITS, 16);

    // WARNING: OpenGL 3.3 Core profile only
    rlgl.glfwWindowHint(rlgl.GLFW_CONTEXT_VERSION_MAJOR, 3);
    rlgl.glfwWindowHint(rlgl.GLFW_CONTEXT_VERSION_MINOR, 3);
    rlgl.glfwWindowHint(rlgl.GLFW_OPENGL_PROFILE, rlgl.GLFW_OPENGL_CORE_PROFILE);
    rlgl.glfwWindowHint(rlgl.GLFW_OPENGL_DEBUG_CONTEXT, rlgl.GL_TRUE);
    // #if defined(__APPLE__)
    //     glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    // #endif

    const window = rlgl.glfwCreateWindow(
        screenWidth,
        screenHeight,
        "rlgl standalone",
        null,
        null,
    );

    if (window == null) {
        rlgl.glfwTerminate();
        std.c.exit(2);
    } else {
        _ = std.c.printf("GLFW3: Window created successfully\n");
    }
    defer rlgl.glfwDestroyWindow(window); // Close window

    rlgl.glfwSetWindowPos(window, 200, 200);

    _ = rlgl.glfwSetKeyCallback(window, KeyCallback);

    rlgl.glfwMakeContextCurrent(window);
    rlgl.glfwSwapInterval(0);

    // Load OpenGL 3.3 supported extensions
    rlgl.rlLoadExtensions(@ptrCast(@constCast(&rlgl.glfwGetProcAddress)));
    //--------------------------------------------------------

    // Initialize OpenGL context (states and resources)
    rlgl.rlglInit(screenWidth, screenHeight);
    defer rlgl.rlglClose(); // Unload rlgl internal buffers and default shader/texture

    // Initialize viewport and internal projection/modelview matrices
    rlgl.rlViewport(0, 0, screenWidth, screenHeight);
    rlgl.rlMatrixMode(rlgl.RL_PROJECTION); // Switch to PROJECTION matrix
    rlgl.rlLoadIdentity(); // Reset current matrix (PROJECTION)
    rlgl.rlOrtho(0, screenWidth, screenHeight, 0, 0.0, 1.0); // Orthographic projection with top-left corner at (0,0)
    rlgl.rlMatrixMode(rlgl.RL_MODELVIEW); // Switch back to MODELVIEW matrix
    rlgl.rlLoadIdentity(); // Reset current matrix (MODELVIEW)

    rlgl.rlClearColor(245, 245, 245, 255); // Define clear color
    rlgl.rlEnableDepthTest(); // Enable DEPTH_TEST for 3D

    var camera = Camera{
        .position = .{ .x = 5.0, .y = 5.0, .z = 5.0 }, // Camera position
        .target = .{ .x = 0.0, .y = 0.0, .z = 0.0 }, // Camera looking at point
        .up = .{ .x = 0.0, .y = 1.0, .z = 0.0 }, // Camera up vector (rotation towards target)
        .fovy = 45.0, // Camera field-of-view Y
    };

    const cubePosition = rlgl.Vector3{ .x = 0.0, .y = 0.0, .z = 0.0 }; // Cube default position (center)
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (rlgl.glfwWindowShouldClose(window) == 0) {
        // Update
        //----------------------------------------------------------------------------------
        camera.position.x += 0.01;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        rlgl.rlClearScreenBuffers(); // Clear current framebuffer

        // Draw '3D' elements in the scene
        //-----------------------------------------------
        // Calculate projection matrix (from perspective) and view matrix from camera look at
        const matProj = rlgl.MatrixPerspective(
            camera.fovy * rlgl.DEG2RAD,
            @as(f32, @floatFromInt(screenWidth)) / @as(f32, @floatFromInt(screenHeight)),
            0.01,
            1000.0,
        );
        const matView = rlgl.MatrixLookAt(camera.position, camera.target, camera.up);

        rlgl.rlSetMatrixModelview(matView); // Set internal modelview matrix (default shader)
        rlgl.rlSetMatrixProjection(matProj); // Set internal projection matrix (default shader)

        DrawCube(cubePosition, 2.0, 2.0, 2.0, RED);
        DrawCubeWires(cubePosition, 2.0, 2.0, 2.0, RAYWHITE);
        DrawGrid(10, 1.0);

        // Draw internal render batch buffers (3D data)
        rlgl.rlDrawRenderBatchActive();
        //-----------------------------------------------

        // Draw '2D' elements in the scene (GUI)
        //-----------------------------------------------
        // #define RLGL_SET_MATRIX_MANUALLY
        // #if defined(RLGL_SET_MATRIX_MANUALLY)
        //             matProj = MatrixOrtho(0.0, screenWidth, screenHeight, 0.0, 0.0, 1.0);
        //             matView = MatrixIdentity();
        //
        //             rlSetMatrixModelview(matView);    // Set internal modelview matrix (default shader)
        //             rlSetMatrixProjection(matProj);   // Set internal projection matrix (default shader)
        //
        // #else   // Let rlgl generate and multiply matrix internally

        rlgl.rlMatrixMode(rlgl.RL_PROJECTION); // Enable internal projection matrix
        rlgl.rlLoadIdentity(); // Reset internal projection matrix
        rlgl.rlOrtho(0.0, screenWidth, screenHeight, 0.0, 0.0, 1.0); // Recalculate internal projection matrix
        rlgl.rlMatrixMode(rlgl.RL_MODELVIEW); // Enable internal modelview matrix
        rlgl.rlLoadIdentity(); // Reset internal modelview matrix

        // #endif
        DrawRectangleV(.{ .x = 10.0, .y = 10.0 }, .{ .x = 780.0, .y = 20.0 }, DARKGRAY);

        // Draw internal render batch buffers (2D data)
        rlgl.rlDrawRenderBatchActive();
        //-----------------------------------------------

        rlgl.glfwSwapBuffers(window);
        rlgl.glfwPollEvents();
        //----------------------------------------------------------------------------------
    }
}

//----------------------------------------------------------------------------------
// Module specific Functions Definitions
//----------------------------------------------------------------------------------

// GLFW3: Error callback
fn ErrorCallback(_: c_int, description: [*c]const u8) callconv(.C) void {
    _ = std.c.printf("%s", description);
}

// GLFW3: Keyboard callback
fn KeyCallback(window: ?*rlgl.GLFWwindow, key: c_int, scancode: c_int, action: c_int, mods: c_int) callconv(.C) void {
    _ = scancode;
    _ = mods;
    if (key == rlgl.GLFW_KEY_ESCAPE and action == rlgl.GLFW_PRESS) {
        rlgl.glfwSetWindowShouldClose(window, rlgl.GL_TRUE);
    }
}

// Draw rectangle using rlgl OpenGL 1.1 style coding (translated to OpenGL 3.3 internally)
fn DrawRectangleV(position: rlgl.Vector2, size: rlgl.Vector2, color: Color) void {
    rlgl.rlBegin(rlgl.RL_TRIANGLES);
    rlgl.rlColor4ub(color.r, color.g, color.b, color.a);

    rlgl.rlVertex2f(position.x, position.y);
    rlgl.rlVertex2f(position.x, position.y + size.y);
    rlgl.rlVertex2f(position.x + size.x, position.y + size.y);

    rlgl.rlVertex2f(position.x, position.y);
    rlgl.rlVertex2f(position.x + size.x, position.y + size.y);
    rlgl.rlVertex2f(position.x + size.x, position.y);
    rlgl.rlEnd();
}

// Draw a grid centered at (0, 0, 0)
fn DrawGrid(slices: i32, spacing: f32) void {
    const halfSlices: i32 = @divTrunc(slices, 2);

    rlgl.rlBegin(rlgl.RL_LINES);
    var i = halfSlices;
    while (i <= halfSlices) : (i += 1) {
        if (i == 0) {
            rlgl.rlColor3f(0.5, 0.5, 0.5);
            rlgl.rlColor3f(0.5, 0.5, 0.5);
            rlgl.rlColor3f(0.5, 0.5, 0.5);
            rlgl.rlColor3f(0.5, 0.5, 0.5);
        } else {
            rlgl.rlColor3f(0.75, 0.75, 0.75);
            rlgl.rlColor3f(0.75, 0.75, 0.75);
            rlgl.rlColor3f(0.75, 0.75, 0.75);
            rlgl.rlColor3f(0.75, 0.75, 0.75);
        }

        rlgl.rlVertex3f(@as(f32, @floatFromInt(i)) * spacing, 0.0, @as(f32, @floatFromInt(-halfSlices)) * spacing);
        rlgl.rlVertex3f(@as(f32, @floatFromInt(i)) * spacing, 0.0, @as(f32, @floatFromInt(halfSlices)) * spacing);

        rlgl.rlVertex3f(@as(f32, @floatFromInt(-halfSlices)) * spacing, 0.0, @as(f32, @floatFromInt(i)) * spacing);
        rlgl.rlVertex3f(@as(f32, @floatFromInt(halfSlices)) * spacing, 0.0, @as(f32, @floatFromInt(i)) * spacing);
    }
    rlgl.rlEnd();
}

// Draw cube
// NOTE: Cube position is the center position
fn DrawCube(position: rlgl.Vector3, width: f32, height: f32, length: f32, color: Color) void {
    const x = 0.0;
    const y = 0.0;
    const z = 0.0;

    rlgl.rlPushMatrix();

    // NOTE: Be careful! Function order matters (rotate -> scale -> translate)
    rlgl.rlTranslatef(position.x, position.y, position.z);
    rlgl.rlScalef(2.0, 2.0, 2.0);
    rlgl.rlRotatef(45, 0, 1, 0);

    rlgl.rlBegin(rlgl.RL_TRIANGLES);
    rlgl.rlColor4ub(color.r, color.g, color.b, color.a);

    // Front Face -----------------------------------------------------
    rlgl.rlVertex3f(x - width / 2, y - height / 2, z + length / 2); // Bottom Left
    rlgl.rlVertex3f(x + width / 2, y - height / 2, z + length / 2); // Bottom Right
    rlgl.rlVertex3f(x - width / 2, y + height / 2, z + length / 2); // Top Left

    rlgl.rlVertex3f(x + width / 2, y + height / 2, z + length / 2); // Top Right
    rlgl.rlVertex3f(x - width / 2, y + height / 2, z + length / 2); // Top Left
    rlgl.rlVertex3f(x + width / 2, y - height / 2, z + length / 2); // Bottom Right

    // Back Face ------------------------------------------------------
    rlgl.rlVertex3f(x - width / 2, y - height / 2, z - length / 2); // Bottom Left
    rlgl.rlVertex3f(x - width / 2, y + height / 2, z - length / 2); // Top Left
    rlgl.rlVertex3f(x + width / 2, y - height / 2, z - length / 2); // Bottom Right

    rlgl.rlVertex3f(x + width / 2, y + height / 2, z - length / 2); // Top Right
    rlgl.rlVertex3f(x + width / 2, y - height / 2, z - length / 2); // Bottom Right
    rlgl.rlVertex3f(x - width / 2, y + height / 2, z - length / 2); // Top Left

    // Top Face -------------------------------------------------------
    rlgl.rlVertex3f(x - width / 2, y + height / 2, z - length / 2); // Top Left
    rlgl.rlVertex3f(x - width / 2, y + height / 2, z + length / 2); // Bottom Left
    rlgl.rlVertex3f(x + width / 2, y + height / 2, z + length / 2); // Bottom Right

    rlgl.rlVertex3f(x + width / 2, y + height / 2, z - length / 2); // Top Right
    rlgl.rlVertex3f(x - width / 2, y + height / 2, z - length / 2); // Top Left
    rlgl.rlVertex3f(x + width / 2, y + height / 2, z + length / 2); // Bottom Right

    // Bottom Face ----------------------------------------------------
    rlgl.rlVertex3f(x - width / 2, y - height / 2, z - length / 2); // Top Left
    rlgl.rlVertex3f(x + width / 2, y - height / 2, z + length / 2); // Bottom Right
    rlgl.rlVertex3f(x - width / 2, y - height / 2, z + length / 2); // Bottom Left

    rlgl.rlVertex3f(x + width / 2, y - height / 2, z - length / 2); // Top Right
    rlgl.rlVertex3f(x + width / 2, y - height / 2, z + length / 2); // Bottom Right
    rlgl.rlVertex3f(x - width / 2, y - height / 2, z - length / 2); // Top Left

    // Right face -----------------------------------------------------
    rlgl.rlVertex3f(x + width / 2, y - height / 2, z - length / 2); // Bottom Right
    rlgl.rlVertex3f(x + width / 2, y + height / 2, z - length / 2); // Top Right
    rlgl.rlVertex3f(x + width / 2, y + height / 2, z + length / 2); // Top Left

    rlgl.rlVertex3f(x + width / 2, y - height / 2, z + length / 2); // Bottom Left
    rlgl.rlVertex3f(x + width / 2, y - height / 2, z - length / 2); // Bottom Right
    rlgl.rlVertex3f(x + width / 2, y + height / 2, z + length / 2); // Top Left

    // Left Face ------------------------------------------------------
    rlgl.rlVertex3f(x - width / 2, y - height / 2, z - length / 2); // Bottom Right
    rlgl.rlVertex3f(x - width / 2, y + height / 2, z + length / 2); // Top Left
    rlgl.rlVertex3f(x - width / 2, y + height / 2, z - length / 2); // Top Right

    rlgl.rlVertex3f(x - width / 2, y - height / 2, z + length / 2); // Bottom Left
    rlgl.rlVertex3f(x - width / 2, y + height / 2, z + length / 2); // Top Left
    rlgl.rlVertex3f(x - width / 2, y - height / 2, z - length / 2); // Bottom Right
    rlgl.rlEnd();
    rlgl.rlPopMatrix();
}

// Draw cube wires
fn DrawCubeWires(position: rlgl.Vector3, width: f32, height: f32, length: f32, color: Color) void {
    const x = 0.0;
    const y = 0.0;
    const z = 0.0;

    rlgl.rlPushMatrix();

    rlgl.rlTranslatef(position.x, position.y, position.z);
    //rlRotatef(45, 0, 1, 0);

    rlgl.rlBegin(rlgl.RL_LINES);
    rlgl.rlColor4ub(color.r, color.g, color.b, color.a);

    // Front Face -----------------------------------------------------
    // Bottom Line
    rlgl.rlVertex3f(x - width / 2, y - height / 2, z + length / 2); // Bottom Left
    rlgl.rlVertex3f(x + width / 2, y - height / 2, z + length / 2); // Bottom Right

    // Left Line
    rlgl.rlVertex3f(x + width / 2, y - height / 2, z + length / 2); // Bottom Right
    rlgl.rlVertex3f(x + width / 2, y + height / 2, z + length / 2); // Top Right

    // Top Line
    rlgl.rlVertex3f(x + width / 2, y + height / 2, z + length / 2); // Top Right
    rlgl.rlVertex3f(x - width / 2, y + height / 2, z + length / 2); // Top Left

    // Right Line
    rlgl.rlVertex3f(x - width / 2, y + height / 2, z + length / 2); // Top Left
    rlgl.rlVertex3f(x - width / 2, y - height / 2, z + length / 2); // Bottom Left

    // Back Face ------------------------------------------------------
    // Bottom Line
    rlgl.rlVertex3f(x - width / 2, y - height / 2, z - length / 2); // Bottom Left
    rlgl.rlVertex3f(x + width / 2, y - height / 2, z - length / 2); // Bottom Right

    // Left Line
    rlgl.rlVertex3f(x + width / 2, y - height / 2, z - length / 2); // Bottom Right
    rlgl.rlVertex3f(x + width / 2, y + height / 2, z - length / 2); // Top Right

    // Top Line
    rlgl.rlVertex3f(x + width / 2, y + height / 2, z - length / 2); // Top Right
    rlgl.rlVertex3f(x - width / 2, y + height / 2, z - length / 2); // Top Left

    // Right Line
    rlgl.rlVertex3f(x - width / 2, y + height / 2, z - length / 2); // Top Left
    rlgl.rlVertex3f(x - width / 2, y - height / 2, z - length / 2); // Bottom Left

    // Top Face -------------------------------------------------------
    // Left Line
    rlgl.rlVertex3f(x - width / 2, y + height / 2, z + length / 2); // Top Left Front
    rlgl.rlVertex3f(x - width / 2, y + height / 2, z - length / 2); // Top Left Back

    // Right Line
    rlgl.rlVertex3f(x + width / 2, y + height / 2, z + length / 2); // Top Right Front
    rlgl.rlVertex3f(x + width / 2, y + height / 2, z - length / 2); // Top Right Back

    // Bottom Face  ---------------------------------------------------
    // Left Line
    rlgl.rlVertex3f(x - width / 2, y - height / 2, z + length / 2); // Top Left Front
    rlgl.rlVertex3f(x - width / 2, y - height / 2, z - length / 2); // Top Left Back

    // Right Line
    rlgl.rlVertex3f(x + width / 2, y - height / 2, z + length / 2); // Top Right Front
    rlgl.rlVertex3f(x + width / 2, y - height / 2, z - length / 2); // Top Right Back
    rlgl.rlEnd();
    rlgl.rlPopMatrix();
}
