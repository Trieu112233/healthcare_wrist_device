# Multi-model AI Deployment: Fall Detection & Scream Detection

## Tổng quan

Thư mục này chứa dữ liệu đã tiền xử lý và mô tả quy trình huấn luyện hai mô hình TinyML dùng trong đồ án:

- **Phát hiện ngã (Fall Detection)** từ dữ liệu IMU 6 trục.
- **Phát hiện tiếng hét/kêu cứu (Scream Detection)** từ tín hiệu âm thanh.

Hai mô hình được xây dựng, huấn luyện, đánh giá và lượng tử hóa độc lập trên Edge Impulse, sau đó được hợp nhất để chạy song song trên thiết bị XIAO ESP32-S3.

## Cấu trúc thư mục

- `fall_detection/`: dữ liệu WEDA-FALL đã tiền xử lý, script minh họa quy trình tiền xử lý và tài liệu mô hình phát hiện ngã.
- `scream_detection/`: dữ liệu âm thanh đã chuẩn hóa, script minh họa quy trình chuẩn hóa âm thanh và tài liệu mô hình phát hiện tiếng hét.

## Giải pháp hợp nhất mô hình

Đồ án sử dụng công cụ mã nguồn mở [edgeimpulse/multi-impulse-deployment-block](https://github.com/edgeimpulse/multi-impulse-deployment-block) để hợp nhất hai C++ library xuất từ Edge Impulse thành một thư viện duy nhất.

Mục tiêu của bước này:

- Tránh lỗi `Multiple Definition` khi đưa hai thư viện Edge Impulse riêng lẻ vào cùng firmware.
- Cô lập namespace và biến số của từng mô hình.
- Loại bỏ các khối xử lý lõi bị trùng lặp.
- Cung cấp API định tuyến: dữ liệu IMU đi vào mô hình Fall, dữ liệu microphone đi vào mô hình Scream.

## Quy trình tổng quát

### 1. Chuẩn bị trên Edge Impulse Studio

1. Tạo hai project độc lập: Fall Detection và Scream Detection.
2. Vào **Dashboard -> Keys** để lấy API key của từng project.
3. Xuất model ở chế độ **Quantized (int8)** và bật **EON Compiler** để tối ưu cho ESP32-S3.

### 2. Sinh thư viện multi-impulse

Chạy trong thư mục của `multi-impulse-deployment-block`:

```bash
python3 generate.py --out-directory ./output \
    --api-keys KEY_CUA_FALL,KEY_CUA_SCREAM \
    --quantization-map 1,1 \
    --force-build
```

- `--api-keys`: API keys của hai project, phân cách bằng dấu phẩy.
- `--quantization-map 1,1`: ép cả hai mô hình dùng định dạng int8.
- `--force-build`: yêu cầu Edge Impulse build lại artifact mới nhất.

Sau khi hoàn tất, file `deploy.zip` trong `output/` được giải nén và tích hợp vào firmware PlatformIO.

## Kết quả khớp với đồ án

- Mô hình Fall Detection trên tập test độc lập đạt accuracy **85,96%**, precision **87%**, recall **86%**, F1-score **86%**, AUC **84%**.
- Mô hình Scream Detection trên tập test độc lập đạt accuracy **83,70%**, precision **91%**, recall **84%**, F1-score **86%**, AUC **85%**.
- Firmware đã gồm cả hai mô hình int8 chiếm **2,06 MB flash**.
- RAM tĩnh theo PlatformIO là **141,8 KB**; RAM động sau suy luận khoảng **117,5 KB**; peak RAM khi chạy AI khoảng **125,2 KB**.
- Thời gian suy luận thực tế trong firmware: Fall **173 ms**, Scream **27 ms**, tổng mỗi chu kỳ AI khoảng **200 ms**.

## Tham khảo

- `fall_detection/README.md`
- `scream_detection/README.md`
- [edgeimpulse/multi-impulse-deployment-block](https://github.com/edgeimpulse/multi-impulse-deployment-block)
- [Edge Impulse Multiple Impulses Guide](https://docs.edgeimpulse.com/tutorials/topics/inference/run-multiple-impulses-cpp#locally)
