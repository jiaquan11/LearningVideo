//
// 视频解码器
// Author: Chen Xiaoping
// Create Date: 2019-08-02
//

#include "v_decoder.h"
#include "../../one_frame.h"

VideoDecoder::VideoDecoder(JNIEnv *env, jstring path, bool for_synthesizer)
        : BaseDecoder(env, path, for_synthesizer) {
}

VideoDecoder::~VideoDecoder() {
    delete m_video_render;
}

void VideoDecoder::SetRender(VideoRender *render) {
    this->m_video_render = render;
}

void VideoDecoder::Prepare(JNIEnv *env) {
    InitRender(env);

    InitBuffer();

    InitSws();
}

void VideoDecoder::InitRender(JNIEnv *env) {
    if (m_video_render != NULL) {
        int dst_size[2] = {-1, -1};
        m_video_render->InitRender(env, width(), height(), dst_size);

        m_dst_w = dst_size[0];//目标渲染尺寸
        m_dst_h = dst_size[1];
        if (m_dst_w == -1) {
            m_dst_w = width();//视频宽
        }
        if (m_dst_h == -1) {
            m_dst_w = height();//视频高
        }
        LOGI(TAG, "dst %d, %d", m_dst_w, m_dst_h)
    } else {
        LOGE(TAG, "Init render error, you should call SetRender first!")
    }
}

void VideoDecoder::InitBuffer() {
    m_rgb_frame = av_frame_alloc();
    // 获取缓存大小
    int numBytes = av_image_get_buffer_size(DST_FORMAT, m_dst_w, m_dst_h, 1);
    // 分配内存
    m_buf_for_rgb_frame = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    // 将内存分配给RgbFrame，并将内存格式化为三个通道后，分别保存其地址
    av_image_fill_arrays(m_rgb_frame->data, m_rgb_frame->linesize,
                         m_buf_for_rgb_frame, DST_FORMAT, m_dst_w, m_dst_h, 1);
}

void VideoDecoder::InitSws() {
    // 初始化格式转换工具
    m_sws_ctx = sws_getContext(width(), height(), video_pixel_format(),
                               m_dst_w, m_dst_h, DST_FORMAT,
                               SWS_FAST_BILINEAR, NULL, NULL, NULL);
}

void VideoDecoder::Render(AVFrame *frame) {
    LOGI(TAG, "VideoDecoder Render 111");
    sws_scale(m_sws_ctx, frame->data, frame->linesize, 0,
              height(), m_rgb_frame->data, m_rgb_frame->linesize);//YUV转RGBA
    LOGI(TAG, "VideoDecoder Render 222");
    OneFrame *one_frame = new OneFrame(m_rgb_frame->data[0], m_rgb_frame->linesize[0], frame->pts,
                                       time_base(), NULL, false);
    m_video_render->Render(one_frame);
    LOGI(TAG, "VideoDecoder Render 333");
    if (m_state_cb != NULL) {
        LOGI(TAG, "VideoDecoder Render 444");
        if (m_state_cb->DecodeOneFrame(this, one_frame)) {
            Wait(0, 200);
            LOGI(TAG, "VideoDecoder Render 5555");
        }
    }
}

bool VideoDecoder::NeedLoopDecode() {
    return true;
}

void VideoDecoder::Release() {
    LOGE(TAG, "[VIDEO] release")
    if (m_rgb_frame != NULL) {
        av_frame_free(&m_rgb_frame);
        m_rgb_frame = NULL;
    }
    if (m_buf_for_rgb_frame != NULL) {
        free(m_buf_for_rgb_frame);
        m_buf_for_rgb_frame = NULL;
    }
    if (m_sws_ctx != NULL) {
        sws_freeContext(m_sws_ctx);
        m_sws_ctx = NULL;
    }
    if (m_video_render != NULL) {
        m_video_render->ReleaseRender();
        m_video_render = NULL;
    }
}