package com.example.smartwatchapp;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.media.AudioAttributes;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.Build;
import android.os.IBinder;
import android.os.SystemClock;

import androidx.core.app.NotificationCompat;

import com.google.firebase.Timestamp;
import com.google.firebase.firestore.FirebaseFirestore;
import com.google.firebase.firestore.ListenerRegistration;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class FirestoreAlertService extends Service {
    private static final String SERVICE_CHANNEL_ID = "AlertServiceChannel";
    private static final String EMERGENCY_CHANNEL_ID = "EmergencyAlertChannel";
    private static final int SERVICE_NOTIFICATION_ID = 1;
    private static final int EMERGENCY_NOTIFICATION_ID = 1001;

    private FirebaseFirestore db;
    private final List<ListenerRegistration> listeners = new ArrayList<>();
    private final Set<String> handledAlertKeys = new HashSet<>();
    private ListenerRegistration userListener;

    @Override
    public void onCreate() {
        super.onCreate();
        db = FirebaseFirestore.getInstance();
        createNotificationChannels();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Notification notification = new NotificationCompat.Builder(this, SERVICE_CHANNEL_ID)
                .setContentTitle("Hệ thống cảnh báo đang chạy")
                .setContentText("Đang giám sát thiết bị của bạn ...")
                .setSmallIcon(R.drawable.ic_launcher_foreground)
                .setPriority(NotificationCompat.PRIORITY_LOW)
                .build();

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            startForeground(
                    SERVICE_NOTIFICATION_ID,
                    notification,
                    android.content.pm.ServiceInfo.FOREGROUND_SERVICE_TYPE_DATA_SYNC);
        } else {
            startForeground(SERVICE_NOTIFICATION_ID, notification);
        }

        startListeningToDevices();

        return START_STICKY;
    }

    @Override
    public void onTaskRemoved(Intent rootIntent) {
        Intent restartServiceIntent = new Intent(getApplicationContext(), this.getClass());
        restartServiceIntent.setPackage(getPackageName());

        PendingIntent restartServicePendingIntent = PendingIntent.getService(
                getApplicationContext(),
                1,
                restartServiceIntent,
                PendingIntent.FLAG_ONE_SHOT | PendingIntent.FLAG_IMMUTABLE);

        android.app.AlarmManager alarmService =
                (android.app.AlarmManager) getApplicationContext().getSystemService(Context.ALARM_SERVICE);
        if (alarmService != null) {
            alarmService.set(
                    android.app.AlarmManager.ELAPSED_REALTIME,
                    SystemClock.elapsedRealtime() + 1000,
                    restartServicePendingIntent);
        }

        super.onTaskRemoved(rootIntent);
    }

    private void startListeningToDevices() {
        String phone = getSharedPreferences("AUTH", MODE_PRIVATE).getString("user_phone", "");
        if (phone.isEmpty()) return;

        if (userListener != null) {
            userListener.remove();
        }

        userListener = db.collection("users").document(phone).addSnapshotListener((snapshot, e) -> {
            if (snapshot != null && snapshot.exists()) {
                @SuppressWarnings("unchecked")
                List<String> deviceIds = (List<String>) snapshot.get("DeviceId");
                if (deviceIds != null) {
                    updateDeviceListeners(deviceIds);
                }
            }
        });
    }

    private void updateDeviceListeners(List<String> deviceIds) {
        for (ListenerRegistration lr : listeners) lr.remove();
        listeners.clear();

        for (String id : deviceIds) {
            ListenerRegistration lr = db.collection("devices").document(id)
                    .addSnapshotListener((doc, e) -> {
                        if (doc != null && doc.exists()) {
                            Boolean isAlert = doc.getBoolean("alert");
                            if (Boolean.TRUE.equals(isAlert)) {
                                String type = doc.getString("alert_type");
                                Object timeObj = doc.get("last_alert_time");
                                String time = formatAlertTime(timeObj);
                                String alertKey = buildAlertKey(id, type, timeObj);

                                if (handledAlertKeys.add(alertKey)) {
                                    triggerEmergencyAlert(id, type, time);
                                }
                            } else {
                                clearHandledAlertsForDevice(id);
                            }
                        }
                    });
            listeners.add(lr);
        }
    }

    private String formatAlertTime(Object timeObj) {
        if (timeObj instanceof Timestamp) {
            return ((Timestamp) timeObj).toDate().toString();
        } else if (timeObj != null) {
            return timeObj.toString();
        } else {
            return "Không rõ";
        }
    }

    private String buildAlertKey(String deviceId, String type, Object timeObj) {
        String timestampKey;
        if (timeObj instanceof Timestamp) {
            Timestamp ts = (Timestamp) timeObj;
            timestampKey = ts.getSeconds() + ":" + ts.getNanoseconds();
        } else if (timeObj != null) {
            timestampKey = timeObj.toString();
        } else {
            timestampKey = "no-time";
        }

        return deviceId + "|" + (type != null ? type : "UNKNOWN") + "|" + timestampKey;
    }

    private void clearHandledAlertsForDevice(String deviceId) {
        List<String> keysToRemove = new ArrayList<>();
        for (String key : handledAlertKeys) {
            if (key.startsWith(deviceId + "|")) {
                keysToRemove.add(key);
            }
        }

        for (String key : keysToRemove) {
            handledAlertKeys.remove(key);
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

        PendingIntent fullScreenIntent = PendingIntent.getActivity(
                this,
                buildNotificationRequestCode(deviceId, type, time),
                intent,
                PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE);

        String title = "Cảnh báo khẩn cấp";
        String content = "Thiết bị " + deviceId + ": " + formatAlertType(type);

        Notification notification = new NotificationCompat.Builder(this, EMERGENCY_CHANNEL_ID)
                .setSmallIcon(R.drawable.ic_launcher_foreground)
                .setContentTitle(title)
                .setContentText(content)
                .setStyle(new NotificationCompat.BigTextStyle().bigText(content + "\nThời gian: " + time))
                .setCategory(NotificationCompat.CATEGORY_ALARM)
                .setPriority(NotificationCompat.PRIORITY_MAX)
                .setVisibility(NotificationCompat.VISIBILITY_PUBLIC)
                .setOngoing(true)
                .setAutoCancel(false)
                .setContentIntent(fullScreenIntent)
                .setFullScreenIntent(fullScreenIntent, true)
                .build();

        NotificationManager manager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        if (manager != null) {
            manager.notify(EMERGENCY_NOTIFICATION_ID, notification);
        }
    }

    private int buildNotificationRequestCode(String deviceId, String type, String time) {
        return (deviceId + "|" + type + "|" + time).hashCode();
    }

    private String formatAlertType(String type) {
        if (type == null || type.isEmpty()) {
            return "CẢNH BÁO";
        }
        return type.replace("_", " ");
    }

    private void createNotificationChannels() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel serviceChannel = new NotificationChannel(
                    SERVICE_CHANNEL_ID,
                    "Alert Service",
                    NotificationManager.IMPORTANCE_DEFAULT);
            serviceChannel.setDescription("Kênh giám sát thiết bị đeo tay");

            Uri alarmSound = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_ALARM);
            AudioAttributes audioAttributes = new AudioAttributes.Builder()
                    .setUsage(AudioAttributes.USAGE_ALARM)
                    .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION)
                    .build();

            NotificationChannel emergencyChannel = new NotificationChannel(
                    EMERGENCY_CHANNEL_ID,
                    "Emergency Alerts",
                    NotificationManager.IMPORTANCE_HIGH);
            emergencyChannel.setDescription("Kênh cảnh báo khẩn cấp");
            emergencyChannel.setLockscreenVisibility(Notification.VISIBILITY_PUBLIC);
            emergencyChannel.enableVibration(true);
            emergencyChannel.setSound(alarmSound, audioAttributes);

            NotificationManager manager = getSystemService(NotificationManager.class);
            if (manager != null) {
                manager.createNotificationChannel(serviceChannel);
                manager.createNotificationChannel(emergencyChannel);
            }
        }
    }

    @Override
    public IBinder onBind(Intent intent) { return null; }

    @Override
    public void onDestroy() {
        if (userListener != null) {
            userListener.remove();
        }
        for (ListenerRegistration lr : listeners) lr.remove();
        listeners.clear();
        handledAlertKeys.clear();
        super.onDestroy();
    }
}
