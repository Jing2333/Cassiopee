package com.example.yejing.iotgateway1;

import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.KeyEvent;
import android.widget.ListView;
import android.widget.SimpleCursorAdapter;

/**
 * Created by YE Jing on 2018/5/11.
 */

public class HumidityHistory extends AppCompatActivity {
    private ListView listView;

    private Cursor cursor;
    private DBHelper2 dbHelper;
    private SimpleCursorAdapter adapter;
    private int _id=0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_humidity);

        dbHelper = new DBHelper2(this);
        cursor = dbHelper.select();
        listView = this.findViewById(R.id.data_list2);

        adapter = new SimpleCursorAdapter(this, android.R.layout.simple_list_item_1, cursor, new String[]{"humidity"}, new int[]{android.R.id.text1}, 0);
        listView.setAdapter(adapter);
    }

    @Override
    public boolean onKeyDown(int keycode,KeyEvent event) {
        if (keycode == KeyEvent.KEYCODE_BACK) {
            Intent intent = new Intent(this, TemperatureDetection.class);
            startActivity(intent);
            finish();
            return true;
        } else {
            return super.onKeyDown(keycode, event);
        }
    }
}
