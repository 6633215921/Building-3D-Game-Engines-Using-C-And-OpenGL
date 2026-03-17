#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 TexCoord;
out float vHeight;

uniform bool isSphere;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

struct Wave {
    vec2 direction;
    float steepness;
    float wavelength;
    float speed;
};
uniform Wave waves[4];

void main() {
    // 1. แปลงพิกัดเป็น World Space ก่อน เพื่อหาตำแหน่งที่แท้จริงในโลก
    vec4 worldPos = model * vec4(aPos, 1.0);
    vec3 p = worldPos.xyz;

    // ถ้าเป็น Sphere ให้ใช้ตำแหน่งศูนย์กลาง (จาก model matrix column 3) มาหาค่าคลื่น เพื่อไม่ให้ลูกบอลบิดเบี้ยว
    vec2 waveSamplePos = isSphere ? vec2(model[3][0], model[3][2]) : worldPos.xz;
    
    // 2. คำนวณ Displacement โดยใช้ p.xz (ตำแหน่งในโลก)
    vec3 offset = vec3(0.0);
    for(int i = 0; i < 4; i++) {
        float k = 2.0 * 3.14159 / waves[i].wavelength;
		float f = k * (dot(waves[i].direction, waveSamplePos) - (waves[i].speed * time));        
        float a = waves[i].steepness / k;
        offset.x += waves[i].direction.x * (a * cos(f));
        offset.y += a * sin(f);
        offset.z += waves[i].direction.y * (a * cos(f));
    }
    
    // 3. บวก Offset เข้ากับตำแหน่ง World Position
    p += offset;
    vHeight = p.y;
    TexCoord = vec2(0.0f, 0.0f); //Dummy
    // 4. คูณแค่ view และ projection (เพราะ p เป็น world space อยู่แล้ว)
    gl_Position = projection * view * vec4(p, 1.0);
}