package com.example.chenmy.nethelper;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
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
 * Created by chenm on 12/15/2017.
 */

public class tcpClient extends Activity {
    public static Context context;
    private MyBroadcastReceiver myBroadcastReceiver = new MyBroadcastReceiver();
    private MyHandler myHandler =   new MyHandler(this);
    public String targetIpAddress;
    public int targetPort;
    public int localPort;
    private TextView communication;
    final tcpClientSender mTCPCL = new tcpClientSender();
    ExecutorService exec = Executors.newCachedThreadPool();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.tcpclient);
        context = this;
        BindReceiver();
        TextView localIp = findViewById(R.id.localIPTCP);
        String ipAddress = getWIFILocalIpAdress(this);
        localIp.append("Local IP is ");
        localIp.append(ipAddress);
    }

    @Override
    protected void onStart() {
        super.onStart();
        final Button btnConnect = (Button)findViewById(R.id.btnConnectTCPCL);
        final Button btnSend = (Button)findViewById(R.id.btnSendTCPCL);
        final Button btnDisconnect = findViewById(R.id.btnDisconnectTCPCL);
        final EditText inputTargetIpAddress = (EditText)findViewById(R.id.editTextIPTCPCL);
        final EditText inputTargetPort = (EditText)findViewById(R.id.editTextTPTCPCL);
        communication = (TextView) findViewById(R.id.communicationTCPCL);
        final EditText inputText = (EditText) findViewById(R.id.editTextSendTCPCL) ;


        btnDisconnect.setEnabled(false);
        btnConnect.setEnabled(true);
        btnSend.setEnabled(false);

        btnConnect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                btnConnect.setEnabled(false);
                btnDisconnect.setEnabled(true);
                btnSend.setEnabled(true);
                targetIpAddress = inputTargetIpAddress.getText().toString();
                targetPort = Integer.parseInt(inputTargetPort.getText().toString());
                mTCPCL.setIpAddress(targetIpAddress);
                mTCPCL.setTargetPort(targetPort);

                communication.append("\r\n Connected to ");
                communication.append(targetIpAddress);
                communication.append(" : ");
                communication.append(inputTargetPort.getText());

                exec.execute(mTCPCL);


                btnSend.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        targetIpAddress = inputTargetIpAddress.getText().toString();
                        targetPort = Integer.parseInt(inputTargetPort.getText().toString());
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
        });
        btnDisconnect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                btnSend.setEnabled(false);
                btnConnect.setEnabled(true);
                btnDisconnect.setEnabled(false);
                mTCPCL.closeSelf();
                communication.append("\r\n Disconnect");
            }
        });
    }

    @Override
    protected void onStop() {
        super.onStop();
        mTCPCL.closeSelf();
    }

    private void BindReceiver(){
        IntentFilter intentFilter = new IntentFilter("tcpClientReceiver");
        registerReceiver(myBroadcastReceiver,intentFilter);
    }

    private class MyBroadcastReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            String mAction = intent.getAction();
            switch (mAction){
                case "tcpClientReceiver":
                    String msg = intent.getStringExtra("tcpClientReceiver");
                    //communication.append(msg);
                    Message message = new Message();
                    message.obj = msg;
                    myHandler.sendMessage(message);
                    //Log.d(TAG, "onReceive: receive!");
                    Toast.makeText(getApplicationContext(),msg,Toast.LENGTH_SHORT).show();
                    break;
            }
        }
    }

    private class MyHandler extends Handler {
        private final WeakReference<tcpClient> mActivity;
        public MyHandler(tcpClient activity) {
            mActivity = new WeakReference<tcpClient>(activity);
        }
        @Override
        public void handleMessage(Message msg) {
            tcpClient activity = mActivity.get();
            if (null != activity){
                String str = msg.obj.toString();
                communication.append("\r\n Message received : ");
                communication.append(str);

            }
        }
    }

    public static String getWIFILocalIpAdress(Context mContext) {
        WifiManager wifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE); // Get Wifi Service
        if (!wifiManager.isWifiEnabled()) {         // Is Wifi enabled
            wifiManager.setWifiEnabled(true);
        }

        WifiInfo wifiInfo = wifiManager.getConnectionInfo();
        int ipAddress = wifiInfo.getIpAddress();
        String ip = formatIpAddress(ipAddress);
        return ip;
    }

    private static String formatIpAddress(int ipAddress) {

        return (ipAddress & 0xFF ) + "." +
                ((ipAddress >> 8 ) & 0xFF) + "." +
                ((ipAddress >> 16 ) & 0xFF) + "." +
                ( ipAddress >> 24 & 0xFF) ;
    }
}

