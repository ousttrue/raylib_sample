// Dear ImGui: standalone example application for Raylib with OpenGL
// (Raylib is a simple learning library for game development)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

const c = @cImport({
    @cDefine("CIMGUI_DEFINE_ENUMS_AND_STRUCTS", "");
    // @cDefine("IMGUI_DISABLE_OBSOLETE_FUNCTIONS", "1");
    @cInclude("cimgui.h");
    @cInclude("rlImGui.h");
    @cInclude("raylib.h");
});

// Main code
pub fn main() void {
    // Setup raylib window
    c.SetConfigFlags(c.FLAG_WINDOW_RESIZABLE | c.FLAG_WINDOW_HIGHDPI);
    c.InitWindow(1280, 720, "Dear ImGui Raylib(OpenGL) example");
    defer c.CloseWindow();

    // Setup Platform/Renderer backends
    // c.ImGui_ImplRaylib_Init();
    c.rlImGuiSetup(true);
    // defer c.ImGui_ImplRaylib_Shutdown();
    defer c.rlImGuiShutdown();

    // Setup Dear ImGui context
    // _ = c.igCreateContext(null);
    // defer c.igDestroyContext(null);
    const io = c.igGetIO();
    io.*.ConfigFlags |= c.ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.*.ConfigFlags |= c.ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Dear ImGui style
    c.igStyleColorsDark(null);
    //c.igStyleColorsLight();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use c.igPushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !

    // c++! cannot
    // io.*.Fonts.*.AddFontDefault();

    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // _ = io.*.Fonts.AddFontFromFileTTF(
    //     "resources/driusstraight.ttf",
    //     18.0,
    //     null,
    //     io.*.Fonts.GetGlyphRangesJapanese(),
    // );
    // IM_ASSERT(font != nullptr);

    //     // required to be called to cache the font texture with raylib
    //     Imgui_ImplRaylib_BuildFontAtlas();
    //
    // Our state
    var show_demo_window = true;
    //     bool show_another_window = false;
    const clear_color = c.ImVec4{ .x = 0.45, .y = 0.55, .z = 0.60, .w = 1.00 };

    // Main loop
    while (!c.WindowShouldClose()) {
        c.BeginDrawing();
        c.ClearBackground(.{
            .r = @intFromFloat(clear_color.x * 255),
            .g = @intFromFloat(clear_color.y * 255),
            .b = @intFromFloat(clear_color.z * 255),
            .a = @intFromFloat(clear_color.w * 255),
        });

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        c.rlImGuiBegin();

        // Start the Dear ImGui frame
        // c.igNewFrame();

        // 1. Show the big demo window (Most of the sample code is in c.igShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            c.igShowDemoWindow(&show_demo_window);

        //         // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        //         {
        //             static float f = 0.0f;
        //             static int counter = 0;
        //
        //             c.igBegin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
        //
        //             c.igText("This is some useful text.");               // Display some text (you can use a format strings too)
        //             c.igCheckbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        //             c.igCheckbox("Another Window", &show_another_window);
        //
        //             c.igSliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        //             c.igColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
        //
        //             if (c.igButton("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        //                 counter++;
        //             c.igSameLine();
        //             c.igText("counter = %d", counter);
        //
        //             c.igText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        //             c.igEnd();
        //         }
        //
        //         // 3. Show another simple window.
        //         if (show_another_window)
        //         {
        //             c.igBegin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        //             c.igText("Hello from another window!");
        //             if (c.igButton("Close Me"))
        //                 show_another_window = false;
        //             c.igEnd();
        //         }
        //

        // Rendering
        c.rlImGuiEnd();

        c.EndDrawing();
    }
}
