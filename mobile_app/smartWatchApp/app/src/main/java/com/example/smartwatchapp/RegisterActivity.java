package com.example.smartwatchapp;

import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.google.firebase.firestore.FirebaseFirestore;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class RegisterActivity extends AppCompatActivity {

    private EditText edtFullName, edtRegisterPhoneNum, edtRegisterPassword, edtConfirmPassword;
    private Button btnRegister;
    private TextView tvBackToLogin;
    private FirebaseFirestore db;

    private void registerUser(String name, String phone, String password) {
        db.collection("users").document(phone).get()
                .addOnSuccessListener(documentSnapshot -> {
                    if (documentSnapshot.exists()) {
                        Toast.makeText(this, "Số điện thoại đã được đăng ký", Toast.LENGTH_SHORT).show();
                    } else {
                        Map<String, Object> user = new HashMap<>();
                        user.put("Name", name);
                        user.put("password", password);
                        user.put("DeviceId", new ArrayList<String>());

                        db.collection("users").document(phone)
                                .set(user)
                                .addOnSuccessListener(aVoid -> {
                                   Toast.makeText(this, "Đăng ký thành công", Toast.LENGTH_SHORT).show();
                                   finish();
                                })
                                .addOnFailureListener(e -> {
                                    Toast.makeText(this, "Lỗi " + e.getMessage(), Toast.LENGTH_SHORT).show();
                                });
                    }
                });
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_register2);

        db = FirebaseFirestore.getInstance();

        btnRegister = findViewById(R.id.regBtnRegister);
        tvBackToLogin = findViewById(R.id.regTextBackToLogin);

        edtFullName = findViewById(R.id.regEdtFullName);
        edtRegisterPhoneNum = findViewById(R.id.regEdtRegisterPhoneNum);
        edtRegisterPassword = findViewById(R.id.regEdtRegisterPassword);
        edtConfirmPassword = findViewById(R.id.regEdtConfirmPassword);

        btnRegister.setOnClickListener(v -> {
            String name = edtFullName.getText().toString().trim();
            String phone = edtRegisterPhoneNum.getText().toString().trim();
            String password = edtRegisterPassword.getText().toString().trim();
            String confirmPassword = edtConfirmPassword.getText().toString().trim();

            if(name.isEmpty() || phone.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()){
                Toast.makeText(this, "Vui lòng nhập đầy đủ thông tin", Toast.LENGTH_SHORT).show();
                return;
            }

            if(!password.equals(confirmPassword)){
                Toast.makeText(this, "Mật khẩu không khớp", Toast.LENGTH_SHORT).show();
                return;
            }

            registerUser(name, phone, password);
        });

        tvBackToLogin.setOnClickListener(v -> {
            Toast.makeText(this, "Chuyển sang màn hình đăng nhập", Toast.LENGTH_SHORT).show();
            Intent intent = new Intent(RegisterActivity.this, LoginActivity.class);
            startActivity(intent);
            finish();
        });
    }
}