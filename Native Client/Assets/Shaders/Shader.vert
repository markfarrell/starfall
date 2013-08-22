uniform vec3 eye;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
varying vec3 viewDirection;
varying vec3 normalDirection;

void main() {

    //mat4 modelMatrix = gl_ModelViewMatrix * inverse(viewMatrix);
    mat4 inverseModelMatrix = inverse(modelMatrix);
    
    viewDirection = vec3(modelMatrix * gl_Vertex);
    normalDirection = normalize(vec3(vec4(gl_Normal, 0.0) * inverseModelMatrix));
    
	gl_FrontColor = gl_Color;
    gl_BackColor = gl_Color;
	gl_Position = ftransform();
}