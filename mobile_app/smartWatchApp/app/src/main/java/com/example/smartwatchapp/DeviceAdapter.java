package com.example.smartwatchapp;import android.app.AlertDialog;
import android.content.Context;
import android.content.SharedPreferences;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.google.firebase.firestore.FieldValue;
import com.google.firebase.firestore.FirebaseFirestore;

import java.util.List;

public class DeviceAdapter extends RecyclerView.Adapter<DeviceAdapter.DeviceViewHolder> {
    private List<Device> deviceList;

    public DeviceAdapter(List<Device> deviceList){
        this.deviceList = deviceList;
    }

    @NonNull
    @Override
    public DeviceViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.item_device, parent, false);
        return new DeviceViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull DeviceViewHolder holder, int position) {
        Device device = deviceList.get(position);

        // Hiển thị tên
        holder.tvDeviceName.setText(device.getName() != null ? device.getName() : "Thiết bị không tên");

        // Hiển thị trạng thái kết nối
        if (device.isOnline()) {
            holder.tvConnectionStatus.setText("● Online");
            holder.tvConnectionStatus.setTextColor(android.graphics.Color.parseColor("#00E676"));
        } else {
            holder.tvConnectionStatus.setText("● Offline");
            holder.tvConnectionStatus.setTextColor(android.graphics.Color.parseColor("#FF5252"));
        }

        // Hiển thị trạng thái an toàn
        if (device.isAlert()) {
            holder.tvSafetyStatus.setText(device.getAlert_type());
            holder.tvSafetyStatus.setTextColor(android.graphics.Color.parseColor("#FF5252"));
        } else {
            holder.tvSafetyStatus.setText("AN TOÀN");
            holder.tvSafetyStatus.setTextColor(android.graphics.Color.parseColor("#00E676"));
        }

        // --- CHỨC NĂNG XÓA THIẾT BỊ KHI NHẤN GIỮ ---
        holder.itemView.setOnLongClickListener(v -> {
            showDeleteDialog(v.getContext(), device.getId());
            return true;
        });
    }

    private void showDeleteDialog(Context context, String deviceId) {
        new AlertDialog.Builder(context)
                .setTitle("Xóa thiết bị")
                .setMessage("Bạn có chắc chắn muốn ngừng theo dõi thiết bị này không?")
                .setPositiveButton("Xóa", (dialog, which) -> {
                    deleteDeviceFromFirestore(context, deviceId);
                })
                .setNegativeButton("Hủy", null)
                .show();
    }

    private void deleteDeviceFromFirestore(Context context, String deviceId) {
        // Lấy số điện thoại người dùng từ SharedPreferences
        SharedPreferences prefs = context.getSharedPreferences("AUTH", Context.MODE_PRIVATE);
        String userPhone = prefs.getString("user_phone", "");

        if (userPhone.isEmpty()) return;

        // Cập nhật Firestore: Xóa deviceId khỏi mảng DeviceId
        FirebaseFirestore.getInstance().collection("users").document(userPhone)
                .update("DeviceId", FieldValue.arrayRemove(deviceId))
                .addOnSuccessListener(aVoid -> {
                    Toast.makeText(context, "Đã xóa thiết bị thành công", Toast.LENGTH_SHORT).show();
                    // Lưu ý: Danh sách sẽ tự cập nhật nhờ SnapshotListener ở MainActivity
                })
                .addOnFailureListener(e -> {
                    Toast.makeText(context, "Lỗi khi xóa: " + e.getMessage(), Toast.LENGTH_SHORT).show();
                });
    }

    @Override
    public int getItemCount() {
        return deviceList.size();
    }

    static class DeviceViewHolder extends RecyclerView.ViewHolder{
        TextView tvDeviceName, tvConnectionStatus, tvSafetyStatus;
        DeviceViewHolder(@NonNull View itemView){
            super(itemView);
            tvDeviceName = itemView.findViewById(R.id.tvDeviceName);
            tvConnectionStatus = itemView.findViewById(R.id.tvConnectionStatus);
            tvSafetyStatus = itemView.findViewById(R.id.tvSafetyStatus);
        }
    }
}