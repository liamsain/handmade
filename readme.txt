You need to call vcvarsall.bat every session to get the cl compiler working
On my current pc this is located at C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC
I recommend putting this location in your path and creating an alias
Something like 'alias initcl=vcvarsall.bat x64'
run it, then call 'cl' on the command line
you should get a message telling you what version of the compiler you're using

Alternatively, you can just use the developer command prompt that comes with visual studio

Once you've got cl working, call build.bat at the root of the repo to compile
After it's built, the exe can be found in the ./build folder

I'm currently using Visual studio to debug the exe. 
You can call debug.bat after the exe has been generated and it will open visual studio
When VS is open you need to give it a working directory in which to run the exe
Point it to the data dir
F11 to start debugging


