const c = @import("c.zig");

// pub const  Dragger = struct  {
//
//   virtual void begin(const Vector2 &cursor) { ; }
//
//   virtual void end(const Vector2 &end) {
//     //
//   }
//
//   virtual void drag(const DragState &state, int w, int h,
//                     const Vector2 &cursor) {}
// };

pub const DragState = struct {
    cursor_begin: c.Vector2 = .{ .x = 0, .y = 0 },
};

pub fn GenericDragger(comptime Dragger: type) type {
    return struct {
        draggable: Dragger,
        state: DragState = .{},
        _button: bool = false,

        pub fn process(self: *@This(), w: i32, h: i32, cursor: c.Vector2, button: bool) void {
            if (self._button != button) {
                if (button) {
                    self.state = .{
                        .cursor_begin = cursor,
                    };
                    self.draggable.begin(cursor);
                } else {
                    self.draggable.end(cursor);
                }
            } else {
                if (button) {
                    c.DrawText(
                        c.TextFormat("[%.0f, %.0f]", self.state.cursor_begin.x, self.state.cursor_begin.y),
                        @as(i32, @intFromFloat(self.state.cursor_begin.x)) + 10,
                        @intFromFloat(self.state.cursor_begin.y),
                        10,
                        c.BLACK,
                    );
                    c.DrawLine(
                        @intFromFloat(self.state.cursor_begin.x),
                        @intFromFloat(self.state.cursor_begin.y),
                        @intFromFloat(cursor.x),
                        @intFromFloat(cursor.y),
                        c.BLACK,
                    );
                    c.DrawText(
                        c.TextFormat("[%.0f, %.0f]", cursor.x, cursor.y),
                        @as(i32, @intFromFloat(cursor.x)) + 10,
                        @intFromFloat(cursor.y),
                        10,
                        c.BLACK,
                    );
                    self.draggable.drag(self.state, w, h, cursor);
                } else {
                    // nothing
                }
            }
            self._button = button;
        }
    };
}

pub fn make_dragger(dragger: anytype) GenericDragger(@TypeOf(dragger)) {
    return GenericDragger(@TypeOf(dragger)){
        .draggable = dragger,
    };
}
