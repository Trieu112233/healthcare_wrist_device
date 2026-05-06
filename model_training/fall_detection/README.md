# Fall Detection Model Training Documentation

## 1. Dataset

- **Nguồn dữ liệu:**  
  Sử dụng bộ dữ liệu WEDA-FALL (joaojtmarques/WEDA-FALL), gồm dữ liệu gia tốc kế (accel) và con quay hồi chuyển (gyro) 6 trục từ cảm biến đeo trên cổ tay trong các tình huống hoạt động thường ngày và ngã.

- **Tiền xử lý:**  
  - Được thực hiện bằng script `preprocess_edge_impulse.py` trong repo này.
  - Quá trình xử lý thực hiện chuẩn hóa tần số dữ liệu về 50Hz, cắt các phân đoạn ngắn gọn từng hoạt động, tự động gán nhãn `fall` (ngã) hoặc `normal` (bình thường) dựa trên dữ liệu hoặc nhãn có sẵn.
  - Kết quả là tập hợp các file .csv độc lập, mỗi file tương ứng với một phiên ghi hình, đã chia thành hai tập `training` và `testing` để đảm bảo kiểm thử công bằng, không trùng lặp người dùng giữa train và test.
  Tập hợp các file trên được đặt ở trong thư mục `WEDA-FALL-main/dataset/processed_individual`.

## 2. Cấu hình Impulse trên Edge Impulse

- **Dữ liệu đầu vào:** 6 trục cảm biến gồm accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z.
- **Cửa sổ dữ liệu (window size):** 4000 mili-giây (4 giây) cho mỗi mẫu vào mạng.
- **Tần số mẫu:** 50 Hz.
- **Stride (bước nhảy cửa sổ):** 1000 mili-giây (1 giây).
- **Chế độ zero-padding:** Có sử dụng, đảm bảo đủ kích thước khi dữ liệu thiếu.
- **Cách xử lý multi-label:** Nếu có gán nhãn "fall" ở bất kỳ đâu trong một cửa sổ thì toàn bộ cửa sổ đó được gán là "fall".
- **Các lớp:** `fall` (ngã) và `normal` (bình thường).
- **Dùng trực tiếp dữ liệu thô, không cần qua phase trích xuất đặc trưng bổ sung.**

## 3. Cấu trúc và Huấn luyện Mô hình

- **Thuật toán:** Fully Convolutional Network 1D (FCN) cho tín hiệu thời gian.
- **Kiến trúc mạng:**
    - Input dạng chuỗi nhiều chiều (window_size x 6).
    - 3 lớp Conv1D liên tiếp (64 filters, kernel size 3, activation relu, batch normalization, dropout nhằm tránh overfitting).
    - GlobalAveragePooling1D để tóm tắt đặc trưng.
    - Dense (softmax) đầu ra với regularization.
- **Hàm mất mát:** Focal Loss (gamma=2.0, alpha=0.25) — nhấn mạnh vào các mẫu khó, giảm ảnh hưởng mất cân bằng class.
- **Tối ưu:** Adam optimizer, learning rate 0.0005.
- **Batch size:** 128.
- **Số epoch:** Tối đa 100.
- **Trọng số class:** Tính toán tự động dựa vào tỷ lệ fall/normal.
- **Chia bộ dữ liệu:** Khoảng 77% training, 23% test.

## 4. Hiệu năng và Đánh giá

- **Độ chính xác (Accuracy trên validation set):** Khoảng 89.2%.
- **F1-score trung bình:** 0.89.
- **Confusion matrix:**  
    - Đúng với mẫu ngã (fall): ~84.6%  
    - Đúng với mẫu bình thường (normal): ~91.1%  
    - Các lỗi chủ yếu xảy ra ở nhóm dữ liệu thực hiện động tác mạnh không phải ngã hoặc ở vùng ranh giới nhãn.
- **ROC-AUC:** 0.88.
- **Thời gian dự đoán trên vi điều khiển (inference time):** Khoảng 586 mili-giây cho mỗi cửa sổ.
- **Bộ nhớ RAM sử dụng:** 31.2 KB.
- **Dung lượng flash model:** 72.3 KB.
- **Model đã được lượng tử hóa (quantized int8) ngay khi huấn luyện để triển khai nhúng, phù hợp tài nguyên hạn chế của ESP32-S3.**

## 5. Triển khai trên thiết bị thực tế

- **Model output:** Đã lượng tử hóa sẵn (int8) để nhúng trên vi điều khiển.
- **Tích hợp:** Hỗ trợ xuất thư viện C++ cho TensorFlow Lite Micro.

## 6. Tài liệu tham khảo

- WEDA-FALL Dataset: https://github.com/joaojtmarques/WEDA-FALL  
- Tiền xử lý: `dataset/WEDA-FALL-main/preprocess_edge_impulse.py`  
- Dữ liệu sau xử lý: `dataset/WEDA-FALL-main/dataset/processed_individual`  
- Code huấn luyện mô hình: xem phần cơ chế `Neural network architecture` trong Edge Impulse project (https://studio.edgeimpulse.com/public/868007/live).

---
