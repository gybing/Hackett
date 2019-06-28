package com.roli.H;

import com.roli.H.Java;

import android.app.Application;

public class JuceApp extends Application
{
    @Override
    public void onCreate ()
    {
        super.onCreate ();
        Java.initialiseH (this);
    }
}
