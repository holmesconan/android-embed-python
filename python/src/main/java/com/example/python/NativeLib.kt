package com.example.python

class NativeLib {

    /**
     * A native method that is implemented by the 'python' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'python' library on application startup.
        init {
            System.loadLibrary("python")
        }
    }
}