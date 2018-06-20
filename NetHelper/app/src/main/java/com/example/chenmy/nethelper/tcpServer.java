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
import android.support.annotation.Nullable;
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

public class tcpServer extends Activity {
    public static Context context;
    private final MyHandler myHandler = new MyHandler(this);
    private MyBroadcastReceiver myBroadcastReceiver = new MyBroadcastReceiver();
    public int localPort;
    private TextView communication;
    private static tcpServerSender mTCPS = null;
    ExecutorService exec = Executors.newCachedThreadPool();

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.tcpserver);
        context = this;
        BindReceiver();
        TextView localIp = findViewById(R.id.localIPTCPS);
        String ipAddress = getWIFILocalIpAdress(this);
        localIp.append("Local IP is ");
        localIp.append(ipAddress);

    }

    @Override
    protected void onStart() {
        super.onStart();
        final Button btnConnect = (Button)findViewById(R.id.btnConnectTCPS);
        final Button btnSend = (Button)findViewById(R.id.btnSendTCPS);
        final Button btnDisconnect = findViewById(R.id.btnDisconnectTCPS);
        final EditText inputLocalPort = (EditText)findViewById(R.id.editTextlocalPortTCPS);
        communication = (TextView) findViewById(R.id.communicationTCPS);
        final EditText inputText = (EditText) findViewById(R.id.editTextSendTCPS);
        btnDisconnect.setEnabled(false);
        btnConnect.setEnabled(true);
        btnSend.setEnabled(false);

        btnConnect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                btnConnect.setEnabled(false);
                btnDisconnect.setEnabled(true);
                btnSend.setEnabled(true);

                mTCPS = new tcpServerSender(Integer.parseInt(inputLocalPort.getText().toString()));
                exec.execute(mTCPS);

                btnSend.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        communication.append("\r\n Message sent ");
                        communication.append(": ");
                        exec.execute(new Runnable() {
                            @Override
                            public void run() {
                                tcpServerSender.SST.send(inputText.getText().toString());
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
                btnConnect.setEnabled(true);
                btnDisconnect.setEnabled(false);
                btnSend.setEnabled(false);
                tcpServerSender.closeSelf();

            }
        });
    }

    private void BindReceiver(){
        IntentFilter intentFilter = new IntentFilter("tcpServerReceiver");
        registerReceiver(myBroadcastReceiver,intentFilter);
    }

    private class MyBroadcastReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            String mAction = intent.getAction();
            switch (mAction){
                case "tcpServerReceiver":
                    String msg = intent.getStringExtra("tcpServerReceiver");
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
        private final WeakReference<tcpServer> mActivity;
        public MyHandler(tcpServer activity) {
            mActivity = new WeakReference<tcpServer>(activity);
        }
        @Override
        public void handleMessage(Message msg) {
            tcpServer activity = mActivity.get();
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
