import os
import glob
import pandas as pd
import numpy as np
import warnings
from scipy.interpolate import interp1d

warnings.filterwarnings('ignore')

# --- CẤU HÌNH ---
BASE_PATH = "/home/trieu/Documents/dataset_fall+adl/WEDA-FALL-main/dataset"
TARGET_FREQ = "50Hz"
FREQ_INTERVAL = 0.02  # 50Hz
OUTPUT_DIR = os.path.join(BASE_PATH, "processed_individual") # Thư mục mới cho file lẻ

# Tạo cấu trúc thư mục Training/Testing riêng biệt
os.makedirs(os.path.join(OUTPUT_DIR, "training"), exist_ok=True)
os.makedirs(os.path.join(OUTPUT_DIR, "testing"), exist_ok=True)

# 🎯 CHIẾN LƯỢC CHIA TEST
TEST_USERS = ['U09', 'U10', 'U14', 'U23', 'U26', 'U29']

# Load timestamp "chính chủ" (Ground Truth)
fall_ts_path = os.path.join(BASE_PATH, "fall_timestamps.csv")
if os.path.exists(fall_ts_path):
    fall_ts_df = pd.read_csv(fall_ts_path)
    print("✅ Đã load fall_timestamps.csv")
else:
    print("⚠️ Không tìm thấy fall_timestamps.csv -> Dùng Auto-Detect.")
    fall_ts_df = pd.DataFrame()

def get_manual_fall_range(filename_key):
    if fall_ts_df.empty: return None, None
    match = fall_ts_df[fall_ts_df['filename'].str.contains(filename_key, na=False)]
    if not match.empty:
        return match.iloc[0]['start_time'], match.iloc[0]['end_time']
    return None, None

def detect_fall_heuristic(accel_x, accel_y, accel_z, time_arr):
    # Tìm đỉnh va chạm mạnh nhất để xác định vị trí ngã
    magnitude = np.sqrt(accel_x**2 + accel_y**2 + accel_z**2)
    peak_idx = np.argmax(magnitude)
    peak_time = time_arr[peak_idx]
    peak_value = magnitude[peak_idx]

    if peak_value < 15.0: return None, None # Rung quá nhẹ, bỏ qua

    # Cắt cửa sổ: lấy -1s đến +2s quanh đỉnh
    start_time = max(time_arr[0], peak_time - 1.0)
    end_time = min(time_arr[-1], peak_time + 2.0)
    return start_time, end_time

def process_and_save_single_file(accel_path, activity_dir, is_fall_file):
    # 1. Kiểm tra file Gyro
    gyro_path = accel_path.replace("_accel.csv", "_gyro.csv")
    if not os.path.exists(gyro_path): return False

    filename = os.path.basename(accel_path) 
    try:
        parts = filename.split('_') 
        user_id = parts[0]          # U01
        repetition = parts[1]       # R01
        base_name = f"{activity_dir}_{user_id}_{repetition}" # Tên file đích: D01_U01_R01
    except:
        return False

    # 2. Đọc và Resample về chuẩn 50Hz (Fix lỗi timestamp lệch)
    accel = pd.read_csv(accel_path)
    gyro = pd.read_csv(gyro_path)
    
    accel_t_raw = accel['accel_time_list'].values
    gyro_t_raw = gyro['gyro_time_list'].values
    
    # Tạo trục thời gian mới bắt đầu từ 0 cho file này
    duration = accel_t_raw[-1] - accel_t_raw[0]
    target_t = np.arange(0, duration, FREQ_INTERVAL) # 0.00, 0.02, ...
    
    if len(target_t) < 10: return False # File quá ngắn

    # Hàm nội suy (map từ thời gian gốc sang thời gian 0 based chuẩn)
    # Lưu ý: cần map dữ liệu gốc vào trục thời gian (target_t + start_t_gốc)
    real_target_t = target_t + accel_t_raw[0] # Khôi phục time thực để map

    def res(src_t, src_val):
        return interp1d(src_t, src_val, bounds_error=False, fill_value="extrapolate")(real_target_t)

    new_ax, new_ay, new_az = res(accel_t_raw, accel['accel_x_list']), res(accel_t_raw, accel['accel_y_list']), res(accel_t_raw, accel['accel_z_list'])
    new_gx, new_gy, new_gz = res(gyro_t_raw, gyro['gyro_x_list']), res(gyro_t_raw, gyro['gyro_y_list']), res(gyro_t_raw, gyro['gyro_z_list'])

    # 3. Tạo DataFrame
    df = pd.DataFrame({
        'timestamp': np.round(target_t, 5), # Timestamp chuẩn 0 based
        'accel_x': new_ax, 'accel_y': new_ay, 'accel_z': new_az,
        'gyro_x': new_gx,   'gyro_y': new_gy,  'gyro_z': new_gz,
        'label': 'normal'
    })

    # 4. Xử lý logic Fall
    if is_fall_file:
        key = f"{activity_dir}/{user_id}_{repetition}"
        
        # Mốc thời gian thực trong file gốc
        start_real, end_real = get_manual_fall_range(key)
        
        # Nếu không có manual label, dùng auto detect trên dữ liệu mới
        if start_real is None:
            # Detect trên trục time 0-based của target_t
            start_local, end_local = detect_fall_heuristic(new_ax, new_ay, new_az, target_t)
            if start_local is not None:
                mask = (target_t >= start_local) & (target_t <= end_local)
                df.loc[mask, 'label'] = 'fall'
            else:
                return False # File Fall mà không detect được gì -> Bỏ
        else:
            # Chuyển đổi mốc thời gian thực về mốc 0-based
            start_local = start_real - accel_t_raw[0]
            end_local = end_real - accel_t_raw[0]
            mask = (target_t >= start_local) & (target_t <= end_local)
            df.loc[mask, 'label'] = 'fall'

    # 5. Lưu File Vào Folder Tương Ứng
    save_folder = "testing" if user_id in TEST_USERS else "training"
    save_path = os.path.join(OUTPUT_DIR, save_folder, base_name + ".csv")
    
    df.to_csv(save_path, index=False, float_format='%.5f')
    return True

def main():
    print(f"🚀 Bắt đầu tách và xử lý từng file {TARGET_FREQ}...")
    freq_path = os.path.join(BASE_PATH, TARGET_FREQ)
    
    count = 0
    subdirs = sorted([d for d in os.listdir(freq_path) if os.path.isdir(os.path.join(freq_path, d))])
    
    for subdir in subdirs:
        is_fall = subdir.startswith("F")
        folder_path = os.path.join(freq_path, subdir)
        files = sorted(glob.glob(os.path.join(folder_path, "*_accel.csv")))

        for f_path in files:
            if process_and_save_single_file(f_path, subdir, is_fall):
                count += 1
                if count % 100 == 0: print(f"Processing... {count} files done.")

    print(f"\n✅ HOÀN TẤT! Đã tạo {count} file riêng biệt.")
    print(f"📂 Thư mục chứa file Training: {os.path.join(OUTPUT_DIR, 'training')}")
    print(f"📂 Thư mục chứa file Testing : {os.path.join(OUTPUT_DIR, 'testing')}")

if __name__ == "__main__":
    main()
