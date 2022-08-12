package com.example.app

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.example.app.R

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
    }

    external fun startPythonThread(apkDir: String, filesDir: String, externalFilesDir: String): Int

    companion object {
        init {
            System.loadLibrary("python")
        }
    }
}
