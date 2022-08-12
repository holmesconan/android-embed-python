package com.example.epython

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle

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
