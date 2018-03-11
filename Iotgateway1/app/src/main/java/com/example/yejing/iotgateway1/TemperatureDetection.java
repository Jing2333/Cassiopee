package com.example.yejing.iotgateway1;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.lang.ref.WeakReference;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Created by YE Jing on 2018/3/3.
 */

public class TemperatureDetection extends Activity {
    public static Context context;
    private MyBroadcastReceiver myBroadcastReceiver = new MyBroadcastReceiver();
    private MyHandler myHandler = new MyHandler(this);
    public String targetIpAddress, sPort;
    public int targetPort;
    private TextView communication;
    final tcpClientSender mTCPCL = new tcpClientSender();
    ExecutorService exec = Executors.newCachedThreadPool();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.temperature);
        context = this;
        BindReceiver();
        targetIpAddress = "192.168.109.88";
        targetPort = 8888;
        mTCPCL.setIpAddress(targetIpAddress);
        mTCPCL.setTargetPort(targetPort);
        final Button btnSend =  findViewById(R.id.btnSendTCPCL);
        communication = findViewById(R.id.communicationTCPCL);
        final EditText inputText =  findViewById(R.id.editTextSendTCPCL) ;

        sPort = Integer.toString(targetPort);

        communication.append("\r\n Connected to ");
        communication.append(targetIpAddress);
        communication.append(" : ");
        communication.append(sPort);

        exec.execute(mTCPCL);


        btnSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                communication.append("\r\n Message sent ");
                communication.append(": ");
                mTCPCL.setTextToBeSent(inputText.getText().toString());
                exec.execute(new Runnable() {
                    @Override
                    public void run() {
                        mTCPCL.send();
                    }
                });
                communication.append(inputText.getText().toString());
            }
        });
    }

    protected void onStop() {
        super.onStop();
        mTCPCL.closeSelf();
    }

    private void BindReceiver() {
        IntentFilter intentFilter = new IntentFilter("tcpClientReceiver");
        registerReceiver(myBroadcastReceiver, intentFilter);
    }

    private class MyBroadcastReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            String mAction = intent.getAction();
            switch (mAction) {
                case "tcpClientReceiver":
                    String msg = intent.getStringExtra("tcpClientReceiver");
                    //communication.append(msg);
                    Message message = new Message();
                    message.obj = msg;
                    myHandler.sendMessage(message);
                    //Log.d(TAG, "onReceive: receive!");
                    Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_SHORT).show();
                    break;
            }
        }
    }

    private class MyHandler extends Handler {
        private final WeakReference<TemperatureDetection> mActivity;

        public MyHandler(TemperatureDetection activity) {
            mActivity = new WeakReference<TemperatureDetection>(activity);
        }

        @Override
        public void handleMessage(Message msg) {
            TemperatureDetection activity = mActivity.get();
            if (null != activity) {
                String str = msg.obj.toString();
                communication.append("\r\n Message received : ");
                communication.append(str);

            }
        }
    }
}
