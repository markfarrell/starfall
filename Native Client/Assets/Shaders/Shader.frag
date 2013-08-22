uniform vec3 eye;
uniform mat4 viewMatrix;
varying vec3 viewDirection;  //screen space position
varying vec3 normalDirection; //screen space normal

void main()
{
    vec3 v = normalize(eye - viewDirection);
    vec3 n = normalize(normalDirection);
    
    float intensity = max(dot(v, n),0.0);
    
    vec4 color = gl_Color;
    
    if(intensity <= 0.3) {
        color = vec4(1.0,1.0,1.0,1.0);
    }

    gl_FragColor = color;
}