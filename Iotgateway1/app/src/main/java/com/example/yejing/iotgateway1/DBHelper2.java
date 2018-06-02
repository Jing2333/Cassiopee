package com.example.yejing.iotgateway1;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;

import java.text.SimpleDateFormat;
import java.util.Date;


public class DBHelper2 extends SQLiteOpenHelper {
    private final static String DB_NAME="humidity.db";
    private final static int DB_VERSION=1;
    private final  static String TABLE_NAME="info";
    private final static String ID="_id";
    SQLiteDatabase database=getWritableDatabase();
    public DBHelper2(Context context){
        super(context,DB_NAME,null,DB_VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase database){
        database.execSQL("create table " + TABLE_NAME + "(_id INTEGER PRIMARY KEY AUTOINCREMENT,humidity TEXT)");
    }
    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
    }
    public long insert(String text){
        ContentValues contentValues=new ContentValues();
        contentValues.put("humidity",text);


//        SimpleDateFormat formatter=new SimpleDateFormat("yy-MM-dd HH:mm");
//        Date curDate =  new Date(System.currentTimeMillis());
//        String time=formatter.format(curDate);


//        contentValues.put("date",time);
        long row=database.insert(TABLE_NAME,null,contentValues);
//        Log.e("time:", time);
        Log.e("humidity", text);
        return row;
    }

    public void update(int _id,String text){
        ContentValues contentValues=new ContentValues();
        contentValues.put("humidity",text);
        database.update(TABLE_NAME,contentValues,ID+"=?",new String[]{Integer.toString(_id)});
    }
    public void delete(int _id){
        database.delete(TABLE_NAME, ID + "=?", new String[]{Integer.toString(_id)});
    }
    public Cursor select(){
        Cursor cursor=database.query(TABLE_NAME,null,null,null,null,null,ID +" DESC");
        return cursor;
    }
}
