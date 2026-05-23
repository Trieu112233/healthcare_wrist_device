package com.example.smartwatchapp;

import android.Manifest;
import android.app.NotificationManager;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.PowerManager;
import android.provider.Settings;
import android.text.InputType;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.firebase.firestore.FieldValue;
import com.google.firebase.firestore.FirebaseFirestore;
import com.google.firebase.firestore.ListenerRegistration;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MainActivity extends AppCompatActivity {
    private static final int REQUEST_POST_NOTIFICATIONS = 101;

    private RecyclerView rvDevices;
    private DeviceAdapter adapter;
    private List<Device> deviceList;
    private FloatingActionButton fabAddDevice;
    private ImageView imgLogout;
    private FirebaseFirestore db;
    private String userPhone;

    private final Map<String, ListenerRegistration> deviceListeners = new HashMap<>();
    private ListenerRegistration userListener;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);

        db = FirebaseFirestore.getInstance();
        SharedPreferences prefs = getSharedPreferences("AUTH", MODE_PRIVATE);
        userPhone = prefs.getString("user_phone", "");

        if (userPhone.isEmpty()) {
            goToLogin();
            return;
        }

        requestAppPermissions();

        startAlertService();

        rvDevices = findViewById(R.id.mainRvDevices);
        fabAddDevice = findViewById(R.id.mainFabAddDevice);
        imgLogout = findViewById(R.id.mainImgLogout);

        deviceList = new ArrayList<>();
        adapter = new DeviceAdapter(deviceList);
        rvDevices.setLayoutManager(new LinearLayoutManager(this));
        rvDevices.setAdapter(adapter);

        loadUserDevices();

        fabAddDevice.setOnClickListener(v -> showAddDeviceDialog());
        imgLogout.setOnClickListener(v -> logout());
    }

    private void showAddDeviceDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Thêm thiết bị mới");

        final EditText input = new EditText(this);
        input.setHint("Nhập mã ID thiết bị (VD: xiao_esp32s3_03)");
        input.setInputType(InputType.TYPE_CLASS_TEXT);
        builder.setView(input);

        builder.setPositiveButton("Thêm", (dialog, which) -> {
            String deviceId = input.getText().toString().trim();
            if (!deviceId.isEmpty()) {
                checkAndAddDevice(deviceId);
            } else {
                Toast.makeText(this, "Vui lòng nhập mã ID", Toast.LENGTH_SHORT).show();
            }
        });

        builder.setNegativeButton("Hủy", (dialog, which) -> dialog.cancel());

        builder.show();
    }

    private void checkAndAddDevice(String deviceId) {
        db.collection("devices").document(deviceId).get()
                .addOnSuccessListener(documentSnapshot -> {
                    if (documentSnapshot.exists()) {
                        addDeviceIdToUser(deviceId);
                    } else {
                        Toast.makeText(this, "Mã thiết bị không tồn tại trên hệ thống!", Toast.LENGTH_LONG).show();
                    }
                })
                .addOnFailureListener(e -> {
                    Toast.makeText(this, "Lỗi kiểm tra thiết bị: " + e.getMessage(), Toast.LENGTH_SHORT).show();
                });
    }

    private void addDeviceIdToUser(String deviceId) {
        db.collection("users").document(userPhone)
                .update("DeviceId", FieldValue.arrayUnion(deviceId))
                .addOnSuccessListener(aVoid -> {
                    Toast.makeText(this, "Thêm thiết bị thành công!", Toast.LENGTH_SHORT).show();
                })
                .addOnFailureListener(e -> {
                    Toast.makeText(this, "Không thể thêm thiết bị: " + e.getMessage(), Toast.LENGTH_SHORT).show();
                });
    }

    private void startAlertService() {
        Intent intent = new Intent(this, FirestoreAlertService.class);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            startForegroundService(intent);
        } else {
            startService(intent);
        }
    }

    private void loadUserDevices() {
        userListener = db.collection("users").document(userPhone)
                .addSnapshotListener((value, error) -> {
                    if (error != null) return;

                    if (value != null && value.exists()) {
                        String name = value.getString("Name");
                        TextView tvName = findViewById(R.id.mainTextUserName);
                        TextView tvPhone = findViewById(R.id.mainTextUserPhone);
                        if (name != null) tvName.setText(name);
                        tvPhone.setText(userPhone);

                        @SuppressWarnings("unchecked")
                        List<String> deviceIds = (List<String>) value.get("DeviceId");
                        if (deviceIds != null) {
                            manageDeviceListeners(deviceIds);
                        }
                    }
                });
    }

    private void manageDeviceListeners(List<String> currentIds) {
        List<String> idsToRemove = new ArrayList<>();
        for (String id : deviceListeners.keySet()) {
            if (!currentIds.contains(id)) {
                deviceListeners.get(id).remove();
                idsToRemove.add(id);
                removeFromList(id);
            }
        }
        for (String id : idsToRemove) deviceListeners.remove(id);

        for (String id : currentIds) {
            if (!deviceListeners.containsKey(id)) {
                listenToDeviceDetails(id);
            }
        }
    }

    private void listenToDeviceDetails(String deviceId) {
        ListenerRegistration reg = db.collection("devices").document(deviceId)
                .addSnapshotListener((doc, e) -> {
                    if (doc != null && doc.exists()) {
                        Device device = doc.toObject(Device.class);
                        if (device != null) {
                            device.setId(doc.getId());
                            updateList(device);
                        }
                    }
                });
        deviceListeners.put(deviceId, reg);
    }

    private void updateList(Device newDevice) {
        int index = -1;
        for (int i = 0; i < deviceList.size(); i++) {
            if (deviceList.get(i).getId().equals(newDevice.getId())) {
                index = i;
                break;
            }
        }
        if (index != -1) {
            deviceList.set(index, newDevice);
        } else {
            deviceList.add(newDevice);
        }
        adapter.notifyDataSetChanged();
    }

    private void removeFromList(String deviceId) {
        for (int i = 0; i < deviceList.size(); i++) {
            if (deviceList.get(i).getId().equals(deviceId)) {
                deviceList.remove(i);
                adapter.notifyDataSetChanged();
                break;
            }
        }
    }

    private void logout() {
        stopService(new Intent(this, FirestoreAlertService.class));
        if (userListener != null) userListener.remove();
        for (ListenerRegistration reg : deviceListeners.values()) reg.remove();

        getSharedPreferences("AUTH", MODE_PRIVATE).edit().clear().apply();
        goToLogin();
    }

    private void goToLogin() {
        Intent intent = new Intent(MainActivity.this, LoginActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
        startActivity(intent);
        finish();
    }

    @Override
    protected void onDestroy() {
        if (userListener != null) userListener.remove();
        for (ListenerRegistration reg : deviceListeners.values()) reg.remove();
        super.onDestroy();
    }

    private void requestAppPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.POST_NOTIFICATIONS)
                    != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this,
                        new String[]{Manifest.permission.POST_NOTIFICATIONS},
                        REQUEST_POST_NOTIFICATIONS);
            }
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.UPSIDE_DOWN_CAKE) {
            NotificationManager notificationManager = getSystemService(NotificationManager.class);
            if (notificationManager != null && !notificationManager.canUseFullScreenIntent()) {
                Toast.makeText(this, "Vui lòng bật thông báo toàn màn hình để nhận cảnh báo khi khóa màn hình", Toast.LENGTH_LONG).show();
                Intent intent = new Intent(Settings.ACTION_MANAGE_APP_USE_FULL_SCREEN_INTENT);
                intent.setData(Uri.parse("package:" + getPackageName()));
                startActivity(intent);
                return;
            }
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (!Settings.canDrawOverlays(this)) {
                Toast.makeText(this, "Vui lòng bật 'Hiển thị trên ứng dụng khác' để nhận cảnh báo", Toast.LENGTH_LONG).show();
                Intent intent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION,
                        Uri.parse("package:" + getPackageName()));
                startActivity(intent);
                return;
            }
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            PowerManager pm = (PowerManager) getSystemService(POWER_SERVICE);
            if (pm != null && !pm.isIgnoringBatteryOptimizations(getPackageName())) {
                Intent intent = new Intent(Settings.ACTION_REQUEST_IGNORE_BATTERY_OPTIMIZATIONS);
                intent.setData(Uri.parse("package:" + getPackageName()));
                startActivity(intent);
            }
        }
    }
}
