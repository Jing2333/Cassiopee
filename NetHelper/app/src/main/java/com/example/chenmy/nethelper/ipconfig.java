package com.example.chenmy.nethelper;

import android.app.Activity;
import android.content.Context;
import android.net.DhcpInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.widget.TextView;

/**
 * Created by chenm on 11/24/2017.
 */

public class ipconfig extends Activity {
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ipconfig);
    }

    @Override
    protected void onStart() {
        super.onStart();
        TextView textSSID = (TextView) findViewById(R.id.Get_SSID);
        String ssid = getWIFISSID(this);
        textSSID.setText(ssid);

        TextView textIp = (TextView) findViewById(R.id.Get_IP);
        String ipAddress = getWIFILocalIpAdress(this);
        textIp.setText(ipAddress);

        TextView textGateway = (TextView) findViewById(R.id.Get_Gateway);
        String gateway = getWIFILocalGateway(this);
        textGateway.setText(gateway);

        TextView textNetmask = (TextView) findViewById(R.id.Get_Netmask);
        String netmask = getWIFILocalNetmask(this);
        textNetmask.setText(netmask);

        TextView textMAC = (TextView) findViewById(R.id.Get_MAC);
        String mac = getWIFILocalMAC(this);
        textMAC.setText(mac);
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

    public static String getWIFISSID(Context mContext){
        WifiManager wifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE); // Get Wifi Service
        if (!wifiManager.isWifiEnabled()) {         // Is Wifi enabled
            wifiManager.setWifiEnabled(true);
        }

        WifiInfo wifiInfo = wifiManager.getConnectionInfo();
        String ssid = wifiInfo.getSSID();
        return  ssid;
    }

    public static String getWIFILocalGateway(Context mContext){
        WifiManager wifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE); // Get Wifi Service
        if (!wifiManager.isWifiEnabled()) {         // Is Wifi enabled
            wifiManager.setWifiEnabled(true);
        }
        DhcpInfo dhcpInfo = wifiManager.getDhcpInfo();
        String gateway = formatIpAddress(dhcpInfo.gateway);
        return gateway;
    }

    public static String getWIFILocalNetmask(Context mContext){
        WifiManager wifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE); // Get Wifi Service
        if (!wifiManager.isWifiEnabled()) {         // Is Wifi enabled
            wifiManager.setWifiEnabled(true);
        }
        DhcpInfo dhcpInfo = wifiManager.getDhcpInfo();
        String netmask = formatIpAddress(dhcpInfo.netmask);
        return netmask;
    }

    public static String getWIFILocalMAC(Context mContext){
        WifiManager wifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE); // Get Wifi Service
        if (!wifiManager.isWifiEnabled()) {         // Is Wifi enabled
            wifiManager.setWifiEnabled(true);
        }

        WifiInfo wifiInfo = wifiManager.getConnectionInfo();
        String mac = wifiInfo.getMacAddress();
        return mac;
    }

    private static String formatIpAddress(int ipAddress) {

        return (ipAddress & 0xFF ) + "." +
                ((ipAddress >> 8 ) & 0xFF) + "." +
                ((ipAddress >> 16 ) & 0xFF) + "." +
                ( ipAddress >> 24 & 0xFF) ;
    }
}