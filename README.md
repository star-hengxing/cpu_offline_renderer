# Introduction

年轻人的第一台离线渲染器

# Getting started

确保你的系统有[xmake](https://xmake.io/)和支持C++20的编译器(MSVC, GCC)

Clang对C++20支持太慢，暂时不支持

xmake初始化项目会自动下载项目的依赖，按照操作提示即可，如果下载失败，请看[这里](https://xmake.io/#/zh-cn/package/remote_package?id=%e8%bf%9c%e7%a8%8b%e5%8c%85%e4%b8%8b%e8%bd%bd%e4%bc%98%e5%8c%96)，我推荐手动设置代理(记得修改代理端口)
```bash
xmake g --proxy="socks5://127.0.0.1:<port>"
```
按照命令行，依赖的Hinae放在cg目录，如果想放在项目目录内，需要修改`xmake.lua`
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
    - [x] 球(Sphere)
    - [x] 圆柱(Cylinder)
    - [x] 圆盘(Disk == Annulus内半径长度为0)
    - [x] 圆锥(Cone)
    - [x] 圆环(Torus)
    - [x] 立方体(Cuboid)
    - [x] 三角形/三角形网格
    - [ ] 构造实体几何(CSG, Constructive Solid Geometry)，提供并集、交集、补集
    - [ ] 曲线(curves) -> 头发

* BxDF
    - [x] 漫反射(Lambertian)
    - [x] 导体(Conductor)
    - [x] 绝缘体(Dielectric)
    - [ ] Oren-Nayer
    - [x] 微表面(GGX microfacet)
    - [ ] 次表面散射(BSSRDF)

* 材质
    - [x] Matte
    - [x] 粗糙/光滑的金属(Metal)
    - [x] 粗糙/光滑的玻璃

* 纹理
    - [x] 常量(constant)
    - [x] 棋盘(checker)
    - [x] 贴图

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
