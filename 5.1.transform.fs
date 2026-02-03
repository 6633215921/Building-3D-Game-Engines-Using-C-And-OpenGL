#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform float iTime;
uniform sampler2D texture1; // สำหรับ Texture ของโลก
uniform sampler2D texture2; // สำหรับ Texture ของพระจันทร์
uniform int useTexture; // 1 = วาดโลก, 2 = วาดดวงพระจันทร์, 3 = วาดวงโคจร
uniform vec2 uMousePos;
uniform vec2 uCenter;


vec3 palette( float t ) {
    vec3 a = vec3(0.5, 0.5, 0.5);
    vec3 b = vec3(0.5, 0.5, 0.5);
    vec3 c = vec3(1.0, 1.0, 1.0);
    vec3 d = vec3(0.263,0.416,0.557);

    return a + b*cos( 6.28318*(c*t+d) );
}

void main()
{
    vec2 uv = TexCoord * 2.0 - 1.0;
    vec2 uv0 = uv;
    if (useTexture == 3) 
    {
        FragColor = vec4(ourColor, 1.0); 
    }
    else if (useTexture > 0) // กรณีเป็น Texture (1=Earth, 2=Moon)
    {
        vec4 texColor = vec4(1.0);
        if (useTexture == 2) {
            texColor = texture(texture2, TexCoord);
        } else {
            texColor = texture(texture1, TexCoord);
        }
        
        if(texColor.a < 0.1) discard; 

        float ambientStrength = 0.5; 
        vec2 pixelPos = gl_FragCoord.xy;
        vec2 lightDir = normalize(uMousePos - pixelPos);
        vec2 distVector = pixelPos - uCenter;
        vec2 normal = normalize(distVector); 
        float diff = max(dot(normal, lightDir), 0.0);
        diff = smoothstep(0.0, 0.2, diff); 
        
        vec3 finalColor = texColor.rgb * (ambientStrength + diff);
        
        FragColor = vec4(finalColor, texColor.a); 
    }
    else
    {

        vec3 finalColor = vec3(0.0);

        for (float i = 0; i < 3; i++){
            uv = fract(uv * 1.5)-0.5;
            float d = length(uv)*exp(-length(uv0));
            vec3 col = palette(length(uv0) + i*0.4 + iTime*0.4);
            d = sin(d*8.0+iTime)/8.0;
            d = abs(d); 
            d = pow(0.01/d, 1.2);
            finalColor += col * d;
        }

        FragColor = vec4(finalColor, 1.0);
    }
}