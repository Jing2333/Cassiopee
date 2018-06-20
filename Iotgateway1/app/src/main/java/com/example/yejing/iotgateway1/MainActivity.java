 package com.example.yejing.iotgateway1;

import android.content.Intent;
import android.os.StrictMode;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

import at.markushi.ui.CircleButton;

 public class MainActivity extends AppCompatActivity {

    private CircleButton btnTemp = null;
    private CircleButton btnLight = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //Autoriser la connextion dans le main thread
        if (android.os.Build.VERSION.SDK_INT > 9) {
            StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
            StrictMode.setThreadPolicy(policy);
        }
        //Premier bouton
        btnTemp = findViewById(R.id.btntemp);
        btnTemp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent i =new Intent(view.getContext(),TemHumDetection.class);
                startActivity(i);
            }
        });
        //Deuxième bouton
        btnLight = findViewById(R.id.btnlight);
        btnLight.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent i =new Intent(view.getContext(),LightControl.class);
                startActivity(i);
            }
        });
    }
}
