package com.example.chenmy.nethelper;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;

public class NetHelper extends AppCompatActivity {
    private Button btnUdp = null;
    private Button btnIpconfig= null;
    private Button btnTCPCL = null;
    private Button btnTCPS = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_net_helper);

        btnUdp = (Button) findViewById(R.id.btnUdp);
        btnUdp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent i =new Intent(view.getContext(),udp.class);
                startActivity(i);
            }
        });

        btnIpconfig = (Button) findViewById(R.id.btnIpConfig);
        btnIpconfig.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent i = new Intent(view.getContext(),ipconfig.class);
                startActivity(i);
            }
        });

        btnTCPS = (Button) findViewById(R.id.btnTcpServer);
        btnTCPS.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent i = new Intent(view.getContext(),tcpServer.class);
                startActivity(i);
            }
        });

        btnTCPCL = (Button) findViewById(R.id.btnTcpClient);
        btnTCPCL.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent i = new Intent(view.getContext(),tcpClient.class);
                startActivity(i);
            }
        });
    }

}
