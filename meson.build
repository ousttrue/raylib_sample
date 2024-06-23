project('tinygizmo', 'c', 'cpp', default_options: ['cpp_std=c++23'])

cc = meson.get_compiler('c')
winmm_lib = cc.find_library('winmm', required: true)
gdi32_lib = cc.find_library('gdi32', required: true)
opengl32_lib = cc.find_library('opengl32', required: true)

raylib_inc = include_directories('raylib')
raylib_lib = static_library(
    'raylib',
    [
        'raylib/rcore.c',
        'raylib/utils.c',
        'raylib/rglfw.c',
        'raylib/rtextures.c',
        'raylib/rshapes.c',
        'raylib/rtext.c',
        'raylib/rmodels.c',
    ],
    include_directories: include_directories(
        'raylib/external/glfw/include',
    ),
    c_args: [
        '-DPLATFORM_DESKTOP',
    ],
    dependencies: [winmm_lib, gdi32_lib, opengl32_lib],
)
raylib_dep = declare_dependency(
    include_directories: raylib_inc,
    link_with: raylib_lib,
)

args = []
if cc.get_id() == 'msvc'
    args += '/utf-8'
else
    args += '-Wno-macro-redefined'
endif

executable(
    'tiny-gizmo-example',
    [
        'examples/tiny-gizmo-example/main.cpp',
        'examples/tiny-gizmo-example/drawable.cpp',
        'examples/tiny-gizmo-example/gizmo_dragger.cpp',
        'tinygizmo/tinygizmo_translation.cpp',
        'tinygizmo/tinygizmo_rotation.cpp',
        'tinygizmo/tinygizmo_scaling.cpp',
    ],
    include_directories: include_directories(
        'tinygizmo',
    ),
    dependencies: [
        raylib_dep,
    ],
    cpp_args: args,
    install: true,
)

executable(
    'hello',
    'hello.cpp',
    install: true,
)
