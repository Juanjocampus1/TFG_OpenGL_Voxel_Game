#version330 core

#ifdef VERTEX_SHADER
layout(location =0) in vec3 aPos;
layout(location =1) in vec2 aTex;
out vec2 TexCoord;
void main() {
 TexCoord = aTex;
 gl_Position = vec4(aPos,1.0);
}
#endif

#ifdef FRAGMENT_SHADER
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler3D voxelTex;
uniform vec3 camPos;
uniform vec3 sunDir; // normalized
uniform float timeOfDay;
uniform mat4 view;
uniform mat4 projection;

// Simple raymarch through voxel texture in texture space [0,1]
float sampleVoxel(vec3 p) {
 // sample occupancy
 return texture(voxelTex, p).r; //0/1
}

void main() {
 // compute ray direction in world space from camera through screen
 vec2 uv = TexCoord *2.0 -1.0;
 vec4 rayClip = vec4(uv, -1.0,1.0);
 vec4 rayEye = inverse(projection) * rayClip; rayEye.z = -1.0; rayEye.w =0.0;
 vec3 rayWorld = normalize((inverse(view) * rayEye).xyz);

 // march from camera position
 vec3 pos = camPos;
 float t =0.0;
 const float MAX_T =100.0;
 const float STEP =0.5; // world units
 float occ =0.0;
 for (int i =0; i <200 && t < MAX_T; ++i) {
 vec3 p = pos + rayWorld * t;
 // convert world p to voxel texture coord - assume voxel region centered at origin and scale known
 // For simplicity, map world coords to [0,1] by scaling by1/64 and offset0.5
 vec3 tc = p *0.015625 +0.5; //1/64 =0.015625
 if (tc.x <0.0 || tc.x >1.0 || tc.y <0.0 || tc.y >1.0 || tc.z <0.0 || tc.z >1.0) {
 t += STEP; continue;
 }
 float v = sampleVoxel(tc);
 if (v >0.5) {
 // hit
 // compute simple shading: ambient + sun visibility
 // shadow test: sample along sunDir from hit point
 float shadow =1.0;
 vec3 sp = p + sunDir *0.5; // start slightly towards sun
 for (int s=0; s<20; ++s) {
 vec3 stc = sp *0.015625 +0.5;
 if (stc.x <0.0 || stc.x >1.0 || stc.y <0.0 || stc.y >1.0 || stc.z <0.0 || stc.z >1.0) break;
 if (sampleVoxel(stc) >0.5) { shadow =0.2; break; }
 sp += sunDir *0.8;
 }
 vec3 base = vec3(0.6,0.6,0.6);
 vec3 col = base * (0.2 +0.8 * shadow);
 FragColor = vec4(col,1.0);
 return;
 }
 t += STEP;
 }

 // no hit: draw sky color based on timeOfDay
 float tt = timeOfDay;
 vec3 sky;
 if (tt <0.25) { float u = tt /0.25; sky = mix(vec3(0.02,0.03,0.08), vec3(0.6,0.4,0.3), u); }
 else if (tt <0.5) { float u = (tt-0.25)/0.25; sky = mix(vec3(0.6,0.4,0.3), vec3(0.53,0.81,0.92), u); }
 else if (tt <0.75) { float u = (tt-0.5)/0.25; sky = mix(vec3(0.53,0.81,0.92), vec3(0.6,0.4,0.3), u); }
 else { float u = (tt-0.75)/0.25; sky = mix(vec3(0.6,0.4,0.3), vec3(0.02,0.03,0.08), u); }
 FragColor = vec4(sky,1.0);
}
#endif