project('tinygizmo', 'c', 'cpp')

glew_dep = dependency('glew', default_options: ['default_library=static'])

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

executable(
    'tiny-gizmo-example',
    [
        'examples/tiny-gizmo-example/example-main.cpp',
        'tinygizmo/tiny-gizmo.cpp',
    ],
    include_directories: include_directories(
        'tinygizmo',
    ),
    dependencies: [
        glew_dep,
        raylib_dep,
    ],
    cpp_args: ['/utf-8'],
    install: true,
)
