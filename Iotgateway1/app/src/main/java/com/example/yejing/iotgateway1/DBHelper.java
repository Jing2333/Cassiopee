package com.example.yejing.iotgateway1;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.os.Environment;
import android.util.Log;
import android.widget.Toast;

import java.io.File;
import java.io.FileWriter;

//Base de données pour stocker les valeurs de température
public class DBHelper extends SQLiteOpenHelper {
    private final static String DB_NAME="temperature.db";
    private final static int DB_VERSION=1;
    private final  static String TABLE_NAME="info";
    private final static String ID="_id";
    SQLiteDatabase database=getWritableDatabase();
    public DBHelper(Context context){
        super(context,DB_NAME,null,DB_VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase database){
        database.execSQL("create table " + TABLE_NAME + "(_id INTEGER PRIMARY KEY AUTOINCREMENT,temperature TEXT)");
    }
    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
    }

    //ajouter une ligne de données
    public long insert(String text){
        ContentValues contentValues=new ContentValues();
        contentValues.put("temperature",text);
        long row=database.insert(TABLE_NAME,null,contentValues);
        Log.e("temperature", text);
        return row;
    }

    //mettre à jour la base de données
    public void update(int _id,String text){
        ContentValues contentValues=new ContentValues();
        contentValues.put("temperature",text);
        database.update(TABLE_NAME,contentValues,ID+"=?",new String[]{Integer.toString(_id)});
    }

    //supprimer une ligne
    public void delete(int _id){
        database.delete(TABLE_NAME, ID + "=?", new String[]{Integer.toString(_id)});
    }

    //vider la base de données
    public void deleteAll() {
        database.execSQL("delete from "+ TABLE_NAME);
    }
    public Cursor select(){
        Cursor cursor=database.query(TABLE_NAME,null,null,null,null,null,ID +" DESC");
        return cursor;
    }

    //exporter la base de données en .CSV
    public void exportDB(Context mcontext) {
        String path = Environment.getExternalStorageDirectory().getAbsolutePath()+"/Android/data/iotgateway";
        File exportDir = new File(path, "");
        if (!exportDir.exists())
        {
            exportDir.mkdirs();
        }

        File file = new File(exportDir, "temperature.csv");
        try {
            file.createNewFile();
            CSVWriter csvWrite = new CSVWriter(new FileWriter(file));
            SQLiteDatabase db = getReadableDatabase();
            Cursor curCSV = db.rawQuery("SELECT * FROM " + TABLE_NAME, null);
            csvWrite.writeNext(curCSV.getColumnNames());
            while (curCSV.moveToNext()) {
                //Which column you want to export
                String arrStr[] = {curCSV.getString(0), curCSV.getString(1)};
                csvWrite.writeNext(arrStr);
            }
            csvWrite.close();
            curCSV.close();
            Toast.makeText(mcontext, "file exported at" + path, Toast.LENGTH_LONG).show();
        }
        catch(Exception sqlEx)
        {
            Log.e("TemperatureHistory", sqlEx.getMessage(), sqlEx);
        }
    }
}
