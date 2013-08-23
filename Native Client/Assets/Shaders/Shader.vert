uniform vec3 eye;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
varying vec3 vertex;
varying vec3 normal;

void main() {

    mat4 inverseModelMatrix = inverse(modelMatrix);
    
    
    vertex = vec3(modelMatrix * gl_Vertex);
    normal = normalize(vec3(vec4(gl_Normal, 0.0) * inverseModelMatrix));
    
	gl_FrontColor = gl_Color;
    gl_BackColor = gl_Color;
	gl_Position = ftransform();
}