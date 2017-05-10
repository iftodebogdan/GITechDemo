## GITechDemo [![Build status](https://ci.appveyor.com/api/projects/status/bk64sfjj2s51t19k?svg=true)](https://ci.appveyor.com/project/iftodebogdan/gitechdemo)
###### Global illumination technical demo

![GITechDemo v1.6.1 screenshot](https://cloud.githubusercontent.com/assets/3004291/15302439/a855f494-1bbb-11e6-85fa-7af9e1429357.png)

GITechDemo is a global illumination technical demo application developed using a proprietary rendering engine and incorporates the following rendering techniques:
- Physically based rendering pipeline implementation
- High-dynamic-range rendering
- Deferred shading
- Cook-Torrance, Blinn-Phong, Ashikhmin-Shirley and Ward BRDFs
- Cascaded shadow maps
- Percentage-closer filtering for soft shadows
- Ray marched volumetric lighting
- Reflective shadow maps for indirect illumination
- Screen space ambient occlusion
- Screen space reflections
- Depth of field with bokeh
- Camera motion blur
- Bloom
- Anamorphic lens flares
- Filmic tone-mapping
- Fast approximate anti-aliasing (FXAA)
- Film grain

Its purpose is to provide a testbed for accelerating the research, development and assessment of various rendering techniques for educational purposes or as proof of concept. It also serves as a showcase for the underlying proprietary graphics engine, Synesthesia3D, and aids in its continued development by exposing missing features or various bugs.

### Getting started
###### Hardware requirements
You will need a PC running the Windows operating system (tested on 7 and 10, both x64, but any flavour since XP and x86 should work) and equipped with a DirectX 9.0c compliant video card (i.e. with support for Shader Model 3.0), with the latest drivers installed.

###### Software requirements
You will need Visual Studio 2017 in order to compile the code. You can [grab the free Community Edition](https://www.visualstudio.com/) from their official website. Older versions of Visual Studio, with older platform toolsets (i.e. pre-v141), should also work with minimal modifications, since GITechDemo was, until recently, compiled using the Visual Studio 2010 toolset (v100). If you do end up using another version of Visual Studio, make sure that all projects that link against Synesthesia3D are built with the same platform toolset as the latter, because of the presence of some STL objects on the DLL boundary (will be resolved at a later time). Also make sure you have installed the [DirectX Software Development Kit](https://www.microsoft.com/en-us/download/confirmation.aspx?id=6812)

###### Compiling from source code guide
Open the solution file found at "GITechDemo/Code/Solutions/GITechDemo.sln", open the Solution Explorer and right-click on the project titled 'GITechDemo'. From the context menu, choose the option 'Set as StartUp Project'. Right-click it again, but this time choose the option 'Properties'. A new window will appear, titled 'GITechDemo Property Pages'. From the drop-down list named 'Configuration' choose the option 'All Configurations' and from the one named 'Platform' choose 'All Platforms'. Next, select 'Configuration Properties' > 'Debugging', from the list box on the left-hand side, and set the working directory to '..\\..\Data'. Press F5 and click on 'Yes' when asked if you want to build the projects.

Congratulations! You've successfully built and ran GITechDemo! If you've encountered any problems along the way, be sure to [create an issue](https://github.com/iftodebogdan/GITechDemo/issues).

###### Release builds
As an alternative to creating your own binaries from the latest code base, you could also [check out some of the already existing release builds](https://github.com/iftodebogdan/GITechDemo/releases) which include x86 and x64 executables in both Release and Profile configurations, with the latter having profile markers inserted at key points in the rendering pipeline. These will aid profiling tools, such as [PIX for Windows](https://en.wikipedia.org/wiki/PIX_(Microsoft)) or [Intel GPA](https://software.intel.com/en-us/gpa), in organizing captured draw calls. Make sure you have installed the Microsoft Visual C++ Redistributable for Visual Studio 2017 for [x86](https://go.microsoft.com/fwlink/?LinkId=746571) and/or [x64](https://go.microsoft.com/fwlink/?LinkId=746572) systems and the [DirectX End-User Runtimes (June 2010)](https://www.microsoft.com/en-us/download/confirmation.aspx?id=8109) before attempting to launch the application. Always use the .bat files to run the application, never the executable directly, since it requires the working directory to be set to the data folder.
