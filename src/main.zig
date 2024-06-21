const std = @import("std");

const zamath = @import("zamath.zig");
const layout = @import("layout.zig");
const c = layout.c;
pub extern fn Custom_ButtonBehaviorMiddleRight() void;

fn tor(v: zamath.Vec3) c.Vector3 {
    return .{
        .x = v.x,
        .y = v.y,
        .z = v.z,
    };
}

const CursorState = enum {
    None,
    // mouse up / cursor on widget
    Hover,
    // mouse down / dragging
    Active,
};

fn draw_frustum(frustum: zamath.Frustum) void {
    c.DrawLine3D(tor(frustum.near_left_top), tor(frustum.near_left_bottom), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.near_left_bottom), tor(frustum.near_right_bottom), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.near_right_bottom), tor(frustum.near_right_top), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.near_right_top), tor(frustum.near_left_top), c.DARKBLUE);

    c.DrawLine3D(tor(frustum.near_left_top), tor(frustum.far_left_top), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.near_left_bottom), tor(frustum.far_left_bottom), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.near_right_bottom), tor(frustum.far_right_bottom), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.near_right_top), tor(frustum.far_right_top), c.DARKBLUE);

    c.DrawLine3D(tor(frustum.far_left_top), tor(frustum.far_left_bottom), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.far_left_bottom), tor(frustum.far_right_bottom), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.far_right_bottom), tor(frustum.far_right_top), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.far_right_top), tor(frustum.far_left_top), c.DARKBLUE);
}

const Scene = struct {
    cubePosition: layout.c.Vector3 = .{ .x = 0.0, .y = 0.0, .z = 0.0 },

    pub fn draw(self: @This()) void {
        // local scene
        c.DrawCube(self.cubePosition, 2.0, 2.0, 2.0, c.RED);
        c.DrawCubeWires(self.cubePosition, 2.0, 2.0, 2.0, c.MAROON);

        c.DrawGrid(10, 1.0);
    }
};

const View = struct {
    camera: zamath.Camera = .{},
    camera_projection: zamath.CameraProjection = .{},
    camera_orbit: zamath.CameraOrbit = .{},

    pub fn frustum(self: @This()) zamath.Frustum {
        return zamath.Frustum.make(
            self.camera.transform_matrix,
            self.camera_projection.fovy,
            self.camera.aspect(),
            self.camera_projection.z_near,
            self.camera_projection.z_far,
        );
    }

    pub fn update_projection_matrix(self: *@This()) void {
        self.camera.projection_matrix = self.camera_projection.calc_matrix(self.camera.viewport);
    }

    pub fn update_view_matrix(self: *@This()) void {
        self.camera.view_matrix, self.camera.transform_matrix = self.camera_orbit.calc_matrix();
    }

    pub fn update(
        self: *@This(),
        cursor_delta_x: f32,
        cursor_delta_y: f32,
        h: f32,
    ) void {
        var active = false;
        if (c.IsMouseButtonDown(c.MOUSE_BUTTON_RIGHT)) {
            self.camera_orbit.yawpitch(cursor_delta_x, cursor_delta_y);
            active = true;
        }
        if (c.IsMouseButtonDown(c.MOUSE_BUTTON_MIDDLE)) {
            self.camera_orbit.shift(
                cursor_delta_x,
                cursor_delta_y,
                h,
                self.camera_projection.fovy,
            );
            active = true;
        }
        if (active) {
            self.update_view_matrix();
        }
    }

    pub fn begin_camera3D(self: *const @This()) void {
        c.rlDrawRenderBatchActive(); // Update and draw internal render batch

        {
            c.rlMatrixMode(c.RL_PROJECTION);
            c.rlPushMatrix();
            c.rlLoadIdentity();
            c.rlMultMatrixf(&self.camera.projection_matrix.m00);
        }
        {
            c.rlMatrixMode(c.RL_MODELVIEW);
            c.rlLoadIdentity();
            c.rlMultMatrixf(&self.camera.view_matrix.m00);
        }

        c.rlEnableDepthTest(); // Enable DEPTH_TEST for 3D
    }

    pub fn end_camera3D(_: @This()) void {
        c.EndMode3D();
    }

    pub fn mouse_near_far(self: @This()) struct { zamath.Vec3, zamath.Vec3 } {
        const x = self.camera.transform_matrix.right();
        const y = self.camera.transform_matrix.up();
        const z = self.camera.transform_matrix.forward();
        const t = self.camera.transform_matrix.translation();
        const tan = std.math.tan(self.camera_projection.fovy / 2);
        // const near = self.projection.z_near;
        const far = self.camera_projection.z_far;
        const aspect = self.camera.viewport.width / self.camera.viewport.height;

        const half_width = self.camera.viewport.width / 2;
        const half_height = self.camera.viewport.height / 2;

        const mouse_h = (self.camera.cursor_x - half_width) / half_width;
        const mouse_v = -(self.camera.cursor_y - half_height) / half_height;
        const horizontal = tan * aspect * mouse_h;
        const vertical = tan * mouse_v;
        // const p0 = t.add(z.scale(near).add(x.scale(near * horizontal)).add(y.scale(near * vertical)));
        const p1 = t.add(z.scale(far).add(x.scale(far * horizontal)).add(y.scale(far * vertical)));

        return .{ t, p1 };
    }
};

pub fn main() !void {
    c.SetConfigFlags(c.FLAG_VSYNC_HINT | c.FLAG_WINDOW_RESIZABLE);
    c.InitWindow(1600, 1200, "experiment");
    defer c.CloseWindow();

    c.SetTargetFPS(60);
    c.rlImGuiSetup(true);
    defer c.rlImGuiShutdown();
    const io = c.igGetIO();

    // Custom timming variables
    var previousTime: f64 = c.GetTime(); // Previous time measure
    var currentTime: f64 = 0.0; // Current time measure
    var updateDrawTime: f64 = 0.0; // Update + Draw time
    var deltaTime: f64 = 0.0; // Frame time (Update + Draw + Wait time)

    var scene: Scene = .{};
    var root_view: View = .{};
    var rendertarget: layout.RenderTarget = .{};
    var fbo_view: View = .{};
    root_view.update_view_matrix();
    fbo_view.update_view_matrix();

    while (!c.WindowShouldClose()) {
        const w: f32 = @floatFromInt(c.GetScreenWidth());
        const h: f32 = @floatFromInt(c.GetScreenHeight());
        const cursor = c.GetMousePosition();
        if (root_view.camera.set_viewport_cursor(
            0,
            0,
            w,
            h,
            cursor.x,
            cursor.y,
        )) {
            root_view.update_projection_matrix();
        }

        const cursor_delta = c.GetMouseDelta();
        const wheel = c.GetMouseWheelMoveV();

        if (!io.*.WantCaptureMouse) {
            root_view.update(
                cursor_delta.x,
                cursor_delta.y,
                h,
            );
            if (wheel.y != 0) {
                root_view.camera_orbit.dolly(wheel.y);
                root_view.update_view_matrix();
            }
        }

        // show ImGui Content
        {
            c.rlImGuiBegin();

            var open = true;
            c.igShowDemoWindow(&open);

            {
                c.igPushStyleVar_Vec2(c.ImGuiStyleVar_WindowPadding, .{ .x = 0, .y = 0 });
                defer c.igPopStyleVar(1);
                var open_fbo = true;
                if (c.igBegin("fbo", &open_fbo, c.ImGuiWindowFlags_NoScrollbar | c.ImGuiWindowFlags_NoScrollWithMouse)) {
                    c.igPushStyleVar_Vec2(c.ImGuiStyleVar_FramePadding, .{ .x = 0, .y = 0 });
                    defer c.igPopStyleVar(1);
                    var pos = c.ImVec2{};
                    c.igGetCursorScreenPos(@ptrCast(&pos));
                    var size = c.ImVec2{};
                    c.igGetContentRegionAvail(@ptrCast(&size));

                    if (size.x > 0 and size.y > 0) {
                        if (fbo_view.camera.set_viewport_cursor(
                            pos.x,
                            pos.y,
                            size.x,
                            size.y,
                            cursor.x,
                            cursor.y,
                        )) {
                            fbo_view.update_projection_matrix();
                        }

                        if (rendertarget.begin(@intFromFloat(size.x), @intFromFloat(size.y))) |texture| {
                            _ = c.igImageButton(
                                "fbo",
                                @constCast(@ptrCast(&texture)),
                                size,
                                .{ .x = 0, .y = 1 },
                                .{ .x = 1, .y = 0 },
                                .{ .x = 1, .y = 1, .z = 1, .w = 1 },
                                .{ .x = 1, .y = 1, .z = 1, .w = 1 },
                            );
                            Custom_ButtonBehaviorMiddleRight();

                            if (c.igIsItemActive()) {
                                fbo_view.update(
                                    cursor_delta.x,
                                    cursor_delta.y,
                                    size.y,
                                );
                            } else if (c.igIsItemHovered(0)) {
                                if (wheel.y != 0) {
                                    fbo_view.camera_orbit.dolly(wheel.y);
                                    fbo_view.update_view_matrix();
                                }
                            }

                            {
                                fbo_view.begin_camera3D();
                                defer fbo_view.end_camera3D();

                                draw_frustum(root_view.frustum());
                                const start, const end = root_view.mouse_near_far();
                                c.DrawLine3D(tor(start), tor(end), c.YELLOW);
                                scene.draw();
                            }

                            rendertarget.end();
                        }
                    }
                }
                c.igEnd();
            }

            {
                _ = c.igBegin("root view", null, 0);
                defer c.igEnd();

                var start, var end = root_view.mouse_near_far();
                _ = c.igInputFloat3("start", &start.x, "%.3f", 0);
                _ = c.igInputFloat3("end", &end.x, "%.3f", 0);

                //   ImGui::ColorEdit3("clear color", ClearColor);

                // camera
                //   ImGui::Separator();
                //   ImGui::TextUnformatted("[camera]");
                //   ImGui::DragFloat("camera near", &Camera.Projection.NearZ);
                //   Camera.Projection.NearZ = std::max(
                //       0.01f, std::min(Camera.Projection.NearZ, Camera.Projection.FarZ - 1));
                //   ImGui::DragFloat("camera far", &Camera.Projection.FarZ);
                //   ImGui::InputFloat3("camera pos", &Camera.Transform.Translation.x);
                //   Camera.Projection.FarZ =
                //       std::max(Camera.Projection.FarZ, Camera.Projection.NearZ + 1);
                //
                //   if (auto ray = Gui.m_context.Ray) {
                //     ImGui::BeginDisabled(false);
                //     ImGui::InputFloat3("ray dir", &ray->Direction.x);
                //   } else {
                //     float zero[3]{0, 0, 0};
                //     ImGui::InputFloat3("ray dir", zero);
                //     ImGui::BeginDisabled(true);
                //   }
                //
                //   ImGui::TextUnformatted("ray hits");
                //   for (auto hit : Gui.m_hits) {
                //     ImGui::Text("hit: %0.3f", hit);
                //   }
                //
                //   ImGui::EndDisabled();
                // }
            }
        }

        {
            c.BeginDrawing();
            c.ClearBackground(c.RAYWHITE);

            {
                root_view.begin_camera3D();
                defer root_view.end_camera3D();

                scene.draw();
            }

            c.DrawText(c.TextFormat("CURRENT FPS: %.0f", (1.0 / deltaTime)), c.GetScreenWidth() - 220, 40, 20, c.GREEN);

            // end ImGui Content
            c.rlImGuiEnd();

            c.EndDrawing();
        }

        currentTime = c.GetTime();
        updateDrawTime = currentTime - previousTime;

        deltaTime = updateDrawTime; // Framerate could be variable
        previousTime = currentTime;
    }
}
