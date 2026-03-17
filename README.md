# Building-3D-Game-Engines-Using-C-And-OpenGL

---

# 🌊 3D Gerstner Wave & Boat Simulation

โปรเจกต์นี้เป็นการสร้างเกม 3D แบบอินเทอร์แอคทีฟที่จำลองพลวัตของผิวน้ำ (Fluid Surface Simulation) พร้อมระบบขับเคลื่อนเรือ การคำนวณฟิสิกส์การชน (Collision) และการเก็บไอเทม โดยพัฒนาด้วย C++ และ OpenGL

## 🌟 ฟีเจอร์หลัก (Key Features)

โปรเจกต์นี้แสดงผลการทำงานของกราฟิกและระบบเกมเพลย์ดังนี้:

### 1. Gerstner Wave Physics & Render (ระบบคลื่นผิวน้ำ)
การจำลองความพริ้วไหวของน้ำใช้สมการ **Gerstner Wave** เพื่อสร้างยอดคลื่นที่มีความแหลมและสมจริง
* **Shader & CPU Synchronization:** คลื่นย่อย 4 ลูก (`waves`) ถูกคำนวณบน Vertex Shader เพื่อการแสดงผลแบบ Real-time และถูกจำลองบน CPU ไปพร้อมๆ กัน เพื่อให้ตัวละคร (เรือ) ลอยขึ้น-ลงและโยกตามยอดคลื่นได้อย่างแม่นยำ
* **Dual Rendering Mode:** แสดงผลผิวน้ำแบบทึบ (Solid Fill) ซ้อนทับด้วยโครงข่ายเส้นตาราง (Wireframe) เพื่อเน้นให้เห็นมิติเรขาคณิตและความสูงชันของคลื่น

### 2. Vehicle Controller (ระบบควบคุมยานพาหนะ)
* ใช้ตรรกะการขับเคลื่อนคล้ายรถยนต์/เรือ (Ackermann-like Steering)
* ผู้เล่นสามารถหมุนพวงมาลัย (A/D) ได้ก็ต่อเมื่อกำลังเหยียบคันเร่ง (W) หรือถอยหลัง (S) เท่านั้น
* มีระบบจำลองการปัดของหน้ารถเมื่อถอยหลังเข้าโค้ง เพิ่มมิติความสมจริงให้กับการควบคุม

### 3. Gameplay Systems (ระบบเกมเพลย์)
* **Bounding Sphere Collision:** ระบบตรวจจับการชนแบบทรงกลม ป้องกันไม่ให้ผู้เล่นขับเรือทะลุโขดหิน (Rock Obstacles) รวมไปถึงการป้องกันไม่ให้กระแสน้ำพัดตัวละครทะลุหิน
* **Collectible Items:** ระบบตรวจจับการเก็บไอเทมในฉาก โดยโมเดลไอเทมจะหมุนลอยตัวอยู่ และจะหายไปทันทีเมื่อเรือแล่นเข้าไปใกล้ในระยะที่กำหนด
* **Third-Person Follow Camera:** กล้อง 3 มิติถูกผูกติดให้วิ่งตามด้านหลังของเรือเสมอ โดยผู้เล่นยังสามารถใช้เมาส์หมุนกล้องเพื่อสำรวจรอบทิศทาง (360 องศา) ได้อย่างอิสระ

## 🛠 เทคโนโลยีที่ใช้ (Tech Stack)

* **Language:** C++ (Standard 11+)
* **Graphics API:** OpenGL 3.3 (Core Profile)
* **Libraries:**
  * `GLFW` (Window & Input handling)
  * `GLAD` (OpenGL Function Loader)
  * `GLM` (Mathematics: Vector & Matrix operations)
  * `Assimp` (3D Model Loading)
* **Shader Language:** GLSL

## 🎮 การควบคุม (Controls)

* **W / S:** เดินหน้า / ถอยหลัง (Accelerate / Reverse)
* **A / D:** เลี้ยวซ้าย / เลี้ยวขวา (เรือต้องกำลังเคลื่อนที่อยู่ถึงจะเลี้ยวได้)
* **Mouse Movement:** หันมุมกล้องมองรอบตัวเรือ
* **Mouse Scroll:** ซูมเข้า - ซูมออก
* **ESC:** ออกจากโปรแกรม

## 📂 โครงสร้างระบบ (System Structure)

* **3D Models:** โหลดโมเดลนามสกุล `.obj` เข้ามาในฉาก เช่น `ship` (ตัวละคร), `rock` (สิ่งกีดขวาง), และ `oil` (ไอเทม)
* **Shaders:**
  * `7.4.camera.vs` / `7.4.camera.fs` สำหรับเรนเดอร์โครงสร้างน้ำและการจำลองคลื่น
  * `1.model_loading.vs` / `1.model_loading.fs` สำหรับเรนเดอร์โมเดล 3D ต่างๆ ในฉากด้วย Projection * View * Model matrix
 
## 📸 ตัวอย่างการทำงาน (Previews)
คลิกที่เพื่อรับชมวิดีโอสาธิตการทำงาน :
<video src= "https://github.com/user-attachments/assets/9b218d48-6c66-4da5-b2fc-1505a8c3b761">
</video>

---

**ผู้จัดทำ:** วรพล พันทอง (Worapol Punthong)
