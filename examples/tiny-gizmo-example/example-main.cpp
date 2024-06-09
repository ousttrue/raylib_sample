// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#include "teapot.h"
#include "tiny-gizmo.hpp"
#include <GL/glew.h>
#include <chrono>
#include <iostream>
#include <linalg.h>
#include <map>
#include <raylib.h>

static inline uint64_t get_local_time_ns() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::high_resolution_clock::now().time_since_epoch())
      .count();
}

const linalg::aliases::float4x4 identity4x4 = {
    {1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

constexpr const char gizmo_vert[] = R"(#version 330
    layout(location = 0) in vec3 vertex;
    layout(location = 1) in vec3 normal;
    layout(location = 2) in vec4 color;
    out vec4 v_color;
    out vec3 v_world, v_normal;
    uniform mat4 u_mvp;
    void main()
    {
        gl_Position = u_mvp * vec4(vertex.xyz, 1);
        v_color = color;
        v_world = vertex;
        v_normal = normal;
    }
)";

constexpr const char gizmo_frag[] = R"(#version 330
    in vec4 v_color;
    in vec3 v_world, v_normal;
    out vec4 f_color;
    uniform vec3 u_eye;
    void main()
    {
        vec3 light = vec3(1) * max(dot(v_normal, normalize(u_eye - v_world)), 0.50) + 0.25;
        f_color = v_color * vec4(light, 1);
    }
)";

constexpr const char lit_vert[] = R"(#version 330
    uniform mat4 u_modelMatrix;
    uniform mat4 u_viewProj;

    layout(location = 0) in vec3 inPosition;
    layout(location = 1) in vec3 inNormal;

    out vec3 v_position, v_normal;

    void main()
    {
        vec4 worldPos = u_modelMatrix * vec4(inPosition, 1);
        v_position = worldPos.xyz;
        v_normal = normalize((u_modelMatrix * vec4(inNormal,0)).xyz);
        gl_Position = u_viewProj * worldPos;
    }
)";

constexpr const char lit_frag[] = R"(#version 330
    uniform vec3 u_diffuse = vec3(1, 1, 1);
    uniform vec3 u_eye;

    in vec3 v_position;
    in vec3 v_normal;

    out vec4 f_color;
    
    vec3 compute_lighting(vec3 eyeDir, vec3 position, vec3 color)
    {
        vec3 light = vec3(0, 0, 0);
        vec3 lightDir = normalize(position - v_position);
        light += color * u_diffuse * max(dot(v_normal, lightDir), 0);
        vec3 halfDir = normalize(lightDir + eyeDir);
        light += color * u_diffuse * pow(max(dot(v_normal, halfDir), 0), 128);
        return light;
    }

    void main()
    {
        vec3 eyeDir = vec3(0, 1, -2);
        vec3 light = vec3(0, 0, 0);
        light += compute_lighting(eyeDir, vec3(+3, 1, 0), vec3(235.0/255.0, 43.0/255.0, 211.0/255.0));
        light += compute_lighting(eyeDir, vec3(-3, 1, 0), vec3(43.0/255.0, 236.0/255.0, 234.0/255.0));
        f_color = vec4(light + vec3(0.5, 0.5, 0.5), 1.0);
    }
)";

//////////////////////////
//   Main Application   //
//////////////////////////

tinygizmo::geometry_mesh make_teapot() {
  tinygizmo::geometry_mesh mesh;
  for (int i = 0; i < 4974; i += 6) {
    tinygizmo::geometry_vertex v;
    v.position = minalg::float3(teapot_vertices[i + 0], teapot_vertices[i + 1],
                                teapot_vertices[i + 2]);
    v.normal = minalg::float3(teapot_vertices[i + 3], teapot_vertices[i + 4],
                              teapot_vertices[i + 5]);
    mesh.vertices.push_back(v);
  }
  for (int i = 0; i < 4680; i += 3)
    mesh.triangles.push_back(minalg::uint3(teapot_triangles[i + 0],
                                           teapot_triangles[i + 1],
                                           teapot_triangles[i + 2]));
  return mesh;
}

//////////////////
//   GlShader   //
//////////////////
static void compile_shader(GLuint program, GLenum type, const char *source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);

  GLint status, length;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

  if (status == GL_FALSE) {
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    std::vector<GLchar> buffer(length);
    glGetShaderInfoLog(shader, (GLsizei)buffer.size(), nullptr, buffer.data());
    glDeleteShader(shader);
    std::cerr << "GL Compile Error: " << buffer.data() << std::endl;
    std::cerr << "Source: " << source << std::endl;
    throw std::runtime_error("GLSL Compile Failure");
  }

  glAttachShader(program, shader);
  glDeleteShader(shader);
}

class GlShader {
  GLuint program;
  bool enabled = false;

protected:
  GlShader(const GlShader &r) = delete;
  GlShader &operator=(const GlShader &r) = delete;

public:
  GlShader() : program() {}

  GlShader(const GLuint type, const std::string &src) {
    program = glCreateProgram();

    compile_shader(program, type, src.c_str());
    glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);

    glLinkProgram(program);

    GLint status, length;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (status == GL_FALSE) {
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
      std::vector<GLchar> buffer(length);
      glGetProgramInfoLog(program, (GLsizei)buffer.size(), nullptr,
                          buffer.data());
      std::cerr << "GL Link Error: " << buffer.data() << std::endl;
      throw std::runtime_error("GLSL Link Failure");
    }
  }

  GlShader(const std::string &vert, const std::string &frag,
           const std::string &geom = "") {
    program = glCreateProgram();

    glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_FALSE);

    ::compile_shader(program, GL_VERTEX_SHADER, vert.c_str());
    ::compile_shader(program, GL_FRAGMENT_SHADER, frag.c_str());

    if (geom.length() != 0)
      ::compile_shader(program, GL_GEOMETRY_SHADER, geom.c_str());

    glLinkProgram(program);

    GLint status, length;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (status == GL_FALSE) {
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
      std::vector<GLchar> buffer(length);
      glGetProgramInfoLog(program, (GLsizei)buffer.size(), nullptr,
                          buffer.data());
      std::cerr << "GL Link Error: " << buffer.data() << std::endl;
      throw std::runtime_error("GLSL Link Failure");
    }
  }

  ~GlShader() {
    if (program)
      glDeleteProgram(program);
  }

  GlShader(GlShader &&r) : GlShader() { *this = std::move(r); }

  GLuint handle() const { return program; }
  GLint get_uniform_location(const std::string &name) const {
    return glGetUniformLocation(program, name.c_str());
  }

  GlShader &operator=(GlShader &&r) {
    std::swap(program, r.program);
    return *this;
  }

  std::map<uint32_t, std::string> reflect() {
    std::map<uint32_t, std::string> locations;
    GLint count;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);
    for (GLuint i = 0; i < static_cast<GLuint>(count); ++i) {
      char buffer[1024];
      GLenum type;
      GLsizei length;
      GLint size, block_index;
      glGetActiveUniform(program, i, sizeof(buffer), &length, &size, &type,
                         buffer);
      glGetActiveUniformsiv(program, 1, &i, GL_UNIFORM_BLOCK_INDEX,
                            &block_index);
      if (block_index != -1)
        continue;
      GLint loc = glGetUniformLocation(program, buffer);
      locations[loc] = std::string(buffer);
    }
    return locations;
  }

  void uniform(const std::string &name, int scalar) const {
    glProgramUniform1i(program, get_uniform_location(name), scalar);
  }
  void uniform(const std::string &name, float scalar) const {
    glProgramUniform1f(program, get_uniform_location(name), scalar);
  }
  void uniform(const std::string &name,
               const linalg::aliases::float2 &vec) const {
    glProgramUniform2fv(program, get_uniform_location(name), 1, &vec.x);
  }
  void uniform(const std::string &name,
               const linalg::aliases::float3 &vec) const {
    glProgramUniform3fv(program, get_uniform_location(name), 1, &vec.x);
  }
  void uniform(const std::string &name,
               const linalg::aliases::float4 &vec) const {
    glProgramUniform4fv(program, get_uniform_location(name), 1, &vec.x);
  }
  void uniform(const std::string &name,
               const linalg::aliases::float3x3 &mat) const {
    glProgramUniformMatrix3fv(program, get_uniform_location(name), 1, GL_FALSE,
                              &mat.x.x);
  }
  void uniform(const std::string &name,
               const linalg::aliases::float4x4 &mat) const {
    glProgramUniformMatrix4fv(program, get_uniform_location(name), 1, GL_FALSE,
                              &mat.x.x);
  }

  void uniform(const std::string &name, const int elements,
               const std::vector<int> &scalar) const {
    glProgramUniform1iv(program, get_uniform_location(name), elements,
                        scalar.data());
  }
  void uniform(const std::string &name, const int elements,
               const std::vector<float> &scalar) const {
    glProgramUniform1fv(program, get_uniform_location(name), elements,
                        scalar.data());
  }
  void uniform(const std::string &name, const int elements,
               const std::vector<linalg::aliases::float2> &vec) const {
    glProgramUniform2fv(program, get_uniform_location(name), elements,
                        &vec[0].x);
  }
  void uniform(const std::string &name, const int elements,
               const std::vector<linalg::aliases::float3> &vec) const {
    glProgramUniform3fv(program, get_uniform_location(name), elements,
                        &vec[0].x);
  }
  void uniform(const std::string &name, const int elements,
               const std::vector<linalg::aliases::float3x3> &mat) const {
    glProgramUniformMatrix3fv(program, get_uniform_location(name), elements,
                              GL_FALSE, &mat[0].x.x);
  }
  void uniform(const std::string &name, const int elements,
               const std::vector<linalg::aliases::float4x4> &mat) const {
    glProgramUniformMatrix4fv(program, get_uniform_location(name), elements,
                              GL_FALSE, &mat[0].x.x);
  }

  void texture(GLint loc, GLenum target, int unit, GLuint tex) const {
    glBindMultiTextureEXT(GL_TEXTURE0 + unit, target, tex);
    glProgramUniform1i(program, loc, unit);
  }

  void texture(const char *name, int unit, GLuint tex, GLenum target) const {
    texture(get_uniform_location(name), target, unit, tex);
  }

  void bind() {
    if (program > 0)
      enabled = true;
    glUseProgram(program);
  }
  void unbind() {
    enabled = false;
    glUseProgram(0);
  }
};

////////////////
//   GlMesh   //
////////////////
template <typename factory_t> class GlObject {
  mutable GLuint handle = 0;
  std::string n;

public:
  GlObject() {}
  GlObject(GLuint h) : handle(h) {}
  ~GlObject() {
    if (handle)
      factory_t::destroy(handle);
  }
  GlObject(const GlObject &r) = delete;
  GlObject &operator=(GlObject &&r) {
    std::swap(handle, r.handle);
    std::swap(n, r.n);
    return *this;
  }
  GlObject(GlObject &&r) { *this = std::move(r); }
  operator GLuint() const {
    if (!handle)
      factory_t::create(handle);
    return handle;
  }
  GlObject &operator=(GLuint &other) {
    handle = other;
    return *this;
  }
  void set_name(const std::string &newName) { n = newName; }
  std::string name() const { return n; }
  GLuint id() const { return handle; };
};
struct GlVertexArrayFactory {
  static void create(GLuint &x) { glGenVertexArrays(1, &x); };
  static void destroy(GLuint x) { glDeleteVertexArrays(1, &x); };
};
typedef GlObject<GlVertexArrayFactory> GlVertexArrayObject;

//////////////////
//   GlBuffer   //
//////////////////
struct GlBufferFactory {
  static void create(GLuint &x) { glGenBuffers(1, &x); };
  static void destroy(GLuint x) { glDeleteBuffers(1, &x); };
};
typedef GlObject<GlBufferFactory> GlBufferObject;

struct GlBuffer : public GlBufferObject {
  GLsizeiptr size;
  GlBuffer() {}
  void set_buffer_data(const GLsizeiptr s, const GLvoid *data,
                       const GLenum usage) {
    this->size = s;
    glNamedBufferDataEXT(*this, size, data, usage);
  }
  void set_buffer_data(const std::vector<GLubyte> &bytes, const GLenum usage) {
    set_buffer_data(bytes.size(), bytes.data(), usage);
  }
  void set_buffer_sub_data(const GLsizeiptr s, const GLintptr offset,
                           const GLvoid *data) {
    glNamedBufferSubDataEXT(*this, offset, s, data);
  }
  void set_buffer_sub_data(const std::vector<GLubyte> &bytes,
                           const GLintptr offset, const GLenum usage) {
    set_buffer_sub_data(bytes.size(), offset, bytes.data());
  }
};

static size_t gl_size_bytes(GLenum type) {
  switch (type) {
  case GL_UNSIGNED_BYTE:
    return sizeof(uint8_t);
  case GL_UNSIGNED_SHORT:
    return sizeof(uint16_t);
  case GL_UNSIGNED_INT:
    return sizeof(uint32_t);
  default:
    throw std::logic_error("unknown element type");
    break;
  }
}

class GlMesh {
  GlVertexArrayObject vao;
  GlBuffer vertexBuffer, instanceBuffer, indexBuffer;

  GLenum drawMode = GL_TRIANGLES;
  GLenum indexType = 0;
  GLsizei vertexStride = 0, instanceStride = 0, indexCount = 0;

public:
  GlMesh() {}
  GlMesh(GlMesh &&r) { *this = std::move(r); }
  GlMesh(const GlMesh &r) = delete;
  GlMesh &operator=(GlMesh &&r) {
    char buffer[sizeof(GlMesh)];
    memcpy(buffer, this, sizeof(buffer));
    memcpy(this, &r, sizeof(buffer));
    memcpy(&r, buffer, sizeof(buffer));
    return *this;
  }
  GlMesh &operator=(const GlMesh &r) = delete;
  ~GlMesh(){};

  void set_non_indexed(GLenum newMode) {
    drawMode = newMode;
    indexBuffer = {};
    indexType = 0;
    indexCount = 0;
  }

  void draw_elements(int instances = 0) const {
    if (vertexBuffer.size) {
      glBindVertexArray(vao);
      if (indexCount) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        if (instances)
          glDrawElementsInstanced(drawMode, indexCount, indexType, 0,
                                  instances);
        else
          glDrawElements(drawMode, indexCount, indexType, nullptr);
      } else {
        if (instances)
          glDrawArraysInstanced(
              drawMode, 0,
              static_cast<GLsizei>(vertexBuffer.size / vertexStride),
              instances);
        else
          glDrawArrays(drawMode, 0,
                       static_cast<GLsizei>(vertexBuffer.size / vertexStride));
      }
      glBindVertexArray(0);
    }
  }

  void set_vertex_data(GLsizeiptr size, const GLvoid *data, GLenum usage) {
    vertexBuffer.set_buffer_data(size, data, usage);
  }
  GlBuffer &get_vertex_data_buffer() { return vertexBuffer; };

  void set_instance_data(GLsizeiptr size, const GLvoid *data, GLenum usage) {
    instanceBuffer.set_buffer_data(size, data, usage);
  }

  void set_index_data(GLenum mode, GLenum type, GLsizei count,
                      const GLvoid *data, GLenum usage) {
    size_t size = gl_size_bytes(type);
    indexBuffer.set_buffer_data(size * count, data, usage);
    drawMode = mode;
    indexType = type;
    indexCount = count;
  }
  GlBuffer &get_index_data_buffer() { return indexBuffer; };

  void set_attribute(GLuint index, GLint size, GLenum type,
                     GLboolean normalized, GLsizei stride,
                     const GLvoid *offset) {
    glEnableVertexArrayAttribEXT(vao, index);
    glVertexArrayVertexAttribOffsetEXT(vao, vertexBuffer, index, size, type,
                                       normalized, stride, (GLintptr)offset);
    vertexStride = stride;
  }

  void set_instance_attribute(GLuint index, GLint size, GLenum type,
                              GLboolean normalized, GLsizei stride,
                              const GLvoid *offset) {
    glEnableVertexArrayAttribEXT(vao, index);
    glVertexArrayVertexAttribOffsetEXT(vao, instanceBuffer, index, size, type,
                                       normalized, stride, (GLintptr)offset);
    glVertexArrayVertexAttribDivisorEXT(vao, index, 1);
    instanceStride = stride;
  }

  void set_indices(GLenum mode, GLsizei count, const uint8_t *indices,
                   GLenum usage) {
    set_index_data(mode, GL_UNSIGNED_BYTE, count, indices, usage);
  }
  void set_indices(GLenum mode, GLsizei count, const uint16_t *indices,
                   GLenum usage) {
    set_index_data(mode, GL_UNSIGNED_SHORT, count, indices, usage);
  }
  void set_indices(GLenum mode, GLsizei count, const uint32_t *indices,
                   GLenum usage) {
    set_index_data(mode, GL_UNSIGNED_INT, count, indices, usage);
  }

  template <class T>
  void set_vertices(size_t count, const T *vertices, GLenum usage) {
    set_vertex_data(count * sizeof(T), vertices, usage);
  }
  template <class T>
  void set_vertices(const std::vector<T> &vertices, GLenum usage) {
    set_vertices(vertices.size(), vertices.data(), usage);
  }
  template <class T, int N>
  void set_vertices(const T (&vertices)[N], GLenum usage) {
    set_vertices(N, vertices, usage);
  }

  template <class V> void set_attribute(GLuint index, float V::*field) {
    set_attribute(index, 1, GL_FLOAT, GL_FALSE, sizeof(V), &(((V *)0)->*field));
  }
  template <class V, int N>
  void set_attribute(GLuint index, linalg::vec<float, N> V::*field) {
    set_attribute(index, N, GL_FLOAT, GL_FALSE, sizeof(V), &(((V *)0)->*field));
  }

  template <class T>
  void set_elements(GLsizei count, const linalg::vec<T, 2> *elements,
                    GLenum usage) {
    set_indices(GL_LINES, count * 2, &elements->x, usage);
  }
  template <class T>
  void set_elements(GLsizei count, const linalg::vec<T, 3> *elements,
                    GLenum usage) {
    set_indices(GL_TRIANGLES, count * 3, &elements->x, usage);
  }
  template <class T>
  void set_elements(GLsizei count, const linalg::vec<T, 4> *elements,
                    GLenum usage) {
    set_indices(GL_QUADS, count * 4, &elements->x, usage);
  }

  template <class T>
  void set_elements(const std::vector<T> &elements, GLenum usage) {
    set_elements((GLsizei)elements.size(), elements.data(), usage);
  }

  template <class T, int N>
  void set_elements(const T (&elements)[N], GLenum usage) {
    set_elements(N, elements, usage);
  }
};

void draw_mesh(GlShader &shader, GlMesh &mesh,
               const linalg::aliases::float3 eye,
               const linalg::aliases::float4x4 &viewProj,
               const linalg::aliases::float4x4 &model) {
  linalg::aliases::float4x4 modelViewProjectionMatrix = mul(viewProj, model);
  shader.bind();
  shader.uniform("u_mvp", modelViewProjectionMatrix);
  shader.uniform("u_eye", eye);
  mesh.draw_elements();
  shader.unbind();
}

void draw_lit_mesh(GlShader &shader, GlMesh &mesh,
                   const linalg::aliases::float3 eye,
                   const linalg::aliases::float4x4 &viewProj,
                   const linalg::aliases::float4x4 &model) {
  shader.bind();
  shader.uniform("u_viewProj", viewProj);
  shader.uniform("u_modelMatrix", model);
  shader.uniform("u_eye", eye);
  mesh.draw_elements();
  shader.unbind();
}

void upload_mesh(const tinygizmo::geometry_mesh &cpu, GlMesh &gpu) {
  const auto &verts =
      reinterpret_cast<const std::vector<linalg::aliases::float3> &>(
          cpu.vertices);
  const auto &tris =
      reinterpret_cast<const std::vector<linalg::aliases::uint3> &>(
          cpu.triangles);
  gpu.set_vertices(verts, GL_DYNAMIC_DRAW);
  gpu.set_attribute(0, 3, GL_FLOAT, GL_FALSE,
                    sizeof(tinygizmo::geometry_vertex),
                    (GLvoid *)offsetof(tinygizmo::geometry_vertex, position));
  gpu.set_attribute(1, 3, GL_FLOAT, GL_FALSE,
                    sizeof(tinygizmo::geometry_vertex),
                    (GLvoid *)offsetof(tinygizmo::geometry_vertex, normal));
  gpu.set_attribute(2, 4, GL_FLOAT, GL_FALSE,
                    sizeof(tinygizmo::geometry_vertex),
                    (GLvoid *)offsetof(tinygizmo::geometry_vertex, color));
  gpu.set_elements(tris, GL_DYNAMIC_DRAW);
}

struct camera {
  float yfov, near_clip, far_clip;
  linalg::aliases::float3 position;
  float pitch, yaw;
  linalg::aliases::float4 get_orientation() const {
    return qmul(rotation_quat(linalg::aliases::float3(0, 1, 0), yaw),
                rotation_quat(linalg::aliases::float3(1, 0, 0), pitch));
  }
  linalg::aliases::float4x4 get_view_matrix() const {
    return mul(rotation_matrix(qconj(get_orientation())),
               translation_matrix(-position));
  }
  linalg::aliases::float4x4
  get_projection_matrix(const float aspectRatio) const {
    return linalg::perspective_matrix(yfov, aspectRatio, near_clip, far_clip);
  }
  linalg::aliases::float4x4 get_viewproj_matrix(const float aspectRatio) const {
    return mul(get_projection_matrix(aspectRatio), get_view_matrix());
  }
};

struct ray {
  linalg::aliases::float3 origin;
  linalg::aliases::float3 direction;
};

struct rect {
  int x0, y0, x1, y1;
  int width() const { return x1 - x0; }
  int height() const { return y1 - y0; }
  linalg::aliases::int2 dims() const { return {width(), height()}; }
  float aspect_ratio() const { return (float)width() / height(); }
};

// Returns a world-space ray through the given pixel, originating at the camera
ray get_ray_from_pixel(const linalg::aliases::float2 &pixel,
                       const rect &viewport, const camera &cam) {
  const float x = 2 * (pixel.x - viewport.x0) / viewport.width() - 1,
              y = 1 - 2 * (pixel.y - viewport.y0) / viewport.height();
  const linalg::aliases::float4x4 inv_view_proj =
      inverse(cam.get_viewproj_matrix(viewport.aspect_ratio()));
  const linalg::aliases::float4 p0 = mul(inv_view_proj,
                                         linalg::aliases::float4(x, y, -1, 1)),
                                p1 = mul(inv_view_proj,
                                         linalg::aliases::float4(x, y, +1, 1));
  return {cam.position, p1.xyz() * p0.w - p0.xyz() * p1.w};
}

struct Teapot {
  GlShader shader;
  GlMesh mesh;

  void upload() {
    auto teapot = make_teapot();
    upload_mesh(teapot, this->mesh);
  }

  void draw(const linalg::aliases::float3 &camera_position,
            const linalg::aliases::float4x4 &camera_matrix,
            const minalg::float4x4 &tmp) {
    auto model_matrix =
        reinterpret_cast<const linalg::aliases::float4x4 &>(tmp);
    draw_lit_mesh(this->shader, this->mesh, camera_position, camera_matrix,
                  model_matrix);
  }
};

static void gl_check_error(const char *file, int32_t line) {
#if defined(_DEBUG) || defined(DEBUG)
  GLint error = glGetError();
  if (error) {
    const char *errorStr = 0;
    switch (error) {
    case GL_INVALID_ENUM:
      errorStr = "GL_INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      errorStr = "GL_INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      errorStr = "GL_INVALID_OPERATION";
      break;
    case GL_OUT_OF_MEMORY:
      errorStr = "GL_OUT_OF_MEMORY";
      break;
    default:
      errorStr = "unknown error";
      break;
    }
    printf("GL error : %s, line %d : %s\n", file, line, errorStr);
    error = 0;
  }
#endif
}

int main(int argc, char *argv[]) {
  bool ml = {};
  bool mr = {};
  bool bf = {};
  bool bl = {};
  bool bb = {};
  bool br = {};

  camera cam = {};
  cam.yfov = 1.0f;
  cam.near_clip = 0.01f;
  cam.far_clip = 32.0f;
  cam.position = {0, 1.5f, 4};

  tinygizmo::gizmo_application_state gizmo_state;
  tinygizmo::gizmo_context gizmo_ctx;

  InitWindow(1280, 800, "tiny-gizmo-example-app");
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    return 1;
  }

  GlMesh gizmoEditorMesh;

  auto wireframeShader = GlShader(gizmo_vert, gizmo_frag);

  auto teapot = Teapot{
      .shader = GlShader(lit_vert, lit_frag),
  };
  teapot.upload();

  gizmo_ctx.render = [&](const tinygizmo::geometry_mesh &r) {
    upload_mesh(r, gizmoEditorMesh);
    draw_mesh(wireframeShader, gizmoEditorMesh, cam.position,
              cam.get_viewproj_matrix((float)GetScreenWidth() /
                                      (float)GetScreenHeight()),
              identity4x4);
  };

  minalg::float2 lastCursor;

  tinygizmo::rigid_transform xform_a;
  xform_a.position = {-2, 0, 0};

  tinygizmo::rigid_transform xform_a_last;

  tinygizmo::rigid_transform xform_b;
  xform_b.position = {+2, 0, 0};

  auto t0 = std::chrono::high_resolution_clock::now();
  while (!WindowShouldClose()) {
    auto w = GetScreenWidth();
    auto h = GetScreenHeight();
    auto aspect = static_cast<float>(w) / static_cast<float>(h);
    auto t1 = std::chrono::high_resolution_clock::now();
    float timestep = std::chrono::duration<float>(t1 - t0).count();
    t0 = t1;

    BeginDrawing();

    // keyboard
    gizmo_state.hotkey_ctrl = IsKeyDown(KEY_LEFT_CONTROL);
    gizmo_state.hotkey_local = IsKeyDown(KEY_L);
    gizmo_state.hotkey_translate = IsKeyDown(KEY_T);
    gizmo_state.hotkey_rotate = IsKeyDown(KEY_R);
    gizmo_state.hotkey_scale = IsKeyDown(KEY_S);
    bf = IsKeyDown(KEY_W);
    bl = IsKeyDown(KEY_A);
    bb = IsKeyDown(KEY_S);
    br = IsKeyDown(KEY_D);
    // mouse
    gizmo_state.mouse_left = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    ml = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    mr = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    auto position = GetMousePosition();
    auto deltaCursorMotion =
        minalg::float2(position.x, position.y) - lastCursor;
    lastCursor = minalg::float2(position.x, position.y);
    if (mr) {
      cam.yaw -= deltaCursorMotion.x * 0.01f;
      cam.pitch -= deltaCursorMotion.y * 0.01f;
    }
    if (mr) {
      const linalg::aliases::float4 orientation = cam.get_orientation();
      linalg::aliases::float3 move;
      if (bf)
        move -= qzdir(orientation);
      if (bl)
        move -= qxdir(orientation);
      if (bb)
        move += qzdir(orientation);
      if (br)
        move += qxdir(orientation);
      if (length2(move) > 0)
        cam.position += normalize(move) * (timestep * 10);
    }

    // gizmo
    auto cameraOrientation = cam.get_orientation();
    const auto rayDir =
        get_ray_from_pixel({lastCursor.x, lastCursor.y}, {0, 0, w, h}, cam)
            .direction;
    // Gizmo input interaction state populated via win->on_input(...) callback
    // above. Update app parameters:
    gizmo_state.viewport_size =
        minalg::float2(static_cast<float>(w), static_cast<float>(h));
    gizmo_state.cam.near_clip = cam.near_clip;
    gizmo_state.cam.far_clip = cam.far_clip;
    gizmo_state.cam.yfov = cam.yfov;
    gizmo_state.cam.position =
        minalg::float3(cam.position.x, cam.position.y, cam.position.z);
    gizmo_state.cam.orientation =
        minalg::float4(cameraOrientation.x, cameraOrientation.y,
                       cameraOrientation.z, cameraOrientation.w);
    gizmo_state.ray_origin =
        minalg::float3(cam.position.x, cam.position.y, cam.position.z);
    gizmo_state.ray_direction = minalg::float3(rayDir.x, rayDir.y, rayDir.z);
    // gizmo_state.screenspace_scale = 80.f; // optional flag to draw the gizmos
    // at a constant screen-space scale

    // render
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glViewport(0, 0, w, h);
    glClearColor(0.725f, 0.725f, 0.725f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // teapot
    teapot.draw(cam.position, cam.get_viewproj_matrix(aspect),
                xform_a.matrix());

    teapot.draw(cam.position, cam.get_viewproj_matrix(aspect),
                xform_b.matrix());

    // gizmo
    glClear(GL_DEPTH_BUFFER_BIT);
    gizmo_ctx.update(gizmo_state);

    if (transform_gizmo("first-example-gizmo", gizmo_ctx, xform_a)) {
      std::cout << get_local_time_ns() << " - " << "First Gizmo Hovered..."
                << std::endl;
      if (xform_a != xform_a_last)
        std::cout << get_local_time_ns() << " - " << "First Gizmo Changed..."
                  << std::endl;
      xform_a_last = xform_a;
    }

    transform_gizmo("second-example-gizmo", gizmo_ctx, xform_b);
    gizmo_ctx.draw();

    gl_check_error(__FILE__, __LINE__);

    EndDrawing();
  }
  return EXIT_SUCCESS;
}
