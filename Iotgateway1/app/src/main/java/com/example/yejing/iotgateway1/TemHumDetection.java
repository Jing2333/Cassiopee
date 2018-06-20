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
import android.text.Editable;
import android.text.TextWatcher;
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

public class TemHumDetection extends Activity {
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
        setContentView(R.layout.detection_temhum);
        context = this;
        BindReceiver();

        dbHelper=new DBHelper(this);
        cursor=dbHelper.select();
        dbHelper2=new DBHelper2(this);
        cursor2=dbHelper2.select();
        final FButton btnConnect =  findViewById(R.id.btnConnectTCPCL);
        final FButton btnTem = findViewById(R.id.btnTemperatureTCPCL);
        final FButton btnHum = findViewById(R.id.btnHumidityTCPCL);
        final FButton btnSync = findViewById(R.id.btnSyncTCPCL);
        communication = findViewById(R.id.communicationTCPCL);
        inputText =  findViewById(R.id.editTextSendTCPCL);

        btnConnect.setEnabled(false);
        btnSync.setEnabled(false);

        inputText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                if(inputText.length()!=0){
                    btnConnect.setEnabled(true);
                }
            }

            @Override
            public void afterTextChanged(Editable editable) {

            }
        });




        btnConnect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                targetIpAddress = inputText.getText().toString();
                targetPort = 8088;
                mTCPCL.setIpAddress(targetIpAddress);
                mTCPCL.setTargetPort(targetPort);
                sPort = Integer.toString(targetPort);
                btnSync.setEnabled(true);

                communication.append("\r\n Connected to ");
                communication.append(targetIpAddress);
                communication.append(" : ");
                communication.append(sPort);

                exec.execute(mTCPCL);
            }
        });

        btnSync.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                communication.append("\r\n Receiving data by sending ");
                communication.append(": ");
                mTCPCL.setTextToBeSent("1");
                exec.execute(new Runnable() {
                    @Override
                    public void run() {
                        mTCPCL.send();
                    }
                });
                communication.append("1");
            }
        });

        btnTem.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(TemHumDetection.this, TemperatureHistory.class);
                startActivity(intent);
                finish();
            }
        });

        btnHum.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(TemHumDetection.this, HumidityHistory.class);
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

        @Override
        public void handleMessage(Message msg) {
            TemHumDetection activity = mActivity.get();
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
    }
}
