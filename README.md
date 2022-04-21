# Introduction

年轻人的第一台离线渲染器

# Getting started

确保你的系统有xmake

只支持MSVC和GCC进行编译

```bash
mkdir cg
cd cg
git clone https://github.com/star-hengxing/Hinae.git
git clone https://github.com/star-hengxing/cpu_offline_renderer
cd cpu_offline_renderer
xmake build main
xmake run main
```

等待渲染结束，你可以在images目录里查看渲染图片

可以通过命令行参数传递spp和线程数，默认单线程，spp=4

```bash
xmake run main <spp> <thread count>
xmake run main 1024 4
```

# Feature

* 光线传输算法
    - [x] 直接光照(Direct Light)
    - [x] 路径追踪(PT, Path Tracing)
    - [ ] 双向路径追踪(BDPT, Bidirectional Path Tracing)
    - [ ] 梅特波利斯光照传输(MLT, Metropolis Light Transport)
    - [ ] 随机渐进光子映射(SPPM, Stochastic Progress Photon Mapping)

* 几何
    - [x] 球
    - [x] 圆柱
    - [x] 圆盘
    - [ ] 圆锥
    - [ ] 圆环
    - [x] 三角形/三角形网格
    - [x] 立方体
    - [ ] 构造实体几何(CSG, Constructive Solid Geometry)，提供并集、交集、补集
    - [ ] 曲线(curves)->头发

* BXDF
    - [x] 漫反射(Lambertian)
    - [ ] Oren-Nayer
    - [x] 微表面(GGX microfacet)
    - [ ] 傅里叶(BSDF)
    - [ ] 次表面散射(BSSRDF)

* 材质
    - [x] matte
    - [x] 镜子(mirror)

* 纹理
    - [x] 常量(constant)
    - [x] 棋盘(checker)
    - [ ] 贴图

* 光源
    - [x] 点光源(Point Light)
    - [x] 聚光灯(Spot Light)
    - [x] 面光源(Area Light)

* 相机
    - [ ] 正交相机(Orthographic Camera)
    - [x] 透视相机(Perspective Camera)
    - [ ] 真实相机
    - [ ] 环境相机

* 杂类
    - [ ] 动态模糊
    - [x] 自定义场景格式
    - [ ] 基于光谱的渲染
    - [ ] 景深(depth of field)
