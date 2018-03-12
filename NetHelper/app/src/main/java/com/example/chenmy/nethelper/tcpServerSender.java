package com.example.chenmy.nethelper;

import android.content.Intent;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * Created by chenm on 12/17/2017.
 */

public class tcpServerSender implements Runnable {
    private int localPort;
    private static boolean isListen = true;
    static ServerSocketThread SST = null;

    public tcpServerSender(int port){
        this.localPort = port;
    }

    static public void closeSelf(){
        isListen = false;
        SST.isRun = false;
    }

    private Socket getSocket(ServerSocket serverSocket){
        try {
            return serverSocket.accept();
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
    }

    @Override
    public void run() {
        try {
            ServerSocket serverSocket = new ServerSocket(localPort);
            serverSocket.setSoTimeout(5000);
            while (isListen){

                Socket socket = getSocket(serverSocket);
                if (socket != null){
                    SST = new ServerSocketThread(socket);
                }
            }
            serverSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public class ServerSocketThread extends Thread{
        Socket socket = null;
        private PrintWriter pw;
        private InputStream is = null;
        private OutputStream os = null;
        private String ip = null;
        private boolean isRun = true;

        ServerSocketThread(Socket socket){
            this.socket = socket;            ip = socket.getInetAddress().toString();


            try {
                socket.setSoTimeout(8000);
                os = socket.getOutputStream();
                is = socket.getInputStream();
                pw = new PrintWriter(os,true);
                start();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void run() {
            byte buff[]  = new byte[4096];
            String textToBeReceived;
            int textReceilvedLength;
            while (isRun && !socket.isClosed() && !socket.isInputShutdown()){
                try {
                    if ((textReceilvedLength = is.read(buff)) != -1 ){
                        textToBeReceived = new String(buff,0,textReceilvedLength);
                        Intent intent =new Intent();
                        intent.setAction("tcpServerReceiver");
                        intent.putExtra("tcpServerReceiver",textToBeReceived);
                        tcpServer.context.sendBroadcast(intent);
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            try {
                socket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        public void send(String msg){
            pw.println(msg);
            pw.flush();
        }
    }



}
