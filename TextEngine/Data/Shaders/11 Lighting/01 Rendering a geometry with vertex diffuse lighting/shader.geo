#version 330 core

layout(points) in; // Input primitive type: points
layout(triangle_strip, max_vertices = 6) out;

in VS_OUT {
    vec4 texCoord;
} gs_in[];

out GS_OUT {
    vec4 texCoord;
} gs_out;

void main() {

    gl_Position = gl_in[0].gl_Position; // Pass position
    gs_out.texCoord = gs_in[0].texCoord; // Pass texture coordinate
    EmitVertex();
    
    gl_Position = gl_in[0].gl_Position + vec4(1.0f, 0.0f, 0.0f, 0.0f); // Pass position
    gs_out.texCoord = gs_in[0].texCoord; // Pass texture coordinate
    EmitVertex();
    
    gl_Position = gl_in[0].gl_Position + vec4(1.0f, 1.0f, 0.0f, 0.0f); // Pass position
    gs_out.texCoord = gs_in[0].texCoord; // Pass texture coordinate
    EmitVertex();


    gl_Position = gl_in[0].gl_Position + vec4(1.0f, 1.0f, 0.0f, 0.0f); // Pass position
    gs_out.texCoord = gs_in[0].texCoord; // Pass texture coordinate
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.0f, 1.0f, 0.0f, 0.0f); // Pass position
    gs_out.texCoord = gs_in[0].texCoord; // Pass texture coordinate
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.0f, 0.0f, 0.0f, 0.0f); // Pass position
    gs_out.texCoord = gs_in[0].texCoord; // Pass texture coordinate
    EmitVertex();

EndPrimitive(); // End the current primitive
}