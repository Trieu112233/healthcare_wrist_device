package com.example.smartwatchapp;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.google.firebase.firestore.FirebaseFirestore;


public class LoginActivity extends AppCompatActivity {
    private Button btnLogin;
    private TextView tvRegister;
    private EditText edtUserphone;
    private EditText edtPassword;

    private void loginUser(String phone, String password) {
        FirebaseFirestore db = FirebaseFirestore.getInstance();

        db.collection("users").document(phone).get()
                .addOnSuccessListener(documentSnapshot -> {
                    if (documentSnapshot.exists()) {
                        String dbPassword = documentSnapshot.getString("password");
                        if (dbPassword != null && dbPassword.equals(password)) {
                            getSharedPreferences("AUTH", MODE_PRIVATE).edit()
                                    .putString("user_phone", phone)
                                    .putBoolean("is_logged_in", true)
                                    .apply();
                            Toast.makeText(this, "Đăng nhập thành công", Toast.LENGTH_SHORT).show();

                            startAlertService();

                            Intent intent = new Intent(LoginActivity.this, MainActivity.class);
                            startActivity(intent);
                            finish();
                        } else {
                            Toast.makeText(this, "Mật khẩu không đúng", Toast.LENGTH_SHORT).show();
                        }
                    } else {
                        Toast.makeText(this, "Số điện thoại chưa đăng ký !", Toast.LENGTH_SHORT).show();
                    }
                })
                .addOnFailureListener(e -> Toast.makeText(this, "Lỗi " + e.getMessage(), Toast.LENGTH_SHORT).show());
    }

    private void startAlertService() {
        Intent intent = new Intent(this, FirestoreAlertService.class);
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O){
            startForegroundService(intent);
        } else {
            startService(intent);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        SharedPreferences prefs = getSharedPreferences("AUTH", MODE_PRIVATE);
        if (prefs.getBoolean("is_logged_in", false)) {
            startActivity(new Intent(this, MainActivity.class));
            finish();
            return;
        }

        setContentView(R.layout.activity_login2);

        btnLogin = findViewById(R.id.loginBtnLogin);
        tvRegister = findViewById(R.id.loginTextForgotPassword);
        edtUserphone = findViewById(R.id.loginEdtUserphone);
        edtPassword = findViewById(R.id.loginEdtPassword);

        btnLogin.setOnClickListener(v -> {
            String phone = edtUserphone.getText().toString();
            String password = edtPassword.getText().toString();

            if (phone.isEmpty() || password.isEmpty()) {
                Toast.makeText(this, "Vui lòng nhập đầy đủ tài khoản và mật khẩu", Toast.LENGTH_SHORT).show();
                return;
            }

            loginUser(phone, password);
        });

        tvRegister.setOnClickListener(v -> {
            Toast.makeText(this, "Chuyển sang đăng ký", Toast.LENGTH_SHORT).show();
            // Chuyển sang màn hình đăng ký (RegisterActivity")
            Intent intent = new Intent(LoginActivity.this, RegisterActivity.class);
            startActivity(intent);
            finish();
        });
    }
}