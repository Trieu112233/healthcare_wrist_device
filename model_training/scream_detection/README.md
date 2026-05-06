# Scream Detection Model Training Documentation

## 1. Dataset

- **Nguồn dữ liệu:**  
  Quá trình huấn luyện sử dụng hai tập dữ liệu âm thanh về tiếng la người và các tiếng động không phải tiếng la:
  1. **Human Screaming Detection Dataset**  
     - Gồm 2.831 mẫu âm thanh, độ dài mỗi mẫu từ 1 đến 10 giây (đa số là 10 giây), tần số lấy mẫu gốc 44,1 kHz.  
     - Bao gồm cả positive và negative samples với các nhãn rõ ràng, ghi chú chi tiết về ứng dụng trong các hệ thống theo dõi sức khoẻ, bảo mật và khẩn cấp.
  2. **Audio Dataset of Scream and Non Scream**  
     - Phân loại rõ các file vào thư mục `scream` và `non_scream`, tổng dung lượng lên tới trên 500 MB, phù hợp cho các dự án AI hướng tới nhận diện sự kiện âm thanh đặc biệt.

- **Tiền xử lý:**  
  - Dữ liệu gốc có nhiều định dạng và tần số lấy mẫu khác nhau, được chuẩn hóa đồng bộ về tần số lấy mẫu 16 kHz bằng script `resample_audio.py`.
  - Tất cả audio đều chuyển thành chuẩn mono PCM. Kết quả lưu theo cấu trúc thư mục gốc nhưng với thư mục đích riêng cho dữ liệu chuẩn.
  - Sau đó, toàn bộ file audio đã chuẩn hóa được upload lên Edge Impulse, phân thành 2 class chính: "scream" và "non-scream".
  - Dữ liệu được chia thành hai tập: training (80%) và test (20%).

## 2. Cấu hình Impulse trên Edge Impulse

- **Tín hiệu đầu vào:** Audio, sample rate 16 kHz.
- **Cửa sổ phân tích (window size):** 1000 ms (1 giây).
- **Stride:** 500 ms (cửa sổ trượt nửa giây).
- **Xử lý dữ liệu:** Zero padding áp dụng tự động.
- **Khối tiền xử lý đặc trưng:** MFCC với cấu hình:
    - 13 hệ số MFCC.
    - Frame length: 25 ms.
    - Frame stride: 40 ms.
    - 20 filter Mel.
    - FFT size: 256.
    - Chuẩn hóa đặc trưng trên cửa sổ 151 mẫu.
    - Pre-emphasis coefficient: 0.98.
    - Tần số cắt thấp: 80 Hz.

## 3. Kiến trúc và quy trình huấn luyện mô hình

- **Thuật toán:** Mạng nơ-ron tích chập 1D (1D CNN) xử lý trực tiếp đặc trưng MFCC:
    - Lớp Input 325 đặc trưng (từ MFCC).
    - Reshape về ma trận (13 hệ số, số frame phù hợp với từng cửa sổ).
    - 3 khối Conv1D nối tiếp với số lượng filter tăng dần (16, 32, 64 filter, kernel size = 3).
    - Dropout sau mỗi conv để tránh overfitting (tỉ lệ dropout 0.25).
    - Flatten, Dense 32 neurons, đầu ra softmax cho 2 lớp (scream, non-scream).
- **Tham số huấn luyện:**
    - Số training cycle: 100.
    - Learning rate: 0.001.
    - Batch size: 128.
    - Auto-weight class tự động.
    - Data augmentation: Thêm noise, che time/frequency bands (ở mức low).
    - Tự profile model lượng tử hóa (int8) để benchmark hiệu năng.

- **Chia dữ liệu:**  
  80% training, 20% test (trên tổng số gần 4.000 file training, gần 900 file test).

## 4. Hiệu năng và đánh giá

- **Trên validation set:**
    - Độ chính xác tổng thể (accuracy): 83.9%.
    - Precision trung bình: 0.91.
    - Recall trung bình: 0.84.
    - F1-score trung bình: 0.86.
- **Chi tiết confusion matrix:**
    - Nhận diện đúng non-scream: 83.7%.
    - Nhận diện đúng scream: 85.6%.
    - Tỉ lệ nhầm lẫn (misclassify) với non-scream: 14.4%, với scream: 16.3%.
- **Mô hình đã được lượng tử hóa int8**, inference nhanh, tận dụng tối đa bộ nhớ trên vi điều khiển.
- **Thời gian inference end-to-end trên thiết bị:** MFCC: 236 ms, classifier: 10 ms, tổng cộng ~246 ms.
- **RAM sử dụng (tối đa):** 17.9 KB.
- **Flash (classifier):** 47.5 KB.

## 5. Triển khai trên thiết bị thực tế

- **Target:** Mô hình xuất ra dưới dạng C++ library, hoàn toàn độc lập, có thể tích hợp trực tiếp vào chương trình nhúng, phù hợp mọi bộ biên dịch C++ hiện đại.
- **Inference engine:** EON Compiler – tối ưu hiệu năng, giảm ~40% RAM, ~33% flash so với giải pháp thông thường.
- **Hỗ trợ:** Có thể chạy tốt trên các vi điều khiển như ESP32-S3 và các hệ thống nhúng khác.

## 6. Tài liệu tham khảo

- **Script preprocess:** `model_training/scream_detection/resample_audio.py`
- **Nguồn dữ liệu:**  
  - Human Screaming Detection Dataset (`https://www.kaggle.com/datasets/whats2000/human-screaming-detection-dataset/data`)
  - Audio Dataset of Scream and Non Scream (`https://www.kaggle.com/datasets/aananehsansiam/audio-dataset-of-scream-and-non-scream`)  
- **Chi tiết tham số, kiến trúc tham khảo:** Edge Impulse project (`https://studio.edgeimpulse.com/public/916888/live`), pipeline đã trình bày trong tài liệu này.

---
