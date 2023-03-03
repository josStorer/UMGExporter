# **简体中文 | [English](README.md)**

这是一个虚幻引擎插件, 用于为UMG蓝图的控件和动画自动生成c++代码绑定

![Preview](Preview.gif)

## 注意事项

1. 希望导出到c++的控件, 需要勾选Is Variable
2. 你仍然需要自己include控件类的头文件, 但一般IDE能够智能提示快捷引入
3. UMG蓝图文件的命名规范应该是WBP_或BP_开头
4. 某个控件是蓝图类型, 但它的父类最终是cpp类的, 会一直向上检测到cpp类, 并插入绑定代码, 方便程序编写
5. 你可能需要根据你的项目目录解构规范, 调整工程设置`Project Settings->Plugins->UMGExporter`
    - `ContentSourcePath`: UMG蓝图文件相对于此目录的相对路径, 将会保持并生成到下面的`ExportTargetPath`目录下, 默认为`Content`目录, 一些项目规范会在Content目录下建立项目同名的目录, 则你需要将此项设为这个目录
    - `ExportTargetPath`: 生成文件的导出相对路径, 默认为`Source/项目名`目录
