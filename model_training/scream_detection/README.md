# Scream Detection Model Training Documentation

## 1. Dataset

- **Nguồn dữ liệu:** tổng hợp từ hai bộ dữ liệu mở:
  1. **Human Screaming Detection Dataset**.
  2. **Audio Dataset of Scream and Non Scream**.
- **Nhãn:** `scream` cho tiếng hét/kêu cứu và `non-scream` cho âm thanh sinh hoạt, tiếng ồn nền, tiếng nói chuyện bình thường.
- **Thư mục dữ liệu dùng để huấn luyện trong repo:** `scream_dataset/Screaming` và `scream_dataset/NotScreaming`. Đây là dữ liệu đã được chuẩn hóa/tiền xử lý sẵn và có thể upload lên Edge Impulse trực tiếp.
- **Script tiền xử lý:** `resample_audio.py`, script tiền xử lý dữ liệu audio đã dùng trong đồ án.

Quy trình tiền xử lý đã thực hiện và được thể hiện trong script:

- Dữ liệu gốc có nhiều định dạng và tần số lấy mẫu khác nhau, ví dụ WAV/MP3 và 44,1 kHz.
- Chuẩn hóa audio về **mono PCM**.
- Hạ tần số lấy mẫu xuống **16 kHz** để đủ bao phủ dải giọng nói người và giảm chi phí xử lý trên vi điều khiển.
- Upload dữ liệu đã chuẩn hóa lên Edge Impulse.
- Chia dữ liệu theo tỷ lệ **80% training** và **20% testing**; tập test độc lập được dùng cho đánh giá cuối ở Chương 4.

## 2. Cấu hình Impulse trên Edge Impulse

- **Input:** audio 16 kHz.
- **Window size:** 1000 ms.
- **Window stride:** 500 ms.
- **Zero-padding:** bật cho file quá ngắn.
- **DSP block:** MFCC.

Cấu hình MFCC:

- **MFCC coefficients:** 13.
- **Frame length:** 25 ms.
- **Frame stride:** 20 ms.
- **Mel filters:** 32.
- **FFT size:** 512.
- **Low-frequency cutoff:** 80 Hz.
- **Pre-emphasis coefficient:** 0,98.
- **Output features:** 637 đặc trưng đầu vào cho mạng học sâu.

## 3. Kiến trúc và huấn luyện

- **Mô hình:** 1D CNN trên đặc trưng MFCC.
- **Input/Reshape:** 637 đặc trưng MFCC được reshape thành biểu diễn 2 chiều theo trục thời gian và hệ số.
- **Conv blocks:** 3 lớp `Conv1D` nối tiếp với số filters tăng dần **16 -> 32 -> 64**, `kernel size = 3`, `padding = same`, activation `ReLU`.
- **Dropout:** tỷ lệ **0,25** xen kẽ sau các khối tích chập.
- **Classifier:** `Flatten` -> Dense 32 neurons -> Softmax 2 lớp.
- **Classes:** `scream`, `non-scream`.
- **Data augmentation:** noise injection và frequency masking.
- **Class balancing:** Auto-weight class.
- **Learning rate:** 0,001.
- **Batch size:** 128.
- **Epochs:** tối đa 100.

## 4. Hiệu năng và đánh giá

### Validation trên Edge Impulse

- **Accuracy:** 89,2%.
- **Precision trung bình:** 0,92.
- **Recall trung bình:** 0,89.
- **F1-score trung bình:** 0,90.
- **ROC-AUC:** 0,87.
- **Recall lớp `scream`:** 84,8%.
- **Recall lớp `non-scream`:** 89,8%.

### Test độc lập trong đồ án

- **Accuracy:** 89,23%.
- **Precision:** 92%.
- **Recall:** 90%.
- **F1-score:** 90%.
- **AUC:** 86%.
- **Confusion matrix:** `non-scream` dự đoán đúng 90,3%, nhầm sang `scream` 9,4%, `uncertain` 0,3%; `scream` dự đoán đúng 82,1%, nhầm sang `non-scream` 17,6%, `uncertain` 0,3%.

### Profile Edge deployment

- **Quantization:** int8.
- **Inference time:** 15 ms classifier.
- **Peak RAM:** 21,7 KB.
- **Flash/ROM model:** 57,2 KB.
- **Compiler:** EON Compiler.

### Chạy thực tế trên firmware multi-impulse

Khi đã tích hợp cùng mô hình phát hiện ngã trong firmware, mô hình tiếng hét tốn khoảng **27 ms** mỗi lần suy luận, gồm **24 ms DSP** và **3 ms classifier**.

## 5. Triển khai

Mô hình được xuất từ Edge Impulse dưới dạng C++ library, lượng tử hóa int8 và hợp nhất với mô hình Fall Detection bằng Multi-impulse Deployment Block. Thư viện cuối cùng được đưa vào firmware PlatformIO cho XIAO ESP32-S3.

## 6. Tài liệu tham khảo

- Script minh họa tiền xử lý: `model_training/scream_detection/resample_audio.py`
- Human Screaming Detection Dataset: https://www.kaggle.com/datasets/whats2000/human-screaming-detection-dataset/data
- Audio Dataset of Scream and Non Scream: https://www.kaggle.com/datasets/aananehsansiam/audio-dataset-of-scream-and-non-scream
- Edge Impulse project: https://studio.edgeimpulse.com/public/916888/live
