# Building-3D-Game-Engines-Using-C-And-OpenGL

---

# 🏃‍♂️ 3D Animated Character Controller

โปรเจกต์นี้เป็นการสร้างระบบควบคุมตัวละคร 3 มิติในมุมมองบุคคลที่สาม (Third-Person Controller) พร้อมระบบแอนิเมชันโครงกระดูก (Skeletal Animation) แผนผังการเคลื่อนที่แบบอ้างอิงมุมกล้อง และสภาพแวดล้อมพื้นฐาน พัฒนาด้วย C++ และ OpenGL

## 🌟 ฟีเจอร์หลัก (Key Features)

โปรเจกต์นี้แสดงผลการทำงานของกราฟิกและระบบเกมเพลย์ดังนี้:

### 1. Skeletal Animation System (ระบบแอนิเมชันโครงกระดูก)

- **Animation State Machine:** ระบบคำนวณและสลับการเล่นแอนิเมชันของตัวละครระหว่างท่ายืนปกติ (Standing) และท่าเดิน (Walking) โดยอัตโนมัติตามสถานะการเคลื่อนที่ (Input state)
- **Bone Transformations:** โหลดและคำนวณเมทริกซ์การเคลื่อนไหวของกระดูก (Bone Matrices) ส่งผ่านไปยัง Vertex Shader เพื่อทำให้โมเดลขยับได้อย่างเป็นธรรมชาติ

### 2. Third-Person Character Controller (ระบบควบคุมตัวละคร)

- **Camera-Relative Movement:** ผู้เล่นสามารถบังคับตัวละครให้เดินไปยังทิศทางต่างๆ (WASD) โดยอ้างอิงจากมุมมองของกล้องเป็นหลัก (คล้ายเกม Action RPG ทั่วไป)
- **Auto-Rotation:** ตัวละครจะคำนวณและหมุนตัวหันหน้าไปทางทิศทางที่กำลังเดินอยู่เสมอโดยใช้ฟังก์ชันทางคณิตศาสตร์ (`atan2`)

### 3. Dynamic Follow Camera (กล้องติดตามตัวละคร)

- กล้อง 3 มิติถูกผูกติดให้วิ่งตามและมองลงมาที่ตัวละครเสมอ
- ผู้เล่นสามารถใช้เมาส์เพื่อหมุนกล้องสำรวจรอบทิศทาง (360 องศา) รอบตัวละครได้อย่างอิสระโดยไม่กระทบกับทิศทางที่ตัวละครกำลังหันหน้าอยู่

## 🛠 เทคโนโลยีที่ใช้ (Tech Stack)

- **Language:** C++ (Standard 11+)
- **Graphics API:** OpenGL 3.3 (Core Profile)
- **Libraries:**
  - `GLFW` (Window & Input handling)
  - `GLAD` (OpenGL Function Loader)
  - `GLM` (Mathematics: Vector & Matrix operations)
  - `Assimp` (3D Model & Animation Loading)
- **Shader Language:** GLSL

## 🎮 การควบคุม (Controls)

- **W / A / S / D:** เคลื่อนที่ตัวละคร (หน้า / ซ้าย / หลัง / ขวา) อ้างอิงจากมุมมองกล้อง
- **Mouse Movement:** หันมุมกล้องมองรอบตัวละคร
- **Mouse Scroll:** ซูมเข้า - ซูมออก
- **ESC:** ออกจากโปรแกรม

## 📂 โครงสร้างระบบ (System Structure)

- **3D Models & Animations:** โหลดโมเดล 3D แบบ Skeletal Mesh นามสกุล `.dae` (Collada) เช่น โมเดลตัวละคร (`Ch03_nonPBR.dae`), ไฟล์แอนิเมชันเดิน (`walk.dae`), และแอนิเมชันยืนพัก (`standing.dae`)
- **Shaders:**
  - `anim_model.vs` / `anim_model.fs` สำหรับคำนวณ Skinning/Bones และเรนเดอร์โมเดลแอนิเมชัน
  - `floor.vs` / `floor.fs` สำหรับเรนเดอร์พื้นทางเดิน (Ground Plane) พร้อมรับค่าแสงพื้นฐาน (Lighting Uniforms)

## 📸 ตัวอย่างการทำงาน (Previews)

คลิกเพื่อรับชมวิดีโอสาธิตการทำงาน :
<video src= "[ใส่ลิงก์วิดีโอใหม่ของคุณที่นี่]">
</video>

---

**ผู้จัดทำ:** วรพล พันทอง (Worapol Punthong)
