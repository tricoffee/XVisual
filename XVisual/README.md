## LOGO

[XVisual](images/XVisual_raw.png)

## SLOGAN
Simplify Code, Enhance AI

## Introduction   
这是利用Qt-CPP编写的一个可视化搭建视觉任务的平台   
This is a platform for visually building computer vision tasks, written in Qt-C++. 

可扩展性:
    只用重写几个简单虚函数即可实现功能扩展
定制化：
    可以根据需求自定义算法
解耦性:
    界面代码和算法逻辑代码没有强关联性
迁移性强：
    虽然用户交互部分依赖于Qt, 但导出的解决方案(不依赖于任何Qt库)可以应用到C#等其它平台
可视化：
    所见即所得, 解决方案搭建过程可视化, 掌控中间节点的每一个输出
   
## Requirements   
Before starting, please make sure to install and configure OpenCV, gflags, glog, protobuf, tensorflow 2.x library    
requirements:   
   
### OpenCV   
  
   
### gflags   
It is necessary to ensure that [gflags] are configured and installed before configuring and installing [glog]   
1. git clone https://github.com/gflags/gflags.git   
2. configure and generate gflags.sln with Visual Studio 2019 Compiler in CMake Gui   
For detailed steps on gflags configuration and installation, please refer to this blog link [https://blog.csdn.net/OOFFrankDura/article/details/103587306]   
Another optional way is to download the already compiled gflags library provided by the author of this repository.   
[gflags libaray for Windows on GoogleDrive](https://www.google.com)   
[gflags libaray for Windows on BaiduYunDrive](https://www.baidu.com)    
   
### glog   
1. git clone https://github.com/google/glog.git   
2. configure and generate glog.sln with Visual Studio 2019 Compiler in CMake Gui   
Refer to this blog [https://blog.csdn.net/m0_37829462/article/details/124695715] for detailed steps on glog configuration and installation.    
The already compiled glog library provided by the author of this repository can be accessed by      
[glog libaray for Windows on GoogleDrive](https://www.google.com)   
[glog libaray for Windows on BaiduYunDrive](https://www.baidu.com)   
   
### protobuf   
The operation of TensorFlow 2.x library depends on [libprotobuf.lib], [libprotoc.lib], [protoc.exe], so it is necessary to compile [protobuf]   
When configuring, please set protobuf-MSVC-STATIC-RUNTIME to false to ensure that protobuf uses dynamically linked CRT (C RunTime Library)   
在配置时，请将 protobuf_MSVC_STATIC_RUNTIME 设置为 false, 确保 protobuf 使用动态链接的 CRT (C RunTime Library)  
![protobufConfig](ReadMeSamples/protobufConfig.png)   
The already compiled protobuf library provided by the author of this repository can be accquired from   
[protobuf libaray for Windows on GoogleDrive](https://www.google.com)   
[protobuf libaray for Windows on BaiduYunDrive](https://www.baidu.com)  

### tensorflow 2.x library for CPP on Windows    
Compiling the TensorFlow CPP library is extremely labor-intensive.     
编译 TensorFlow CPP library 是极其耗费心力的，  
And there is [evidence] (https://github.com/tensorflow/tensorflow/issues/61226) to indicate that Tensorflow 2.10.0 and later versions do not support native Windows C++ compile   
并且有![证据] (https://github.com/tensorflow/tensorflow/issues/61226) 表明 Tensorflow 2.10.0 及其之后的版本是不支持 native windows c++ compile 的

1. 安装配置好 CUDA 和 CUDNN
推荐安装的 CUDA/cuDNN version 是：
cuDNN 8.1 for Cuda 11.2
具体安装包名称是：
Cuda 11.2.0 from cuda_11.2.0_460.89_win10.exe
Cudnn for cuda 11.2.0 is from cudnn-11.2-windows-x64-v8.1.0.77.zip

2. git clone `tensorflow-2.9.3`
`git clone -b v2.9.3 https://github.com/tensorflow/tensorflow.git`
这里假设 `tensorflow-2.9.3` 存放地址 `%Path_to_tensorflow-2.9.3%` 是 `D:\NDev\tensorflow-2.9.3`, 并以此做说明

3. Download `bazel-5.4.1`
`tensorflow-2.9.3` 目录下的 `configure.py` 文件里面显示有
``` python 
_TF_MIN_BAZEL_VERSION = '4.2.2'
_TF_MAX_BAZEL_VERSION = '5.99.0'
```
表示支持的bazel版本是4.2.2~5.99.0, 但这里推荐采用 5.4.1
下载 https://github.com/bazelbuild/bazel/releases/download/5.4.1/bazel-5.4.1-windows-x86_64.exe
解压到本地 `%Path_to_Bazel_binary%`, 压缩包只有一个 `bazel-5.4.1-windows-x86_64.exe` 文件，将这个 `bazel-5.4.1-windows-x86_64.exe` 文件重命名为 `bazel.exe`。
将路径 `%Path_to_Bazel_binary%` 添加到 `环境变量` -----> `系统环境变量` -----> `Path 变量`

4.  在 `.bazelrc` 文件里面修改 tensorflow 构建的输出路径
在执行bazel build命令之前, 可能需要修改 tensorflow 构建的输出路径
If your C drive is not large enough, it is necessary to modify the path for tensorflow build output. Open [.bazelrc](tensorflow-2.9.3的网络链接) file under tensorflow-2.9.3, and add a line at the end of this file, the appending content is:
`startup --output_user_root=%Path_to_tf293_build_output%`   
The `%Path_to_tf293_build_output%`after the equal sign is the path you specified for the tensorflow build output.   
比如，我的 `%Path_to_tf293_build_output%` 对应的实际路径是 `D:/NDev/tf293_build`  

5. 安装 MSYS2
安装 `MSYS2`，以获取构建 `TensorFlow` 所需的 bin 工具。如果 `MSYS2` 已安装到 `C:\msys64` 下，请将 `C:\msys64\usr\bin` 添加到 %PATH% 系统环境变量中。
然后，使用 cmd.exe 运行以下命令
``` bash 
pacman -S git patch unzip
```
6. 安装 Visual C++ 生成工具 2019 或者 Visual Studio 2019
Please ref to https://www.tensorflow.org/install/source_windows?hl=zh-cn#install_visual_c_build_tools_2019

7. 配置 build
切换路径并转到 `%Path_to_tensorflow-2.9.3%`
我的 `%Path_to_tensorflow-2.9.3%` 的真实路径是 `D:\NDev\tensorflow-2.9.3`
执行如下命令:
`python ./configure.py`
具体如何操作可以参考 [issues 62688](https://github.com/tensorflow/tensorflow/issues/62688)

8. 认识以 `bazel build` 打头的三条命令
8. Understand the three commands starting with 'bazel build'
在 Windows 平台编译 tensorflow 2.x  CPP library 主要命令包含*.dll的编译、*.lib的编译、头文件安装三个方面, 具体命令如下：  
编译 tensorflow 2.x 的 tensorflow_cc.dll 的命令, 为了叙述方便, 给这条命令起名为 Command_1  
`bazel build --config=opt --config=cuda --define=no_tensorflow_py_deps=true --copt=-nvcc_options=disable-warnings --local_ram_resources=2048 //tensorflow:tensorflow_cc.dll`
编译 tensorflow 2.x 的 tensorflow_cc.lib 的命令, 为了叙述方便, 给这条命令起名为 Command_2    
`bazel build --config=opt --config=cuda --define=no_tensorflow_py_deps=true --copt=-nvcc_options=disable-warnings --local_ram_resources=2048 //tensorflow:tensorflow_cc_dll_import_lib`
安装 头文件 的命令, 为了叙述方便, 给这条命令起名为 Command_3    
`bazel build --config=opt --config=cuda --define=no_tensorflow_py_deps=true --copt=-nvcc_options=disable-warnings --local_ram_resources=2048 //tensorflow:install_headers`
最终编译好的 tensorflow_cc.dll, tensorflow_cc.lib, include 文件夹都位于 `%Path_to_tf293_build_output%`
特别提醒： 如果计算机资源受限，可以使用 --local_ram_resources 限制内存用量

9. 编译完成后在 `%Path_to_tensorflow-2.9.3%` 目录出现 `bazel-bin`, `bazel-out`, `bazel-tensorflow-2.9.3`, `bazel-testlogs` 这4个文件夹，
并且 `tensorflow_cc.dll`, `tensorflow_cc.lib`, `include` 出现在 `bazel-bin/tensorflow` 目录之下:
bazel-bin/tensorflow/
bazel-bin/tensorflow/tensorflow_cc.dll
bazel-bin/tensorflow/tensorflow_cc.lib
bazel-bin/tensorflow/include

注意这些文件夹 `bazel-bin`, `bazel-out`, `bazel-tensorflow-2.9.3`, `bazel-testlogs` 是符号链接，文件夹的实际位置并不在当前目录下，可以用以下命令列举出这些文件夹的实际位置:
``` bash 
dir bazel-*
```
列举出这些文件夹的实际位置如下：
``` bash 
2023/12/21  14:13    <JUNCTION>     bazel-bin [D:\ndev\tf293_build\hu3sahy6\execroot\org_tensorflow\bazel-out\x64_windows-opt\bin]
2023/12/21  14:13    <JUNCTION>     bazel-out [D:\ndev\tf293_build\hu3sahy6\execroot\org_tensorflow\bazel-out]
2023/12/21  14:13    <JUNCTION>     bazel-tensorflow-2.9.3 [D:\ndev\tf293_build\hu3sahy6\execroot\org_tensorflow]
2023/12/21  14:13    <JUNCTION>     bazel-testlogs [D:\ndev\tf293_build\hu3sahy6\execroot\org_tensorflow\bazel-out\x64_windows-opt\testlogs]
```
注意: 我的 `%Path_to_tf293_build_output%` 是 `D:\ndev\tf293_build`, 请自行对应你的文件夹的实际位置

10. 将 `tensorflow_cc.dll`, `tensorflow_cc.lib`, `include` 归并到一起另存起来, 以便后续编码使用


tensorflow 编译参考资料
[build tensorflow from source code on Windows] (https://www.tensorflow.org/install/source_windows)
[Windows10 Bazel 编译 Tensorflow 2 C++ dll 和 lib 文件-CSDN博客](https://blog.csdn.net/yx123919804/article/details/107042822)
[Windows 10 环境从源码编译构建 TensorFlow 2.5.0 (GPU) - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/410757318)
[CUDA GPUs - Compute Capability | NVIDIA Developer](https://developer.nvidia.com/cuda-gpus)
[hidon/Tensorflow-to-C- (github.com)](https://github.com/hidon/Tensorflow-to-C-)

The compiled tensorflow 2.9.3 c++ library for Windows provided by the author of this repository can be obtained from these connection addresses:
[tensorflow 2.9.3 c++ libaray for Windows on GoogleDrive](https://www.google.com)       
[tensorflow 2.9.3 c++ libaray for Windows on BaiduYunDrive](https://www.baidu.com)     


## Quick start     
可以从链接地址下载 (xxx) 获得快递体验
当前可直接体验的功能点有:
1. 导入已完成的解决方案 solutions.json，加载显示，执行计算逻辑，显示相关结果



2. 应用 LoadImage, CVCrop, ImagePre, PreInver, RevertBox, DrawBox 构建一个解决方案




## How Does XVisual Works      
     
       Colleague      XInfer
     /          \     /
    Item  --->  Handle
    /   \       /
loader   XParser   
    \    /        
     json

Item 负责界面显示和用户交互, Handle 负责算法逻辑。
Item 可以处理的事务逻辑包含图像路径选择、RoI设置、AI算法模型选择等。
Handle 可以处理的算法逻辑包含传统算法、AI算法、视觉算法、2D算法、3D算法、NLP算法、音视频算法等。
Item 和 Handle 都是 `Colleague` 的孩子。
Item 和 Handle 加载或解析的数据都以 json 的形式存储。
XInfer 负责整个解决方案的加载和推理, 但不依赖于Qt或C#等Gui。

## Customized    
How to add custom handles and items 

1. 继承 XBaseItem 并重写几个函数


2. 继承 XBaseHandle 并重写几个函数


3. 在 XBaseHandle.h 添加 friend class 声明


4. 在 MainWindow.cpp 添加 item 





## Terminology interpretation

| Handle Name | Meanings | Sources | Dests | innerParam | Other statements |
|-----:|-----------| -----------| -----------|-----------|-----------|
| LoadImage   | 加载图像  |  imagePath : std::string  | image : cv::Mat | ************** | ************** |
| CVCrop   |  裁剪图像   | image : cv::Mat     | croppedImage : cv::Mat   | roi : cv::Rect | ************** |
| ImagePre   | 图像预处理  | image : cv::Mat     | outputImage : cv::Mat,  outputParam : PreParam   | ************** | ************** |
| PreInver | 还原被预处理的图像  | image : cv::Mat,  preParam : PreParam  | restoredImage : cv::Mat   | ************** | ************** |
| RevertBox   | 将检测输出的Boxes还原到原始图像的尺度空间   | detectResults : std::vector<DetectResult>, preParam : PreParam    | detections : std::vector<DetectResult>   | ************** | ************** |
| DrawBox   | 将检测结果画在图像上  | image : cv::Mat, detections : std::vector<DetectResult>, classNames : std::vector<std::string>  | resultImage : cv::Mat  | ************** | ************** |
| TFDetect   | 基于 TensorFlow 2.x 的目标检测 | image : cv::Mat, detections : std::vector<DetectResult>, classNames : std::vector<std::string>  | resultImage : cv::Mat  | 目标检测模型必须以saved_model的形式提供, 且必须提供classnames.txt以便读取class names |

Sources 维护 Handle 的输入, Dests 维护 Handle 的输出, innerParam 维护 Handle 的 内部参数,
以名为 `CVCrop` 的 Handle 为例说明, 它的 `Sources` 是 `image : cv::Mat`, 表示有且仅有 `cv::Mat` 类型且名为 `image` 的输入变量一个, 冒号前面表示输入变量名称, 冒号后面表示输入变量类型
以名为 `ImagePre` 的 Handle 为例说明, 它的 `Dests` 是 `outputImage : cv::Mat,  outputParam : PreParam`, 这里用逗号分割了多个 `输入变量名称:输入变量类型`
如果某个 Handle 不存在 innerParam, 则在对应栏用 `*` 填充, 比如名为 `LoadImage` 的 Handle, 第一行的最后一列是用 `*` 填充的

## License     

This project is licensed under the terms of the MIT license with additional terms. See the [LICENSE](./LICENSE) file for details.     

For commercial use, please contact [lihenghe93@gmail.com] to obtain a commercial license.      

## Citation

@misc{XVisual,
  author = {Liheng He},
  title = {XVisual，这是利用Qt-CPP编写的一个可视化搭建视觉任务的平台},
  year = {2024},
  publisher = {GitHub},
  journal = {GitHub repository},
  howpublished = {\url{https://github.com/tricoffee/XVisual}},
}

## Donation

There are many evolutions and features planned but resources are scarce. In particular, if this project `XVisual` helps you quickly build AI solutions, please think about contributing a bit; you will also most likely quickly benefit from the improvements.
I'd like to list all our donors but value your wishes; please state whether you'd like to have your name, pseudonym or company on the website.
Below is a list of donors, starting from May 2024. 

## Status      
<!-- OnGoing -->

## Task Progress   
| Task | Progress |
|-----:|-----------|
| 2D   | OnGoing   |
| 3D   | ToDo      |
| OCR  | ToDo      |
