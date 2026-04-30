package com.example.smartwatchapp;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;

import androidx.core.app.NotificationCompat;

import com.google.firebase.firestore.FirebaseFirestore;
import com.google.firebase.firestore.ListenerRegistration;

import java.util.ArrayList;
import java.util.List;

public class FirestoreAlertService extends Service {
    private static final String CHANNEL_ID = "AlertServiceChannel";
    private FirebaseFirestore db;
    private List<ListenerRegistration> listeners = new ArrayList<>();

    @Override
    public void onCreate() {
        super.onCreate();
        db = FirebaseFirestore.getInstance();
        createNotificationChannel();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Notification notification = new NotificationCompat.Builder(this, CHANNEL_ID)
                .setContentTitle("Hệ thống cảnh báo đang chạy")
                .setContentText("Đang giám sát thiết bị của bạn ...")
                .setSmallIcon(R.drawable.ic_launcher_foreground)
                .setPriority(NotificationCompat.PRIORITY_LOW)
                .build();


        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            startForeground(1, notification, android.content.pm.ServiceInfo.FOREGROUND_SERVICE_TYPE_DATA_SYNC);
        } else {
            startForeground(1, notification);
        }

        startListeningToDevices();

        return START_STICKY;
    }

    @Override
    public void onTaskRemoved(Intent rootIntent) {
        // Tạo một Intent để khởi động lại chính nó
        Intent restartServiceIntent = new Intent(getApplicationContext(), this.getClass());
        restartServiceIntent.setPackage(getPackageName());

        // Sử dụng PendingIntent để hệ thống kích hoạt lại sau khi bị đóng
        android.app.PendingIntent restartServicePendingIntent = android.app.PendingIntent.getService(
                getApplicationContext(), 1, restartServiceIntent,
                android.app.PendingIntent.FLAG_ONE_SHOT | android.app.PendingIntent.FLAG_IMMUTABLE);

        android.app.AlarmManager alarmService = (android.app.AlarmManager) getApplicationContext().getSystemService(Context.ALARM_SERVICE);
        alarmService.set(android.app.AlarmManager.ELAPSED_REALTIME,
                android.os.SystemClock.elapsedRealtime() + 1000,
                restartServicePendingIntent);

        super.onTaskRemoved(rootIntent);
    }

    private void startListeningToDevices() {
        String phone = getSharedPreferences("AUTH", MODE_PRIVATE).getString("user_phone", "");
        if(phone.isEmpty()) return;

        db.collection("users").document(phone).addSnapshotListener((snapshot, e) -> {
            if(snapshot != null && snapshot.exists()){
                List<String> deviceIds = (List<String>) snapshot.get("DeviceId");
                if(deviceIds != null) {
                    updateDeviceListeners(deviceIds);
                }
            }
        });
    }

    private void updateDeviceListeners(List<String> deviceIds) {
        for(ListenerRegistration lr : listeners) lr.remove();
        listeners.clear();

        for(String id : deviceIds) {
            ListenerRegistration lr = db.collection("devices").document(id)
                    .addSnapshotListener((doc, e) -> {
                        if(doc != null && doc.exists()) {
                            boolean isAlert = doc.getBoolean("alert");
                            if(isAlert) {
                                String type = doc.getString("alert_type");
                                Object timeObj = doc.get("last_alert_time");
                                String time = "";
                                if (timeObj instanceof com.google.firebase.Timestamp) {
                                    time = ((com.google.firebase.Timestamp) timeObj).toDate().toString();
                                } else if (timeObj != null) {
                                    time = timeObj.toString();
                                } else {
                                    time = "Không rõ";
                                }

                                triggerEmergencyAlert(id, type, time);
                            }
                        }
                    });
            listeners.add(lr);
        }
    }

    private void triggerEmergencyAlert(String deviceId, String type, String time) {
        Intent intent = new Intent(this, EmergencyActivity.class);
        intent.putExtra("device_id", deviceId);
        intent.putExtra("alert_type", type);
        intent.putExtra("last_alert_time", time);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK |
                Intent.FLAG_ACTIVITY_CLEAR_TOP |
                Intent.FLAG_ACTIVITY_SINGLE_TOP);
        startActivity(intent);
    }

    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel serviceChannel = new NotificationChannel(
                    CHANNEL_ID, "Alert Service", NotificationManager.IMPORTANCE_DEFAULT);
            serviceChannel.setDescription("Kênh giám sát thiết bị đeo tay");
            NotificationManager manager = getSystemService(NotificationManager.class);
            if (manager != null) {
                manager.createNotificationChannel(serviceChannel);
            }
        }
    }

    @Override
    public IBinder onBind(Intent intent) { return null; }

    @Override
    public void onDestroy() {
        for (ListenerRegistration lr : listeners) lr.remove();
        super.onDestroy();
    }
}
