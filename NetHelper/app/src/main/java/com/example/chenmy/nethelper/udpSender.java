package com.example.chenmy.nethelper;

import android.content.Intent;
import android.util.Log;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;

/**
 * Created by chenm on 12/1/2017.
 */

public class udpSender implements Runnable{
    private int localPort;
    private int targetPort;
    private InetAddress ipAddress;
    private byte[] textToBeSent;
    private byte[] textToBeReceived = new byte[1024];
    private DatagramPacket udpPacket = null;
    private DatagramSocket udpSocket = null;
    private boolean isRun = false;




    public void setTextToBeSent(String textToBeSent) {
        this.textToBeSent = textToBeSent.getBytes();
    }
    public void setLocalPort(int localPort) {
        this.localPort = localPort;
    }

    public void setTargetPort(int targetPort) {
        this.targetPort = targetPort;
    }

    public void setIpAddress(String ipAddress) {
        try {
        this.ipAddress = InetAddress.getByName(ipAddress);
        }
        catch (IOException e){
            Log.d("UDPSender", e.getMessage());
        }
    }



    public void sendUdpPackage(){
        try {
            udpPacket = new DatagramPacket(textToBeSent, textToBeSent.length, ipAddress, targetPort);
            udpSocket.send(udpPacket);
        }
        catch (SocketException e){
            Log.d("UDPSend", e.getMessage());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run() {
        try {
            udpSocket = new DatagramSocket(localPort);
            isRun = true;
            byte[] buffer = "Hello world".getBytes();
            udpPacket = new DatagramPacket(buffer, buffer.length, ipAddress, targetPort);
            udpSocket.send(udpPacket);
        }
        catch (SocketException e){
            Log.d("UDPConnect", e.getMessage());
        } catch (IOException e) {
            e.printStackTrace();
        }

        DatagramPacket udpRecPacket = new DatagramPacket(textToBeReceived,textToBeReceived.length);
        while (isRun) {
            try {
                udpSocket.receive(udpRecPacket);
                Intent intent =new Intent();
                intent.setAction("udpReceiver");
                String stringTest = new String(udpRecPacket.getData(), udpRecPacket.getOffset(), udpRecPacket.getLength());
                intent.putExtra("udpReceiver",stringTest);
                //intent.putExtra("udpReceiver",textToBeReceived.toString());
                udp.context.sendBroadcast(intent);


            } catch (IOException e) {
                e.printStackTrace();
            }

        }
    }

    public void closeUdp(){
        if (isRun){
            isRun = false;
            udpSocket.close();
        }

    }




}
