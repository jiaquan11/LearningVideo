package com.cxp.learningvideo

import android.annotation.SuppressLint
import android.app.Application
import android.content.Context

/**
 *
 * @author Chen Xiaoping (562818444@qq.com)
 * @since LearningVideo
 * @version LearningVideo
 * @Datetime 2019-10-10 09:28
 *
 */
@SuppressLint("StaticFieldLeak")
var CONTEXT: Context? = null

//得到应用程序对象的上下文
class MainApp : Application() {
    override fun onCreate() {
        super.onCreate()
        CONTEXT = this
    }
}
