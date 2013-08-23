uniform vec3 eye;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
varying vec3 vertex;  //screen space position
varying vec3 normal; //screen space normal

void main()
{

    vec3 v = normalize(eye-vertex);
    vec3 n = normalize(normal);
    
    float intensity = max(dot(v, n),0.0);
    
    vec4 color = gl_Color;
    
    if(intensity <= 0.3) {
        color = vec4(1.0,1.0,1.0,1.0);
    }

    gl_FragColor = color;
}