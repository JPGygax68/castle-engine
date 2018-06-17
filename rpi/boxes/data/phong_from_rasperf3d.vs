#ifdef GL_ES
precision lowp float;
#endif

// input constants
uniform mat4 castle_ModelViewMatrix;
uniform mat4 castle_ProjectionMatrix;
// uniform mat3 castle_NormalMatrix; // unused, but see comment marked "Michalis" below

// input variables, different for each vertex
attribute vec4 castle_Vertex;
attribute vec3 castle_Normal;
attribute vec4 castle_MultiTexCoord0;

// outputs
varying vec3 position_viewspace;
varying vec3 normal_viewspace;
varying vec2 texCoord0;

void main()
{
    // pass the texture coordinate unchanged
    texCoord0 = castle_MultiTexCoord0.xy;

    // vertex position in viewspace is needed for lighting in the fragment shader
    position_viewspace = vec3(castle_ModelViewMatrix * castle_Vertex);

    // normal vector in viewspace is also needed for lighting in the fragment shader
    // this math is only correct if castle_ModelViewMatrix has a uniform scale! Otherwise use its inverse transpose.
    normal_viewspace = vec3(castle_ModelViewMatrix * vec4(castle_Normal,0));

    // Michalis comment: The line below would be better than above,
    // but I didn't use it to stay as close to original rasperf3d version.
    //normal_viewspace = normalize(castle_NormalMatrix * castle_Normal);

    // OpenGL needs the fully-projected vertex position for rendering the triangle
    gl_Position = castle_ProjectionMatrix * (castle_ModelViewMatrix * castle_Vertex);
}
