package com.example.smartwatchapp;

import android.app.KeyguardManager;
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

public class EmergencyActivity extends AppCompatActivity {
    private String deviceId;
    private Ringtone ringtone;
    private TextView tvAlertType, tvDeviceName, tvAlertTime;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Cấu hình để hiển thị đè lên màn hình khóa (Rất quan trọng cho app khẩn cấp)
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

        // Ánh xạ View
        tvAlertType = findViewById(R.id.tvAlertType);
        tvDeviceName = findViewById(R.id.tvDeviceName);
        tvAlertTime = findViewById(R.id.tvAlertTime);

        // Lấy dữ liệu từ Intent (Gửi từ Foreground Service)
        deviceId = getIntent().getStringExtra("device_id");
        String type = getIntent().getStringExtra("alert_type");
        String time = getIntent().getStringExtra("last_alert_time");
        if (time == null) time = "Không rõ";

        // Hiển thị lên UI
        tvAlertType.setText(type != null ? type.replace("_", " ") : "CẢNH BÁO");
        tvDeviceName.setText("Thiết bị: " + deviceId);
        tvAlertTime.setText("Thời gian: " + time);

        // Âm thanh báo động
        Uri notification = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_ALARM);
        ringtone = RingtoneManager.getRingtone(getApplicationContext(), notification);
        if (ringtone != null) ringtone.play();

        // Rung
        Vibrator v = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
        if (v != null) {
            v.vibrate(VibrationEffect.createOneShot(3000, VibrationEffect.DEFAULT_AMPLITUDE));
        }

        findViewById(R.id.btnAcknowledge).setOnClickListener(v1 -> acknowledgeAlert());
        listenToSelfClose();
    }

    private void acknowledgeAlert() {
        FirebaseFirestore.getInstance().collection("devices").document(deviceId)
                .update("alert", false)
                .addOnSuccessListener(aVoid -> {
                    stopEffects();
                    finish();
                });
    }

    private void listenToSelfClose() {
        FirebaseFirestore.getInstance().collection("devices").document(deviceId)
                .addSnapshotListener((snapshot, e) -> {
                    if (snapshot != null && snapshot.exists()) {
                        Boolean alert = snapshot.getBoolean("alert");
                        if (alert != null && !alert) {
                            stopEffects();
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

    @Override
    protected void onDestroy() {
        stopEffects();
        super.onDestroy();
    }
}