package com.cxp.learningvideo

import android.os.Bundle
import android.os.Environment
import android.support.v7.app.AppCompatActivity
import android.view.View
import com.cxp.learningvideo.media.decoder.AudioDecoder
import com.cxp.learningvideo.media.decoder.VideoDecoder
import com.cxp.learningvideo.media.muxer.MP4Repack
import kotlinx.android.synthetic.main.activity_simple_player.*
import java.util.concurrent.Executors

/**
 * 简单播放器页面
 *
 * @author Chen Xiaoping (562818444@qq.com)
 * @since LearningVideo
 * @version LearningVideo
 * @Datetime 2019-10-12 09:33
 *
 */
class SimplePlayerActivity : AppCompatActivity() {
    val path = Environment.getExternalStorageDirectory().absolutePath + "/testziliao/demo_video.mp4"
    lateinit var videoDecoder: VideoDecoder
    lateinit var audioDecoder: AudioDecoder

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_simple_player)

        initPlayer()
    }

    private fun initPlayer() {
        val threadPool = Executors.newFixedThreadPool(10)//创建一个线程池

        /*从SurfaceView控件中拿到surface缓冲区，并配置给硬件解码器,
        硬件解码器将解码数据输出到surface缓冲区中，并刷新到SurfaceView控件中渲染
         */
        videoDecoder = VideoDecoder(path, sfv, null)
        threadPool.execute(videoDecoder)

        audioDecoder = AudioDecoder(path)
        threadPool.execute(audioDecoder)

        /**
         * 音频线程和视频线程各自在独立地解封装，解码和播放
         */
        videoDecoder.goOn()
        audioDecoder.goOn()
    }

    /*
    * 将视频文件音视频数据解封装又重新封装写入到文件中
    * */
    fun clickRepack(view: View) {
        repack()
    }

    private fun repack() {
        val repack = MP4Repack(path)
        repack.start()
    }

    override fun onDestroy() {
        videoDecoder.stop()
        audioDecoder.stop()
        super.onDestroy()
    }
}
