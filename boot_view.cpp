// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include "view_decl.h"
#include "simple_shader.h"
#include "simple_vectors.h"
#include "simple_mesh.h"
#include "system.h"

#include "editor_meshes.h"
#include "gl_all.h"

struct boot_vert
{
    vector3 location;
    vector3 normal;
    vector4 color;
};

VertexDef boot_vert_def()
{
    boot_vert* proxy = 0;
    VertexDef VD = CreateVertexDef(sizeof(boot_vert), 3);
    int i = 0;
    AddVertexAttribute(VD, i++, VERTEX_POSITION_ATTR, (size_t)&proxy->location, 3, GL_FLOAT);
    AddVertexAttribute(VD, i++, VERTEX_NORMAL_ATTR, (size_t)&proxy->normal, 3, GL_FLOAT);
    AddVertexAttribute(VD, i++, VERTEX_COLOR_ATTR, (size_t)&proxy->color, 4, GL_FLOAT);

    return VD;
}

// Example view implementation.

struct ViewInfo
{
    GLuint basic_shader;
    GLint diffuse_color_uniform;
    VertexDef boot_vert;
    GLuint test_mesh;
    Editor_Mesh* grid;

    bool key_down;
    bool bMouseDown;
};

ViewInfo* InitView()
{
    ViewInfo* view = new ViewInfo();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    view->basic_shader = CreateShaderProgram(SHADER_CONSTANT_COLOR);
    view->diffuse_color_uniform = glGetUniformLocation(view->basic_shader,
                                                       "diffuse_color");
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    view->boot_vert = boot_vert_def();

    boot_vert verts[3];
    verts[0].location.x = 0.0;
    verts[0].location.y = 1.0;
    verts[0].location.z = 0.0;

    verts[1].location.x = -1.0;
    verts[1].location.y = -1.0;
    verts[1].location.z = 0.0;

    verts[2].location.x = 1.0;
    verts[2].location.y = -1.0;
    verts[2].location.z = 0.0;

    view->test_mesh = CreateMesh(3, sizeof(boot_vert), verts);

    InitEditor();
    view->grid = CreateGridMesh(10, 0.1f);

    view->key_down = false;

    view->bMouseDown = false;

    return view;
}

void FinishView(ViewInfo* view)
{
    DestroyMesh(view->grid);
    CleanupEditor();

    DestroyMesh(view->test_mesh);
    DestroyVertexDef(view->boot_vert);
    delete view;
}

void ResizeView(ViewInfo*, int, int)
{}

void UpdateView(ViewInfo* view)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(view->basic_shader);
    if(view->bMouseDown)
        glUniform4f(view->diffuse_color_uniform,
                    0.0f, 1.0f, 0.0f, 1.0f);
    else
        glUniform4f(view->diffuse_color_uniform,
                    1.0f, 0.0f, 0.0f, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, view->test_mesh);
    ApplyVertexDef(view->boot_vert);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    if(view->key_down)
    {
        glUniform4f(view->diffuse_color_uniform,
                    0.0f, 0.0f, 1.0f, 1.0f);
    }
    else
    {
        glUniform4f(view->diffuse_color_uniform,
                    1.0f, 0.0f, 0.0f, 1.0f);
    }
    DrawEditorMesh(view->grid);
}

InputHandler* GetInputHandler(ViewInfo* view)
{
    return (InputHandler*)view;
}

void InputReceiveKey(InputHandler* input, int code, KeyStatus status)
{
    ViewInfo* view = (ViewInfo*)input;
    if(code == GetKeyCodeFromAscii('W'))
    {
        if(status == Key_Down)
        {
            view->key_down = true;
        }
        else if(status == Key_Up)
        {
            view->key_down = false;
        }
    }
}

void InputMouseEvent(InputHandler* input, int x, int y, MouseButton button, MouseStatus status)
{
    ViewInfo* view = (ViewInfo*)input;
    if(status == Mouse_Down)
        view->bMouseDown = true;
    else if(status == Mouse_Up)
        view->bMouseDown = false;
}
