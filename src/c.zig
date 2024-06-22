pub usingnamespace @cImport({
    @cInclude("raylib.h");
    @cInclude("rlgl.h");
    // @cInclude("rcamera.h");
    // @cInclude("raymath.h");
    @cDefine("CIMGUI_DEFINE_ENUMS_AND_STRUCTS", "");
    @cDefine("IMGUI_DISABLE_OBSOLETE_FUNCTIONS", "1");
    @cInclude("cimgui.h");
    @cInclude("rlImGui.h");
});
