# Fall Detection Model Training Documentation

## 1. Dataset

- **Nguồn dữ liệu:** WEDA-FALL (`joaojtmarques/WEDA-FALL`), gồm tín hiệu IMU 6 bậc tự do từ cảm biến đeo cổ tay: `accel_x`, `accel_y`, `accel_z`, `gyro_x`, `gyro_y`, `gyro_z`.
- **Dữ liệu gốc:** các hoạt động sống thường ngày (ADL, thư mục `Dxx`) và các sự kiện té ngã (thư mục `Fxx`).
- **Script tiền xử lý:** `preprocess_edge_impulse.py`, giữ lại để minh họa/tái lập quy trình xử lý đã dùng trong đồ án.
- **Dữ liệu dùng để huấn luyện:** `WEDA-FALL-main/dataset/processed_individual`, đây là dữ liệu đã tiền xử lý sẵn và có thể upload lên Edge Impulse trực tiếp.

Quy trình tiền xử lý đã thực hiện và được thể hiện trong script:

- Nội suy tín hiệu gia tốc và con quay hồi chuyển về cùng trục thời gian bắt đầu từ 0.
- Chuẩn hóa tần số lấy mẫu về **50 Hz**, tương ứng bước thời gian **0,02 s**.
- Mặc định gán nhãn `normal` cho dữ liệu.
- Với file ngã, ưu tiên dùng `fall_timestamps.csv` để lấy mốc ngã thủ công.
- Nếu thiếu mốc thủ công, dùng heuristic theo độ lớn gia tốc tổng hợp, ngưỡng va chạm **15.0**, rồi gán nhãn `fall` cho đoạn **3 giây** quanh đỉnh va chạm: 1 giây trước đỉnh và 2 giây sau đỉnh.
- Chia train/test theo người dùng để tránh rò rỉ dữ liệu giữa huấn luyện và kiểm thử. Nhóm test trong script: `U09`, `U10`, `U14`, `U23`, `U26`, `U29`.

Không cần chạy lại script nếu chỉ muốn kiểm tra hoặc tái huấn luyện theo dữ liệu hiện có trong repo.

## 2. Cấu hình Impulse trên Edge Impulse

- **Input:** 6 trục IMU (`accel_x`, `accel_y`, `accel_z`, `gyro_x`, `gyro_y`, `gyro_z`).
- **Sample rate:** 50 Hz.
- **Window size:** 4000 ms.
- **Window stride:** 1000 ms.
- **Zero-padding:** bật.
- **Multi-label rule:** nếu một phần cửa sổ 4 giây chứa nhãn `fall`, toàn bộ cửa sổ được gán là `fall`.
- **Classes:** `fall`, `normal`.
- **Feature extraction:** dùng trực tiếp chuỗi IMU thô, không qua khối trích xuất đặc trưng thủ công.

## 3. Kiến trúc và huấn luyện

- **Mô hình:** Fully Convolutional Network 1D (FCN-1D).
- **Reshape:** đầu vào được đưa về ma trận `(window_length, 6)`.
- **Feature extraction:** 3 lớp `Conv1D`, mỗi lớp **64 filters**, `kernel size = 3`, `padding = same`, activation `ReLU`.
- **Batch Normalization:** sau các khối tích chập để ổn định hội tụ.
- **Dropout:** tỷ lệ **0,2** ở khối 1 và khối 2; khối 3 không dùng dropout.
- **Pooling:** `GlobalAveragePooling1D`.
- **Output:** Dense + Softmax cho 2 lớp, có **L2 regularization = 0,001**.
- **Loss:** Focal Loss với `gamma = 2.0`, `alpha = 0.25`.
- **Optimizer:** Adam, learning rate **0,0005**, `beta1 = 0,9`, `beta2 = 0,999`.
- **Batch size:** 128.
- **Epochs:** tối đa 100.

## 4. Hiệu năng và đánh giá

### Validation trên Edge Impulse

- **Accuracy:** 89,2%.
- **F1-score trung bình:** 0,89.
- **ROC-AUC:** 0,88.
- **Recall lớp `fall`:** 84,6%.
- **Recall lớp `normal`:** 91,1%.

Các lỗi chủ yếu xuất hiện ở dữ liệu vung tay mạnh, hoạt động mạnh không phải ngã, hoặc các đoạn nằm ở ranh giới pha ngã.

### Test độc lập trong đồ án

- **Accuracy:** 85,96%.
- **Precision:** 87%.
- **Recall:** 86%.
- **F1-score:** 86%.
- **AUC:** 84%.
- **Confusion matrix:** `fall` dự đoán đúng 78,4%, nhầm sang `normal` 20,5%, `uncertain` 1,1%; `normal` dự đoán đúng 88,7%, nhầm sang `fall` 10,9%, `uncertain` 0,3%.

### Profile Edge deployment

- **Quantization:** int8.
- **Inference time:** 586 ms cho một cửa sổ.
- **Peak RAM:** 31,2 KB.
- **Flash/ROM model:** 72,3 KB.
- **Compiler:** EON Compiler.

### Chạy thực tế trên firmware multi-impulse

- Trên 120 tình huống ngã mô phỏng, hệ thống phát hiện đúng **103** trường hợp, tương ứng khoảng **85,8%**; bỏ sót **17** trường hợp.
- Khi đã tích hợp cùng mô hình tiếng hét trong firmware, mô hình ngã tốn khoảng **173 ms** mỗi lần suy luận, gồm **2 ms DSP** và **170 ms classifier**.

## 5. Triển khai

Mô hình được xuất từ Edge Impulse dưới dạng C++ library, lượng tử hóa int8 và hợp nhất với mô hình Scream Detection bằng Multi-impulse Deployment Block. Thư viện cuối cùng được đưa vào firmware PlatformIO cho XIAO ESP32-S3.

## 6. Tài liệu tham khảo

- WEDA-FALL Dataset: https://github.com/joaojtmarques/WEDA-FALL
- Script minh họa tiền xử lý: `model_training/fall_detection/preprocess_edge_impulse.py`
- Dữ liệu đã tiền xử lý: `model_training/fall_detection/WEDA-FALL-main/dataset/processed_individual`
- Edge Impulse project: https://studio.edgeimpulse.com/public/868007/live
