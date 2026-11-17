#include "playthread.h"
#include <QDebug>
#include "SDL.h"

#define END(judge, func) \
    if (judge) { \
        qDebug() << #func << "Error" << SDL_GetError(); \
        goto end; \
    }

PlayThread::PlayThread(QObject *parent) : QThread(parent)
{
    connect(this,&QThread::finished,
            this,&QThread::deleteLater);

}

void PlayThread::run()
{
    // 窗口
    SDL_Window *window = nullptr;
    // 渲染上下文
    SDL_Renderer *renderer = nullptr;
    // 像素数据
    SDL_Surface *surface = nullptr;
    // 纹理（直接跟特定驱动程序相关的像素数据）
    SDL_Texture *texture = nullptr;

    // 初始化Video子系统
    END(SDL_Init(SDL_INIT_VIDEO), SDL_Init);

    // 加载BMP
    surface = SDL_LoadBMP("test.bmp");
    END(!surface, SDL_LoadBMP);

    // 创建窗口
    window = SDL_CreateWindow(
                 // 窗口标题
                 "SDL显示BMP图片",
                 // 窗口X（未定义）
                 SDL_WINDOWPOS_UNDEFINED,
                 // 窗口Y（未定义）
                 SDL_WINDOWPOS_UNDEFINED,
                 // 窗口宽度（跟图片宽度一样）
                 surface->w,
                 // 窗口高度（跟图片高度一样）
                 surface->h,
                 // 显示窗口
                 SDL_WINDOW_SHOWN
             );
    END(!window, SDL_CreateWindow);

    // 创建渲染上下文（默认的渲染目标是window）
    renderer = SDL_CreateRenderer(window, -1,
                                  SDL_RENDERER_ACCELERATED |
                                  SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) { // 说明开启硬件加速失败
        renderer = SDL_CreateRenderer(window, -1, 0);
    }
    END(!renderer, SDL_CreateRenderer);

    // 创建纹理
    texture = SDL_CreateTextureFromSurface(
                  renderer,
                  surface);
    END(!texture, SDL_CreateTextureFromSurface);

    // 设置绘制颜色（这里随便设置了一个颜色：黄色）
    END(SDL_SetRenderDrawColor(renderer,
                                 255, 255, 0,
                                 SDL_ALPHA_OPAQUE),
          SDL_SetRenderDrawColor);

    // 用DrawColor清除渲染目标
    END(SDL_RenderClear(renderer),
          SDL_RenderClear);

    // 复制纹理到渲染目标上
    END(SDL_RenderCopy(renderer, texture, nullptr, nullptr),
          SDL_RenderCopy);

    // 将此前的所有需要渲染的内容更新到屏幕上
    SDL_RenderPresent(renderer);
    // 延迟3秒退出
    SDL_Delay(3000);
end:
    // 释放资源
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();


}

PlayThread::~PlayThread()
{
    disconnect();
    requestInterruption();
    quit();
    wait();
    qDebug()<<this;
}

