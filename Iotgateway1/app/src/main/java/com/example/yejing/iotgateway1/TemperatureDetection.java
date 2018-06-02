package com.example.yejing.iotgateway1;

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
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.lang.ref.WeakReference;
import java.util.Date;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import info.hoang8f.widget.FButton;

/**
 * Created by YE Jing on 2018/3/3.
 */

public class TemperatureDetection extends Activity {
    public static Context context;
    private MyBroadcastReceiver myBroadcastReceiver = new MyBroadcastReceiver();
    private MyHandler myHandler = new MyHandler(this);
    public String targetIpAddress, sPort;
    public int targetPort;

    private Date curDate;
    private TextView communication;
    final tcpClientSender mTCPCL = new tcpClientSender();
    ExecutorService exec = Executors.newCachedThreadPool();
    private DBHelper dbHelper;
    private DBHelper2 dbHelper2;
    private Cursor cursor, cursor2;
    private int _id=0;
    private  EditText inputText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.temperature);
        context = this;
        BindReceiver();
        targetIpAddress = "192.168.1.100";
        targetPort = 8088;
        mTCPCL.setIpAddress(targetIpAddress);
        mTCPCL.setTargetPort(targetPort);
        dbHelper=new DBHelper(this);
        cursor=dbHelper.select();
        dbHelper2=new DBHelper2(this);
        cursor2=dbHelper2.select();
        final FButton btnSend =  findViewById(R.id.btnSendTCPCL);
        final FButton btnTem = findViewById(R.id.btnTemperatureTCPCL);
        final FButton btnHum = findViewById(R.id.btnHumidityTCPCL);
        communication = findViewById(R.id.communicationTCPCL);
        inputText =  findViewById(R.id.editTextSendTCPCL) ;

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

        btnTem.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(TemperatureDetection.this, TemperatureHistory.class);
                startActivity(intent);
                finish();
            }
        });

        btnHum.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(TemperatureDetection.this, HumidityHistory.class);
                startActivity(intent);
                finish();
            }
        });
    }

    protected void onStop() {
        unregisterReceiver(myBroadcastReceiver);
        super.onStop();
        mTCPCL.closeSelf();
    }

    private void addData1(String str){

            dbHelper.insert(str);
            cursor.requery();
            _id=0;
    }
    private void addData2(String str){

        dbHelper2.insert(str);
        cursor2.requery();
        _id=0;
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
                    curDate = new Date(System.currentTimeMillis());
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
                String str2="";
                communication.append("\r\n Message received : ");
                communication.append(str);
                for(int i=0;i<str.length();i++){
                    if(str.charAt(i)=='1'&&str.charAt(i+1)==','){
                        int j = i+2;
                        while(j<str.length()&&str.charAt(j)!=',') {
                            str2+=str.charAt(j);
                            j++;
                        }
                        addData1(str2);
                        str2="";
                        i=j;
                    }
                    else if(str.charAt(i)=='2'&&str.charAt(i+1)==','){
                        int j = i+2;
                        while(j<str.length()&&str.charAt(j)!=',') {
                            str2+=str.charAt(j);
                            j++;
                        }
                        addData2(str2);
                        str2="";
                        i=j;
                    }
                }
            }
        }
        public Date getCurDate() {
            return curDate;
        }
    }
}
