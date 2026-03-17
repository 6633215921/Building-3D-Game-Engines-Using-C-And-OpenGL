# Building-3D-Game-Engines-Using-C-And-OpenGL

---

# 🌊 3D Gerstner Wave Simulation

โปรเจกต์นี้เป็นการจำลองพลวัตของผิวน้ำ (Fluid Surface Simulation) แบบ 3D 

## 🌟 ฟีเจอร์หลัก (Key Features)

โปรเจกต์นี้แสดงผลการทำงานของกราฟิกและการคำนวณทางคณิตศาสตร์ดังนี้:

### 1. Gerstner Wave Algorithm (สมการคลื่นผิวน้ำ)

การจำลองความพริ้วไหวของน้ำไม่ได้ใช้เพียงแค่ Sine wave ธรรมดา แต่ใช้สมการ **Gerstner Wave** ซึ่งทำให้จุดบนผิวน้ำมีการเคลื่อนที่ทั้งแนวตั้งและแนวนอน (Circular motion) ทำให้ยอดคลื่นมีความแหลมและสมจริงกว่า

* **Superposition:** มีการสร้างคลื่นย่อย 4 ลูก (`waves` vector) ที่มีทิศทาง (Direction), ความชัน (Steepness), ความยาวคลื่น (Wavelength) และความเร็ว (Speed) แตกต่างกัน นำมารวมกันเพื่อสร้างพื้นผิวที่ดูเป็นธรรมชาติ
<img width="891" height="663" alt="image" src="https://github.com/user-attachments/assets/0c308f06-4d7d-45f8-bdcf-d22183714897" /><img width="287" height="311" alt="image" src="https://github.com/user-attachments/assets/a7411a70-c94b-4792-ab3f-23398d5e0dbf" />
* **Real-time Physics:** การคำนวณตำแหน่ง  ของจุดทั้งหมดเกิดขึ้นใหม่ทุกเฟรมตามเวลา (`glfwGetTime`)

### 2. Dynamic Grid Rendering (การเรนเดอร์โครงสร้าง)

มีการแสดงผลข้อมูลชุดเดียวกันใน 2 รูปแบบพร้อมกันเพื่อ Visualize โครงสร้างข้อมูล:

* **Icosphere Particles:** แสดงจุดยอดของคลื่นแต่ละจุดด้วยรูปทรงทรงกลม (Icosphere) โดยมีการอัปเดต Model Matrix ของแต่ละลูกทุกเฟรม
* **Grid Connections (GL_LINES):** มีอัลกอริทึมในการเชื่อมจุดต่อจุด (Connectivity Logic) เพื่อวาดเส้นตารางถักทอเชื่อมต่อกันระหว่างทรงกลม ทำให้เห็นความโค้งและความลาดชันของคลื่นได้อย่างชัดเจน

### 3. Interactive Camera System

ผู้ใช้งานสามารถสำรวจโมเดล 3 มิติได้อย่างอิสระผ่านระบบกล้องแบบ FPS

* สามารถบินสำรวจรอบๆ คลื่นได้ 360 องศา
* รองรับการ Zoom เข้า-ออก (ผ่าน Field of View)

## 🛠 เทคโนโลยีที่ใช้ (Tech Stack)

* **Language:** C++ (Standard 11+)
* **Graphics API:** OpenGL 3.3 (Core Profile)
* **Libraries:**
* `GLFW` (Window & Input)
* `GLAD` (OpenGL Loader)
* `GLM` (Mathematics: Vector & Matrix)

* **Shader Language:** GLSL

## 🎮 การควบคุม (Controls)

* **Mouse Movement:** หันหน้ากล้อง / มองรอบทิศทาง
* **Mouse Scroll:** ซูมเข้า - ซูมออก
* **W / A / S / D:** เคลื่อนที่กล้อง (หน้า, ซ้าย, หลัง, ขวา)
* **ESC:** ปิดโปรแกรม

## 📂 โครงสร้างไฟล์ (File Structure)

* `camera_class.cpp`: โค้ดหลักในการจัดการ Window, Loop การทำงาน, และการคำนวณสมการ Gerstner Wave บน CPU ก่อนส่งไปวาด
* `Icosphere.h`: Class สำหรับสร้าง Vertex Data ของทรงกลม
* `7.4.camera.vs` / `7.4.camera.fs`: Shader พื้นฐานสำหรับจัดการ Coordinate Systems (Projection * View * Model)

## 📸 ตัวอย่างการทำงาน (Previews)
คลิกที่เพื่อรับชมวิดีโอสาธิตการทำงาน :
<video src= "https://github.com/user-attachments/assets/9b218d48-6c66-4da5-b2fc-1505a8c3b761">
</video>

---

**ผู้จัดทำ:** วรพล พันทอง (Worapol Punthong)
