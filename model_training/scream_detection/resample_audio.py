import os
import librosa
import soundfile as sf
from tqdm import tqdm

# Script minh họa/tái lập quy trình chuẩn hóa audio; dữ liệu trong scream_dataset
# của repo đã được tiền xử lý sẵn, không cần chạy lại nếu chỉ huấn luyện lại model.
# Cấu hình đường dẫn
base_dir = os.path.dirname(__file__)
input_root_dir = os.path.join(base_dir, 'scream_dataset')
output_root_dir = os.path.join(base_dir, 'scream_dataset_16k')
target_sr = 16000  # Tần số mẫu mục tiêu cho Edge Impulse

def resample_dataset(input_dir, output_dir, sr):
    # Tạo thư mục đầu ra nếu chưa tồn tại
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # Duyệt qua các thư mục con và file
    total_files = sum([len(files) for r, d, files in os.walk(input_dir)])
    
    with tqdm(total=total_files, desc="Đang xử lý") as pbar:
        for root, dirs, files in os.walk(input_dir):
            # Xác định cấu trúc thư mục tương ứng ở đầu ra
            rel_path = os.path.relpath(root, input_dir)
            target_path = os.path.join(output_dir, rel_path)
            
            if not os.path.exists(target_path):
                os.makedirs(target_path)

            for file in files:
                if file.lower().endswith(('.wav', '.mp3', '.flac', '.ogg')):
                    input_file_path = os.path.join(root, file)
                    output_file_path = os.path.join(target_path, file)

                    try:
                        # Load file âm thanh và resample ngay lập tức
                        # sr=target_sr buộc librosa resample khi load
                        y, s = librosa.load(input_file_path, sr=sr)
                        
                        # Lưu file với tần số mẫu mới
                        sf.write(output_file_path, y, sr)
                    except Exception as e:
                        print(f"Lỗi khi xử lý file {file}: {e}")
                
                pbar.update(1)

if __name__ == "__main__":
    print(f"Bắt đầu chuyển đổi dữ liệu từ '{input_root_dir}' sang '{output_root_dir}' với tần số {target_sr}Hz...")
    resample_dataset(input_root_dir, output_root_dir, target_sr)
    print("Hoàn tất!")
