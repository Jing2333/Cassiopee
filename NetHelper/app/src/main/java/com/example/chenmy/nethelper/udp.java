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
 * Created by chenm on 11/21/2017.
 */

public class udp extends Activity {
    public static Context context;
    private MyBroadcastReceiver myBroadcastReceiver = new MyBroadcastReceiver();
    private MyHandler myHandler =   new MyHandler(this);
    public String targetIpAddress;
    public int targetPort;
    public int localPort;
    //public String textToBeSent = null;
    final udpSender mUdp = new udpSender();
    private TextView communication;
    ExecutorService exec = Executors.newCachedThreadPool();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.udpclient);
        context = this;
        BindReceiver();
        TextView localIp = (TextView) findViewById(R.id.localIPUDP);
        String ipAddress = getWIFILocalIpAdress(this);
        localIp.append("Local IP is ");
        localIp.append(ipAddress);
    }

    @Override
    protected void onStart() {
        super.onStart();
        Button btnConnect = (Button)findViewById(R.id.btnConnect);
        final Button btnSend = (Button)findViewById(R.id.btnSend);
        final EditText inputTargetIpAddress = (EditText)findViewById(R.id.editTextIP);
        final EditText inputTargetPort = (EditText)findViewById(R.id.editTextTP);
        final EditText inputLocalPort = (EditText) findViewById(R.id.editTextSP);
        communication = (TextView) findViewById(R.id.communicationUDP);
        final EditText inputText = (EditText) findViewById(R.id.editTextSend) ;
        //communication.setText("NO!");





        btnConnect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                targetIpAddress = inputTargetIpAddress.getText().toString();
                targetPort = Integer.parseInt(inputTargetPort.getText().toString());
                localPort = Integer.parseInt(inputLocalPort.getText().toString());
                mUdp.setIpAddress(targetIpAddress);
                mUdp.setLocalPort(localPort);
                mUdp.setTargetPort(targetPort);

                    communication.append("\r\n Connected to ");
                    communication.append(targetIpAddress);
                    communication.append(" : ");
                    communication.append(inputTargetPort.getText());
                    communication.append(" from ");
                    communication.append(inputLocalPort.getText());
                    //Thread thread = new Thread(mUdp);
                    //thread.start();
                exec.execute(mUdp);
//                mUdp.udpConnect();




                btnSend.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        targetIpAddress = inputTargetIpAddress.getText().toString();
                        targetPort = Integer.parseInt(inputTargetPort.getText().toString());
                        localPort = Integer.parseInt(inputLocalPort.getText().toString());
                        communication.append("\r\n Message sent ");
                        communication.append(": ");
                        mUdp.setTextToBeSent(inputText.getText().toString());
                        //mUdp.sendUdpPackage();
                        exec.execute(new Runnable() {
                            @Override
                            public void run() {
                                mUdp.sendUdpPackage();
                            }
                        });
                        communication.append(inputText.getText().toString());
                    }
                });





            }
        });




    }

    @Override
    protected void onStop() {
        super.onStop();
        mUdp.closeUdp();

    }
    private void BindReceiver(){
        IntentFilter intentFilter = new IntentFilter("udpReceiver");
        registerReceiver(myBroadcastReceiver,intentFilter);
    }

    private class MyBroadcastReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            String mAction = intent.getAction();
            switch (mAction){
                case "udpReceiver":
                    String msg = intent.getStringExtra("udpReceiver");
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

    private class MyHandler extends Handler{
        private final WeakReference<udp> mActivity;
        public MyHandler(udp activity) {
            mActivity = new WeakReference<udp>(activity);
        }
        @Override
        public void handleMessage(Message msg) {
            udp activity = mActivity.get();
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
