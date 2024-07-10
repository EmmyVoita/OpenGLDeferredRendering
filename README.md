# OpenGLDeferredRendering

This project demonstrates an OpenGL deferred rendering pipeline with the ability to load and render blend OBJ files. It aims to recreate a scene with several items on a table and utilizes screen space ambient occlusion (SSAO) for enhanced visual realism.

![image](https://github.com/EmmyVoita/OpenGLDeferredRendering/assets/82542924/e2fbda9c-671f-43e6-b917-c1c448d5d951)


To run this program you will need everything in the repository. 
If anything is missing the program will throw an error or a segmentation fault will occur. 

To compile, navigate to the download file and run the following command in an application that runs Linux like Ubuntu:

g++ DeferredRendering.cpp -o run -lglfw -lGL -lGLEW -lSOIL

How to run:

./run

