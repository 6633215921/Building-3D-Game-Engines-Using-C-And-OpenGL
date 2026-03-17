#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float vHeight;

uniform bool isWireframe;

void main()
{
    // 1. กำหนดคู่สีน้ำ (ปรับเลข RGB ได้ตามชอบ)
    vec3 deepColor = vec3(0.0f, 0.15f, 0.4f);  // น้ำเงินเข้ม (จุดต่ำสุด)
    vec3 crestColor = vec3(0.0f, 0.6f, 0.9f);  // ฟ้าสว่าง (ยอดคลื่น)

    // 2. แปลงช่วงความสูงให้อยู่ในสัดส่วน 0.0 ถึง 1.0
    // คลื่นของคุณน่าจะสวิงอยู่แถวๆ -1.5 ถึง 1.5 
    // เราเลยบวก 1.5 ให้ค่าต่ำสุดกลายเป็น 0 แล้วหาร 3.0 ให้เป็นอัตราส่วน 0 ถึง 1
    float factor = clamp((vHeight + 1.5) / 3.0, 0.0, 1.0);

    // 3. ผสมสีตามสัดส่วน factor
    // ถ้า factor = 0 จะได้สี deepColor, ถ้า = 1 จะได้ crestColor
    vec3 finalColor = mix(deepColor, crestColor, factor);

    if (isWireframe) {
        // ถ้าเป็นเส้นขอบ ให้ใช้สีขาว
        finalColor = vec3(0.0f, 0.4f, 0.8f);
    }
    FragColor = vec4(finalColor, 1.0f);
}