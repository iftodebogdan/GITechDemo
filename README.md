## GITechDemo
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
You will need at least Visual Studio 2010 (any edition will do, but you can [grab the Express Edition for free](http://download.microsoft.com/download/1/E/5/1E5F1C0A-0D5B-426A-A603-1798B951DDAE/VS2010Express1.iso)), since the projects are configured to be built with that specific platform toolset (i.e. v100). If using Visual Studio 2010, make sure you install its [Service Pack 1 update](https://www.microsoft.com/en-us/download/details.aspx?id=23691) so that you can open the solution file, which was created with Visual Studio 2015 Update 2. For Visual Studio 2010 Express Edition, in order to build x64 binaries, you will also need [this](http://msdn.microsoft.com/en-us/windowsserver/bb980924.aspx) and [this](http://www.microsoft.com/download/en/details.aspx?displaylang=en&id=4422). Newer platform toolsets should work by manually changing the projects' properties, however care must be taken that all projects that link against Synesthesia3D are built with the same platform toolset as the latter, because of the presence of some STL objects on the DLL boundary (will be resolved at a later time).

###### Compiling from source code guide
Open the solution file found at "GITechDemo/Code/Solutions/GITechDemo.sln", open the Solution Explorer and right-click on the project titled 'GITechDemo'. From the context menu, choose the option 'Set as StartUp Project'. Right-click it again, but this time choose the option 'Properties'. A new window will appear, titled 'GITechDemo Property Pages'. From the drop-down list named 'Configuration' choose the option 'All Configurations' and from the one named 'Platform' choose 'All Platforms'. Next, select 'Configuration Properties' > 'Debugging', from the list box on the left-hand side, and set the working directory to '..\\..\Data'. Press F5 and click on 'Yes' when asked if you want to build the projects.

Congratulations! You've successfully built and ran GITechDemo! If you've encountered any problems along the way, be sure to [create an issue](https://github.com/iftodebogdan/GITechDemo/issues).

###### Release builds
As an alternative to creating your own binaries from the latest code base, you could also [check out some of the already existing release builds](https://github.com/iftodebogdan/GITechDemo/releases) which include x86 and x64 executables in both Release and Profile configurations, with the latter having profile markers inserted at key points in the rendering pipeline. These will aid profiling tools, such as [PIX for Windows](https://en.wikipedia.org/wiki/PIX_(Microsoft)) or [Intel GPA](https://software.intel.com/en-us/gpa), in organizing captured draw calls.
