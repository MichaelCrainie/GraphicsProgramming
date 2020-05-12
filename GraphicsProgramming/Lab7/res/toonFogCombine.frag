#version 400
out vec4 FragColor;
in vec3 v_norm;
in vec4 v_pos;

uniform vec3 fogColor;

//uniform mat4 u_pm; // uniform_ProjectionMatrix
//uniform mat4 u_vm; // uniform_ViewMatrix
uniform vec3 lightDir;


uniform float maxDist; //fog max distance
uniform float minDist; //fog min distance

void main()
{
float dist = abs( v_pos.z );
float fogFactor = (minDist - dist) / (minDist - maxDist);
fogFactor = clamp( fogFactor, 0.0, 1.0 );
vec3 lightColor = vec3(0.1,0.1,0.1);
vec3 color = mix( fogColor, lightColor, fogFactor);
float intensity;
vec4 color2;
intensity = dot(lightDir,v_norm);

if (intensity > 0.95)
	color2 = vec4(1.0,0.9,0.0,1.0);
else if (intensity > 0.5)
	color2 = vec4(0.8,0.7,0.3,1.0);
else if (intensity > 0.25)
	color2 = vec4(0.6,0.5,0.2,1.0);
else
	color2 = vec4(0.4,0.3,0.1,1.0);
FragColor = (vec4(color, 1.0) * color2);

}