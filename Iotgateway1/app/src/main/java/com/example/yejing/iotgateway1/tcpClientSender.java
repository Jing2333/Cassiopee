package com.example.yejing.iotgateway1;

import android.content.Intent;
import android.util.Log;

import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;

/**
 * Created by YE Jing on 2018/3/3.
 */

public class tcpClientSender implements Runnable {
    //private int localPort = 60000;
    private int targetPort;
    private InetAddress ipAddress;
    private String textToBeSent;
    //    private byte[] textToBeSent;
    private byte[] textToBeReceived = new byte[1024];
    private InputStream is;
    private OutputStream os;
    private DataInputStream dis;
    private PrintWriter pw;
    private Socket socket = null;
    private boolean isRun = true;
    private String textReceived;
    private int textReceilvedLength;

    //    public void setTextToBeSent(String textToBeSent) {
//        this.textToBeSent = textToBeSent.getBytes();
//    }
    public void setTextToBeSent(String textToBeSent) {
        this.textToBeSent = textToBeSent;
    }

    public void setTargetPort(int targetPort) {
        this.targetPort = targetPort;
    }

    public void setIpAddress(String ipAddress) {
        try {
            this.ipAddress = InetAddress.getByName(ipAddress);
        }
        catch (IOException e){
            Log.d("TCPCLSender", e.getMessage());
        }
    }

    public void closeSelf(){
        isRun = false;
    }

    public void send(){
        if(pw != null) {
            pw.print(textToBeSent);

            pw.flush();
        }

    }

    @Override
    public void run() {
        try {
            socket = new Socket(ipAddress,targetPort);
            socket.setSoTimeout(8000);
            isRun = true;
            os = socket.getOutputStream();

            pw = new PrintWriter(new BufferedWriter(new OutputStreamWriter( socket.getOutputStream(), "UTF-8")), true);

            is = socket.getInputStream();
            dis = new DataInputStream(is);
        } catch (IOException e)
        {
            e.printStackTrace();
        }

        while (isRun){
            try {
                textReceilvedLength = dis.read(textToBeReceived);
                textReceived = new String(textToBeReceived,0,textReceilvedLength,"utf-8");
                Intent intent =new Intent();
                intent.setAction("tcpClientReceiver");
                intent.putExtra("tcpClientReceiver",textReceived);
                TemperatureDetection.context.sendBroadcast(intent);
                if (textReceived.equals("QuitClient")){
                    isRun = false;
                }
            } catch (IOException e) {
                e.printStackTrace();
            }

        }
        try {
            if (socket != null) {
                pw.close();
                is.close();
                dis.close();
                socket.close();
                socket = null;
            }


        } catch (IOException e) {
            e.printStackTrace();
        }



    }
}
