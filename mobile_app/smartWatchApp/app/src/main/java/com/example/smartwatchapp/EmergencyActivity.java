package com.example.smartwatchapp;

import android.app.KeyguardManager;
import android.app.NotificationManager;
import android.content.Context;
import android.media.Ringtone;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.view.WindowManager;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.google.firebase.firestore.FirebaseFirestore;
import com.google.firebase.firestore.ListenerRegistration;

public class EmergencyActivity extends AppCompatActivity {
    private static final int EMERGENCY_NOTIFICATION_ID = 1001;

    private String deviceId;
    private Ringtone ringtone;
    private ListenerRegistration alertListener;
    private TextView tvAlertType;
    private TextView tvDeviceName;
    private TextView tvAlertTime;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O_MR1) {
            setShowWhenLocked(true);
            setTurnScreenOn(true);
            KeyguardManager keyguardManager = (KeyguardManager) getSystemService(Context.KEYGUARD_SERVICE);
            if (keyguardManager != null) {
                keyguardManager.requestDismissKeyguard(this, null);
            }
        } else {
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD |
                    WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED |
                    WindowManager.LayoutParams.FLAG_TURN_SCREEN_ON);
        }

        setContentView(R.layout.activity_emergency);

        tvAlertType = findViewById(R.id.tvAlertType);
        tvDeviceName = findViewById(R.id.tvDeviceName);
        tvAlertTime = findViewById(R.id.tvAlertTime);

        deviceId = getIntent().getStringExtra("device_id");
        String type = getIntent().getStringExtra("alert_type");
        String time = getIntent().getStringExtra("last_alert_time");
        if (time == null) time = "Không rõ";

        tvAlertType.setText(type != null ? type.replace("_", " ") : "CẢNH BÁO");
        tvDeviceName.setText("Thiết bị: " + (deviceId != null ? deviceId : "Không rõ"));
        tvAlertTime.setText("Thời gian: " + time);

        Uri notification = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_ALARM);
        ringtone = RingtoneManager.getRingtone(getApplicationContext(), notification);
        if (ringtone != null) ringtone.play();

        Vibrator vibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
        if (vibrator != null) {
            vibrator.vibrate(VibrationEffect.createOneShot(3000, VibrationEffect.DEFAULT_AMPLITUDE));
        }

        findViewById(R.id.btnAcknowledge).setOnClickListener(v1 -> acknowledgeAlert());
        listenToSelfClose();
    }

    private void acknowledgeAlert() {
        if (deviceId == null) {
            stopEffects();
            dismissEmergencyNotification();
            finish();
            return;
        }

        FirebaseFirestore.getInstance().collection("devices").document(deviceId)
                .update("alert", false)
                .addOnSuccessListener(aVoid -> {
                    stopEffects();
                    dismissEmergencyNotification();
                    finish();
                })
                .addOnFailureListener(e -> stopEffects());
    }

    private void listenToSelfClose() {
        if (deviceId == null) return;

        alertListener = FirebaseFirestore.getInstance().collection("devices").document(deviceId)
                .addSnapshotListener((snapshot, e) -> {
                    if (snapshot != null && snapshot.exists()) {
                        Boolean alert = snapshot.getBoolean("alert");
                        if (alert != null && !alert) {
                            stopEffects();
                            dismissEmergencyNotification();
                            finish();
                        }
                    }
                });
    }

    private void stopEffects() {
        if (ringtone != null && ringtone.isPlaying()) {
            ringtone.stop();
        }
    }

    private void dismissEmergencyNotification() {
        NotificationManager manager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        if (manager != null) {
            manager.cancel(EMERGENCY_NOTIFICATION_ID);
        }
    }

    @Override
    protected void onDestroy() {
        if (alertListener != null) {
            alertListener.remove();
        }
        stopEffects();
        super.onDestroy();
    }
}
