package com.example.yejing.iotgateway1;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.Toast;

import com.suke.widget.SwitchButton;

import java.lang.ref.WeakReference;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Created by YE Jing on 2018/3/3.
 */

public class LightControl extends Activity {
    public static Context context;
    private MyBroadcastReceiver myBroadcastReceiver = new MyBroadcastReceiver();
    public String targetIpAddress;
    public int targetPort;

    final tcpClientSender mTCPCL = new tcpClientSender();
    ExecutorService exec = Executors.newCachedThreadPool();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.control_light);
        context = this;
        BindReceiver();
        targetIpAddress = "192.168.1.111";
        targetPort = 6666;
        mTCPCL.setIpAddress(targetIpAddress);
        mTCPCL.setTargetPort(targetPort);
        final SwitchButton switchButton = findViewById(R.id.switch_button);


        exec.execute(mTCPCL);


        switchButton.setChecked(false);
        switchButton.isChecked();
        switchButton.toggle();     //switch state
        switchButton.toggle(true);//switch with animation
        switchButton.setShadowEffect(true);//enable shadow effect
        switchButton.setEnabled(true);//enable button
        switchButton.setEnableEffect(true);//enable the switch animation
        switchButton.setOnCheckedChangeListener(new SwitchButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(SwitchButton view, boolean isChecked) {
                if (isChecked) {
                    mTCPCL.setTextToBeSent("1");
                    exec.execute(new Runnable() {
                        @Override
                        public void run() {
                            mTCPCL.send();
                        }
                    });
                    Toast.makeText(getApplicationContext(), "Light On", Toast.LENGTH_SHORT).show();
                } else {
                    mTCPCL.setTextToBeSent("0");
                    exec.execute(new Runnable() {
                        @Override
                        public void run() {
                            mTCPCL.send();
                        }
                    });
                    Toast.makeText(getApplicationContext(), "Light Off", Toast.LENGTH_SHORT).show();
                }
            }
        });

    }

    protected void onStop() {
        unregisterReceiver(myBroadcastReceiver);
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
//                    myHandler.sendMessage(message);
                    //Log.d(TAG, "onReceive: receive!");
                    Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_SHORT).show();
                    break;
            }
        }
    }

    private class MyHandler extends Handler {
        private final WeakReference<TemHumDetection> mActivity;

        public MyHandler(TemHumDetection activity) {
            mActivity = new WeakReference<TemHumDetection>(activity);
        }
    }
}

