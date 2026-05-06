# Multi-model AI Deployment: Fall Detection & Scream Detection

## Tổng quan

Dự án này hiện thực việc nhận diện đồng thời hai hiện tượng quan trọng trong chăm sóc sức khoẻ thông minh:  
- **Phát hiện ngã (Fall Detection)**
- **Phát hiện tiếng la (Scream Detection)**

Mỗi mô hình được xây dựng, huấn luyện và đánh giá độc lập trên Edge Impulse (sử dụng dữ liệu riêng, pipeline đặc thù cho từng nhiệm vụ). Yêu cầu của hệ thống là phải triển khai song song nhiều mô hình trên cùng một thiết bị nhúng (ví dụ: XIAO ESP32-S3) với hiệu năng tối ưu và bộ nhớ hạn chế.

## Giải pháp hợp nhất mô hình

Để hiện thực multi-task AI trên thiết bị nhúng, các mô hình được xuất ra dưới dạng C++ library và kết hợp lại thành **một thư viện duy nhất** nhờ tận dụng bộ công cụ mã nguồn mở [edgeimpulse/multi-impulse-deployment-block](https://github.com/edgeimpulse/multi-impulse-deployment-block) của Edge Impulse.

Kiến trúc này cho phép:
- Tích hợp nhiều mô hình inference vào chung một binary/firmware.
- Dữ liệu cảm biến hoặc tín hiệu audio được tự động định tuyến đến đúng mô hình.
- Quản lý tài nguyên bộ nhớ, RAM và flash tốt nhờ EON Compiler.

## Cách tổ chức và cấu trúc

1. **Mỗi mô hình AI** được phát triển, đánh giá, xuất khẩu C++ library riêng biệt từ Edge Impulse.
2. **Hợp nhất mô hình**:  
    Hai (hoặc nhiều) thư viện C++ được tổ chức lại trong cấu trúc chuẩn theo khung của Edge Impulse Multi-Impulse.  
    Hệ thống tạo một bộ API duy nhất cho phép gọi từng mô hình theo tên/class đã đăng ký.
3. **Nội dung thư viện hợp nhất**:
    - Chứa toàn bộ mã nguồn, weight, tham số của từng mô hình.
    - Giữ namespace riêng biệt, không xung đột tài nguyên/hàm.
    - Có cấu hình định tuyến input (audio chuyển về scream, imu chuyển về fall...).
4. **Triển khai**:  
    Bộ thư viện cuối cùng được nạp lên vi điều khiển, có thể sử dụng trực tiếp trên Arduino IDE hoặc hệ thống nhúng C++ bất kỳ.

## Quy trình tổng quát

### Bước 1: Chuẩn bị trên Edge Impulse Studio

1. Vào từng project (ví dụ: Fall Detection, Scream Detection).
2. Vào **Dashboard → Keys** để lấy các mã API Key. Bạn cần copy đầy đủ từng key cho mỗi project.
3. Đảm bảo dự án Edge Impulse đã cấu hình **Quantization: Quantized (int8)** phù hợp với vi điều khiển (ESP32-S3).

### Bước 2: Thiết lập môi trường Local

1. Chuẩn bị môi trường sạch:
    ```bash
    python3 -m venv venv
    source venv/bin/activate
    ```
2. Di chuyển vào thư mục chứa mã nguồn `multi-impulse-deployment-block`.
3. Cài đặt các thư viện cần thiết:
    ```bash
    pip install -r requirements.txt
    ```
    > Nếu thiếu file này, hãy đảm bảo có `requests` và các lib Python cho thao tác file.

### Bước 3: Chạy script hợp nhất (generate)

Chạy lệnh sau, thay KEY_CUA_FALL, KEY_CUA_SCREAM bằng API Key thực tế bạn lấy ở Bước 1:

```bash
python3 generate.py --out-directory ./output \
    --api-keys KEY_CUA_FALL,KEY_CUA_SCREAM \
    --quantization-map 1,1 \
    --force-build
```

- `--api-keys`: API keys của các project, phân cách bởi dấu phẩy.
- `--quantization-map`: Ví dụ `1,1` nếu tất cả đều Quantized (int8); dùng `0` nếu xuất dạng float.
- `--force-build`: Yêu cầu Edge Impulse build firmware mới nhất mỗi lần, tránh lỗi cache.

### Bước 4: Xử lý file đầu ra và tích hợp lên ESP32-S3

1. Sau khi script chạy xong sẽ nhận được một file `deploy.zip` trong thư mục `output/`.
2. Giải nén file này ra sẽ được cấu trúc một thư viện C++ đầy đủ gồm 2 mô hình.

## Tham khảo

- Repo hợp nhất chuẩn: [edgeimpulse/multi-impulse-deployment-block](https://github.com/edgeimpulse/multi-impulse-deployment-block)  
- Chi tiết technical: [Edge Impulse Multiple Impulses Guide](https://docs.edgeimpulse.com/tutorials/topics/inference/run-multiple-impulses-cpp#locally)

---

**Tóm lại:**  
Dự án triển khai kiến trúc multi-impulse, cho phép phát hiện ngã và tiếng la đồng thời trên cùng một thiết bị nhúng với hiệu năng cao, dễ bảo trì, nâng cấp và tích hợp mở rộng về sau.