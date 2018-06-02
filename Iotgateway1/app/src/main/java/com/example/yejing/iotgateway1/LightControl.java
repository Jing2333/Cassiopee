package com.example.yejing.iotgateway1;

/**
 * Created by YE Jing on 2018/5/29.
 */

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.Cursor;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.lang.ref.WeakReference;
import java.util.Date;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Created by YE Jing on 2018/3/3.
 */

public class LightControl extends Activity {
    public static Context context;
    private MyBroadcastReceiver myBroadcastReceiver = new MyBroadcastReceiver();
//    private MyHandler myHandler = new MyHandler(this);
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
        final Button btnOn =  findViewById(R.id.btnOnTCPCL);
        final Button btnOff = findViewById(R.id.btnOffTCPCL);



        exec.execute(mTCPCL);


        btnOn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mTCPCL.setTextToBeSent("1");
                exec.execute(new Runnable() {
                    @Override
                    public void run() {
                        mTCPCL.send();
                    }
                });
                Toast.makeText(getApplicationContext(), "Light On", Toast.LENGTH_SHORT).show();
            }
        });

        btnOff.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mTCPCL.setTextToBeSent("0");
                exec.execute(new Runnable() {
                    @Override
                    public void run() {
                        mTCPCL.send();
                    }
                });
                Toast.makeText(getApplicationContext(), "Light Off", Toast.LENGTH_SHORT).show();
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
        private final WeakReference<TemperatureDetection> mActivity;

        public MyHandler(TemperatureDetection activity) {
            mActivity = new WeakReference<TemperatureDetection>(activity);
        }

//        @Override
//        public void handleMessage(Message msg) {
//            TemperatureDetection activity = mActivity.get();
//            if (null != activity) {
//                String str = msg.obj.toString();
//                String str2="";
//                communication.append("\r\n Message received : ");
//                communication.append(str);
//                for(int i=0;i<str.length();i++){
//                    if(str.charAt(i)=='1'&&str.charAt(i+1)==','){
//                        int j = i+2;
//                        while(j<str.length()&&str.charAt(j)!=',') {
//                            str2+=str.charAt(j);
//                            j++;
//                        }
//                        addData1(str2);
//                        str2="";
//                        i=j;
//                    }
//                    else if(str.charAt(i)=='2'&&str.charAt(i+1)==','){
//                        int j = i+2;
//                        while(j<str.length()&&str.charAt(j)!=',') {
//                            str2+=str.charAt(j);
//                            j++;
//                        }
//                        addData2(str2);
//                        str2="";
//                        i=j;
//                    }
//                }
//            }
//        }
    }
}

