package com.roli.H;

import android.content.Context;

public class Java
{
    static
    {
        System.loadLibrary ("jni");
    }

    public native static void initialiseH (Context appContext);
}
